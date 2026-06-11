-- This is the Unittest module for stringutil.lua. This module returns a function which can be called.
-- The function returns a tree of name->function pairs and each function represents a test.

require('stringutil')
require('common.testutils')

local paramStringsSplit = {'This is the stringutil test module', 'This  ', '  This', 'This ', ' This', 'This  is  the  stringutil  test  module', 'ThisIsTheStringutilTestModule'}
local paramStringsTrimWS = {'This is the stringutil test module', 'This \r\n\t ', ' \r\n\t This', ' \r\n\t This \r\n\t ', 'ThisIsTheStringutilTestModule'}
local paramStringsTrimLWS = {'This is the stringutil test module', 'This \r\n\t ', ' \r\n\t This', ' \r\n\t This \r\n\t ', 'ThisIsTheStringutilTestModule'}
local paramStringsTrimRWS = {'This is the stringutil test module', 'This \r\n\t ', ' \r\n\t This', ' \r\n\t This \r\n\t ', 'ThisIsTheStringutilTestModule'}
local paramStringsCompactWS = {'This  is  the  stringutil  test  module', 'This  ', '  This', '  This  ', ' This ', 'ThisIsTheStringutilTestModule'}
local paramStringsCapitalize = {'this is the stringutil test module', 'thisIsTheStringutilTestModule'}

local sep = {' '}

local splitResults = {{'This', 'is', 'the', 'stringutil', 'test', 'module'}; {'This', '', ''}; {'', '', 'This'}; {'This', ''}; {'', 'This'}; {'This', '', 'is', '', 'the', '', 'stringutil', '', 'test', '', 'module'}; {'ThisIsTheStringutilTestModule'}}
local trimWSResults = {'This is the stringutil test module', 'This', 'This', 'This', 'ThisIsTheStringutilTestModule'}
local trimLWSResults = {'This is the stringutil test module', 'This \r\n\t ', 'This', 'This \r\n\t ', 'ThisIsTheStringutilTestModule'}
local trimRWSResults = {'This is the stringutil test module', 'This', ' \r\n\t This', ' \r\n\t This', 'ThisIsTheStringutilTestModule'}
local compactWSResults = {'This is the stringutil test module', 'This ', ' This', ' This ', ' This ', 'ThisIsTheStringutilTestModule'}
local capitalizeResults = {'This is the stringutil test module', 'Thisisthestringutiltestmodule'}

-- test string.split
local function testSplit()
   testutils.assertEquals(#splitResults, #paramStringsSplit)
   for i = 1, #paramStringsSplit do
     local fields = string.split(paramStringsSplit[i], sep[1])
     testutils.assertEquals(#fields, #splitResults[i])
     for j = 1, #fields do
        testutils.assertEquals(fields[j], splitResults[i][j])
     end
   end
end

-- test string.trimWS
local function testTrimWS()
   testutils.assertEquals(#trimWSResults, #paramStringsTrimWS)
   for i = 1, #paramStringsTrimWS do
     local field = string.trimWS(paramStringsTrimWS[i])
     testutils.assertEquals(field, trimWSResults[i])
   end
end

-- string.trimLWS
local function testTrimLWS()
   testutils.assertEquals(#trimLWSResults, #paramStringsTrimLWS)
   for i = 1, #paramStringsTrimLWS do
     local field = string.trimLWS(paramStringsTrimLWS[i])
     testutils.assertEquals(field, trimLWSResults[i])
   end
end

--test string.trimRWS
local function testTrimRWS()
   testutils.assertEquals(#trimRWSResults, #paramStringsTrimRWS)
   for i = 1, #paramStringsTrimRWS do
     local field = string.trimRWS(paramStringsTrimRWS[i])
     testutils.assertEquals(field, trimRWSResults[i])
   end   
end

--test string.compactWS
local function testCompactWS()
   testutils.assertEquals(#compactWSResults, #paramStringsCompactWS)
   for i = 1, #paramStringsCompactWS do
     local field = string.compactWS(paramStringsCompactWS[i])
     testutils.assertEquals(field, compactWSResults[i])
   end  
end

-- test string.capitalize
local function testCapitalize()
   testutils.assertEquals(#capitalizeResults, #paramStringsCapitalize)
   for i = 1, #paramStringsCapitalize do
     local field = string.capitalize(paramStringsCapitalize[i])
     testutils.assertEquals(field, capitalizeResults[i])
   end     
end

return {['testSplit'] = testSplit, ['testTrimWS'] = testTrimWS, ['testTrimLWS'] = testTrimLWS, ['testTrimRWS'] = testTrimRWS, ['testCompactWS'] = testCompactWS, ['testCapitalize']= testCapitalize }
