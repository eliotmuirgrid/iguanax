//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODTtestOperations
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Thursday, September 23rd, 2010 @ 02:29:54 PM
//
//---------------------------------------------------------------------------
#include "NODTprecomp.h"
#pragma hdrstop

#include "NODTtestOperations.h"

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <NODT/NODTplaces.h>
#include <NODT/NODTutils.h>

#include <NOD/NODoperation.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#define NODT_ASSERT_ANY_EXCEPTION(_EVALUATE) \
{ \
   bool __ExceptionOccurred = false; \
   try{ \
      _EVALUATE; \
      __ExceptionOccurred = true; \
   } catch (const COLerror& Error) {} \
   if (__ExceptionOccurred){ \
      COLstring __ErrorMessage(#_EVALUATE); \
      __ErrorMessage += " did not throw as expected."; \
      TSTexception __Error(__ErrorMessage.c_str(),__LINE__,__FILE__,TSTexception::eAssertionFailure); \
      throw __Error; \
   } \
}


// Test copying to/from simple nodes.
static void NODTtestCopySimpleSimple()
{
   NODTleaf Source("Field1", false);
   NODTleaf Dest("SomeOtherField", false);

   // Both have values
   Source.setValue("foo");
   Dest.setValue("bar");
   NODcopyNode(Source, Dest);
   TST_ASSERT_EQUALS("foo", Dest.value());

   // Source is null.
   Source.setIsNull(true);
   Source.clear();
   NODcopyNode(Source, Dest);
   TST_ASSERT_EQUALS(false, Dest.isNull());
   TST_ASSERT_EQUALS("", Dest.value());
}

// Test copying to/from vector nodes.
static void NODTtestCopyVectorVector()
{
   NODTleafVector Source("Field1");
   NODTleafVector Dest("Field1");

   // Both are empty
   NODcopyNode(Source, Dest);
   TST_ASSERT_EQUALS(0, Dest.size());

   // Source has one item, dest is empty
   Source.child(0).setValue("foo");
   NODcopyNode(Source, Dest);
   TST_ASSERT_EQUALS("foo", Dest.child(0).value());

   // Dest has one item, source is empty
   Source.clear();
   NODcopyNode(Source, Dest);
   TST_ASSERT_EQUALS(true, Dest.isNull());

   // Source has two items, dest has one
   Source.child(0).setValue("foo");
   Source.child(1).setValue("bar");
   Dest.child(0).setValue("FOO");
   NODcopyNode(Source, Dest);
   TST_ASSERT_EQUALS(2, Dest.size());
   TST_ASSERT_EQUALS("foo", Dest.child(0).value());
   TST_ASSERT_EQUALS("bar", Dest.child(1).value());

   // Dest has 4 items, source has 2
   Dest.child(0).setValue("A");
   Dest.child(1).setValue("B");
   Dest.child(2).setValue("FOO");
   Dest.child(3).setValue("BAR");
   NODcopyNode(Source, Dest);
   TST_ASSERT_EQUALS(2, Dest.size());
   TST_ASSERT_EQUALS("foo", Dest.child(0).value());
   TST_ASSERT_EQUALS("bar", Dest.child(1).value());
}

// Test copying to/from complex nodes.
static void NODTtestCopyComplexComplex()
{
   NODTcomplex Source("Root", false);
   NODTcomplex Dest("Root", false);

   // Both are empty
   NODcopyNode(Source, Dest);
   const NODplace* pConstDest = &Dest;
   TST_ASSERT_EQUALS(true, pConstDest->child(0).isNull());
   TST_ASSERT_EQUALS(true, pConstDest->child(1).isNull());
   TST_ASSERT_EQUALS(true, pConstDest->child(2).isNull());
   TST_ASSERT_EQUALS(true, pConstDest->child(3).isNull());

   // Source has values, dest does not
   Source.child(0).setValue("foo");
   Source.child(2).child(0).setValue("bar");
   NODcopyNode(Source, Dest);
   TST_ASSERT_EQUALS(false, pConstDest->child(0).isNull());
   TST_ASSERT_EQUALS(true,  pConstDest->child(1).isNull());
   TST_ASSERT_EQUALS(false, pConstDest->child(2).isNull());
   TST_ASSERT_EQUALS(true,  pConstDest->child(3).isNull());

   // Dest has values, source does not
   Source.clear();
   NODcopyNode(Source, Dest);
   TST_ASSERT_EQUALS(true, pConstDest->child(0).isNull());
   TST_ASSERT_EQUALS(true, pConstDest->child(1).isNull());
   TST_ASSERT_EQUALS(true, pConstDest->child(2).isNull());
   TST_ASSERT_EQUALS(true, pConstDest->child(3).isNull());
}

// Test copying to/from heterogeneous vectors.
static void NODTtestCopyUntypedVectorUntypedVector()
{
   NODTuntypedVector Source("Vector");
   NODTuntypedVector Dest("Vector");

   // Both are empty
   Source.clear();
   Dest.clear();
   NODcopyNode(Source, Dest);
   TST_ASSERT_EQUALS(0, Dest.size());

   // Source has one item, dest is empty
   Source.add(NODT_LEAF_NAME.c_str(), NULL).setValue("foo");
   NODcopyNode(Source, Dest);
   TST_ASSERT_EQUALS("foo", Dest.child(0).value());

   // Dest has one item, source is empty
   Source.clear();
   NODcopyNode(Source, Dest);
   TST_ASSERT_EQUALS(true, Dest.isNull());

   // Source has two items, dest has one
   Source.add(NODT_LEAF_NAME.c_str(), NULL).setValue("foo");
   Source.add(NODT_COMPLEX_NAME.c_str(), NULL).child(1).setValue("bar");
   Dest.add(NODT_LEAF_NAME.c_str(), NULL).setValue("FOO");
   NODcopyNode(Source, Dest);
   TST_ASSERT_EQUALS(2, Dest.size());
   TST_ASSERT_EQUALS("foo", Dest.child(0).value());
   TST_ASSERT_EQUALS("bar", Dest.child(1).child(1).value());

   // Dest has 4 items, source has 2
   Dest.clear();
   Dest.add(NODT_LEAF_NAME.c_str(), NULL).setValue("A");
   Dest.add(NODT_LEAF_NAME.c_str(), NULL).setValue("B");
   Dest.add(NODT_LEAF_NAME.c_str(), NULL).setValue("C");
   Dest.add(NODT_LEAF_NAME.c_str(), NULL).setValue("D");
   NODcopyNode(Source, Dest);
   TST_ASSERT_EQUALS(2, Dest.size());
   TST_ASSERT_EQUALS("foo", Dest.child(0).value());
   TST_ASSERT_EQUALS("bar", Dest.child(1).child(1).value());
}

// Test copying to/from mismatching nodes.
static void NODTtestCopyMismatch()
{
   NODTleaf Leaf("Leaf", false);
   NODTleafVector LeafVector1("LeafVector");
   NODTcomplex Complex1("Complex1", false);
   NODTcomplex Complex2("Complex2", false);
   Complex2.add("Field5", NODT_LEAF_NAME.c_str());
   NODTuntypedVector Untyped("Untyped");

   NODcopyNode(Leaf, Leaf); // this should not throw an exception.
   TST_ASSERT_EXCEPTION(NODcopyNode(LeafVector1, LeafVector1), COLerror("Left and right side refer to the same tree - LeafVector", COLerror::PreCondition));
   TST_ASSERT_EXCEPTION(NODcopyNode(Leaf, LeafVector1), COLerror("Left tree LeafVector is not compatible with right tree Leaf", COLerror::PreCondition));
   // The exact error message may change over time, but as long as an exception is thrown, we're happy.
   NODT_ASSERT_ANY_EXCEPTION(NODcopyNode(Leaf, Complex1));
   NODT_ASSERT_ANY_EXCEPTION(NODcopyNode(Leaf, Untyped));
   NODT_ASSERT_ANY_EXCEPTION(NODcopyNode(LeafVector1, Leaf));
   NODT_ASSERT_ANY_EXCEPTION(NODcopyNode(LeafVector1, Complex1));
   NODT_ASSERT_ANY_EXCEPTION(NODcopyNode(LeafVector1, Untyped));
   NODT_ASSERT_ANY_EXCEPTION(NODcopyNode(Complex1, Leaf));
   NODT_ASSERT_ANY_EXCEPTION(NODcopyNode(Complex1, LeafVector1));
   NODT_ASSERT_ANY_EXCEPTION(NODcopyNode(Complex1, Complex2));
   NODT_ASSERT_ANY_EXCEPTION(NODcopyNode(Complex1, Untyped));

   // Populate values (different code will be executed if source nodes are non-null).
   LeafVector1.child(0).setValue("A");
   Complex1.child(0).setValue("B");
   Complex1.child(2).child(1).setValue("C");
   TST_ASSERT_EXCEPTION(NODcopyNode(LeafVector1, LeafVector1), COLerror("Left and right side refer to the same tree - LeafVector", COLerror::PreCondition));
   TST_ASSERT_EXCEPTION(NODcopyNode(Leaf, LeafVector1), COLerror("Left tree LeafVector is not compatible with right tree Leaf", COLerror::PreCondition));
   // The exact error message may change over time, but as long as an exception is thrown, we're happy.
   NODT_ASSERT_ANY_EXCEPTION(NODcopyNode(Leaf, Complex1));
   NODT_ASSERT_ANY_EXCEPTION(NODcopyNode(Leaf, Untyped));
   NODT_ASSERT_ANY_EXCEPTION(NODcopyNode(LeafVector1, Leaf));
   NODT_ASSERT_ANY_EXCEPTION(NODcopyNode(LeafVector1, Complex1));
   NODT_ASSERT_ANY_EXCEPTION(NODcopyNode(LeafVector1, Untyped));
   NODT_ASSERT_ANY_EXCEPTION(NODcopyNode(Complex1, Leaf));
   NODT_ASSERT_ANY_EXCEPTION(NODcopyNode(Complex1, LeafVector1));
   NODT_ASSERT_ANY_EXCEPTION(NODcopyNode(Complex1, Complex2));
   NODT_ASSERT_ANY_EXCEPTION(NODcopyNode(Complex1, Untyped));
}

// Test mapping from simple to simple.
static void NODTtestMapSimpleSimple()
{
   NODTleaf Source("s", false);
   NODTleaf Dest("d", false);
   Source.setValue("foo");

   NODmapTree(Source, Dest);
   TST_ASSERT_EQUALS("foo", Dest.value());
}

// Test mapping from simple to vector.
static void NODTtestMapSimpleVector()
{
   NODTleaf Source("s", false);
   NODTleafVector Dest("d");
   Source.setValue("foo");

   NODmapTree(Source, Dest);
   TST_ASSERT_EQUALS("foo", Dest.child(0).value());
}

// Test mapping from simple to complex.
static void NODTtestMapSimpleComplex()
{
   NODTleaf Source("s", false);
   NODTcomplex Dest("d", false);
   Source.setValue("foo");
   Dest.child(1).setValue("bar");

   NODmapTree(Source, Dest);
   TST_ASSERT_EQUALS("foo", Dest.child(0).value());
   TST_ASSERT_EQUALS("bar", Dest.child(1).value()); // make this it wasn't whiped out
}

// Test mapping from vector to simple.
static void NODTtestMapVectorSimple()
{
   NODTleafVector Source("s");
   NODTleaf Dest("d", false);
   Source.child(0).setValue("foo");
   Source.child(1).setValue("bar");

   NODmapTree(Source, Dest);
   TST_ASSERT_EQUALS("foo", Dest.value());
}

// Test mapping from vector to vector.
static void NODTtestMapVectorVector()
{
   NODTleafVector Source("s");
   NODTleafVector Dest("d");
   Source.child(0).setValue("foo");
   Source.child(1).setValue("bar");

   NODmapTree(Source, Dest);
   TST_ASSERT_EQUALS("foo", Dest.child(0).value());
   TST_ASSERT_EQUALS("bar", Dest.child(1).value());
}

// Test mapping from vector to complex.
static void NODTtestMapVectorComplex()
{
   NODTleafVector Source("s");
   NODTcomplex Dest("d", false);
   Source.child(0).setValue("foo");
   Source.child(1).setValue("bar");

   NODmapTree(Source, Dest);
   TST_ASSERT_EQUALS("foo", Dest.child(0).value());
   const NODplace* pConstDest = &Dest;
   TST_ASSERT_EQUALS(true, pConstDest->child(1).isNull());

   NODTuntypedVector Source2("s");
   Source2.clear();
   Source2.add(NODT_COMPLEX_NAME.c_str(), NULL).child(0).setValue("foo");
   Source2.child(0).child(1).setValue("bar");

   NODmapTree(Source2, Dest);
   TST_ASSERT_EQUALS("foo", Dest.child(0).value());
   TST_ASSERT_EQUALS("bar", Dest.child(1).value());
}

// Test mapping from complex to simple.
static void NODTtestMapComplexSimple()
{
   NODTcomplex Source("s", false);
   NODTleaf Dest("d", false);
   Source.child(0).setValue("foo");

   NODmapTree(Source, Dest);
   TST_ASSERT_EQUALS("foo", Dest.value());
}

// Test mapping from complex to vector.
static void NODTtestMapComplexVector()
{
   NODTcomplex Source("s", false);
   NODTleafVector Dest("d");
   Source.child(0).setValue("foo");
   Source.child(1).setValue("bar");

   // Dest has no items
   NODmapTree(Source, Dest);
   TST_ASSERT_EQUALS(false, Dest.isNull());
   TST_ASSERT_EQUALS("foo", Dest.child(0).value());

   // Dest has an item
   Dest.child(0).setValue("FOO");
   NODmapTree(Source, Dest);
   TST_ASSERT_EQUALS("foo", Dest.child(0).value());

   // Dest is a heterogeneous vector with no item (nothing will be mapped)
   NODTuntypedVector Dest2("d");
   Dest2.clear();
   TST_ASSERT_EQUALS(true, Dest2.isNull());
   NODmapTree(Source, Dest2);
   TST_ASSERT_EQUALS(true, Dest2.isNull());

   // Dest is a heterogeneous vector with a leaf item (one member will be mapped)
   Dest2.add(NODT_LEAF_NAME.c_str(), NULL);
   TST_ASSERT_EQUALS(1, Dest2.size());
   NODmapTree(Source, Dest2);
   TST_ASSERT_EQUALS("foo", Dest2.child(0).value());

   // Dest is a heterogeneous vector with a complex item (entire complex object will be mapped)
   Dest2.child(0).setValue("don't change");
   Dest2.insert(0, NODT_COMPLEX_NAME.c_str(), NULL);
   NODmapTree(Source, Dest2);
   TST_ASSERT_EQUALS("foo", Dest2.child(0).child(0).value());
   TST_ASSERT_EQUALS("bar", Dest2.child(0).child(1).value());
   TST_ASSERT_EQUALS("don't change", Dest2.child(1).value());
}

// Test mapping from complex to complex.
static void NODTtestMapComplexComplex()
{
   NODTcomplex Source("s", false);
   NODTcomplex Dest("d", false);
   Source.child(0).setValue("foo");
   Source.child(2).child(0).setValue("bar");

   // Mapping null data - shouldn't modify the destination
   NODmapTree(Dest, Source);
   TST_ASSERT_EQUALS("foo", Source.child(0).value());
   TST_ASSERT_EQUALS(true, ((const NODplace*)&Source)->child(1).isNull());
   TST_ASSERT_EQUALS("bar", Source.child(2).child(0).value());

   // Regular map
   NODmapTree(Source, Dest);
   TST_ASSERT_EQUALS("foo", Dest.child(0).value());
   TST_ASSERT_EQUALS("bar", Dest.child(2).child(0).value());
}

// Test mapping a range from vector to vector.
static void NODTtestMapRangeVectorVector()
{
   NODTleafVector Vector1("v1");
   NODTleafVector Vector2("v2");
   Vector1.child(0).setValue("A");
   Vector1.child(1).setValue("B");
   Vector1.child(2).setValue("C");
   Vector1.child(3).setValue("D");

   // Map to/from empty vector (invalid cases)
   TST_ASSERT_EXCEPTION(NODmapRange(Vector1, Vector2, -4), COLerror("The range -3 to 0 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(NODmapRange(Vector1, Vector2, 0), COLerror("The range 1 to 0 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(NODmapRange(Vector1, Vector2, 0, 1), COLerror("The range 1 to 1 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(NODmapRange(Vector1, Vector2, 0, 2), COLerror("The range 1 to 2 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(NODmapRange(Vector1, Vector2, 0, 0), COLerror("The range 1 to 0 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(NODmapRange(Vector2, Vector1, -4), COLerror("The range -3 to 0 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(NODmapRange(Vector2, Vector1, 0), COLerror("The range 1 to 0 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(NODmapRange(Vector2, Vector1, 0, 1), COLerror("The range 1 to 1 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(NODmapRange(Vector2, Vector1, 0, 2), COLerror("The range 1 to 2 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(NODmapRange(Vector2, Vector1, 0, 0), COLerror("The range 1 to 0 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));

   // Map to/from populated vectors (invalid cases)
   Vector2.child(0).setValue("1");
   Vector2.child(1).setValue("2");
   Vector2.child(2).setValue("3");
   TST_ASSERT_EXCEPTION(NODmapRange(Vector1, Vector2, -4), COLerror("The range -3 to 3 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(NODmapRange(Vector2, Vector1, -4), COLerror("The range -3 to 3 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(NODmapRange(Vector1, Vector2, 0, 4), COLerror("The range 1 to 4 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(NODmapRange(Vector2, Vector1, 0, 4), COLerror("The range 1 to 4 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));

   // Map full range
   NODmapRange(Vector1, Vector2, 0, 3);
   TST_ASSERT_EQUALS("A", Vector2.child(0).value());
   TST_ASSERT_EQUALS("B", Vector2.child(1).value());
   TST_ASSERT_EQUALS("C", Vector2.child(2).value());
   TST_ASSERT_EQUALS(3, Vector2.size());

   // Map full range (with source and dest swapped)
   Vector2.child(0).setValue("1");
   Vector2.child(1).setValue("2");
   Vector2.child(2).setValue("3");
   NODmapRange(Vector2, Vector1, 0, 3);
   TST_ASSERT_EQUALS("1", Vector1.child(0).value());
   TST_ASSERT_EQUALS("2", Vector1.child(1).value());
   TST_ASSERT_EQUALS("3", Vector1.child(2).value());
   TST_ASSERT_EQUALS("D", Vector1.child(3).value());
   TST_ASSERT_EQUALS(4, Vector1.size());

   // With start > 0
   Vector1.child(0).setValue("A");
   Vector1.child(1).setValue("B");
   Vector1.child(2).setValue("C");
   NODmapRange(Vector1, Vector2, 1);
   TST_ASSERT_EQUALS("1", Vector2.child(0).value());
   TST_ASSERT_EQUALS("B", Vector2.child(1).value());
   TST_ASSERT_EQUALS("C", Vector2.child(2).value());

   // With end < 3
   Vector2.child(0).setValue("1");
   Vector2.child(1).setValue("2");
   Vector2.child(2).setValue("3");
   NODmapRange(Vector2, Vector1, 1, 2);
   TST_ASSERT_EQUALS("A", Vector1.child(0).value());
   TST_ASSERT_EQUALS("2", Vector1.child(1).value());
   TST_ASSERT_EQUALS("C", Vector1.child(2).value());
   TST_ASSERT_EQUALS("D", Vector1.child(3).value());
   TST_ASSERT_EQUALS(4, Vector1.size());
}

// Test mapping a range from complex to complex.
static void NODTtestMapRangeComplexComplex()
{
   NODTcomplex Complex1("c1", false);
   NODTcomplex Complex2("c2", false);
   Complex1.child(0).setValue("A");
   Complex1.child(1).setValue("B");
   Complex1.child(2).child(0).setValue("C1");
   Complex1.child(2).child(1).setValue("C2");
   Complex1.child(3).setValue("D");

   // Invalid cases
   TST_ASSERT_EXCEPTION(NODmapRange(Complex1, Complex2, -4), COLerror("The range -3 to 4 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(NODmapRange(Complex1, Complex2, 0, 0), COLerror("Start of range must be less than or equal to end of range.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(NODmapRange(Complex2, Complex1, -4), COLerror("The range -3 to 4 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(NODmapRange(Complex2, Complex1, 0, 0), COLerror("Start of range must be less than or equal to end of range.", NOD_INDEX_OUT_OF_BOUNDS));

   // Map full range
   NODmapRange(Complex1, Complex2, 0, 4);
   TST_ASSERT_EQUALS("A", Complex2.child(0).value());
   TST_ASSERT_EQUALS("B", Complex2.child(1).value());
   TST_ASSERT_EQUALS("C1", Complex2.child(2).child(0).value());
   TST_ASSERT_EQUALS("C2", Complex2.child(2).child(1).value());
   TST_ASSERT_EQUALS("D", Complex2.child(3).value());
   TST_ASSERT_EQUALS(4, Complex2.size());

   // With start > 0
   Complex2.clear();
   NODmapRange(Complex1, Complex2, 1);
   TST_ASSERT_EQUALS("", Complex2.child(0).value());
   TST_ASSERT_EQUALS("B", Complex2.child(1).value());
   TST_ASSERT_EQUALS("C1", Complex2.child(2).child(0).value());
   TST_ASSERT_EQUALS("C2", Complex2.child(2).child(1).value());
   TST_ASSERT_EQUALS("D", Complex2.child(3).value());

   // With end < 3
   Complex2.clear();
   NODmapRange(Complex1, Complex2, 1, 2);
   TST_ASSERT_EQUALS("", Complex2.child(0).value());
   TST_ASSERT_EQUALS("B", Complex2.child(1).value());
   TST_ASSERT_EQUALS("", Complex2.child(2).child(0).value());
   TST_ASSERT_EQUALS("", Complex2.child(2).child(1).value());
   TST_ASSERT_EQUALS("", Complex2.child(3).value());

   // Null items shouldn't overwrite non-null values
   NODTcomplex Complex3("c3", false);
   NODmapRange(Complex3, Complex1, 0);
   TST_ASSERT_EQUALS("A", Complex1.child(0).value());
   TST_ASSERT_EQUALS("B", Complex1.child(1).value());
   TST_ASSERT_EQUALS("C1", Complex1.child(2).child(0).value());
   TST_ASSERT_EQUALS("C2", Complex1.child(2).child(1).value());
   TST_ASSERT_EQUALS("D", Complex1.child(3).value());
}

// Test mapping a range from vector to complex.
static void NODTtestMapRangeVectorComplex()
{
   // Don't want to get too complicated with these tests - I know
   // from writing NODmapRange that this will work as long as the above
   // tests work.
   NODTleafVector Vector("v");
   Vector.child(0).setValue("A");
   Vector.child(1).setValue("B");
   Vector.child(2).setValue("C");
   Vector.child(3).setValue("D");
   NODTcomplex Complex("c", false);
   Complex.child(0).setValue("1");
   Complex.child(1).setValue("2");
   Complex.child(2).child(0).setValue("3.1");
   Complex.child(2).child(1).setValue("3.2");
   Complex.child(3).setValue("4");

   NODmapRange(Vector, Complex, 0);
   TST_ASSERT_EQUALS("A", Complex.child(0).value());
   TST_ASSERT_EQUALS("B", Complex.child(1).value());
   TST_ASSERT_EQUALS("C", Complex.child(2).child(0).value());
   TST_ASSERT_EQUALS("3.2", Complex.child(2).child(1).value());
   TST_ASSERT_EQUALS("D", Complex.child(3).value());
}

// Test mapping a range from complex to vector.
static void NODTtestMapRangeComplexVector()
{
   // Don't want to get too complicated with these tests - I know
   // from writing NODmapRange that this will work as long as the above
   // tests work.
   NODTleafVector Vector("v");
   Vector.child(0).setValue("A");
   Vector.child(1).setValue("B");
   Vector.child(2).setValue("C");
   Vector.child(3).setValue("D");
   NODTcomplex Complex("c", false);
   Complex.child(0).setValue("1");
   Complex.child(1).setValue("2");
   Complex.child(2).child(0).setValue("3.1");
   Complex.child(2).child(1).setValue("3.2");
   Complex.child(3).setValue("4");

   NODmapRange(Complex, Vector, 0);
   TST_ASSERT_EQUALS(4, Vector.size());
   TST_ASSERT_EQUALS("1", Vector.child(0).value());
   TST_ASSERT_EQUALS("2", Vector.child(1).value());
   TST_ASSERT_EQUALS("3.1", Vector.child(2).value());
   TST_ASSERT_EQUALS("4", Vector.child(3).value());
}

// Test clearing range on a vector.
// NODclearRange has the same range validation code as NODmapRange, so we won't
// bother to test error cases.
static void NODTtestClearRangeVector()
{
   NODTleafVector Vector("v");
   Vector.child(0).setValue("A");
   Vector.child(1).setValue("B");
   Vector.child(2).setValue("C");
   Vector.child(3).setValue("D");
   TST_ASSERT_EQUALS(4, Vector.size());

   NODclearRange(Vector, 2);
   TST_ASSERT_EQUALS(2, Vector.size());
   TST_ASSERT_EQUALS("A", Vector.child(0).value());
   TST_ASSERT_EQUALS("B", Vector.child(1).value());

   Vector.child(2).setValue("C");
   Vector.child(3).setValue("D");
   TST_ASSERT_EQUALS(4, Vector.size());

   NODclearRange(Vector, 0, 2);
   TST_ASSERT_EQUALS(2, Vector.size());
   TST_ASSERT_EQUALS("C", Vector.child(0).value());
   TST_ASSERT_EQUALS("D", Vector.child(1).value());

   Vector.child(0).setValue("A");
   Vector.child(1).setValue("B");
   Vector.child(2).setValue("C");
   Vector.child(3).setValue("D");
   TST_ASSERT_EQUALS(4, Vector.size());

   NODclearRange(Vector, 1, 3);
   TST_ASSERT_EQUALS(2, Vector.size());
   TST_ASSERT_EQUALS("A", Vector.child(0).value());
   TST_ASSERT_EQUALS("D", Vector.child(1).value());
}

// Test clearing range on a complex.
static void NODTtestClearRangeComplex()
{
   NODTcomplex Complex("c", false);
   Complex.child(0).setValue("1");
   Complex.child(1).setValue("2");
   Complex.child(2).child(0).setValue("3.1");
   Complex.child(2).child(1).setValue("3.2");
   Complex.child(3).setValue("4");

   NODclearRange(Complex, 2);
   const NODplace* pConstPlace = (const NODplace*)&Complex;
   TST_ASSERT_EQUALS(4, pConstPlace->size());
   TST_ASSERT_EQUALS("1", pConstPlace->child(0).value());
   TST_ASSERT_EQUALS("2", pConstPlace->child(1).value());
   TST_ASSERT_EQUALS(true, pConstPlace->child(2).isNull());
   TST_ASSERT_EQUALS("", pConstPlace->child(3).value());

   Complex.child(2).child(0).setValue("3.1");
   Complex.child(2).child(1).setValue("3.2");
   Complex.child(3).setValue("4");

   NODclearRange(Complex, 0, 2);
   TST_ASSERT_EQUALS(4, Complex.size());
   TST_ASSERT_EQUALS(true, pConstPlace->child(0).isNull());
   TST_ASSERT_EQUALS(true, pConstPlace->child(1).isNull());
   TST_ASSERT_EQUALS(2, pConstPlace->child(2).size());
   TST_ASSERT_EQUALS("4", pConstPlace->child(3).value());

   Complex.child(0).setValue("1");
   Complex.child(1).setValue("2");

   NODclearRange(Complex, 1, 3);
   TST_ASSERT_EQUALS(4, pConstPlace->size());
   TST_ASSERT_EQUALS("1", pConstPlace->child(0).value());
   TST_ASSERT_EQUALS(true, pConstPlace->child(1).isNull());
   TST_ASSERT_EQUALS(true, pConstPlace->child(2).isNull());
   TST_ASSERT_EQUALS("4", pConstPlace->child(3).value());
}

// Test clearing a child on a vector.
static void NODTtestClearChildVector()
{
   NODTleafVector Vector("v");
   Vector.child(0).setValue("A");
   Vector.child(1).setValue("B");
   Vector.child(2).setValue("C");
   Vector.child(3).setValue("D");
   TST_ASSERT_EQUALS(4, Vector.size());

   NODclearChild(Vector, 2);
   TST_ASSERT_EQUALS(3, Vector.size());
   TST_ASSERT_EQUALS("A", Vector.child(0).value());
   TST_ASSERT_EQUALS("B", Vector.child(1).value());
   TST_ASSERT_EQUALS("D", Vector.child(2).value());

   NODclearChild(Vector, 0);
   TST_ASSERT_EQUALS(2, Vector.size());
   TST_ASSERT_EQUALS("B", Vector.child(0).value());
   TST_ASSERT_EQUALS("D", Vector.child(1).value());

   NODclearChild(Vector, 1);
   TST_ASSERT_EQUALS(1, Vector.size());
   TST_ASSERT_EQUALS("B", Vector.child(0).value());
}

// Test clearing a child on a complex.
static void NODTtestClearChildComplex()
{
   NODTcomplex Complex("c", false);
   Complex.child(0).setValue("1");
   Complex.child(1).setValue("2");
   Complex.child(2).child(0).setValue("3.1");
   Complex.child(2).child(1).setValue("3.2");
   Complex.child(3).setValue("4");

   NODclearChild(Complex, 2);
   const NODplace* pConstPlace = (const NODplace*)&Complex;
   TST_ASSERT_EQUALS(true, pConstPlace->child(2).isNull());

   NODclearChild(Complex, 0);
   TST_ASSERT_EQUALS(true, pConstPlace->child(0).isNull());

   NODclearChild(Complex, 3);
   TST_ASSERT_EQUALS(true, pConstPlace->child(3).isNull());

   TST_ASSERT_EQUALS(4, Complex.size());
}

static void NODTtestNamedChildRepeat(){
   NODTcomplex Complex("c", false);
   // Default children are Field1, Field2, Field3, and Field4

   TST_ASSERT_EQUALS(1, NODcountOfNamedChildRepeat(Complex, "Field1"));
   TST_ASSERT_EQUALS(1, NODcountOfNamedChildRepeat(Complex, "Field2"));
   TST_ASSERT_EQUALS(1, NODcountOfNamedChildRepeat(Complex, "Field3"));
   TST_ASSERT_EQUALS(1, NODcountOfNamedChildRepeat(Complex, "Field4"));
   TST_ASSERT_EQUALS(0, NODcountOfNamedChildRepeat(Complex, "Field5"));

   TST_ASSERT_EQUALS("Field1", NODTgetNamedChildRepeat(Complex, "Field1", 0).name());
   TST_ASSERT_EQUALS("Field2", NODTgetNamedChildRepeat(Complex, "Field2", 0).name());
   TST_ASSERT_EQUALS("Field3", NODTgetNamedChildRepeat(Complex, "Field3", 0).name());
   TST_ASSERT_EQUALS("Field4", NODTgetNamedChildRepeat(Complex, "Field4", 0).name());
   TST_ASSERT_EXCEPTION(NODTgetNamedChildRepeat(Complex, "Field2", 1), COLerror("Index 1 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));

   Complex.child("Field1").setValue("A");
   Complex.child("Field2").setValue("Foo");

   // Add some new children, with duplicate names.
   Complex.insert(0, "Field1", NODT_LEAF_NAME.c_str()).setValue("B");
   Complex.add("Field2", NODT_LEAF_NAME.c_str()).setValue("Bar");
   Complex.add("Field1", NODT_LEAF_NAME.c_str()).setValue("C");

   TST_ASSERT_EQUALS(3, NODcountOfNamedChildRepeat(Complex, "Field1"));
   TST_ASSERT_EQUALS(2, NODcountOfNamedChildRepeat(Complex, "Field2"));
   TST_ASSERT_EQUALS(1, NODcountOfNamedChildRepeat(Complex, "Field3"));
   TST_ASSERT_EQUALS(1, NODcountOfNamedChildRepeat(Complex, "Field4"));
   TST_ASSERT_EQUALS(0, NODcountOfNamedChildRepeat(Complex, "Field5"));

   TST_ASSERT_EQUALS("B", NODTgetNamedChildRepeat(Complex, "Field1", 0).value());
   TST_ASSERT_EQUALS("A", NODTgetNamedChildRepeat(Complex, "Field1", 1).value());
   TST_ASSERT_EQUALS("C", NODTgetNamedChildRepeat(Complex, "Field1", 2).value());
   TST_ASSERT_EXCEPTION(NODTgetNamedChildRepeat(Complex, "Field1", 3), COLerror("Index 3 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EQUALS("Foo", NODTgetNamedChildRepeat(Complex, "Field2", 0).value());
   TST_ASSERT_EQUALS("Bar", NODTgetNamedChildRepeat(Complex, "Field2", 1).value());
   TST_ASSERT_EXCEPTION(NODTgetNamedChildRepeat(Complex, "Field2", 2), COLerror("Index 2 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
}

TSTtestable* NODTtestOperations()
{
   TST_START_COLLECTION("NODTtestOperations")
      TST_ADD_TEST_FUNCTION(NODTtestCopySimpleSimple);
      TST_ADD_TEST_FUNCTION(NODTtestCopyVectorVector);
      TST_ADD_TEST_FUNCTION(NODTtestCopyComplexComplex);
      TST_ADD_TEST_FUNCTION(NODTtestCopyUntypedVectorUntypedVector);
      TST_ADD_TEST_FUNCTION(NODTtestCopyMismatch);
      TST_ADD_TEST_FUNCTION(NODTtestMapSimpleSimple);
      TST_ADD_TEST_FUNCTION(NODTtestMapSimpleVector);
      TST_ADD_TEST_FUNCTION(NODTtestMapSimpleComplex);
      TST_ADD_TEST_FUNCTION(NODTtestMapVectorSimple);
      TST_ADD_TEST_FUNCTION(NODTtestMapVectorVector);
      TST_ADD_TEST_FUNCTION(NODTtestMapVectorComplex);
      TST_ADD_TEST_FUNCTION(NODTtestMapComplexSimple);
      TST_ADD_TEST_FUNCTION(NODTtestMapComplexVector);
      TST_ADD_TEST_FUNCTION(NODTtestMapComplexComplex);
      TST_ADD_TEST_FUNCTION(NODTtestMapRangeVectorVector);
      TST_ADD_TEST_FUNCTION(NODTtestMapRangeComplexComplex);
      TST_ADD_TEST_FUNCTION(NODTtestMapRangeVectorComplex);
      TST_ADD_TEST_FUNCTION(NODTtestMapRangeComplexVector);
      TST_ADD_TEST_FUNCTION(NODTtestClearRangeVector);
      TST_ADD_TEST_FUNCTION(NODTtestClearRangeComplex);
      TST_ADD_TEST_FUNCTION(NODTtestClearChildVector);
      TST_ADD_TEST_FUNCTION(NODTtestClearChildComplex);
      TST_ADD_TEST_FUNCTION(NODTtestNamedChildRepeat);
   TST_END_FIXTURE
}
