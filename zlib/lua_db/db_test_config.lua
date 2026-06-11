db_test_config = {}

local function chmTestDb()
   return os.getenv('CHM_TEST_DB') or 'test'
end

local function dbParams(Api, Name, User, Password)
   return {api=Api, name=Name, user=User, password=Password}
end

function db_test_config.mySql()
   local CHM_TEST_DB = chmTestDb()
   return dbParams(db.MY_SQL, chmTestDb()..'@mysql-server', CHM_TEST_DB, CHM_TEST_DB)
end

function db_test_config.mySql64()
   local CHM_TEST_DB = chmTestDb()
   return dbParams(db.MY_SQL, CHM_TEST_DB..'@mysql64', CHM_TEST_DB, CHM_TEST_DB)
end

function db_test_config.mysqlOdbc()
   local CHM_TEST_DB = chmTestDb()
   return dbParams(db.MY_SQL_ODBC, 'MyODBCTest', CHM_TEST_DB, CHM_TEST_DB)
end

function db_test_config.oracleOci()
   local CHM_TEST_DB = chmTestDb()
   return dbParams(db.ORACLE_OCI,
      '(description=(address=(host=oraclexe)(protocol=tcp)(port=1521))(connect_data=(sid=XE)))',
      CHM_TEST_DB, CHM_TEST_DB)
end

function db_test_config.oracleOci64()
   local CHM_TEST_DB = chmTestDb()
   return dbParams(db.ORACLE_OCI,
      '(description=(address=(host=oracle64)(protocol=tcp)(port=1521))(connect_data=(sid=ORCL)))',
      'ut_'..CHM_TEST_DB, 'ut_'..CHM_TEST_DB)
end

function db_test_config.oracleOdbc()
   local CHM_TEST_DB = chmTestDb()
   return dbParams(db.ORACLE_ODBC, 'OracleTest', CHM_TEST_DB, CHM_TEST_DB)
end

function db_test_config.postgres()
   return dbParams(db.POSTGRES, 'PostgreSQLTest')
end

function db_test_config.access()
   return dbParams(db.ACCESS, 'AccessTest', '', '')
end

function db_test_config.sqlServer()
   local CHM_TEST_DB = chmTestDb()
   return dbParams(db.SQL_SERVER, 'SqlServerTest', CHM_TEST_DB, CHM_TEST_DB)
end

function db_test_config.sybaseAsa()
   local CHM_TEST_DB = chmTestDb()
   return dbParams(db.SYBASE_ASA, 'SybaseAsaTest', CHM_TEST_DB, CHM_TEST_DB)
end

function db_test_config.sybaseAse()
   local CHM_TEST_DB = chmTestDb()
   local Password = CHM_TEST_DB;
   if #Password < 6 then
      -- for user "dev" - Sybase ASE requires minimum 6 chars in password.
      -- padding with underscores
      for i=#Password+1,6 do
         Password = Password..'_'
      end
   end
   return dbParams(db.SYBASE_ASE, 'SybaseAseTest', CHM_TEST_DB, Password)
end

function db_test_config.interbase()
   return dbParams(db.INTERBASE, 'InterbaseTest', 'TEST_STATIC', 'gh45iop')
end

function db_test_config.db2()
   return dbParams(db.DB2, 'Db2Test')
end

function db_test_config.informix()
   return dbParams(db.INFORMIX, 'InformixTest', 'test', 'gh45iop')
end

function db_test_config.filemaker()
   return dbParams(db.FILEMAKER, 'FilemakerTest', 'admin', 'gh45iop')
end

function db_test_config.sqlite()
   return dbParams(db.SQLITE, 'SqliteTest.sqlite')
end

function db_test_config.platform()
   local Os = 'unknown'
   local x64

   local Makefile = os.getenv('IFW_MAKEFILE')
   if Makefile then
      -- get rid of the ".makefile" suffix to get the platform
      local Suffix = Makefile:sub(-9)
      if Suffix == '.makefile' then
         Os = Makefile:sub(1, -10)
      end
   end

   local Make32 = os.getenv('IFW_MAKE_32_BIT')
   x64 = (Make32 == nil) or (Make32 ~= '1')
   
   return {['os'] = Os, ['x64'] = x64}
end

return db_test_config
