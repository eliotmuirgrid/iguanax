require 'db_test_config'
require 'common.testutils'

---------------
-- Utilities --
---------------

local function trace(a,b,c,d) return end

iguana.isTest = function() return true end -- iguana.isTest does not exist as part of core NODL

local function copy(tbl, memo)
   if type(tbl) ~= 'table' then
      return tbl
   end
   if not memo then memo = {} end
   local out = memo[tbl]
   if out ~= nil then return out end
   out = {}; memo[tbl] = out
   for k,v in pairs(tbl) do out[k] = copy(v,memo) end
   return out
end

local function sameTable(a, b)
   assert(a:nodeType() == 'result_set')
   assert(b:nodeType() == 'table')
   if #a ~= #b then
      return false, 'rows: '..#a..' '..#b
   end
   local ncols = #a[1]
   if #b[1] ~= ncols then
      return false, 'cols: '..#a[1]..' '..#b[1]
   end
   for r = 1, #a do
      for c = 1, ncols do
         local x = a[r][c]
         local y = b[r][x:nodeName():lower()]
         x = tostring(x)
         y = tostring(y)
         if x ~= y then
            local s = 'cell(%d,%d) = %s and %s'
            return false, s:format(r,c,x,y)
         end
      end
   end
   return true
end

local function checkForTable(res, tableName)   
   for i = 1,#res do
      if res[i]:isLeaf() and res[i]:nodeValue() == tableName then return true end
      for j = 1,#res[i] do
         if res[i][j]:isLeaf() and res[i][j]:nodeValue() == tableName then return true end
      end
   end
   return false
end

local function sqlCreateBasic(Db, name)
   if Db.api == db.SQLITE then
      return 'CREATE TABLE ' .. name .. 
      '(key_1 INTEGER NOT NULL, name TEXT(30), PRIMARY KEY(key_1))'
   else
      local Sql = 'CREATE TABLE ' .. name .. 
      '(key_1 INTEGER NOT NULL, name VARCHAR(30), PRIMARY KEY(key_1))'
      if Db.api == db.MY_SQL then
         -- This is needed in MySQL for the table to support transactions.
         Sql = Sql .. ' ENGINE = InnoDB'
      end
      return Sql
   end
end

local function sqlCreateTK(Db)
   if Db.api == db.SQLITE then
      return [[CREATE TABLE TK 
      (key_1 INTEGER NOT NULL, key_2 INTEGER NOT NULL,
      an_integer INTEGER,a_string TEXT(255),a_double DOUBLE,
      a_date TEXT,PRIMARY KEY (key_1,key_2))]]
   elseif Db.api == db.SQL_SERVER or Db.api == db.SYBASE_ASE then
      -- "FLOAT" instead of "DOUBLE"
      return [[CREATE TABLE TK 
      (key_1 INTEGER NOT NULL ,key_2 INTEGER NOT NULL ,
      an_integer INTEGER,a_string VARCHAR(255), a_double FLOAT,
      a_date DATETIME,PRIMARY KEY (key_1,key_2))]]
   elseif Db.api == db.ORACLE_ODBC or Db.api == db.ORACLE_OCI then
      -- "FLOAT" instead of "DOUBLE", "DATE" instead of "DATETIME"
      return [[CREATE TABLE TK 
      (key_1 INTEGER NOT NULL ,key_2 INTEGER NOT NULL ,
      an_integer INTEGER,a_string VARCHAR(255), a_double FLOAT,
      a_date DATE,PRIMARY KEY (key_1,key_2))]]
   else
      return [[CREATE TABLE TK 
      (key_1 INTEGER NOT NULL, key_2 INTEGER NOT NULL,
      an_integer INTEGER,a_string VARCHAR(255),a_double DOUBLE,
      a_date DATETIME,PRIMARY KEY (key_1,key_2))]]
   end
end

---------------
-- Tests --
---------------

