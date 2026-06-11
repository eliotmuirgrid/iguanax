require('common.testutils')

local xmlStr = [[
<?xml version="1.0" encoding="ISO-8859-1"?>
<breakfast_menu>
	<food>
		<name>Belgian Waffles</name>
		<price>$5.95</price>
		<description>two of our famous Belgian Waffles with plenty of real maple syrup</description>
		<calories>650</calories>
	</food>
	<food>
		<name>Strawberry Belgian Waffles</name>
		<price>$7.95</price>
		<description>light Belgian waffles covered with strawberries and whipped cream</description>
		<calories>900</calories>
	</food>
	<food>
		<name>Berry-Berry Belgian Waffles</name>
		<price>$8.95</price>
		<description>light Belgian waffles covered with an assortment of fresh berries and whipped cream</description>
		<calories>900</calories>
	</food>
	<food>
		<name>French Toast</name>
		<price>$4.50</price>
		<description>thick slices made from our homemade sourdough bread</description>
		<calories>600</calories>
	</food>
	<food>
		<name>Homestyle Breakfast</name>
		<price>$6.95</price>
		<description>two eggs, bacon or sausage, toast, and our ever-popular hash browns</description>
		<calories>950</calories>
	</food>
</breakfast_menu>
]]

local assertEquals = testutils.assertEquals

-- Unit tests start here
-- We can probably add a lot more comprehensive tests here.
local function XMLparseWalkwithChild()
	local leaf = xml.parse(xmlStr):child('breakfast_menu'):child('food'):child('price')
	local price = leaf[1]
	assertEquals(price:isLeaf(), true)
	local str = price:nodeValue()
	assertEquals(str, '$5.95')
end

local function XMLparseWalkNode()
	local name = xml.parse(xmlStr).breakfast_menu.food.name[1]

	assertEquals(name:nodeValue(), 'Belgian Waffles')
	assertEquals(name:isLeaf(), true)
end

local function XMLparseNamedParam()
	local name = xml.parse{data=xmlStr}.breakfast_menu[4].name[1]

	assertEquals(name:nodeValue(), 'French Toast')
	assertEquals(name:isLeaf(), true)
end

-- Roll-up tests and return them
return {
   ['XMLparseWalkwithChild'] = XMLparseWalkwithChild,
   ['XMLparseWalkNode'] = XMLparseWalkNode,
   ['XMLparseNamedParam'] = XMLparseNamedParam,
}
