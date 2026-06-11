-- $Revision: 1.8 $
-- $Date: 2012-09-12 13:32:08 $

-- This is the Unittest module for dbc. This module returns a function which can be called.
-- The function returns a tree of name->function pairs and each function represents a test.
require("dbc")
require("db_test_config")

unit = {}

-- parameters that will be used in dbc unit test functions
--local params = {api = db.MY_SQL, name = 'test', user = 'root', password = 'its_a_secret'}
local params = db_test_config.mySql()
local isLive = true
local testQuery = 'SHOW TABLES'

-- test dbc.Connection
local function testConnection()
   local DB = dbc.Connection(params)
   assert( DB ~= nil )
   assert( type(DB.name) == 'string' )
   assert( type(DB.user) == 'string' )
   assert( type(DB.password) == 'string' )   
   assert( type(DB.api) == 'number' )
   assert( type(DB.execute) == 'function' )
   assert( type(DB.query) == 'function' )
   assert( type(DB.merge) == 'function' )
   return DB
end

-- test dbc.query
local function testBasicQuery()
   local DB = testConnection(params)
   local A, B = DB:query(testQuery,true)
   assert( A ~= nil )
   assert( B ~= nil )
   assert( type(A)  == 'userdata')
   assert( type(B)  == 'table')
   assert( #A >= 0 )
end

-- test dbc.execute
local function testBasicExecute()
   local DB = testConnection(params)
   local A, B = DB:execute(testQuery, isLive) 
   assert( A~= nil )
   assert( B~= nil )
   assert( type(A)  == 'userdata')
   assert( type(B)  == 'table')
   assert( #A >= 0 )
   assert( #B >= 0 )
end

-- return test functions
return {
   ['testConnection'] = testConnection, 
   ['testBasicQuery'] = testBasicQuery,
   ['testBasicExecute'] = testBasicExecute,
}