local function testStuff(Db)   
   local sqlCreate = sqlCreateBasic(Db, 'stuff')
   
   local name1 = [[testing "foo"/'bar']]  
   local name2 = [[testing "bar"/'foo']]
   
   Db.live = true
   
   local conn = db.connect(Db)

   local res
   
   pcall(db.execute, conn, {sql = 'DROP TABLE stuff', live = true})
   
   conn:execute{sql = sqlCreate, live = true}      
   conn:query{sql = 'SELECT * FROM stuff'} 
   
   conn:begin()
   conn:execute{sql = 'INSERT into stuff VALUES (37,' .. conn:quote(name1) .. ')', live = true}
   conn:commit()
   res = conn:query{sql = 'SELECT * FROM stuff'}
   
   if res[1].key_1:nodeValue() ~= '37' then
      error('value not committed')
   end
   testutils.assertEquals(res[1].name:nodeValue(), name1)
   
   conn:begin{}      
   conn:execute{sql = 'INSERT into stuff VALUES (38,' .. conn:quote(name2) .. ')', live = true}      
   conn:query{sql = 'SELECT * FROM stuff'}                             
   conn:rollback{}     
   res = conn:query{sql = 'SELECT * FROM stuff'}
   
   if #res > 1 and res[2].key_1:nodeValue() == '38' then
      error('value not rolled back')
   end
   
   conn:execute{sql = 'drop table stuff', live = true}    
end

--combine multiple tables into one, with later arguments overwriting earlier ones
local function addParams(...)
   local t = {}
   for i = 1, select('#', ...) do
      local arg = select(i, ...)
      if arg then
         for k,v in pairs(arg) do
            t[k] = v
         end
      end
   end
   return t
end
   
local function bogusDbConnectionTable(AdditionalParams)
   return addParams({api = db.MY_SQL, name = 'not a name', user='admin', password='foo', live = false}, AdditionalParams)   
end

