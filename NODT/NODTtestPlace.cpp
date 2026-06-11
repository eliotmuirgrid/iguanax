//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODTtestPlace
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Wednesday, September 22nd, 2010 @ 10:11:57 AM
//
//---------------------------------------------------------------------------
#include "NODTprecomp.h"
#pragma hdrstop

#include "NODTtestPlace.h"
#include "NODTplaces.h"

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


static void NODTtestSimple()
{
   NODTleaf Leaf("Field1", false);

   TST_ASSERT_EQUALS("Field1", Leaf.name());
   TST_ASSERT_EQUALS("", Leaf.value());
   TST_ASSERT_EQUALS(false, Leaf.isNull());
   TST_ASSERT_EQUALS(0, Leaf.size());
   TST_ASSERT_EQUALS(true, Leaf.isSimple());
   TST_ASSERT_EQUALS(false, Leaf.isVector());
   TST_ASSERT_EQUALS(false, Leaf.isComplex());

   Leaf.setValue("foo");
   TST_ASSERT_EQUALS("foo", Leaf.value());

   COLcout << Leaf << newline;

   Leaf.setTreeValue("bar");
   TST_ASSERT_EQUALS("bar", Leaf.value());

   Leaf.clear();
   TST_ASSERT_EQUALS(false, Leaf.isNull());
   TST_ASSERT_EQUALS("", Leaf.value());

   TST_ASSERT_EXCEPTION(Leaf.remove(0), COLerror("This is a leaf node without children.", NOD_IS_LEAF));
   TST_ASSERT_EXCEPTION(Leaf.add("foo", "bar"), COLerror("This is a leaf node to which children cannot be added.", NOD_IS_LEAF));
   TST_ASSERT_EXCEPTION(Leaf.insert(0, "foo", "bar"), COLerror("This is a leaf node to which children cannot be added.", NOD_IS_LEAF));
   TST_ASSERT_EXCEPTION(Leaf.child(0), COLerror("This is a leaf node without children.", NOD_IS_LEAF));
   TST_ASSERT_EXCEPTION(Leaf.child("Field1"), COLerror("This is a leaf node without children.", NOD_IS_LEAF));
}

