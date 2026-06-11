--[[ Nov. 26 2012 

Tests the table node lua apis.

]]

require('common.testutils')

local Vmd = 'test.vmd'
local Data = [[MSH|^~\&|AcmeMed|E\T\R|Main HIS|St. Micheals|20110213150034||ADT^A03|9B38584DB703ED4885722A4E6A238EB623EB6FB9B3B51C53C3C99B9578D19B4D|P|2.6|
EVN||20110213145501||||20110213144945|
PID|||3769952^^^ADT1||Fitzgerald^Sabrina||19870225|F||Martian|342 Delphi Cres.^^ST. LOUIS^MO^85610|||||||58-632-358|215-397-681|
NK1|1|Smith^Sabrina|Sibling|
PV1||E||||||5101^Meade^Mary^F^^DR|||||||||||8904385^^^ADT1|||||||||||||||||||||||||20110213145810|
ZID||Sabrina@awesome.com|||]]

local function GetNodeTree()
	local msg, Name = hl7.parse{vmd=Vmd, data=Data}
	return db.tables{vmd=Vmd, name=Name}
end 

local tests = {}

function tests.setNodeValue_sets_value()
	local tree = GetNodeTree()
	
	tree[1][1][1]:setNodeValue("super"); 
	
	assert(tree[1][1][1]:nodeValue()=="super"); 
end

function tests.remove_by_index_1()
   local tree = GetNodeTree()
   
   testutils.assertEquals(tree[1]:nodeName(), 'patient')
   testutils.assertEquals(tree[2]:nodeName(), 'kin')
   testutils.assertEquals(#tree, 2)
   
   tree[1][1][1]:setNodeValue("super")
   testutils.assertEquals(#tree[1], 1)
   
   tree:remove(1)
   
   -- Ensure both tables are still there.
   testutils.assertEquals(tree[1]:nodeName(), 'patient')
   testutils.assertEquals(tree[2]:nodeName(), 'kin')
   testutils.assertEquals(#tree, 2)
   
   -- Ensure data was removed from the first table.
   testutils.assertEquals(#tree[1], 0)
end

function tests.remove_by_name_patient()
   local tree = GetNodeTree()
   
   testutils.assertEquals(tree[1]:nodeName(), 'patient')
   testutils.assertEquals(tree[2]:nodeName(), 'kin')
   testutils.assertEquals(#tree, 2)
   
   tree[1][1][1]:setNodeValue("super")
   testutils.assertEquals(#tree[1], 1)
   
   tree:remove('patient')
   
   -- Ensure both tables are still there.
   testutils.assertEquals(tree[1]:nodeName(), 'patient')
   testutils.assertEquals(tree[2]:nodeName(), 'kin')
   testutils.assertEquals(#tree, 2)
   
   -- Ensure data was removed from 'patient'.
   testutils.assertEquals(#tree[1], 0)
end

return tests