local function testNotLive(Db)   
   local sqlCreate = sqlCreateBasic(Db, 'stuff')
   local sqlInsert = 'INSERT INTO things VALUES(123, \'foo\')'
   local sqlSelect = 'SELECT * FROM things'
   
   local name1 = [[testing "foo"/'bar']]  
   local name2 = [[testing "bar"/'foo']]
   
   Db.live = false
   
   local conn = db.connect(Db)
   
   local Err = {}
   local trackerror = function(Func, ...)          
      local success, res = pcall(Func, ...)
      
      if not success then
         Err[#Err + 1] = res
         return
      end
      
      return res
   end
   
   local res = trackerror(db.query, conn, sqlSelect)
   if iguana.isTest() and res and #res > 0 then
      Err[#Err+1] = "Not 'live' query returning a value"
   end
   
   trackerror(db.query, conn, sqlSelect)
   trackerror(db.execute, conn, "DROP TABLE things")
   trackerror(db.execute, conn, sqlCreate)
   
   trackerror(db.query, conn, {sql = sqlSelect, live = true})
   
   trackerror(db.begin, conn, {live = true})
   trackerror(db.begin, conn)      
   
   trackerror(db.execute, conn, {sql = 'INSERT into things VALUES (37,' .. conn:quote(name1) .. ')'})
   
   trackerror(db.commit, conn, {live = true})
   trackerror(db.commit, conn)
   
   trackerror(db.query, conn, "SELECT * FROM things")
   trackerror(db.begin, conn)
   
   trackerror(db.execute, conn, {sql = 'INSERT into things VALUES (38,' .. conn:quote(name2) .. ')', live = true})
   trackerror(db.query, conn, "SELECT * FROM things")
   trackerror(db.rollback, conn, {live = true})
   trackerror(db.rollback, conn)
   trackerror(db.query, conn, 'SELECT * FROM things')
   
   trackerror(db.execute, conn, "DROP TABLE things")
   
   res = trackerror(db.query, bogusDbConnectionTable({sql = sqlSelect}))   
   
   if iguana.isTest() and res and #res > 0 then
      Err[#Err+1] = "Not 'live' query returning a value"
   end   
   trackerror(db.query, bogusDbConnectionTable({sql = sqlSelect}))
   trackerror(db.execute, bogusDbConnectionTable({sql = 'DROP TABLE things'}))
   trackerror(db.execute, bogusDbConnectionTable({sql = sqlCreate}))
   trackerror(db.query, bogusDbConnectionTable({sql = 'SELECT * FROM things'}))
   trackerror(db.begin, bogusDbConnectionTable())
   trackerror(db.execute, bogusDbConnectionTable({sql = 'INSERT into things VALUES (37,' .. conn:quote(name1) .. ')'}))
   trackerror(db.commit, bogusDbConnectionTable())   
   trackerror(db.query, bogusDbConnectionTable({sql = 'SELECT * FROM things'}))
   trackerror(db.begin, bogusDbConnectionTable())
   trackerror(db.execute, bogusDbConnectionTable({sql = 'INSERT into things VALUES (38,' .. conn:quote(name2) .. ')'}))
   trackerror(db.query, bogusDbConnectionTable({sql = 'SELECT * FROM things'}))
   trackerror(db.rollback, bogusDbConnectionTable())
   trackerror(db.query, bogusDbConnectionTable({sql = 'SELECT * FROM things'}))
   trackerror(db.execute, bogusDbConnectionTable({sql = 'drop table things'}))
   
   if iguana.isTest() and #Err > 0 then
      error("Nothing should break when run in test mode, but something did.")
   elseif not iguana.isTest() and #Err == 0 then
      error("Something should break when run as a channel, but nothing did.")
   end  
end

local function testLiveDefault(Db)   
   local sqlCreate = sqlCreateBasic(Db, 'things')
   local sqlInsert = 'INSERT INTO things VALUES(123, \'foo\')'
   local sqlSelect = 'SELECT * FROM things'
   
   local success, res
   
   Db.live = nil
   local conn = db.connect(Db)
         
   -- Create the table
   pcall(db.execute, conn, {sql = 'DROP TABLE things', live = true})
   conn:execute{sql = sqlCreate, live=true}
   
   -- First test: real test - execute() should not be live.
   conn:execute(sqlInsert)
   res = conn:query{sql = sqlSelect, live = true}
   if iguana.isTest() and #res > 0 then
      error('default execute was run, or live query did not run')
   end
   
   -- Second test: now that we say it's live, it should be executed.
   -- query() should be live.
   conn:execute{sql = sqlInsert, live = true}
   res = conn:query(sqlSelect)
   if #res == 0 then
      error('live execute was not run, or default query did not run')
   end
   
   -- clean up
   conn:execute{sql = 'DELETE FROM things', live = true}
   
   -- Third test: begin and commit default behaviour.
   conn:begin{}
   conn:execute{sql = "INSERT into things VALUES (37,'test123')", live = true}
   conn:commit{}
   res = conn:query{sql = 'SELECT * FROM things'}
   
   if res[1].key_1:nodeValue() ~= '37' then
      error('value not committed')
   end      
   
   -- Fourth test: rollback default behaviour.
   conn:begin{} 
   conn:execute{sql = "INSERT into things VALUES (38,'test321')", live = true}      
   res = conn:query{sql = 'SELECT * FROM things'}
   if #res > 1 and res[2].key_1:nodeValue() ~= '38' then
      error('transaction not being executed')
   end
   conn:rollback{}      
   res = conn:query{sql = 'SELECT * FROM things'}      
   if iguana.isTest() and #res > 1 and res[2].key_1:nodeValue() == '38' then
      error('value not rolled back')
   end
   
   --clean up
   pcall(conn.execute, conn, {sql = 'DROP TABLE things', live = true})
   
   --test merge
   local spec = {vmd='basics.vmd',name='MK'}
   local send   = db.tables(spec)
   local expect = db.tables(spec).TK    
   
   pcall(db.execute, conn, {sql = 'DROP TABLE TK', live = true})
   
   conn:execute{sql = sqlCreateTK(Db), live = true}
   
   local row = send.TK[1]
   row.an_integer = 42
   row.a_double   = 3.1415
   row.a_string   = 'hello'
   row.a_date     = '1977-10-20 00:00:00'
   for i = 1, 2 do
      row.key_1, row.key_2 = 1, i         
      expect[i] = row
      if iguana.isTest() then conn:merge{data = send} end
   end       
   res = conn:query('select * from TK')
   if iguana.isTest() and #res > 0 then
      error('merge executed by default')
   end
   
   for i = 1, 2 do
      row.key_1, row.key_2 = 1, i         
      conn:merge{data = send, live = true}
   end
   res = conn:query('select * from TK')
   
   if Db.api == db.SQLITE then
      -- SQLite has no Date type.  So dates are serialized as ISO8601 strings.
      -- See 18383.
      expect[1].a_date = '1977-10-20T00:00:00'
      expect[2].a_date = '1977-10-20T00:00:00'
   end
   
   local ok, why = sameTable(res, expect)
   if not ok then error('live merge failed: ' .. why) end      
   
   conn:execute{sql = 'DROP TABLE TK', live = true}
   
   --test close      
   conn:close()
   success, res = pcall(db.query, conn, sqlSelect)      
   if iguana.isTest() and success then
      error('close() was not run')
   end   
end

local function testLive(Db)
   local sqlCreate = sqlCreateBasic(Db, 'things')
   local sqlInsert = 'INSERT INTO things VALUES(123, \'foo\')'
   local sqlSelect = 'SELECT * FROM things'
   
   local name1 = [[testing "foo"/'bar']]  
   local name2 = [[testing "bar"/'foo']]            
   
   Db.live = true
   local conn = db.connect(Db)
   
   local res
   
   -- Create the table
   pcall(db.execute, conn, {sql = 'DROP TABLE things', live = true})
   conn:execute{sql = sqlCreate, live=true}
   
   -- Assert non-live query is not run.
   res = conn:query{sql = sqlSelect, live = false}
   if iguana.isTest() and #res > 0 then
      error('not-live query being run')
   end
   
   -- Assert non-live execute is not run.
   conn:execute{sql = sqlInsert, live = false}
   res = conn:query{sql = sqlSelect}
   if #res > 0 then
      error('not-live execute being run')
   end            
   
   -- clean up
   conn:execute{sql = 'DELETE FROM things', live = true}
   
   conn:query{sql = 'SELECT * FROM things'} 
   conn:begin{}
   conn:execute{sql = 'INSERT into things VALUES (37,' .. conn:quote(name1) .. ')', live = true}
   conn:commit{}
   res = conn:query{sql = 'SELECT * FROM things'}
   
   if res[1].key_1:nodeValue() ~= '37' then
      error('value not committed')
   end
   testutils.assertEquals(res[1].name:nodeValue(), name1)
   
   conn:begin{}      
   conn:execute{sql = 'INSERT into things VALUES (38,' .. conn:quote(name2) .. ')', live = true}      
   conn:query{sql = 'SELECT * FROM things'}
   if iguana.isTest() then
      conn:commit{live = false}
      conn:begin{live = false}
   end
   conn:rollback{}
   res = conn:query{sql = 'SELECT * FROM things'}
   
   if #res > 1 and res[2].key_1:nodeValue() == '38' then
      error('value not rolled back')
   end
   
   conn:execute{sql = 'drop table things'}    
end

local function removeTestSqlite()
   -- Close implicitly cached connection.
   pcall(db.close, {api=db.SQLITE, name='test.sqlite'})
   -- Delete file.
   pcall(os.remove, 'test.sqlite')
end

local function testSingleParameterCalls()
   removeTestSqlite()
   
   local dbsettings = {api=db.SQLITE,name='test.sqlite',live=true}
   local conn = db.connect(dbsettings)
   
   conn:execute{sql='CREATE TABLE IF NOT EXISTS stupidtable(TestKey INTEGER)',live=true}

   local function ver(res)
      if #res ~= 1 then
         error('Expected result set with 1 row.  Instead got '..tostring(#res)..' rows.', 2)
      end 
   end
   
   local sqlstr = 'SELECT * FROM sqlite_master LIMIT 1;'
   
   ver(conn:query{sql=sqlstr})
   ver(conn:query(sqlstr))
   
   ver(conn:execute{sql=sqlstr,live=true})
   conn:execute(sqlstr)

   local function callWith(func,tbl)
      local params = {}
      for i,v in pairs(tbl) do
         params[i] = v
      end
      for i,v in pairs(dbsettings) do
         params[i] = v
      end
      return func(params)      
   end

   ver(callWith(db.query,{sql=sqlstr}))
   ver(callWith(db.execute,{sql=sqlstr}))
   
   --db merge functionality

   --create the tables
   local createsql = [[CREATE TABLE Patient 
      (PatientId TEXT(255) NOT NULL ,
       FirstName TEXT(255),
       LastName TEXT(255),
       DOB TEXT,
       SSN TEXT(255),
       Discharged TEXT(255),
       PRIMARY KEY (PatientId));]]
   conn:execute{sql=createsql,live=true}
   
   local msg = [[MSH|^~\&|AcmeMed|Lab|Main HIS|St. Micheals|20110213144932||ADT^A04|9B38584D9903051F0D2B52CC0148965775D2D23FE4C51BE060B33B6ED27DA820|P|2.6|\r
EVN||20110213144532||||20110213145902|\r
PID|||4525285^^^ADT1||Smith^Tracy||19980210|F||Martian|86 Yonge St.^^ST. LOUIS^MO^51460|||||||10-346-6|284-517-569|\r
NK1|1|Smith^Gary|Second Cousin|\r
PV1||E||||||5101^Garland^Mary^F^^DR|||||||||||1318095^^^ADT1|||||||||||||||||||||||||20110213144956|\r
OBX|||WT^WEIGHT||102|pounds|\r
OBX|||HT^HEIGHT||32|cm|\r\n]]
   
   local parsed,msg_name = hl7.parse{vmd='hl7.vmd',data=msg}
   local tbl = db.tables{vmd='hl7.vmd',name=msg_name}
   
   tbl.Patient[1].PatientId = parsed.PID[3][1][1]
   tbl.Patient[1].FirstName = parsed.PID[5][1][2]
   tbl.Patient[1].LastName = parsed.PID[5][1][1][1]
   tbl.Patient[1].SSN = parsed.PID[19]
   
   conn:merge{data=tbl,live=true}
   conn:merge(tbl)   
end

local function testLegacyLive()
   if not iguana.isTest() then return end
   
   removeTestSqlite()
   
   local dbsettings = {api=db.SQLITE,name='test.sqlite'}
   
   local function ver(num, res)
      if #res ~= num then
         error('Expected result set with ' .. tostring(num) or 1 .. ' row.  Instead got '..tostring(#res)..' rows.', 2)
      end 
   end
   
   --db.execute live defaults  
   pcall(db.execute, addParams(dbsettings, {sql='DROP TABLE stupidtable', live = true}))   
   
   local createsql = 'CREATE TABLE stupidtable(TestKey INTEGER)'
   local checksql = 'SELECT * FROM sqlite_master where name = "stupidtable"'
   
   db.execute(addParams(dbsettings, {sql=createsql, live = false})) --should not be live
   ver(0, db.query(addParams(dbsettings, {sql=checksql})))
   
   db.execute(addParams(dbsettings, {sql=createsql})) --should not be live
   ver(0, db.query(addParams(dbsettings, {sql=checksql})))
   
   db.execute(addParams(dbsettings, {sql=createsql, live = true})) --should be live
   ver(1, db.query(addParams(dbsettings, {sql=checksql})))
   
   
   --db.merge live defaults

   --create the tables
   local createsql = [[CREATE TABLE Patient 
      (PatientId TEXT(255) NOT NULL ,
       FirstName TEXT(255),
       LastName TEXT(255),
       DOB TEXT,
       SSN TEXT(255),
       Discharged TEXT(255),
       PRIMARY KEY (PatientId));]]
   pcall(db.execute, addParams(dbsettings, {sql='DROP TABLE Patient', live = true}))
   db.execute(addParams(dbsettings, {sql=createsql,live=true}))
   
   local msg = [[MSH|^~\&|AcmeMed|Lab|Main HIS|St. Micheals|20110213144932||ADT^A04|9B38584D9903051F0D2B52CC0148965775D2D23FE4C51BE060B33B6ED27DA820|P|2.6|\r
EVN||20110213144532||||20110213145902|\r
PID|||4525285^^^ADT1||Smith^Tracy||19980210|F||Martian|86 Yonge St.^^ST. LOUIS^MO^51460|||||||10-346-6|284-517-569|\r
NK1|1|Smith^Gary|Second Cousin|\r
PV1||E||||||5101^Garland^Mary^F^^DR|||||||||||1318095^^^ADT1|||||||||||||||||||||||||20110213144956|\r
OBX|||WT^WEIGHT||102|pounds|\r
OBX|||HT^HEIGHT||32|cm|\r\n]]
   
   local parsed,msg_name = hl7.parse{vmd='hl7.vmd',data=msg}
   local tbl = db.tables{vmd='hl7.vmd',name=msg_name}
     
   tbl.Patient[1].PatientId = parsed.PID[3][1][1]   
   tbl.Patient[1].FirstName = parsed.PID[5][1][2]
   tbl.Patient[1].LastName = parsed.PID[5][1][1][1]
   tbl.Patient[1].SSN = parsed.PID[19]
   
   checksql = 'SELECT * FROM Patient'      
   
   trace(checksql)
   
   db.merge(addParams(dbsettings, {data=tbl, live=false}))  --should not be live
   ver(0, db.query(addParams(dbsettings, {sql=checksql})))
   
   db.merge(addParams(dbsettings, {data=tbl}))  --should not be live
   ver(0, db.query(addParams(dbsettings, {sql=checksql})))
   
   db.merge(addParams(dbsettings, {data=tbl, live=true})) --should be live
   ver(1, db.query(addParams(dbsettings, {sql=checksql})))   
end

local function testCustomMerge(Db)   
   require('custom_merge')
      
   local conn = db.connect(addParams(Db, {live = true}))      
   
    --test merge
   local spec = {vmd='basics.vmd',name='MK'}
   local send   = db.tables(spec)
   local expect = db.tables(spec).TK    
   
   pcall(db.execute, conn, {sql = 'DROP TABLE TK', live = true})
   
   conn:execute{sql = sqlCreateTK(Db), live = true}
   
   local row = send.TK[1]
   row.an_integer = 42
   row.a_double   = 3.1415
   row.a_string   = 'hello'
   row.a_date     = '1977-10-20 00:00:00'   
   
   for i = 1, 2 do
      row.key_1, row.key_2 = 1, i         
      expect[i] = row
      if iguana.isTest() then conn:customMerge{data = send} end
   end
   for i = 1, 2 do
      row.key_1, row.key_2 = 1, i         
      conn:customMerge{data = send, live = true}
   end
   res = conn:query('select * from TK')      
   local ok, why = sameTable(res, expect)
   if not ok then error('live merge failed: ' .. why) end      
   
   conn:execute{sql = 'DROP TABLE TK', live = true}
   
   --test close      
   conn:close()
end

---------------------
-- Test Generation --
---------------------

local platform = db_test_config.platform()
local databases = {
   ['MySQL'] = db_test_config.mySql(),
   ['Oracle OCI'] = db_test_config.oracleOci(),
   ['SQLite'] = db_test_config.sqlite()
}
if platform.os:find('win') then
   databases['SQL Server'] = db_test_config.sqlServer()
   databases['Oracle ODBC'] = db_test_config.oracleOdbc()
   databases['MySQL ODBC'] = db_test_config.mysqlOdbc()
   if not platform.x64 then
      databases['Sybase ASA'] = db_test_config.sybaseAsa()
      databases['Sybase ASE'] = db_test_config.sybaseAse()
      databases['Access'] = db_test_config.access()
   end
end
local function makeTests(Db)
   Db = copy(Db)
   local DbTests = {
      stuff       = function() testStuff(Db) end, --simple operations
      testnotlive = function() testNotLive(Db) end, --testing operations that shouldn't be live
      testlivedefault = function() testLiveDefault(Db) end, --testing operation of db connection with live = nil
      testlive    = function() testLive(Db) end, --testing a live connection with assorted live/not-live operations
   }
   if Db.api ~= db.ACCESS and Db.api ~= db.SYBASE_ASA and Db.api ~= db.SYBASE_ASE then
      -- db.customMerge() doesn't support Access or Sybase (at this point).
      DbTests.testCustomMerge = function() testCustomMerge(Db) end --basic test of merge functionality
   end
   return DbTests
end
local tests = {}
for k,Db in pairs(databases) do
   tests[k] = makeTests(Db)      
end
tests.testSingleParameterCalls = testSingleParameterCalls
tests.testLegacyLive = testLegacyLive

function main()
   -- If in the Translator IDE, call tests you want to run in here.
end

return tests
