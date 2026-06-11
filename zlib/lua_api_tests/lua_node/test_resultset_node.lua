--[[ Nov. 26 2012 

Tests the result set node lua apis.

]]

require('common.testutils')

local function GetNodeTree()
	return testexpose.newResultSet()
end 

local function setNodeValue_on_readonly_node_throws()
	local tree = GetNodeTree()
	
	-- attempt to set value on read only node and assert error was thrown
    testutils.AssertError(function(x) tree[1]:setNodeValue("awesome") end, '')
end

return {
   ['setNodeValue_on_readonly_node_throws'] = setNodeValue_on_readonly_node_throws,
}