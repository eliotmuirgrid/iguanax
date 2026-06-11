--[[ Nov. 23 2012 

Test utils contains utilities for unit testing.

]]

testutils = {}

-- Supports calling functions with 0 or 1 parameters.
-- ExpectedErr, MatchSubstring are optional.
-- MatchSubstring causes the assertion to pass if ExpectedErr is
-- found as a substring in the actual error returned.
function testutils.AssertError(F, Arg, ExpectedErr, MatchSubstring)
   local Status, ActualErr = pcall(F, Arg)

   if Status then
      error('Function did not throw an error.', 2)
   else
      if ExpectedErr ~= nil then
        local foundMatch = false
	if MatchSubstring then
		foundMatch = string.find(ActualErr, ExpectedErr, 1, true) ~= nil
	else
		foundMatch = ExpectedErr == ActualErr
	end

	if not foundMatch then
            print(ExpectedErr, ActualErr)
            error('Function did not throw expected error.\n'..
                     '      Expected: ['..tostring(ExpectedErr)..']\n'..
                     '      Actual:   ['..tostring(ActualErr)..']', 2)
         end
      end
      -- else, the fact than an error was thrown is good enough
   end
end

-- Better than assert(A == B) because:
--  1) This will show you the line number of the failed assertion, and
--  2) It will show you the two unequal values (rather than a generic
--     "assertion failed!" message.
function testutils.assertEquals(A,B)
   if A ~= B then
      error('"'..tostring(A)..'" ~= "'..tostring(B)..'"', 2)
   end
end

local function CompareTable(T1, T2, D)
   for K,V in pairs(T1) do
      if type(V) == 'table' then
         CompareTable(T1[K], T2[K], D+1)
      else
         if T1[K] ~= T2[K] then
            error('T1[' ..K..'] was '..tostring(T1[K])..' and T2['..K..'] was '..tostring(T2[K]),D+2)
         else
            print(T1[K]..' is equal to '..T2[K])
         end
      end
   end
end


function testutils.compareJson(A,B)
   local AJ = json.parse{data=A}
   local BJ = json.parse{data=B}
   CompareTable(AJ, BJ, 1)
   CompareTable(BJ, AJ, 1)
end

