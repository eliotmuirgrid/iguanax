-- This is the Unittest module for dateparse.lua. This module returns a function which can be called.
-- The function returns a tree of name->function pairs and each function represents a test.

require('dateparse')
require('common.testutils')

local function trace(a,b,c,d) end

local timeDateTableElems = {['hour'] = 0, ['min']=0, ['wday']=0, ['day']=0, ['month']=0, ['year']=0, ['sec']=0, ['yday']=0, ['isdst']=0, ['tz_offset']=0, ['tz']=0, ['sec_fraction']=0}
local timeFuzzyStrings = {'20120130143028.11','01/30/2012 02:30:28 PM ACT', '2012-01-30  02:30:28 PM ACT', 
                          'Monday, January 30, 2012 02:30:28 PM ACT', '02:30:28 PM ACT, Monday, January 30, 2012', 
                          'Monday, 30 January 2012 02:30:28 PM ACT', 'Mon, 30 Jan 2012 14:30:28 -05:00',
                          'Monday, January 30, 02:30:28 PM 2012', '30-January-2012 02:30:28 PM ACT'}

local timeStrings = {"2012-01-30 14:30:28", "2:30:28PM January 30 2012", "14:30:28 Jan, 30th '12", "2012-01-30 14:30:28 -05:00", "01/30/2012 14:30:28 AKST", '30-January-2012123 02:30:28 PM ACT', 'Mon, 30 Jan@ 2012 14:30:28 -05:00' }
local timeFormats = {"yy[yy]-mm-dd HH:MM:SS", "H:MM[:SS]tt mmmm d yyyy", "H:MM:SS[tt] mmm, dw [']yy[yy][]", "yy[yy]-mm-dd HH:MM:SS zzzz", "m/d/yy[yy] HH:MM:SS ZZZ", "dd-mmmm-yyyyn HH:MM:SS [tt] ZZZ", "ddd, dd mmm[@] yyyy HH:MM:SS zzzz"}
local resultString = '2012-01-30 14:30:28'
local resultDatabaseString = '2012-01-30 00:00:00'
local resultTSString = '20120130143028'

-- verify that the return table from dateparse.parse() contains all required fields
local function verifyFields(d)
   assert( d~=nil )
   for K, V in pairs(d) do
      if not timeDateTableElems[K] then
         error('Missing date time elemements')
      end
   end
end

-- test dateparse.parse() with default formats
local function testFuzzyParse()
   for i = 1, #timeFuzzyStrings do
     local T, d = dateparse.parse(timeFuzzyStrings[i])
     assert(tostring(T) == resultString)
     verifyFields(d)
   end
end

-- test dateparse.parse() with specified formats
local function testDateParse()
   for i = 1, #timeStrings do
     local T, d = dateparse.parse(timeStrings[i], timeFormats[i])
     assert(tostring(T) == resultString)
     verifyFields(d)

   end
end

-- test string:T() with default formats and specified formats
local function testStringT()
   for i = 1, #timeFuzzyStrings do
     local T = timeFuzzyStrings[i]:T()
     assert(T == resultString )
   end
   
   --test with format
   for i = 1, #timeStrings do
      local T, d = timeStrings[i]:T(timeFormats[i])
      assert(T == resultString)
   end   
end

-- test string:D() with default formats and specified formats
local function testStringD()
   for i = 1, #timeFuzzyStrings do
     local T = timeFuzzyStrings[i]:D()
     assert(T == resultDatabaseString )
   end

   --test with format
   for i = 1, #timeStrings do
      local T, d = timeStrings[i]:D(timeFormats[i])
      assert(T == resultDatabaseString)
   end   
end

-- test string:TS() with default formats and specified formats
local function testStringTS()
   for i = 1, #timeFuzzyStrings do
     local T = timeFuzzyStrings[i]:TS()
     assert(T == resultTSString )
   end
   
   --test with format
   for i = 1, #timeStrings do
      local T,d = timeStrings[i]:TS(timeFormats[i])
      assert(T == resultTSString)
   end
end

local function testParseErrorSpecificFormatBadInput()
   -- input string doesn't match pattern
   testutils.AssertError(function () dateparse.parse('z2013-07-15', 'yyyy-mm-dd') end, nil, '"z2013-07-15" does not match:\n"yyyy-mm-dd" expected year (yyyy), got "z2013-07-15"', true)
end

local function testParseErrorSpecificFormatBadPattern()
   -- format pattern isn't recognized
   testutils.AssertError(function () dateparse.parse('2013-07-15', 'Xyyyy-mm-dd') end, nil, '"2013-07-15" does not match:\n"Xyyyy-mm-dd" unknown date/time pattern: X', true)
end

return {
   testFuzzyParse = testFuzzyParse,
   testDateParse = testDateParse,
   testStringT = testStringT,
   testStringD = testStringD,
   testStringTS = testStringTS,
   testParseErrorSpecificFormatBadInput = testParseErrorSpecificFormatBadInput,
   testParseErrorSpecificFormatBadPattern = testParseErrorSpecificFormatBadPattern,
}
