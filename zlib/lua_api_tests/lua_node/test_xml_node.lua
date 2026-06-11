--[[ Nov. 26 2012 

Tests the xml node lua apis.

]]

local function GetXMLNodeTree()
	return xml.parse{data=[[<ADT>awesome</ADT>]]}
end 

local function setNodeValue_sets_value()
	local tree = GetXMLNodeTree()
	
	tree[1][1]:setNodeValue("super"); 
	
	assert(tree[1][1]:nodeValue()=="super"); 
end

return {
   ['setNodeValue_sets_value'] = setNodeValue_sets_value,
}