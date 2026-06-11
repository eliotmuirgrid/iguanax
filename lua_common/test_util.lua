--[[
Unit tests for functions in the util namespace.
]]

require("common.testutils")

local utilTests = {}

-- Just one GUID test for now. More tests (written in Python) can be found in
-- ChameleonPy/ChameleonPyTests/guidTest.py.
-- If this test fails then it may be due to a seeding issue with the RNG used
-- for GUID generation. See #22563.
function utilTests.testGuidUniqueness()
   local NumGuids = 10000
   local GuidSize = 128 -- minimum number of bits the function will allow
   
   local GeneratedGuids = {}
   for GuidCount = 1, NumGuids do
      local Guid = util.guid(GuidSize)
      testutils.assertEquals(GeneratedGuids[Guid], nil)
      
      -- We store the GUID as a key rather than a value for faster lookup.
      GeneratedGuids[Guid] = not nil
   end
end

return utilTests