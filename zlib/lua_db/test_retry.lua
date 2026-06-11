-- This is the Unittest module for retry.lua. This module returns a function which can be called.
-- The function returns a tree of name->function pairs and each function represents a test.

require('retry')

iguana.isTest = function() return true end -- iguana.isTest does not exist as part of core NODL

function assert_equals(A, B)
   if A ~= B then
      error('\nStrings not equal:\nEXPECTED: '..tostring(A)..'\nACTUAL:   '..tostring(B)..'\n\n'..debug.traceback())
   end
end

-- this is a test function that does nothing
local function testFunc(call_counter)
   call_counter.count = call_counter.count+1;
   if call_counter.retry_bool then
     error('test')
   end
end

-- run retry.call() with retry/not retry action specified 
local function testSetup(Retry)
  -- be sneaky and temporarily change iguana.isTest()
  local oldIsTest = iguana.isTest;
  iguana.isTest = function() return false; end
  iguana.setChannelStatus = function () end  -- iguana.setChannelStatus does not exist as part of core NODL.
     
  local call_counter = {count=0, retry_bool = Retry, retryNumber = 2}
  
  function innerCall()
    local R, D = retry.call{func = testFunc, arg1 = call_counter, retry = call_counter.retryNumber, pause = 1}
  end
  local result,errorString = pcall(innerCall)
  
  --restore iguana.isTest
  iguana.isTest = oldIsTest
   
  return result, errorString, call_counter
end   

-- test retry.call with retry
local function testWithRetry()
  local result, errorString, call_counter = testSetup(true)
  assert_equals(result, false)
  assert_equals(call_counter.count, call_counter.retryNumber)
  assert(errorString:find('Unable to recover.  Stopping channel.'))
end
   
-- test retry.call without retry
local function testWithoutRetry()
  local result, errorString, call_counter = testSetup(false)
  assert_equals(result, true)
  assert_equals(call_counter.count, 1)
  assert_equals(errorString, nil)
end   
   
--return test functions
return {
   ['testWithRetry']    = testWithRetry,
   ['testWithoutRetry'] = testWithoutRetry
}

