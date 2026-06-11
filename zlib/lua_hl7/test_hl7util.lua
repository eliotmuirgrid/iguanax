-- $Revision: 1.4 $
-- $Date: 2012-06-19 00:20:41 $

-- This is the Unittest module for hl7util.lua. This module returns a function which can be called.
-- The function returns a tree of name->function pairs and each function represents a test.
require('hl7util')
require('common.testutils')
local assertEquals = testutils.assertEquals

local function trace(a,b,c,d) end

local SAMPLE_MESSAGE = 
"MSH|^~\\&|iNTERFACEWARE|Lab|Main HIS|St. Micheals|20110213144932||ADT^A03|9B38584D9903051F|P|2.6|\r"..
"EVN||20110213144532||||20110213145902|\r"..
"PID|||4525285^^^ADT1||Smith^Tracy||10-Feb-1998|F||Martian|86 Yonge St.^^ST. LOUIS^MO^51460||1185438871|8530031194||||10-346-6|284-517-569|\r"..
"NK1|1|Smith^Gary|Second Cousin|\r"..
"PV1||E||||||5101^Garland^Mary^F^^DR|||||||||||1318095^^^ADT1|||||||||||||||||||||||||20110213144956|\r"..
"OBX|||WT^WEIGHT||102|pounds|\r"..
"OBX|||HT^HEIGHT||32\\\\\\r|cm|\r"..
"PR1|\r"..
"PR1|\r"..
"ZID||tracy.smith@acmemed.com|||\r"..
""

local EXPECTED_SEGMENT_LIST = { 'PID', 'EVN', 'OBX', 'PR1'}
local UNEXPECTED_SEGMENT = 'PIG'

local function segmentHasName(ExpectedSegmentName)
   -- this creates and returns a filter function for hl7util.findSegment()
   -- for finding the segment with a given name
   -- e.g.: PID = hl7util.findSegment(Message, segmentHasName('PID'))
   --
   return function(Segment)
      return Segment:nodeName()==ExpectedSegmentName 
   end
end

local function testFindSegment()
   trace(SAMPLE_MESSAGE)
   local Message, MessageName = hl7.parse{vmd = 'test_hl7util.vmd', data = SAMPLE_MESSAGE}
   trace(Message)
   assert(Message ~= nil)
   
   for _, ExpectedSegmentName in ipairs(EXPECTED_SEGMENT_LIST) do
      local Segment = hl7util.findSegment(Message, segmentHasName(ExpectedSegmentName))
      assert(Segment, "Failed to find segment: "..ExpectedSegmentName)
      assert(Segment:nodeType() == 'segment')
      assert(Segment:nodeName() == ExpectedSegmentName) 
   end
   
   local NonSegment = hl7util.findSegment(Message, segmentHasName(UNEXPECTED_SEGMENT))
   assert(NonSegment==nil, "Found segment that shouldn't be there: "..UNEXPECTED_SEGMENT)
end

local function testRedefNode()
   local f = function()
      node = hl7.parse {
      vmd = 'test_hl7util.vmd',
      data = SAMPLE_MESSAGE,
      }

      local val = node.MSH[7][1]:nodeValue()
      return val
   end

   local status, result = pcall(f)
   trace(result)
   assertEquals(status, true)
   assertEquals(result, '20110213144932')
end

return {
   ['testFindSegment'] = testFindSegment,
   ['testRedefNode'] = testRedefNode,
}
