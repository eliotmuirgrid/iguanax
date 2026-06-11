-- Unit tests for json.*

require('common.testutils')

local assertEquals = testutils.assertEquals
local AssertError = testutils.AssertError

local jsonStr1 = '{ "firstName": "John","lastName": "Smith","age": 25,"address": {"streetAddress": "21 2nd Street", "city": "New York","state": "NY","postalCode": 10021},"phoneNumber": [ {"type": "home","number": "212 555-1234"},{"type": "fax","number": "646 555-4567"}]}'

local jsonStr2 = [[
{
   "firstName": "John",
   "address": {
      "streetAddress": "21 2nd Street",
      "state": "NY",
      "city": "New York",
      "postalCode": 10021
   },
   "phoneNumber": [
      {
         "number": "212 555-1234",
         "type": "home"
      },
      {
         "number": "646 555-4567",
         "type": "fax"
      }
   ],
   "lastName": "Smith",
   "age": 25
}]]

local keyCount = 11

-- Convenience function to recurse a table, counting nodes.
-- Maybe this can be moved to testutils?
local function getKeyCount(table, count)
	local count = count
	for k,v in pairs(table) do
		if type(v) == "table" then
			count = getKeyCount(v, count)
		else
			count = count + 1
		end
	end
	return count
end

-- Unit tests start
local function JSONparseNoTable()
	local r = json.parse(jsonStr1)

	local count = getKeyCount(r, 0)
	assertEquals(count, keyCount)
end

local function JSONparseNamedParam()
	local r = json.parse{data=jsonStr1}

	local count = getKeyCount(r, 0)
	assertEquals(count, keyCount)
end

local function JSONparserSerializes()
   local r = json.parse(jsonStr1)
   local s = json.serialize{data=r}
   testutils.compareJson(jsonStr2,s)
end

local function JSONserializesAlphasort()
   local unsorted = {g=8, b=2, a=1, d=4, e=5, c=3, f=7}
   local sortedresult = '{"a":1,"b":2,"c":3,"d":4,"e":5,"f":7,"g":8}'
   local result = json.serialize{data=unsorted, alphasort=true, compact=true}
   assertEquals(result, sortedresult)
end

-- No longer accepting any calls without the table form
local function JSONserializeRequiresNamedArguments()
   local t = {g=8, b=2, a=1, d=4, e=5, c=3, f=7}
   AssertError(function()
      local r = json.serialize(t)
   end, nil, "not using named argument form", true)
end 

local errStr1 = 'Error parsing JSON: unexpected character at position 0'
local function JSONparseBadParam()
	AssertError(function()
		local r = json.parse("xyzzy")
	end, nil, errStr1, true)
end

local errStr2 = 'Parameter table expected, got no value'
local function JSONparseNoParam()
	AssertError(function()
		local r = json.parse()
	end, nil, errStr2, true)
end

local errStr3 = "Parameter 'data' is required"
local function JSONparseNoParamTable()
	AssertError(function()
		local r = json.parse{}
	end, nil, errStr3, true)
end

local function JSONcreateObject()
	local o = json.createObject()
	local res = type(o) == "table"
	assertEquals(res, true)
end

local function JSONcreateObjectWithParam()
	AssertError(function()
		local o = json.createObject('Fnord')
	end, nil, 'No arguments expected.', true)
end

local function JSONcreateObjectWithEmptyTable()
	AssertError(function()
		local o = json.createObject{}
	end, nil, 'No arguments expected.', true)
end

local function JSONserializeEmptyTableParam()
	AssertError(function()
		local s = json.serialize{}
	end, nil, errStr3, true)
end

local function JSONserializeNoParam()
	AssertError(function()
		local r = json.serialize()
	end, nil, errStr2, true)
end

local function JSONparseHex()
   local res = json.parse('["\\u004b"]') -- 0x4b == 'K'
   assertEquals(res[1], 'K')

   res = json.parse('["\\u004B"]') -- 0x4B == 'K'
   assertEquals(res[1], 'K')
end

local function JSONserializeHex()
   local res = json.serialize{data={'\026'}}
   assertEquals(res, '[\n   "\\u001A"\n]')
end

local Tests = {
   ['JSONparseNoTable'] = JSONparseNoTable,
   ['JSONparseNamedParam'] = JSONparseNamedParam,
   ['JSONparserSerializes'] = JSONparserSerializes,
   ['JSONserializesAlphasort'] = JSONparserSerializesAlphasort,
   ['JSONserializeRequiresNamedArguments'] = JSONserializeRequiresNamedArguments,
   ['JSONparseBadParam'] = JSONparseBadParam,
   ['JSONparseNoParam'] = JSONparseNoParam,
   ['JSONparseNoParamTable'] = JSONparseNoParamTable,
   ['JSONcreateObject'] = JSONcreateObject,
   ['JSONcreateObjectWithParam'] = JSONcreateObjectWithParam,
   ['JSONcreateObjectWithEmptyTable'] = JSONcreateObjectWithEmptyTable,
   ['JSONserializeEmptyTableParam'] = JSONserializeEmptyTableParam,
   ['JSONserializeNoParam'] = JSONserializeNoParam,
   ['JSONparseHex'] = JSONparseHex,
   ['JSONserializeHex'] = JSONserializeHex,
}


function main()
   --[[for K,V in pairs(Tests) do
      V()
   end]]
   JSONparseNoTable()
   JSONparseNamedParam()
   JSONparserSerializes()
   JSONparseBadParam()
   
end


-- Roll up the tests and return them.
return Tests


