require 'db_test_config'

iguana.isTest = function() return true end -- iguana.isTest does not exist as part of core NODL

local DbConnIndex = getmetatable(db)._submeta.db_connection.__index

local cxx_conn_merge = DbConnIndex.merge
local cxx_merge = db.merge
local OnlyImplicitConnections
require 'db_merge' -- Lua implementation
assert(db.merge ~= cxx_merge)
local lua_merge = db.merge
db.merge = cxx_merge

require 'custom_merge' -- new Lua implementation

-- Temporary!
local line = 0
local function showSql(f)
   return function(a1, a2, ...)
      local args = a1
      if a2 ~= nil then
         args = a2
      end
      local s = args
      if type(s) == 'table' then
         s = args.sql
      end
      if s then
         line = line + 1
         s = s:gsub('\n$', '')
         s = s:gsub('\n', '\n'..line..': ')
         io.stderr:write(line, ': ', s, '\n')
      end
      return f(a1, a2, ...)
   end
end

if iguana.isTest() then
   db.query   = showSql(db.query)
   db.execute = showSql(db.execute)
   if getmetatable(db) then
      DbConnIndex.query = showSql(DbConnIndex.query)
      DbConnIndex.execute = showSql(DbConnIndex.execute)
   end
end

local function traceTest(Arg)
   io.stderr:write('\n', Arg, '\n')
end

---------------
-- Utilities --
---------------

local function mapRange(from, to, f)
   local out = {}
   for i = from, to do
      local v = f(i)
      table.insert(out, v)
   end
   return out
end

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

local function upcall(up, f, ...)
   local out = { pcall(f, ...) }
   if not out[1] then
      error(out[2], up+1)
   end
   return unpack(out, 2)
end

