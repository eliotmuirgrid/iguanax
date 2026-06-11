-- $Revision: 1.4 $
-- $Date: 2012-06-26 21:30:14 $

-- This is the Unittest module for sqlescape.lua. This module returns a function which can be called.
-- The function returns a tree of name->function pairs and each function represents a test.

require('sqlescape')

-- A dummy database type used to verify that the module still provides an
-- escaping function when an invalid database type is given.
local DUMMY_DATABASE = "foo"

-- parameters that will be used in sqlescape unit test functions
local params = { db.MY_SQL,  db.ORACLE_OCI, db.ORACLE_ODBC, db.SQLITE, db.SQL_SERVER, db.POSTGRES, db.DB2, db.INFORMIX, db.INTERBASE, db.FILEMAKER, db.SYBASE_ASA, db.SYBASE_ASE, db.ACCESS,
   DUMMY_DATABASE }
-- Input value for the tests.
-- *Note: Escape sequences are not interpreted when present in multi-line strings
-- so the test input and output values must be surrounded in single or double
-- quotes if we want to test for things like the null character.
local value = "'\\\"\0"

-- test sqlescape.EscapeFunction
local function testEscapeFunction()
   EscapeFuncs = {}
   local j = 1
   for i, v in pairs(params) do
     local EscapeFunc = sqlescape.EscapeFunction(v)
     assert( EscapeFunc ~= nil)
     assert( type(EscapeFunc)=='function' )
     EscapeFuncs[j] = EscapeFunc
     j = j+1
   end
   return EscapeFuncs
end

-- check escaped result
-- In all cases the expected output should be surrounded by a pair of
-- single quotes.
local function checkEscapedValue(EscapedValue, i)
   local SingleQuoteSet = { db.ORACLE_OCI, db.ORACLE_ODBC, db.SQLITE, db.SQL_SERVER, db.DB2, db.INFORMIX, db.INTERBASE, db.FILEMAKER, db.SYBASE_ASA, db.SYBASE_ASE, db.ACCESS, DUMMY_DATABASE }
   local EscapeWithBackSlashesSet = { db.MY_SQL }
   local EscapeByDuplicationSet = { db.POSTGRES }
   local dbName = params[i]
   
   -- Only single quotes need to be escaped in this set, and this is done
   -- by duplication.
   for i = 1, #SingleQuoteSet do
      if SingleQuoteSet[i] == dbName then
          assert(EscapedValue == "'''\\\"\0'")
          return
      end
   end
  
   -- Back slashes, single quotes, double quotes, and null characters need
   -- to be escaped in this set, and this is done by preceding the characters
   -- with a back slash.
   for i = 1, #EscapeWithBackSlashesSet do
      if EscapeWithBackSlashesSet[i] == dbName then
          assert(EscapedValue == "'\\'\\\\\\\"\\0'")
         return
      end
   end
   
   -- Back slashes and single quotes need to be escaped in this set as well,
   -- but in this case it is done by duplication.
   for i = 1, #EscapeByDuplicationSet do
      if EscapeByDuplicationSet[i] == dbName then
          assert(EscapedValue == "'''\\\\\"\0'")
         return
      end
   end
end
   
-- test each escape method
local function testEscapeMethod()
   local EscapeMethods = testEscapeFunction()
   for i, v in pairs(EscapeMethods) do
     local oldValue = value
     local EscapedValue = v(oldValue)
     assert(EscapedValue ~= nil)
     checkEscapedValue(EscapedValue, i)
   end
end

-- return test functions
return {['testEscapeFunction'] = testEscapeFunction, ['testEscapeMethod'] = testEscapeMethod}