static void NODTtestHomogeneousVector()
{
   NODTleafVector LeafVector("Field1");
   
   TST_ASSERT_EQUALS("Field1", LeafVector.name());
   TST_ASSERT_EQUALS(true, LeafVector.isNull());
   TST_ASSERT_EQUALS(0, LeafVector.size());
   TST_ASSERT_EQUALS(false, LeafVector.isSimple());
   TST_ASSERT_EQUALS(true, LeafVector.isVector());
   TST_ASSERT_EQUALS(false, LeafVector.isComplex());
   TST_ASSERT_EQUALS(false, LeafVector.setTreeValue("foo"));

   // Explicit addition of child.
   LeafVector.add(NULL, NULL);
   TST_ASSERT_EQUALS(1, LeafVector.size());
   TST_ASSERT_EQUALS(true, LeafVector.child(0).isNull());
   LeafVector.child(0).setValue("foo");
   TST_ASSERT_EQUALS(false, LeafVector.child(0).isNull());
   TST_ASSERT_EQUALS(1, LeafVector.size());
   TST_ASSERT_EQUALS("foo", LeafVector.child(0).value());

   // Implicit addition of child.
   LeafVector.child(1).setValue("bar");
   TST_ASSERT_EQUALS(2, LeafVector.size());
   TST_ASSERT_EQUALS(false, LeafVector.child(1).isNull());
   TST_ASSERT_EQUALS("bar", LeafVector.child(1).value());

   // Implicit addition of several children.
   LeafVector.child(4).setValue("foo2");
   TST_ASSERT_EQUALS(5, LeafVector.size());
   TST_ASSERT_EQUALS(true, LeafVector.child(2).isNull());
   TST_ASSERT_EQUALS(true, LeafVector.child(3).isNull());
   TST_ASSERT_EQUALS(false, LeafVector.child(4).isNull());
   TST_ASSERT_EQUALS("foo2", LeafVector.child(4).value());

   COLcout << LeafVector << newline;

   // Insertion of child.
   LeafVector.insert(1, NULL, NULL);
   LeafVector.child(1).setValue("bar2");
   TST_ASSERT_EQUALS(6, LeafVector.size());
   TST_ASSERT_EQUALS("bar2", LeafVector.child(1).value());

   // Removal of child.
   LeafVector.remove(1);
   TST_ASSERT_EQUALS(1, LeafVector.m_LastRemovedChild);
   TST_ASSERT_EQUALS(5, LeafVector.size());

   // Clear.
   LeafVector.clear();
   TST_ASSERT_EQUALS(true, LeafVector.isNull());
   TST_ASSERT_EQUALS(4, LeafVector.m_LastRemovedChild);
   TST_ASSERT_EQUALS(0, LeafVector.size());

   // Old values are really gone after the clear.
   LeafVector.child(4).setValue("");
   TST_ASSERT_EQUALS(5, LeafVector.size());
   TST_ASSERT_EQUALS(true, LeafVector.child(0).isNull());
   TST_ASSERT_EQUALS(true, LeafVector.child(1).isNull());
   TST_ASSERT_EQUALS(true, LeafVector.child(2).isNull());
   TST_ASSERT_EQUALS(true, LeafVector.child(3).isNull());
   TST_ASSERT_EQUALS(false, LeafVector.child(4).isNull());

   // Index out of bounds.
   TST_ASSERT_EXCEPTION(LeafVector.child(-1), COLerror("Index -1 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(LeafVector.insert(-1, NULL, NULL), COLerror("Index -1 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(LeafVector.remove(-1), COLerror("Index -1 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(LeafVector.insert(11, NULL, NULL), COLerror("Index 11 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(LeafVector.remove(11), COLerror("Index 11 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));

   // Indexing by name
   TST_ASSERT_EXCEPTION(LeafVector.child("Field1"), COLerror("This node does not have named members.", NOD_NOT_COMPLEX_TYPE));

   // Implicit addition on a const vector - should fail.
   const NODplace* pConstPlace = &LeafVector;
   TST_ASSERT_EXCEPTION(pConstPlace->child(11), COLerror("Index 11 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));

   // Getting a dummy grammar node for an item in the vector.
   COLref<NODplace> Dummy = LeafVector.createDummyItem();
   TST_ASSERT_EQUALS(true, Dummy->isNull());
   TST_ASSERT_EQUALS(true, Dummy->isSimple());
}

static void NODTtestComplex()
{
   NODTcomplex Complex("Root", false);

   TST_ASSERT_EQUALS("Root", Complex.name());
   TST_ASSERT_EQUALS(false, Complex.isNull());
   TST_ASSERT_EQUALS(4, Complex.size());
   TST_ASSERT_EQUALS(true, Complex.isComplex());
   TST_ASSERT_EQUALS(false, Complex.setTreeValue("foo"));

   // Ensure children start off as null.  We have to do this with a const reference, to
   // avoid automatically de-nullifying the children.
   const NODplace* pConstPlace = &Complex;
   TST_ASSERT_EQUALS(true, pConstPlace->child(0).isNull());
   TST_ASSERT_EQUALS(true, pConstPlace->child(2).isNull());

   // Ensure children do not automatically become non-null when we access them in a non-const context.
   TST_ASSERT_EQUALS(true, Complex.child(0).isNull());
   TST_ASSERT_EQUALS(true, Complex.child(1).isNull());
   TST_ASSERT_EQUALS(true, Complex.child(2).isNull()); // will still be null, since it contains no items.

   // Fiddle with the children.
   Complex.child(0).setValue("foo");
   TST_ASSERT_EQUALS("foo", Complex.child(0).value());
   Complex.child(2).child(0);
   TST_ASSERT_EQUALS(false, Complex.child(2).isNull());
   TST_ASSERT_EQUALS(1, Complex.child(2).size());

   // Size should not have changed.
   TST_ASSERT_EQUALS(4, Complex.size());

   // Nullify children.
   Complex.nullify(0);
   TST_ASSERT_EQUALS(0, Complex.m_LastNullifiedChild);
   TST_ASSERT_EQUALS(-1, Complex.m_LastRemovedChild); // shouldn't have removed the member.
   TST_ASSERT_EQUALS(true, pConstPlace->child(0).isNull());
   TST_ASSERT_EQUALS("", pConstPlace->child(0).value());
   Complex.nullify(2);
   TST_ASSERT_EQUALS(2, Complex.m_LastNullifiedChild);
   TST_ASSERT_EQUALS(-1, Complex.m_LastRemovedChild); // shouldn't have removed the member.
   TST_ASSERT_EQUALS(true, pConstPlace->child(2).isNull());
   TST_ASSERT_EQUALS(0, pConstPlace->child(2).size());

   // Clear all children.
   Complex.child(0).setValue("foo");
   Complex.child(2).child(1).setValue("bar");
   Complex.clear();
   TST_ASSERT_EQUALS(3, Complex.m_LastNullifiedChild);
   TST_ASSERT_EQUALS(-1, Complex.m_LastRemovedChild); // shouldn't have removed any members.
   TST_ASSERT_EQUALS(false, Complex.isNull());
   TST_ASSERT_EQUALS(4, Complex.size());
   TST_ASSERT_EQUALS(true, pConstPlace->child(0).isNull());
   TST_ASSERT_EQUALS("",   pConstPlace->child(0).value());
   TST_ASSERT_EQUALS(true, pConstPlace->child(1).isNull());
   TST_ASSERT_EQUALS(true, pConstPlace->child(2).isNull());
   TST_ASSERT_EQUALS(0,    pConstPlace->child(2).size());
   TST_ASSERT_EQUALS(true, pConstPlace->child(3).isNull());

   // Index out of bounds
   TST_ASSERT_EXCEPTION(Complex.child(-1), COLerror("Index -1 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(Complex.child(4), COLerror("Index 4 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(Complex.insert(-1, NULL, NULL), COLerror("Index -1 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(Complex.remove(-1), COLerror("Index -1 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(Complex.insert(5, NULL, NULL), COLerror("Index 5 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));
   TST_ASSERT_EXCEPTION(Complex.remove(5), COLerror("Index 5 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));

   // Adding to the grammar.
   Complex.add("Field5", NODT_COMPLEX_NAME.c_str());
   TST_ASSERT_EQUALS(5, Complex.size());
   TST_ASSERT_EQUALS(false, pConstPlace->child(4).isNull());
   TST_ASSERT_EQUALS(true, Complex.child(4).isComplex());
   TST_ASSERT_EQUALS(4, Complex.child(4).size());
   TST_ASSERT_EQUALS(true, pConstPlace->child(4).child(0).isNull());
   TST_ASSERT_EQUALS(true, pConstPlace->child(4).child(1).isNull());
   TST_ASSERT_EQUALS(true, pConstPlace->child(4).child(2).isNull());
   TST_ASSERT_EQUALS(true, pConstPlace->child(4).child(3).isNull());

   // Nullifying complex child.
   Complex.child(4).child(0).setValue("foo");
   TST_ASSERT_EQUALS(false, pConstPlace->child(4).isNull());
   Complex.nullify(4);
   TST_ASSERT_EQUALS(5, Complex.size());
   TST_ASSERT_EQUALS(true, pConstPlace->child(4).isNull());
   TST_ASSERT_EQUALS(4, pConstPlace->child(4).size());
   TST_ASSERT_EQUALS(true, pConstPlace->child(4).child(0).isNull());
   TST_ASSERT_EQUALS("", pConstPlace->child(4).child(0).value());

   // Nullifying named child.
   Complex.child(0).setValue("foo");
   Complex.nullify("Field1");
   TST_ASSERT_EQUALS(true, pConstPlace->child(0).isNull());

   // Inserting into the grammar.
   Complex.insert(0, "Field0", NODT_LEAF_NAME.c_str());
   TST_ASSERT_EQUALS(6, Complex.size());
   TST_ASSERT_EQUALS(true, pConstPlace->child(0).isNull());
   TST_ASSERT_EQUALS(true, Complex.child(1).isSimple());
   Complex.insert(6, "Field6", NODT_LEAF_NAME.c_str());
   TST_ASSERT_EQUALS(7, Complex.size());
   TST_ASSERT_EQUALS(true, pConstPlace->child(6).isNull());
   TST_ASSERT_EQUALS(true, Complex.child(6).isSimple());

   COLcout << Complex << newline;

   // Indexing by name.
   Complex.clear();
   Complex.child(0).setValue("foo");
   TST_ASSERT_EQUALS(false, pConstPlace->child("Field0").isNull());
   TST_ASSERT_EQUALS("foo", pConstPlace->child("Field0").value());
   TST_ASSERT_EQUALS(true,  pConstPlace->child("Field1").isNull());
   TST_ASSERT_EQUALS("",    pConstPlace->child("Field1").value());
   TST_ASSERT_EQUALS(&Complex.child(0),      &Complex.child("Field0"));
   TST_ASSERT_EQUALS(&Complex.child(1),      &Complex.child("Field1"));
   TST_ASSERT_EQUALS(&pConstPlace->child(2), &Complex.child("Field2"));
   TST_ASSERT_EQUALS(&Complex.child(3),      &pConstPlace->child("Field3"));
   TST_ASSERT_EQUALS(&pConstPlace->child(4), &pConstPlace->child("Field4"));
   TST_ASSERT_EQUALS(&Complex.child(5),      &Complex.child("Field5"));
   TST_ASSERT_EQUALS(&Complex.child(6),      &Complex.child("Field6"));
   TST_ASSERT_EXCEPTION(Complex.child("foo"), COLerror("'foo' member does not exist. Named members are Field0, Field1, Field2, Field3, Field4, Field5, Field6.", NOD_NO_SUCH_MEMBER));

   // Add a duplicate name.  We allow it - indexing by name will return the first match.
   Complex.add("Field6", NODT_LEAF_NAME.c_str());
   TST_ASSERT_EQUALS(&Complex.child(6), &Complex.child("Field6"));
   TST_ASSERT_EQUALS("Field6", Complex.child(6).name());
   TST_ASSERT_EQUALS("Field6", Complex.child(7).name());

   // Removing from the grammar.
   Complex.remove(7);
   TST_ASSERT_EQUALS(7, Complex.size());
   TST_ASSERT_EQUALS(&Complex.child(0),      &Complex.child("Field0"));
   TST_ASSERT_EQUALS(&Complex.child(1),      &Complex.child("Field1"));
   TST_ASSERT_EQUALS(&pConstPlace->child(2), &Complex.child("Field2"));
   TST_ASSERT_EQUALS(&Complex.child(3),      &pConstPlace->child("Field3"));
   TST_ASSERT_EQUALS(&pConstPlace->child(4), &pConstPlace->child("Field4"));
   TST_ASSERT_EQUALS(&Complex.child(5),      &Complex.child("Field5"));
   TST_ASSERT_EQUALS(&Complex.child(6),      &Complex.child("Field6"));
   Complex.remove(0);
   TST_ASSERT_EQUALS(6, Complex.size());
   TST_ASSERT_EQUALS(&Complex.child(0),      &Complex.child("Field1"));
   TST_ASSERT_EQUALS(&pConstPlace->child(1), &Complex.child("Field2"));
   TST_ASSERT_EQUALS(&Complex.child(2),      &pConstPlace->child("Field3"));
   TST_ASSERT_EQUALS(&pConstPlace->child(3), &pConstPlace->child("Field4"));
   TST_ASSERT_EQUALS(&Complex.child(4),      &Complex.child("Field5"));
   TST_ASSERT_EQUALS(&Complex.child(5),      &Complex.child("Field6"));

   // Removing named member from the grammar.
   Complex.remove("Field1");
   TST_ASSERT_EQUALS(5, Complex.size());
   TST_ASSERT_EXCEPTION(Complex.child("Field1"), COLerror("'Field1' member does not exist. Named members are Field2, Field3, Field4, Field5, Field6.", NOD_NO_SUCH_MEMBER));

   // Add invalid type.
   TST_ASSERT_EXCEPTION(Complex.add("Field7", "foo"), COLerror("Unrecognized/invalid child type 'foo'.", COLerror::PreCondition));
}

static void NODTtestHeterogeneousVector()
{
   NODTuntypedVector Untyped("Root");

   TST_ASSERT_EQUALS("Root", Untyped.name());
   TST_ASSERT_EQUALS(false, Untyped.isNull());
   TST_ASSERT_EQUALS(2, Untyped.size());
   TST_ASSERT_EQUALS(true, Untyped.isVector());
   TST_ASSERT_EQUALS(false, Untyped.isHomogeneous());
   TST_ASSERT_EQUALS(false, Untyped.setTreeValue("foo"));
   TST_ASSERT_EQUALS(NODT_LEAF, Untyped.child(0).nodeType());
   TST_ASSERT_EQUALS(NODT_COMPLEX_OBJECT, Untyped.child(1).nodeType());

   // Implicitly add a child (not allowed)
   TST_ASSERT_EXCEPTION(Untyped.child(2), COLerror("Index 2 is out of bounds.", NOD_INDEX_OUT_OF_BOUNDS));

   // Indexing by name (not allowed)
   TST_ASSERT_EXCEPTION(Untyped.child("Root"), COLerror("This node does not have named members.", NOD_NOT_COMPLEX_TYPE));

   // Explicitly add a child
   Untyped.add(NODT_LEAF_VECTOR_NAME.c_str(), NULL);
   TST_ASSERT_EQUALS(3, Untyped.size());
   TST_ASSERT_EQUALS(NODT_LEAF_VECTOR, Untyped.child(2).nodeType());

   // Insert a child
   Untyped.insert(2, NODT_LEAF_NAME.c_str(), NULL);
   TST_ASSERT_EQUALS(4, Untyped.size());
   TST_ASSERT_EQUALS(NODT_LEAF, Untyped.child(2).nodeType());
   TST_ASSERT_EQUALS(NODT_LEAF_VECTOR, Untyped.child(3).nodeType());

   COLcout << Untyped << newline;

   // Remove a child
   Untyped.remove(1);
   TST_ASSERT_EQUALS(3, Untyped.size());
}

TSTtestable* NODTtestPlace()
{
   TST_START_COLLECTION("NODTtestPlace")
      TST_ADD_TEST_FUNCTION(NODTtestSimple);
      TST_ADD_TEST_FUNCTION(NODTtestHomogeneousVector);
      TST_ADD_TEST_FUNCTION(NODTtestComplex);
      TST_ADD_TEST_FUNCTION(NODTtestHeterogeneousVector);
   TST_END_FIXTURE
}
