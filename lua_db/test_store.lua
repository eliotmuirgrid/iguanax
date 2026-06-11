-- Unit tests for the store module.

local store = require("store")

local test_store = {}


-- Test cases.
local TEST_CASES = {
   test1 = "test1", -- normal key and value pair
   test2 = 2, -- number value
   test3 = true, -- boolean value
   test4 = nil, -- nil value
   [5] = "test5", -- number key
   [6] = 6, -- number key and value pair
   
   test7 = "'test7", -- value with embedded single quote
   test8 = "'test8'", -- value wtih embedded single quotes
   test9 = "\"test9", -- value with embedded double quote
   test10 = "\"test10\"", -- value with embedded double quotes
   
   ["'test11"] = "test11", -- key with embedded single quote
   ["'test12'"] = "test12", -- key with embedded single quotes
   ["\"test13"] = "test13", -- key with embedded double quote
   ["\"test14\""] = "test14", -- key with embedded double quotes
   
   ["'test15"] = "'test15", -- key and value pair with embedded single quote
   ["'test16'"] = "'test16'", -- key and value pair with embedded single quotes
   ["\"test17"] = "\"test17", -- key and value pair with embedded double quote
   ["\"test18\""] = "\"test18\"", -- key and value pair with embedded double quotes
   
   test19 = "'test19\"", -- value with a mix of single and double quotes
   ["'test20\""] = "'\"test20'\"'" -- key and value pair with a mix of single and double quotes
}


-- Functions.
function test_store.runTests()
   for key, value in pairs(TEST_CASES) do
      store.put(key, value)
      local output = store.get(key)
      
      -- The tostring function is called on the value inserted when making the
      -- the following comparison because all values are stored in the database
      -- as strings. It is up to the programmer using the store module to convert
      -- non-string values back to their original type after they have been
      -- retrieved from the database.
      assert(output == tostring(value), "A test has failed! " ..
         "Key: " .. tostring(key) .. " Value: " .. tostring(value))
   end
end


return test_store