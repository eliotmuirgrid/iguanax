--[[ Nov. 23 2012 

Tests the hl7 data struct apis.

]]

require('common.testutils')


local Vmd = 'test.vmd'
local Data = [[MSH|^~\&|AcmeMed|E\T\R|Main HIS|St. Micheals|20110213150034||ADT^A03|9B38584DB703ED4885722A4E6A238EB623EB6FB9B3B51C53C3C99B9578D19B4D|P|2.6|
EVN||20110213145501||||20110213144945|
PID|||3769952^^^ADT1||Fitzgerald^Sabrina||19870225|F||Martian|342 Delphi Cres.^^ST. LOUIS^MO^85610|||||||58-632-358|215-397-681|
NK1|1|Smith^Sabrina|Sibling|
PV1||E||||||5101^Meade^Mary^F^^DR|||||||||||8904385^^^ADT1|||||||||||||||||||||||||20110213145810|
ZID||Sabrina@awesome.com|||]]


local function setNodeValue_on_readonly_node_throws()
   local parsedMsg, name = hl7.parse{vmd=Vmd, data=Data}
   
   -- set the node value and assert error was thrown
   testutils.AssertError(function(x) in_msg[1][6][1]:setNodeValue("awesome") end, '')
end

local function setNodeValue_on_message_node()
   local msg = hl7.message{vmd=Vmd, name='ADT'}
   
   msg[1][6][1]:setNodeValue("awesome")
   
   assert(msg[1][6][1]:nodeValue() == "awesome", "setNodeValue didn't correctly set the value of a node.")
end

local function setNodeValue_on_message_complex_node_throws()
   local msg = hl7.message{vmd=Vmd, name='ADT'}
   
   -- set complex node value and assert error was thrown
   testutils.AssertError(function(x) msg[1]:setNodeValue("awesome") end, '')
end

return {
   ['setNodeValue_on_readonly_node_throws'] = setNodeValue_on_readonly_node_throws,
   ['setNodeValue_on_message_node'] = setNodeValue_on_message_node,
   ['setNodeValue_on_message_complex_node_throws'] = setNodeValue_on_message_complex_node_throws,
}
