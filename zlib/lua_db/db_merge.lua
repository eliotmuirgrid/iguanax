-- $Revision: 1.11 $
-- $Date: 2012-12-06 17:11:49 $

--
-- The db_merge module
-- Copyright (c) 2011-2012 iNTERFACEWARE Inc. ALL RIGHTS RESERVED
-- iNTERFACEWARE permits you to use, modify, and distribute this file in accordance
-- with the terms of the iNTERFACEWARE license agreement accompanying the software
-- in which it is used.
--

require 'dbc'
require 'node'

local function trace(a,b,c,d) return end

local function OracleDateTimeCvt(N, R)
   R[#R+1] = 'TO_DATE('
   R[#R+1] = '\''
   R[#R+1] = tostring(N)
   R[#R+1] = '\''
   R[#R+1] = ', \'YYYY-MM-DD HH24:MI:SS\')'
end

local function StringValue(N, R, DbType)
   if N:isNull() then
      R[#R+1] = 'NULL'
   elseif N:S():match('0x') then
      R[#R+1] = tostring(N)
   else
      R[#R+1] = '\''
      R[#R+1] = DbType.Escape(tostring(N))
      R[#R+1] = '\'' 
   end
end

local function DateValue(N, R, DbType)
   if N:isNull() then
      R[#R+1] = 'NULL'
   elseif DbType.DateTimeCvt ~= nil then
      DbType.DateTimeCvt(N, R)
   else
      R[#R+1] = '\''
      R[#R+1] = tostring(N)
      R[#R+1] = '\''
   end
end

local function DoubleValue(N, R)
   if N:isNull() then
      R[#R+1] = 'NULL'
   else
      R[#R+1] = tostring(N)
   end      
end

local function IntegerValue(N, R)
   if N:isNull() then
      R[#R+1] = 'NULL'
   else
      R[#R+1] = tostring(N)
      trace(R)
   end    
end

local ValueTable={
   string=StringValue,   
   datetime=DateValue,
   double=DoubleValue,
   integer=IntegerValue,
}

local function MergeTableValue(N, R, DbType)
   ValueTable[N:nodeType()](N, R, DbType)     
end

local function MergeTableRow(T, R, DbType, merge_null) 
   -- it may be a little counter-intuitive to do a 
   -- sparse insert when merge_null = false but it
   -- means you get column defaults with the insert
   -- which is the desired behaviour
   R[#R+1] = '('
   for i=1, #T do
      if T[i]:nodeValue() ~= '' or
         (T[i]:nodeValue() == '' and merge_null) then
         if i ~= 1 and R[#R] ~= '(' then
            R[#R+1] = ', '
         end
         R[#R+1] = T[i]:nodeName()
      end
   end
   trace(R)
   R[#R+1] = ') VALUES('
   for i=1, #T do
      if T[i]:nodeValue() ~= '' or
         (T[i]:nodeValue() == '' and merge_null) then
         if i ~= 1 and R[#R] ~= ') VALUES(' then
            R[#R+1] = ', '
         end
         MergeTableValue(T[i], R, DbType)
      end
   end
   R[#R+1] = '); '
   trace(R)
end

local function UpdateTableRow(T, R, DbType, TableKeySet, merge_null)
   R[#R+1] = ' SET '  
   for i=1, #T do
      trace(T[i]:nodeValue())
      if T[i]:nodeValue() ~= '' or
         (T[i]:nodeValue() == '' and merge_null) then
         R[#R+1] = T[i]:nodeName()
         R[#R+1] = ' = '
         MergeTableValue(T[i], R, DbType)
         R[#R+1] = ', '
      end
   end
   trace(R)
   R[#R] = ' '
   R[#R+1] = ' WHERE '
   
   -- use local TableKeySet to match case for tables (T)
   -- to match the hard coded case in the VMD
   local tablekeyset = {}
   for i = 1, #TableKeySet do    
      for j = 1, #T do  
         local tname = T[j]:nodeName()
         if TableKeySet[i]:lower() == tname:lower() then 
            tablekeyset[i] = tname
         end
      end
   end
   trace(tablekeyset)
   
   for i = 1, #TableKeySet do    
      R[#R+1] = tablekeyset[i]
      R[#R+1] = ' = '
      MergeTableValue(T[tablekeyset[i]], R, DbType)
      R[#R+1] = ' AND '
   end   
   R[#R] = ' ' 
end

local function mergeKeys(T, R, TableKeySet, DbType)
   -- using NULL when we get a '' node value may seem
   -- counter-intuitive but it allows code which will
   -- (attempt) to do an insert as no row will be 
   -- found with a NULL key value this has two advantages
   -- (1) nicer error messages when insert fails
   -- (2) when an autoincrementing PK is used the 
   -- insert will SUCCEED which is the correct
   -- behaviour   
   for j=1, #TableKeySet do
      if j > 1 then
         R[#R+1] = ' AND '
      end
      R[#R+1] = TableKeySet[j]
      R[#R+1] = ' = '
      for k=1, #T do
         trace(j,TableKeySet[j],k)
         if T[k]:nodeName():lower() == TableKeySet[j]:lower() then
            if T[k]:nodeValue() == '' then 
               R[#R+1] = 'NULL'
            else
               MergeTableValue(T[k], R, DbType)
            end
         end
      end
   end
   trace(R)
end

local function SqlServerReplace(T, output, DbType, TableKeySet, P)
   local R = {}
   for i=1, #T do
      R[#R+1] = '\n'
      if TableKeySet[1] then
         R[#R+1] = 'IF (SELECT COUNT('
         R[#R+1] = TableKeySet[1]
         trace('hello',TableKeySet[1])
         R[#R+1] = ') FROM '
         R[#R+1] = T:nodeName()
         R[#R+1] = ' WHERE '
         mergeKeys(T[i], R, TableKeySet, DbType)
         R[#R+1] = ') = 1'
         R[#R+1] = '\n\t'
         R[#R+1] = 'UPDATE '
         R[#R+1] = T:nodeName()
         UpdateTableRow(T[i], R, DbType, TableKeySet, P.merge_null)
         R[#R+1] = '\n'
         R[#R+1] = 'ELSE'
         R[#R+1] = '\n\t'
      end
      R[#R+1] = 'INSERT INTO '
      R[#R+1] = T:nodeName()
      MergeTableRow(T[i], R, DbType, P.merge_null)
      output[#output+1] = table.concat(R) 
      trace(output[2])
      R={}         
   end
end

local function MySQLReplace(T, output, DbType, TableKeySet, P)
   local R = {}
   for i=1, #T do
      local r = {}
      R[#R+1] = '\n'
      local cnt
      if TableKeySet[1] then
         r[#r+1] = 'SELECT COUNT('
         r[#r+1] = TableKeySet[1]
         r[#r+1] = ') FROM '
         r[#r+1] = T:nodeName()
         r[#r+1] = ' WHERE '
         mergeKeys(T[i], r, TableKeySet, DbType)
         r[#r+1] = ';'
         local sql = table.concat(r)
         cnt = DB:query(sql)[1][1]:nodeValue()
      end
      if cnt == '1' then
         R[#R+1] = 'UPDATE '
         R[#R+1] = T:nodeName()
         UpdateTableRow(T[i], R, DbType, TableKeySet, P.merge_null)
         R[#R+1] = ';'
      else
         R[#R+1] = 'REPLACE '
         R[#R+1] = T:nodeName()
         MergeTableRow(T[i], R, DbType, P.merge_null)
      end
      R[#R+1] = '\n'
      output[#output+1] = table.concat(R) 
      trace(output[2])
      R={}         
   end
end

local function OracleReplace(T, output, DbType, TableKeySet, P)
   local R = {}
   for i=1, #T do
      if not P.transaction then
         R[#R+1] = 'DECLARE R INT; '
         R[#R+1] = '\n'
         R[#R+1] = 'BEGIN '
      end
      if TableKeySet[1] then
         R[#R+1] = '\n'
         R[#R+1] = 'SELECT COUNT('
         R[#R+1] = TableKeySet[1]
         R[#R+1] = ')INTO R '
         R[#R+1] = 'FROM '
         R[#R+1] = T:nodeName()
         R[#R+1] = ' WHERE '
         mergeKeys(T[i], R, TableKeySet, DbType)
         R[#R+1] = ';'
         R[#R+1] = '\n'
         R[#R+1] = 'IF R = 1 THEN'
         R[#R+1] = '\n\t'
         R[#R+1] = 'UPDATE '
         R[#R+1] = T:nodeName()
         UpdateTableRow(T[i], R, DbType, TableKeySet, P.merge_null)
         R[#R+1] = ';'
         R[#R+1] = '\n'
         R[#R+1] = 'ELSE'
      end
      R[#R+1] = '\n\t'
      R[#R+1] = 'INSERT INTO '
      R[#R+1] = T:nodeName()
      MergeTableRow(T[i], R, DbType, P.merge_null)
      R[#R+1] = '\n'
      if TableKeySet[1] then
         R[#R+1] = 'END IF;'
         R[#R+1] = '\n'
      end
      if not P.transaction then
         R[#R+1] = 'COMMIT;'
         R[#R+1] = 'END;'
      end
      output[#output+1] = table.concat(R) 
      trace(output[1])
      R={}         
   end
end

local function MySQLEscape(Value)
   return Value:gsub('["\'\\%z]', {
         ['"']  = '\\"', ['\0'] = '\\0',
         ["'"]  = "\\'", ['\\'] = '\\\\',
      })
end

local function SingleQuoteEscape(Value)
   return Value:gsub("'", "''")
end

local function PostgresEscape(Value)
   return Value:gsub("['\\]", {["'"] = "''", ["\\"] = "\\\\"})
end

local db_TYPE ={ 
   [db.MY_SQL]      = {TableKeyMap = false, Escape = MySQLEscape, Replace =  MySQLReplace,  Transaction = {BeginTransaction = 'START TRANSACTION;', Commit = 'COMMIT;'}},
   [db.MY_SQL_ODBC] = {TableKeyMap = false, Escape = MySQLEscape, Replace =  MySQLReplace,  Transaction = {BeginTransaction = 'START TRANSACTION;', Commit = 'COMMIT;'}},
   [db.ORACLE_OCI]  = {TableKeyMap = true, Escape = SingleQuoteEscape, Replace =  OracleReplace, DateTimeCvt = OracleDateTimeCvt, Transaction = {BeginTransaction = 'DECLARE R INT; BEGIN', Commit = 'COMMIT; END;'}},
   [db.ORACLE_ODBC] = {TableKeyMap = true, Escape = SingleQuoteEscape, Replace =  OracleReplace, DateTimeCvt = OracleDateTimeCvt, Transaction = {BeginTransaction = 'DECLARE R INT; BEGIN', Commit = 'COMMIT; END;'}},
   [db.SQLITE]      = {Escape = SingleQuoteEscape}, 
   [db.SQL_SERVER]  = {TableKeyMap = true, Escape = SingleQuoteEscape, Replace =  SqlServerReplace, Transaction = {BeginTransaction = 'BEGIN TRANSACTION;', Commit = 'COMMIT;'}},
   [db.POSTGRES]    = {Escape = PostgresEscape},
   [db.DB2]         = {Escape = SingleQuoteEscape},
   [db.INFORMIX]    = {Escape = SingleQuoteEscape},
   [db.INTERBASE]   = {Escape = SingleQuoteEscape},
   [db.FILEMAKER]   = {Escape = SingleQuoteEscape},
   [db.SYBASE_ASA]  = {Escape = SingleQuoteEscape},
   [db.SYBASE_ASE]  = {Escape = SingleQuoteEscape},
   [db.ACCESS]      = {Escape = SingleQuoteEscape}
}

local function MergeTable(T, output, DbType, TableKeySet, P)
   DbType.Replace(T, output, DbType, TableKeySet, P)
end

local function GetTableList(T)
   local R ={}
   for i=1, #T do
      R[i] = T[i]:nodeName()  
   end
   return R
end

local function CheckTable(T,Usage)
   if type(T) ~= 'table' then
      error(Usage,3)
   end
end

local function CheckParam(T, List, Usage)
   for i=1, #List do
      if not T[List[i]] then error('Missing parameter "'..List[i]..'"\n'..Usage, 3) end
   end
end

local Usage = [[
Merges table records into the database. This is a Special Implementation 
written in Lua it is NOT the default (builtin) implementation of db.merge.
It currently supports MySQL, SQL_SERVER and ORACLE_ODBC and will be extended to other databases in the future

Returns: nothing.
Accepts a table with the required entries:
   'name'        - database name/address. 
   'user'        - user name 
   'password'    - password 
   'data'        - set to a node table tree created using db.tables
   'api'         - database api, should be in the form: db.SQL_SERVER, db.MY_SQL, db.POSTGRES and etc.
   'transaction' - when this is true the merge will be performed as a single transaction (default = true)
These additional optional entries exist:
   'live'          - if true, the merge will be executed in the editor
   'pre_queries'   - table of queries to be executed BEFORE the merge in the same transaction
   'post_queries'  - table of queries to be executed AFTER the merge in the same transaction
   'table_order'   - optionally give an array table names in the order you would like to insert them
   'merge_null'    - when false do not update/insert any NULL values, to preserve current column value
                     defaults to true to match behaviour of the builtin db.merge()
   'table_key_map' - optional for SQL Server and Oracle - only for backward compatibility (IGNORED if supplied)
                     NOTE: the mapping is now generated automatically from database system tables

e.g. db.merge{
   name='test@localhost', 
   user='root', 
   password='secret',
   data=Out, 
   merge_null=false, -- do not use quotes
   api = db.SQL_SERVER,
   live = true
}
]]
 
local function MakeSql(P, TableKeyMap, DbType)
   local T = P.data
   if TableKeyMap == nil then
      TableKeyMap = {}
   end
   if not P.table_order then
      P.table_order=GetTableList(T)   
   end
   local output = {}
   if P.transaction then
      output = {DbType.Transaction.BeginTransaction}
   end
   if not P.preQueries then P.preQueries = {} end
   for k,v in ipairs(P.preQueries) do
      output[#output + 1] = '\n'..v..'\n'
   end
   for t=1, #P.table_order do
      local TableName = P.table_order[t]
      trace(TableName)
      trace(TableKeyMap)
      local keyname = {}
      -- loop to get keyname case independent
      for k,v in pairs(TableKeyMap) do
         trace (k,v)
         if k:lower() == TableName:lower() then
            keyname = v
         end
      end
      MergeTable(T[TableName], output, DbType, keyname, P)
   end
   if not P.postQueries then P.postQueries = {} end
   for k,v in ipairs(P.postQueries) do
      output[#output + 1] = '\n'..v..'\n'
   end
   if P.transaction then
      output[#output + 1] = DbType.Transaction.Commit
   end
--   trace(tostring(output))
   return output
end

local function mkMap(DB, sql)
   local r = DB:query(sql)
   local map = {}
   for i=1,#r do
      if r[i].HasKey:S():lower() == 'true' then
         if i > 1 and (r[i].Table:S():lower() ~= r[i-1].Table:S():lower()) then
            map[r[i].Table:S()] = {r[i].KeyCol:S()}
         else
            if map[r[i].Table:S()] == nil then
               map[r[i].Table:S()] = {}
               map[r[i].Table:S()][1] = r[i].KeyCol:S()
            else
               local len = #map[r[i].Table:S()]
               map[r[i].Table:S()][len+1] = r[i].KeyCol:S()
            end
         end
      else
         -- table without a PK
         map[r[i].Table:S()] = {}
      end
   end
   return map
end

-- NOTE: the ODBC source must point to the correct DB
-- MSSQL specific query (based on system views) 
-- NOTE: using CASE for backward compatibility
--       IIF only works with SQL Server 2012 (Denali)
local sqlMSSQL =[[
SELECT s.TABLE_NAME [Table] 
   ,CASE WHEN c.CONSTRAINT_TYPE = 'PRIMARY KEY'
      THEN 'TRUE' ELSE 'FALSE' END HasKey
   ,k.COLUMN_NAME KeyCol
   ,c.CONSTRAINT_NAME KeyName
   ,k.ORDINAL_POSITION IndexOrder
FROM INFORMATION_SCHEMA.TABLES s
   LEFT JOIN INFORMATION_SCHEMA.TABLE_CONSTRAINTS c
      ON s.TABLE_NAME = c.TABLE_NAME 
      AND s.TABLE_TYPE = 'BASE TABLE'
      AND c.CONSTRAINT_TYPE = 'PRIMARY KEY'
   LEFT JOIN INFORMATION_SCHEMA.KEY_COLUMN_USAGE k
      ON c.TABLE_NAME = k.TABLE_NAME 
      AND c.CONSTRAINT_NAME = k.CONSTRAINT_NAME
ORDER BY k.TABLE_NAME, k.COLUMN_NAME, k.ORDINAL_POSITION]]

local sqlOracle =[[
SELECT t.table_name "Table"
   ,CASE WHEN k.constraint_type = 'P' THEN 'TRUE' ELSE 'FALSE' END "HasKey"
   ,c.column_name "KeyCol"
   ,k.constraint_name "KeyName"
   ,c.position "IndexOrder"
FROM sys.all_tables t
   LEFT JOIN all_constraints k
      ON k.table_name = t.table_name AND k.constraint_type = 'P'
   LEFT JOIN all_cons_columns c
      ON c.table_name = k.table_name AND c.constraint_name = k.constraint_name
WHERE t.owner NOT IN ( 'SYS','SYSTEM','XDB','CTXSYS','OUTLN','MDSYS','HR','FLOWS_FILES','APEX_040000')
ORDER BY "Table", c.position]]

local function sqlMySQL(dbname)
   local sql = [[SELECT s.TABLE_NAME "Table"
      ,CASE WHEN c.CONSTRAINT_NAME = 'PRIMARY' THEN 'TRUE' ELSE 'FALSE' END HasKey
      ,c.COLUMN_NAME KeyCol
      ,c.CONSTRAINT_NAME KeyName
      ,c.ORDINAL_POSITION IndexOrder
   FROM INFORMATION_SCHEMA.TABLES s
      LEFT JOIN INFORMATION_SCHEMA.KEY_COLUMN_USAGE c
         ON s.TABLE_NAME = c.TABLE_NAME 
         AND s.TABLE_TYPE = 'BASE TABLE'
         AND c.CONSTRAINT_NAME = 'PRIMARY'
   WHERE s.TABLE_SCHEMA = ']]..dbname..[['
   ORDER BY c.TABLE_NAME, c.COLUMN_NAME, c.ORDINAL_POSITION]]
   return sql
end

local function dbName(api)
   if type(api) == type(db.MY_SQL) or type(api) == type(db.MY_SQL_ODBC) then
      for k,v in pairs(db) do
         if v == api then return 'db.'..k end
      end
   end
   return 'unknown API ('..tostring(api)..')'
end

--replace merge function
function db.merge(P)
   CheckTable(P, Usage)
   CheckParam(P, {'name','user','password', 'data','api'}, Usage) 
   local DbType = db_TYPE[P.api]

   if not DbType or DbType.Transaction == nil then 
      error('Merge using '..dbName(P.api)..' is not currently supported.', 2) end
   
   if type(P.data) ~= 'userdata' or P.data:nodeType() ~= 'table_collection' then
      error('The data parameter must be a table collection as produced from db.tables{}\n'..Usage,2)
   end 
   
   -- merge_null defaults to true to match legacy behaviour
   if P.merge_null == nil then P.merge_null = true end
   -- transaction defaults to true to match legacy behaviour
   if P.transaction == nil then P.transaction = true end

   local Sql = ''
   -- generate 'table_key_map' (ignore P.table_key_map)
   DB = dbc.Connection{ 
      api=P.api, 
      name=P.name,
      user =P.user,
      password = P.password,
      use_unicode = P.use_unicode,
      live = true}
   if P.api == db.SQL_SERVER then
      Sql = MakeSql(P, mkMap(DB, sqlMSSQL), DbType)
   elseif P.api == db.ORACLE_ODBC or P.api == db.ORACLE_OCI then
      Sql = MakeSql(P, mkMap(DB, sqlOracle), DbType)
   elseif P.api == db.MY_SQL then
      local dbname = P.name:gsub('@.*','')
      Sql = MakeSql(P, mkMap(DB, sqlMySQL(dbname)), DbType)
   elseif P.api == db.MY_SQL_ODBC then
      local dbname = P.user
      Sql = MakeSql(P, mkMap(DB, sqlMySQL(dbname)), DbType)
   --elseif P.api == db.SQLITE then
   -- need different logic for SQLite as it only has one system table
   -- sqlite_master will need to parse the table create statements 
   -- to find the primary key details... (not so hard)
   else
      -- use table_key_map in other cases
      -- NOTE: AT PRESENT THIS CODE WILL NEVER EXECUTE
      --       there will be an unsupported DB first (this could be changed)
      Sql = MakeSql(P.data, P.table_order, P.table_key_map, DbType, P)
   end

   trace (Sql)
   local SQL = ''
   if P.api == db.MY_SQL or P.api == db.MY_SQL_ODBC then
      -- execute transaction as a single "big query" does not work
      -- so always execute each line separately
      for i=1,#Sql do
         DB:execute(Sql[i], P.live)
      end
   elseif P.transaction then 
      -- execute queries as a single transaction
      for i=1,#Sql do
         SQL = SQL..Sql[i]
      end
      trace(SQL)
      DB:execute(SQL, P.live)
   else
      -- execute each query separately
      for i=1,#Sql do
         DB:execute(Sql[i], P.live)
      end
   end

   if P.live == nil or P.live == false then
      return "Not executed in editor.  To execute set live=true in input parameters", Sql
   else
      return "Executed in editor since live=true.", Sql
   end  
end