-- Return all values of an iteration.
local function all(f,s,v)
   local out = {}
   while true do
      v = f(s,v)
      if v == nil then break end
      out[#out+1] = v
   end
   return out
end

-- Enumerate the values of an iteration.
local function enumerate(f,s,v)
   local i = 1
   return function()
      if not i then return end
      local vs = {f(s,v)}; v = vs[1]
      if v == nil then i = nil; return end
      local idx = i; i = i+1
      return idx, unpack(vs)
   end
end

function tokenize(s, pattern, plain)
   local i = 1
   if not plain then pattern = '('..pattern..')' end
   return function()
      if not i then return end
      local j,k,out = s:find(pattern,i,plain)
      if j and j <= k then  out,i = s:sub(i,j-1),  k+1
      else                  out,i = s:sub(i),      nil
      end
      return out
   end
end

---------------------
-- All-Types Tests --
---------------------

-- Split on whitespace, but allow quoted phrases.
local function split(text)
   local words = all(tokenize(text, ' +'))
   local out, last = {}
   for _,s in ipairs(words) do
      last = last and last..' '..s or s
      if last:match('^[^\'"]') or last:match('^(.).*%1$') then
         table.insert(out, last)
         last = nil
      end
   end
   assert(not last, 'Incomplete: '..(last or ''))
   return out
end

local function unquote(s)
   local out = s:gsub('^([\'"])(.*)%1$', '%2')
   return out
end

-- For parsing the data types and sample data.
local function parse(s)
   local out = {}
   for _,line in ipairs(s) do
      local parts = split(line)
      local name  = unquote(parts[1])
      local base  = assert(parts[2], 'No base type: '..line)
      local insert, merge, expect = {}, {}, {}
      local i = 3; while i <= #parts do
         local a,b,c,d = unpack(parts, i, i+3)
         if not b or b == '/' then
            b = a;  c = b;  i = i+2
         elseif not c or c == '/' then
            c = b;  i = i+3
         else
            assert(not d or d == '/', line)
            i = i+4
         end
         table.insert(insert, a)
         table.insert(merge,  unquote(b))
         table.insert(expect, unquote(c))
      end
      assert(#insert > 0, 'No samples: '..line)
      local tbl = 't_'..(base == 'binary' and 'string' or base)
      local msg = tbl:gsub('^t_', 'm_')
      table.insert(out, {
            name    = name,    -- type name
            base    = base,    -- internal type
            table   = tbl,     -- the table to merge with
            message = msg,     -- the message to merge
            insert  = insert,  -- samples for insert
            merge   = merge,   -- samples for merge
            expect  = expect,  -- data to select
         })
   end
   return out
end

local ascii = ('.'):rep(128):gsub('().',
   function(i) return string.char(i-1) end)

local function standardQuote(s)
   assert(not s:find('%z'), "can't quote NULs")
   s = s:gsub("'", "''")
   return table.concat{"'", s, "'"}
end

local function mySqlQuote(s)
   s = s:gsub("[%z'\\]", {
      ['\0'] = '\\0',
      ["'"]  = "\\'",
      ['\\'] = '\\\\'
   })
   return table.concat{"'", s, "'"}
end

local function mySqlOdbcQuote(s)
   s = s:gsub("[%z'\\]", {
      ['\0'] = '\\0',
      ["'"]  = "\\'",
      ['\\'] = '\\\\\\\\'
   })
   return table.concat{"'", s, "'"}
end

local quote = {
   [db.MY_SQL]      = mySqlQuote,
   [db.MY_SQL_ODBC] = mySqlOdbcQuote,
   [db.SQL_SERVER]  = standardQuote,
   [db.ORACLE_ODBC] = standardQuote,
   [db.ORACLE_OCI]  = standardQuote,
}

local function sample(db,s)
   -- The test parser is a bit particular...
   assert(not s:find('\' '))
   assert(not s:find('  +'))
   local insert = quote[db](s)
   local merge  = table.concat{"'", s, "'"}
   return insert..' '..merge
end

-- dbtype basetype insert merge expect ('/' insert merge expect)*
local data_types = {
   [db.MY_SQL] = parse {
      "tinyint       integer NULL / -1 / 0 / 1",
      "boolean       integer NULL / TRUE 1 / FALSE 0",
      "smallint      integer NULL / -1 / 0 / 1",
      "mediumint     integer NULL / -1 / 0 / 1",
      "int           integer NULL / -1 / 0 / 1",
      "bigint        integer NULL / -1 / 0 / 1",
      "decimal       string NULL / -1 / 0 / 1 / 0.01 0",
      "decimal(10,2) string NULL / -0.01 / 0.00 / 0.01",
      "float         double NULL / -0.01 -0.010000 / 0.0 0.000000 / 0.01 0.010000",
      "double        double NULL / -0.01 -0.010000 / 0.0 0.000000 / 0.01 0.010000",
      "date          datetime NULL / '1977-10-20' '1977-10-20 00:00:00'",
      "datetime      datetime NULL / '1977-10-20 21:30:08'",
      "'timestamp null' datetime NULL / '1977-10-20 21:30:08'",
      "time          string NULL / '00:00:00' / '21:30:08'",
      "year(4)       integer NULL / 1977",
      "char(63)      string NULL / '' / 'a'",
      "varchar(63)   string NULL / '' / 'a'",
      "nchar(63)     string NULL / '' / 'a'",
      "nvarchar(63)  string NULL / '' / 'a'",
      "tinytext      string NULL / '' / 'a'",
      "mediumtext    string NULL / '' / 'a'",
      "longtext      string NULL / '' / 'a'",
      "text          string NULL / '' / 'a' / "..sample(db.MY_SQL, ascii:sub(2)),
      "varbinary(2)  binary NULL / '' / 'a' / '\\0a' '\0a'",
      "binary(2)     binary NULL / '' '\0\0' / 'a' 'a\0' / '\\0a' '\0a'",
      "tinyblob      binary NULL / '\\0' '\0' / 'a\\0' 'a\0'",
      "mediumblob    binary NULL / '\\0' '\0' / 'a\\0' 'a\0'",
      "longblob      binary NULL / '\\0' '\0' / 'a\\0' 'a\0'",
      "blob          binary NULL / '\\0' '\0' / 'a\\0' 'a\0' / "..sample(db.MY_SQL, ascii),
      "bit(16)       binary NULL / 0 '\000\000' / 1 '\000\001' / 256 '\001\000'",
      "enum('a','b') string NULL / 'a'", --Cannot use '' as a test case, it is reserved for enums as the 0th index
      "set('a','b')  string NULL / '' / 'a'",
   };
   [db.MY_SQL_ODBC] = parse {
      "tinyint       integer NULL / -1 / 0 / 1",
      "boolean       integer NULL / TRUE 1 / FALSE 0",
      "smallint      integer NULL / -1 / 0 / 1",
      "mediumint     integer NULL / -1 / 0 / 1",
      "int           integer NULL / -1 / 0 / 1",
      "bigint        integer NULL / -1 / 0 / 1",
      "decimal       integer NULL / -1 / 0 / 1 / 0.01 0",
      "decimal(10,2) double NULL / -0.01 -0.010000 / 0.00 0.000000 / 0.01 0.010000",
      "float         double NULL / -0.01 -0.010000 / 0.0 0.000000 / 0.01 0.010000",
      "double        double NULL / -0.01 -0.010000 / 0.0 0.000000 / 0.01 0.010000",
      "date          datetime NULL / '1977-10-20' '1977-10-20 00:00:00'",
      "datetime      datetime NULL / '1977-10-20 21:30:08'",
      "'timestamp null' datetime NULL / '1977-10-20 21:30:08'",
      "year(4)       integer NULL / 1977",
      "char(63)      string NULL / '' / 'a'",
      "varchar(63)   string NULL / '' / 'a'",
      "nchar(63)     string NULL / '' / 'a'",
      "nvarchar(63)  string NULL / '' / 'a'",
      "tinytext      string NULL / '' / 'a'",
      "mediumtext    string NULL / '' / 'a'",
      "longtext      string NULL / '' / 'a'",
    --[[ "text          string NULL / '' / 'a' / "..sample(db.MY_SQL_ODBC, ascii:sub(2)),
      "varbinary(2)  binary NULL / '' / 'a' / '\\0a' '\0a'",
      "binary(2)     binary NULL / '' '\0\0' / 'a' 'a\0' / '\\0a' '\0a'",
      "tinyblob      binary NULL / '\\0' '\0' / 'a\\0' 'a\0'",
      "mediumblob    binary NULL / '\\0' '\0' / 'a\\0' 'a\0'",
      "longblob      binary NULL / '\\0' '\0' / 'a\\0' 'a\0'",
      "blob          binary NULL / '\\0' '\0' / 'a\\0' 'a\0' / "..sample(db.MY_SQL_ODBC, ascii),
      "bit(16)       binary NULL / 0 '\000\000' / 1 '\000\001' / 256 '\001\000'",]]--
      "enum('a','b') string NULL / 'a'",  --Cannot use '' as a test case, it is reserved for enums as the 0th index
      "set('a','b')  string NULL / '' / 'a'",
   };
   [db.SQL_SERVER] = parse {
      "bit           integer NULL / 0 / 1",
      "tinyint       integer NULL / 0 / 1 / 255",   -- They are unsigned.
      "smallint      integer NULL / -32768 / -1 / 0 / 1 / 32767",
      "int           integer NULL / -2147483648 / -1 / 0 / 1 / 2147483647",
      "bigint        integer NULL / -1 / 0 / 1",  -- Full range not supported.
      "numeric(16,2) double NULL / -0.01 -0.010000 / 0.00 0.000000 / 0.01 0.010000",
      "decimal(16,2) double NULL / -0.01 -0.010000 / 0.00 0.000000 / 0.01 0.010000",
      "smallmoney    double NULL / -0.01 -0.010000 / 0.00 0.000000 / 0.01 0.010000",
      "money         double NULL / -0.01 -0.010000 / 0.00 0.000000 / 0.01 0.010000",
      "float         double NULL / -0.01 -0.010000 / 0.00 0.000000 / 0.01 0.010000",
      "real          double NULL / -0.01 -0.010000 / 0.00 0.000000 / 0.01 0.010000",
      "datetime      datetime NULL / '1977-10-20 21:30:08'",
      "smalldatetime datetime NULL / '1977-10-20 21:30:00'",
      "char(2)       string NULL / 'a' 'a '",
      "nchar(2)      string NULL / 'a' 'a '",
      "text          string NULL / '' / 'a' / 'ab' / "..sample(db.SQL_SERVER, ascii:sub(2)),
      "ntext         string NULL / '' / 'a' / 'ab' / "..sample(db.SQL_SERVER, ascii:sub(2)),
      "varchar       string NULL / '' / 'a'",
      "nvarchar      string NULL / '' / 'a'",
      "binary(2)     binary NULL / 0x61 'a\0' / 0x0062 '\0b' / 0x6300 'c\0'",
      "varbinary(2)  binary NULL / 0x61 'a'   / 0x0062 '\0b' / 0x6300 'c\0'",
      "image         binary NULL / 0x61 'a'   / 0x0062 '\0b' / 0x6300 'c\0'",
      "uniqueidentifier string NULL / CONVERT(uniqueidentifier,'6F9619FF-8B86-D011-B42D-00C04FC964FF') '6F9619FF-8B86-D011-B42D-00C04FC964FF'",
      --timestamp   string DEFAULT  -- These are always random.
      --sql_variant string DEFAULT  -- We cannot support mixed-type columns.
      --
      -- * Not Available on Our Server *
      --date, datetime2, datetimeoffset, hierarchyid, time, xml
   };
   [db.ORACLE_ODBC] = parse {
      "number(*,0)   integer NULL / -1 / 0 / 1",
      "number        double NULL / -0.01 -0.010000 / 0.00 0.000000 / 0.01 0.010000",
      "number(*,2)   double NULL / -0.01 -0.010000 / 0.00 0.000000 / 0.01 0.010000",
      "binary_float  double NULL / -0.01 -0.010000 / 0.00 0.000000 / 0.01 0.010000",
      "binary_double double NULL / -0.01 -0.010000 / 0.00 0.000000 / 0.01 0.010000",
      "date          datetime NULL / '20-OCT-1977'            '1977-10-20 00:00:00'",
      "timestamp     datetime NULL / '20-OCT-1977 9:30:08 PM' '1977-10-20 21:30:08'",
      -- ODBC reports these as SQL_UNKNOWN_TYPE
      --"'timestamp with time zone'       datetime '20-OCT-1977 9:30:00 PM -2:00' '1977-10-20 21:30:00'",
      --"'timestamp with local time zone' datetime '20-OCT-1977 9:30:00 PM'       '1977-10-20 21:30:00'",
      --"'interval day to second' string NUMTODSINTERVAL(1,'SECOND') '+00 00:00:01.000000'",
      --"'interval year to month' string NUMTOYMINTERVAL(1,'MONTH')  '+00-01'",
      "char          string NULL / 'a'",
      "nchar         string NULL / 'a'",
      "varchar2(63)  string NULL / 'a' / 'ab'",
      "varchar(63)   string NULL / 'a' / 'ab'",
      "nvarchar2(63) string NULL / 'a' / 'ab'",
      "long          string NULL / 'a' / 'ab'",
      "clob          string NULL / 'a' / 'ab' / "..sample(db.ORACLE_ODBC, ascii:sub(2)),
      "nclob         string NULL / 'a' / 'ab' / "..sample(db.ORACLE_ODBC, ascii:sub(2)),
      "blob          binary NULL / HEXTORAW('61') 'a' / HEXTORAW('0062') '\0b' / HEXTORAW('6300') 'c\0'",
      "raw(63)       binary NULL / HEXTORAW('61') 'a' / HEXTORAW('0062') '\0b' / HEXTORAW('6300') 'c\0'",
      "'long raw'    binary NULL / HEXTORAW('61') 'a' / HEXTORAW('0062') '\0b' / HEXTORAW('6300') 'c\0'",
      --"bfile         binary ...",  -- These are read-only.
      "rowid         string NULL / 'AAAA8mAALAAAAQkAAA'",
      "urowid        string NULL / 'AAAA8mAALAAAAQkAAA'",
      -- ODBC reports these as SQL_UNKNOWN_TYPE
      --"XmlType       string '<xml>hello world</xml>'",
      --"UriType       string HttpUriType.createURI('http://www.postgresql.org/') 'http://www.postgresql.org/'",
   };
   [db.ORACLE_OCI] = parse {
      "integer       integer NULL / -1 / 0 / 1",
      "number(*,0)   integer NULL / -1 / 0 / 1",
      "number        double NULL / -0.01 -0.010000 / 0.00 0.000000 / 0.01 0.010000",
      "number(*,2)   double NULL / -0.01 -0.010000 / 0.00 0.000000 / 0.01 0.010000",
      "binary_float  double NULL / -0.01 -0.010000 / 0.00 0.000000 / 0.01 0.010000",
      "binary_double double NULL / -0.01 -0.010000 / 0.00 0.000000 / 0.01 0.010000",
      "date          datetime NULL / '20-OCT-1977'            '1977-10-20 00:00:00'",
      "timestamp     datetime NULL / '20-OCT-1977 9:30:08 PM' '1977-10-20 21:30:08'",
      -- We strip timezone information, unfortunately.
      "'timestamp with time zone'       datetime NULL / '20-OCT-1977 9:30:08 PM -2:00' '1977-10-20 21:30:08'",
      "'timestamp with local time zone' datetime NULL / '20-OCT-1977 9:30:08 PM'       '1977-10-20 21:30:08'",
      "'interval day to second' string NULL / NUMTODSINTERVAL(1,'SECOND') '+00 00:00:01.000000'",
      "'interval year to month' string NULL / NUMTOYMINTERVAL(1,'MONTH')  '+00-01'",
      "char          string NULL / 'a'",
      "nchar         string NULL / 'a'",
      "varchar2(63)  string NULL / 'a' / 'ab'",
      "varchar(63)   string NULL / 'a' / 'ab'",
      "nvarchar2(63) string NULL / 'a' / 'ab'",
      "long          string NULL / 'a' / 'ab'",
      "clob          string NULL / 'a' / 'ab' / "..sample(db.ORACLE_OCI, ascii:sub(2)),
      "nclob         string NULL / 'a' / 'ab' / "..sample(db.ORACLE_OCI, ascii:sub(2)),
      "blob          binary NULL / HEXTORAW('61') '61' 'a' / HEXTORAW('0062') '0062' '\0b' / HEXTORAW('6300') '6300' 'c\0'",
      "raw(2)        binary NULL / HEXTORAW('61') '61' 'a' / HEXTORAW('0062') '0062' '\0b' / HEXTORAW('6300') '6300' 'c\0'",
      "'long raw'    binary NULL / HEXTORAW('61') '61' 'a' / HEXTORAW('0062') '0062' '\0b' / HEXTORAW('6300') '6300' 'c\0'",
      --"bfile         binary ...",  -- These are read-only.
      "rowid         string NULL / 'AAAA8mAALAAAAQkAAA'",
      "urowid        string NULL / 'AAAA8mAALAAAAQkAAA'",
      -- This is a special named data type (SQLT_NTY).
      --"XmlType       string '<xml>hello world</xml>'",
      -- CAUTION: THIS CRASHES IGUANA!
      --"UriType       string HttpUriType.createURI('http://www.postgresql.org/') 'http://www.postgresql.org/'",
   };
}

local function oracle(api)
   return api == db.ORACLE_OCI or api == db.ORACLE_ODBC
end

local function failuresTable()
   local failures = {}
   setmetatable(failures,
      {
         __tostring = function(self)
            local AllMessages = tostring(#self)..' error(s) encountered:\n'
            for i,E in ipairs(self) do
               AllMessages = AllMessages..tostring(i)..': '..tostring(E.e)..'\n'
            end
            return AllMessages
         end
      }
   )
   return failures
end

local methods = {
   query = function(self, sql)
      if self.conn then
         return upcall(2, self.conn.execute, self.conn, {['sql']=sql, live=true})
      else
         self.sql  = sql
         self.live = true
         return upcall(2, db.execute, self)
      end
   end;
   withRollback = function(self, f)
      return unwind_protect(
         function()
            if self.conn then
               self.conn:begin()
            else
               self.sql = nil
               self.live = true
               db.begin(self)
            end
            return f()
         end,
         function()
            -- We never commit.
            if self.conn then
               self.conn:rollback()
            else
               self.sql, self.merge_null = nil
               db.rollback(self)
            end
         end)
   end;
   table = function(self, t)
      return (self.api == db.SQL_SERVER and '#' or '')..t.table
   end;
   message = function(self, t)
      return (self.api == db.SQL_SERVER and 'h' or '')..t.message
   end;
   create = function(self, t)
      local sql = {
         [db.MY_SQL]      = 'create temporary table %s (x %s)',
         [db.MY_SQL_ODBC] = 'create temporary table %s (x %s)',
         [db.ORACLE_ODBC] = 'create global temporary table %s (x %s) on commit preserve rows',
         [db.ORACLE_OCI]  = 'create global temporary table %s (x %s) on commit preserve rows',
         [db.SQL_SERVER]  = 'create table %s (x %s)',
      }
      local q = sql[self.api]:format(self:table(t), t.name)
      return self:query(q)
   end;
   drop = function(self, t)
      local tab = self:table(t)
      if oracle(self.api) then
         pcall(self.query, self, 'truncate table '..tab)
      end
      return self:query('drop table '..tab)
   end;
   insert = function(self, t)
      local tab = self:table(t)
      local sql = 'insert into '..tab..' values (%s)'
      for _,v in ipairs(t.insert) do
         local q = sql:format(v)
         self:query(q)
      end
   end;
   select = function(self, t)
      local sql = 'select * from '..self:table(t)
      local out, failures = self:query(sql), failuresTable()
      for i,v in ipairs(t.expect) do
         local x = tostring(out[i].x)
         local b = out[i].x:nodeType()
         if b ~= t.base then
            local err = 'got %s (%q) not %s for %s'
            err = err:format(b,x,t.base,t.name)
            table.insert(failures, err)
         elseif x ~= v then
            local err = 'got %q not %q (%s)'
            err = err:format(x,v,t.name)
            table.insert(failures, err)
         elseif v == 'NULL' and not out[i].x:isNull() then
            local err = 'expected isNull(), got value %q'
            err = err:format(x)
            table.insert(failures, err)
         end
      end
      if #failures > 0 then
         failures = table.concat(failures, '; ')
         error(failures, 2)
      end
      return out
   end;
   mergeData = function(self, data)
      if self.conn then
         return self.conn:merge {
            data = data, live = true,
            merge_null  = self.merge_null,
            transaction = false,
         }
      else
         return db.merge {
            api  = self.api,  user     = self.user,
            name = self.name, password = self.password,
            data = data,      live     = true,
            use_unicode = self.use_unicode,
            merge_null  = self.merge_null,
            transaction = false,
         }
      end
   end;
   merge = function(self, t)
      local data = db.tables {
         vmd  = 'alltypes.vmd',
         name = self:message(t)
      }
      local tbl = data[self:table(t)]
      for i,v in ipairs(t.merge) do
         if v ~= 'NULL' then  tbl[i].x = v
         else                 tbl[i].x = nil
         end
      end
      return self:mergeData(data)
   end;
}

local function connect(info)
   local out = copy(info)
   if not OnlyImplicitConnections then
      out.conn = db.connect(out)
   end
   return setmetatable(out, {__index=methods})
end

local function testInsert(Db, postConnect)
   Db = connect(Db)
   if postConnect then Db = postConnect(Db) end
   local failures = failuresTable()
   for _,t in ipairs(data_types[Db.api]) do
      pcall(Db.drop, Db, t)
      Db:create(t)
      Db:insert(t)
      local ok,err = pcall(Db.select, Db, t)
      if not ok then
         table.insert(failures, {t=t, e=err})
      end
      Db:drop(t)
   end
   if #failures > 0 then
      error(failures)
   end
end

--[[
local function testDbexec(Db)
   local conn
   local out = testInsert(Db, function(Db)
         Db.live    = true
         Db.timeout = 60
         conn = dbexec.connect(Db)
         Db.query = function(_, sql)
            assert(_ == Db)
            return conn:query{sql=sql}
         end
         return Db
      end)
   if conn then conn:close() end
   return out
end
--]]

local function testMergeTypes(Db, test)
   Db = connect(Db)
   test = test or function(t) return true end
   local failures = failuresTable()
   for _,t in ipairs(data_types[Db.api]) do
      if test(t) then
         pcall(Db.drop, Db, t)
         Db:create(t)
         local ok, err = pcall(function()
               Db:merge(t); Db:select(t)
            end)
         if not ok then
            table.insert(failures, {t=t, e=err})
         end
         Db:drop(t)
      end
   end
   if Db.conn then
      Db.conn:close()
   end
   if #failures > 0 then
      error(failures)
   end
end

local function withLuaMerge(f, ...)
   local args = {...}
   return unwind_protect(
      function()
         OnlyImplicitConnections = true
         db.merge = lua_merge
         return f(unpack(args))
      end,
      function()
         OnlyImplicitConnections = nil
         db.merge = cxx_merge
      end)
end

local function withCustomMerge(f, ...)
   local args = {...}
   return unwind_protect(
      function()
         DbConnIndex.merge = DbConnIndex.customMerge
         return f(unpack(args))
      end,
      function()
         DbConnIndex.merge = cxx_conn_merge
      end)
end
   
local is_unix = os.fs.abspath('.'):sub(1,1) == '/'
local function testCxxMergeTypes(Db)
   assert(db.merge == cxx_merge)
   local Filter
   if not is_unix or Db.api ~= db.SQL_SERVER then
      -- Run all tests unless on Unix with SQL Server (FreeTDS).
   elseif Db.use_unicode then
      -- In Unicode-mode, FreeTDS can handle everything but binary.
      Filter = function(t) return t.base ~= 'binary' end
   else
      -- The C++ db.merge() won't work with FreeTDS without Unicode.
      Filter = function(t) return false end
   end
   return testMergeTypes(Db, Filter)
end

local function testLuaMergeTypes(Db)
   local Filter
   if Db.api ~= db.MY_SQL and Db.api ~= db.MY_SQL_ODBC then
      -- Skip binary tests outside MySQL; they don't work.
      Filter = function(t) return t.base ~= 'binary' end
   end
   return withLuaMerge(testMergeTypes, Db, Filter)
end

local function testCustomMergeTypes(Db)
   local Filter
   if Db.api ~= db.MY_SQL and Db.api ~= db.MY_SQL_ODBC then
      -- Skip binary tests outside MySQL; they don't work.
      Filter = function(t) return t.base ~= 'binary' end
   end
   return withCustomMerge(testMergeTypes, Db, Filter)
end

--------------------------
-- Basic db.merge Tests --
--------------------------

-- Integer division a/b with remainder.
local function div(a,b)
   return math.floor(a/b), a%b
end

-- Execute f(out) with all combinations of
-- 'data' entries placed into 'out'.
-- Steps through data, tries combination with and without current key
local function withAllCombinations(out, data, f, prevkey) 
   local k,v = next(data, prevkey)  
   if not k then
      f(out)      
   else      
      out[k] = v
      withAllCombinations(out, data, f, k)
      out[k] = nil
      withAllCombinations(out, data, f, k)    
   end
end

local function dbTypeFor(Db)
   local map = {
      integer  = 'integer',
      double   = 'decimal(16,6)',
      string   = 'varchar(63)',
      datetime = 'datetime',
   }
   if oracle(Db.api) then
      map.datetime = 'date'
   end
   return function(c)
      local k = c:nodeType()
      return assert(map[k])
   end
end

local function dropTable(Db, t)
   assert(t:nodeType() == 'table')
   local name = t:nodeName()
   local sql = 'drop table '..name
   return upcall(2, Db.query, Db, sql)
end

local function createTable(Db, t)
   assert(t:nodeType() == 'table')
   local typeFor = dbTypeFor(Db)
   local name, row = t:nodeName(), t[1]
   local keys = {}
   local sql = table.concat {
      'create table ', name, '(\n\t',
      table.concat(
         mapRange(1, row:childCount(), 
            function(i)
               local c = row[i]
               local k = c:nodeName()
               local out = k..' '..typeFor(c)
               if c:isKey() then
                  out = out..' not null'
                  table.insert(keys, k)
               end
               return out
            end),
         ',\n\t'),
      (#keys > 0 and table.concat {
            ',\n\tprimary key (',
            table.concat(keys, ','),
            ')'
         } or ''),
      ')'
   }
   return upcall(2, Db.query, Db, sql)
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

local function testMergeInsert(Db)
   Db = connect(Db)
   return Db:withRollback(
      function()
         local spec = {vmd='basics.vmd', name='M'}
         local send   = db.tables(spec)
         local expect = db.tables(spec).T
         pcall(dropTable, Db, expect)
         createTable(Db, expect)
         local n = 0
         withAllCombinations(send.T[1], {
               an_integer = 42,
               a_double   = 3.1415,
               a_string   = 'hello',
               a_date     = '1977-10-20 00:00:00',
            },
            function(row)
               Db:mergeData(send)
               n = n + 1  -- not #expect+1
               expect[n] = row
            end)
         local got = Db:query('select * from T')
         local ok, why = sameTable(got, expect)
         if not ok then return why end
      end)
end

local function testMergeUpdate(Db)
   Db = connect(Db)
   return Db:withRollback(
      function()
         local spec = {vmd='basics.vmd',name='MK'}
         local send   = db.tables(spec)
         local expect = db.tables(spec).TK
         pcall(dropTable, Db, expect)
         createTable(Db, expect)
         local n = 0
         withAllCombinations(send.TK[1], {
               an_integer = 42,
               a_double   = 3.1415,
               a_string   = 'hello',
               a_date     = '1977-10-20 00:00:00',
            },
            function(row)
               n = n + 1
               row.key_1, row.key_2 = div(n,2)
               Db:mergeData(send)
               expect[n] = row
            end)
         local got = Db:query('select * from TK')
         local ok, why = sameTable(got, expect)
         if not ok then return 'insert: '..why end
         withAllCombinations(send.TK[1], {
               an_integer = 496,
               a_double   = 2.7182,
               a_string   = 'good-bye',
               a_date     = '1982-09-08 00:00:00',
            },
            function(row)
               row.key_1, row.key_2 = div(n,2)
               Db:mergeData(send)
               expect[n] = row
               n = n - 1
            end)
         local got = Db:query('select * from TK')
         local ok, why = sameTable(got, expect)
         if not ok then return 'update: '..why end
      end)
end

local function testMergeDefaults(Db)
   Db = connect(Db)
   return Db:withRollback(
      function()
         local spec = {vmd='basics.vmd',name='MK'}
         local send   = db.tables(spec)
         local expect = db.tables(spec).TK
         pcall(dropTable, Db, expect)
         createTable(Db, expect)
         local row = send.TK[1]
         row.an_integer = 42
         row.a_double   = 3.1415
         row.a_string   = 'hello'
         row.a_date     = '1977-10-20 00:00:00'
         for i = 1, 2 do
            row.key_1, row.key_2 = 1, i
            Db:mergeData(send)
            expect[i] = row
         end
         local got = Db:query('select * from TK')
         local ok, why = sameTable(got, expect)
         if not ok then return 'insert: '..why end
         for i = 1, row:childCount() do
            row[i] = nil  -- Wipe the row.
         end
         row.key_1, row.key_2 = 1, 1
         unwind_protect(
            function()
               Db.merge_null = false
               Db:mergeData(send)
            end,
            function()
               Db.merge_null = nil
            end)
         local got = Db:query('select * from TK')
         local ok, why = sameTable(got, expect)
         if not ok then return 'update: '..why end
      end)
end

local function runCxxMergeTest(Test, Db)
   assert(DbConnIndex.merge == cxx_conn_merge)
   if not is_unix or Db.use_unicode or Db.api ~= db.SQL_SERVER then
      return Test(Db)
   end
end

---------------------
-- Test Generation --
---------------------

local platform = db_test_config.platform()
local databases = {
   ['MySQL'] = db_test_config.mySql(),
   ['Oracle OCI'] = db_test_config.oracleOci(),
}
if platform.os:find('win') then
   databases['SQL Server'] = db_test_config.sqlServer()
   databases['Oracle ODBC'] = db_test_config.oracleOdbc()
   databases['MySQL ODBC'] = db_test_config.mysqlOdbc()
end
local function makeTests(Db)
   Db = copy(Db)
   return {
      insert    = function() testInsert(Db)   end;
      --dbexec    = function() testDbexec(Db)   end;
      cxx_merge = {
         alltypes = function() testCxxMergeTypes(Db) end;
         insert   = function() runCxxMergeTest(testMergeInsert,Db) end;
         update   = function() runCxxMergeTest(testMergeUpdate,Db) end;
      },
      -- lua_merge = {
      --    alltypes = function() testLuaMergeTypes(Db) end;
      --    insert   = function() withLuaMerge(testMergeInsert,Db) end;
      --    update   = function() withLuaMerge(testMergeUpdate,Db) end;
      --    defaults = function() withLuaMerge(testMergeDefaults,Db) end;
      -- },
      custom_merge = {
         alltypes = function() return testCustomMergeTypes(Db) end;
         insert   = function() return withCustomMerge(testMergeInsert,Db) end;
         update   = function() return withCustomMerge(testMergeUpdate,Db) end;
         defaults = function() return withCustomMerge(testMergeDefaults,Db) end;
      },
   }
end
local tests = {}
for k,Db in pairs(databases) do
   if not Db.use_unicode then
      tests[k] = makeTests(Db)
   end
   Db.use_unicode = true
   tests[k..':Unicode'] = makeTests(Db)
end

function main()
   -- Annotations wrap our global functions with other functions, so we want
   -- the wrapped version here.
   cxx_conn_merge = DbConnIndex.merge
   -- Call any test case you want here, if running in the Translator IDE.
end

return tests
