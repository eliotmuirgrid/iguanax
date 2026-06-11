-- $Revision: 1.1 $
-- $Date: 2013-05-14 21:30:14 $

-- This is the Unittest module for our db API, with sqlite.

require('common.testutils')

local sqliteTests = {}

-- Test case for #22669 (NULL values were represented as 0).
function sqliteTests.nullColumn()
   local RS = db.query{api=db.SQLITE, name='test.tmp', sql='SELECT NULL AS ShouldBeNull'}
   
   testutils.assertEquals(RS[1].ShouldBeNull:isNull(), true)
end

-- Test case for #22646.
function sqliteTests.nullAndNonNullRow()
   local RS = db.query{api=db.SQLITE, name='test.tmp',
                       sql='SELECT NULL AS Name UNION SELECT \'Kevin\' AS Name'}
   
   testutils.assertEquals(RS[1].Name:isNull(), true)
   testutils.assertEquals(RS[2].Name:isNull(), false)
end

-- return test functions
return sqliteTests