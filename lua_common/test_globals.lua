--[[
Unit tests for functions defined in our API at the global level.
]]

local globalTests = {}


-- Test cases.

local TRACE_TESTS = {
   -- Verify that trace can accept arguments of varying types.
   function()
      trace(1)
   end,
   function()
      trace("test")
   end,
   function()
      trace{1, 2, 3}
   end,
   -- Verify that trace can accept varying numbers of arguments.
   function()
      trace()
   end,
   function()
      trace("test1", "test2")
   end,
   function()
      trace(1, 2, 3)
   end,
   -- Verify that trace does not modify a mutable input value.
   function()
      local Table = {test = 1}
      local PreTraceLength = #Table
      local PreTraceFieldValue = Table.test
      
      trace(Table)
      assert(#Table == PreTraceLength, "Length of the table has increased.")
      assert(Table.test == PreTraceFieldValue, "The table's field value has changed.")
   end
}


-- Functions.

local function runTests(TestCases)
   for _, TestFunc in ipairs(TestCases) do
      TestFunc()
   end
end

function globalTests.testTrace()
   runTests(TRACE_TESTS)
end

return globalTests
