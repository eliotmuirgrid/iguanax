//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODTplaces
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Wednesday, September 22nd, 2010 @ 12:07:49 PM
//
//---------------------------------------------------------------------------
#include "NODTprecomp.h"
#pragma hdrstop

#include "NODTplaces.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

NODplace* NODTleafVector::newChild(int i, const char* Arg1, const char* Arg2)
{
   // i == -1 means we're creating a dummy item.  In this case, it doesn't
   // matter either way.
   return new NODTleaf(NODT_EMPTY_NAME);
}

bool NODTleafVector::onRemoveChild(int i)
{
   // Just to test that this actually gets called.
   m_LastRemovedChild = i;
   return true;
}

NODTcomplex::NODTcomplex(const COLstring& Name, bool IsNull)
 : m_Name(Name), m_LastRemovedChild(-1), m_LastNullifiedChild(-1), m_IsNull(IsNull)
{
   // We'll use fictional "grammar" that would already be constructed by
   // this point normally.
   // A more realistic example would be to set an HL7 segment node's size
   // to the number of fields in the segment.
   m_Grammar.resize(4);
   m_Grammar[0].m_Name = "Field1";
   m_Grammar[0].m_Type = NODT_LEAF;
   m_Grammar[1].m_Name = "Field2";
   m_Grammar[1].m_Type = NODT_LEAF;
   m_Grammar[2].m_Name = "Field3";
   m_Grammar[2].m_Type = NODT_LEAF_VECTOR;
   m_Grammar[3].m_Name = "Field4";
   m_Grammar[3].m_Type = NODT_LEAF;

   setSize(m_Grammar.size());
}

// Most Complex nodes will just throw an error here, as it usually doesn't make sense
// to modify the grammar on the fly.  But for testing purposes, we'll implement it.
//
NODplace* NODTcomplex::newChild(int i, const char* Arg1, const char* Arg2)
{
   if (!Arg1 || !Arg2)
   {
      COL_ERROR_STREAM_PLAIN(COL_T("name, type"), NOD_BAD_ARGUMENTS);
   }

   m_Grammar.insert(i);
   NODTcomplexField& FieldGrammar = m_Grammar[i];
   FieldGrammar.m_Name = Arg1;

   if (Arg2 == NODT_LEAF_NAME)
   {
      FieldGrammar.m_Type = NODT_LEAF;
   }
   else if (Arg2 == NODT_COMPLEX_NAME)
   {
      FieldGrammar.m_Type = NODT_COMPLEX_OBJECT;
   }
   else if (Arg2 == NODT_LEAF_VECTOR_NAME)
   {
      FieldGrammar.m_Type = NODT_LEAF_VECTOR;
   }
   else if (Arg2 == NODT_UNTYPED_VECTOR_NAME)
   {
      FieldGrammar.m_Type = NODT_UNTYPED_VECTOR;
   }
   else
   {
      COL_ERROR_STREAM_PLAIN(COL_T("Unrecognized/invalid child type '") << Arg2 << COL_T("'."), COLerror::PreCondition);
   }

   return initChild(i);
}

NODplace* NODTcomplex::initChild(int i) const
{
   COLPRECONDITION(i < m_Grammar.size());
   const NODTcomplexField& FieldGrammar = m_Grammar[i];
   NODplace* pChild = NULL;
   switch (FieldGrammar.m_Type)
   {
   case NODT_LEAF:
      return new NODTleaf(FieldGrammar.m_Name);
   case NODT_LEAF_VECTOR:
      return new NODTleafVector(FieldGrammar.m_Name);
   case NODT_COMPLEX_OBJECT:
      return new NODTcomplex(FieldGrammar.m_Name);
   case NODT_UNTYPED_VECTOR:
      return new NODTuntypedVector(FieldGrammar.m_Name);
   default:
      COLASSERT(false);
      return NULL;
   }
}

// Most Complex nodes won't implement this, as it usually doesn't make sense
// to modify the grammar on the fly.  But for testing purposes, we'll implement it.
//
bool NODTcomplex::onRemoveChild(int i)
{
   COLPRECONDITION(i < m_Grammar.size());
   m_Grammar.remove(i);
   m_LastRemovedChild = i;
   return true;
}

void NODTcomplex::onNullifyChild(int i)
{
   // Just to test that this actually gets called.
   m_LastNullifiedChild = i;
}

NODTuntypedVector::NODTuntypedVector(const COLstring& Name)
 : m_Name(Name), m_LastRemovedChild(-1)
{
   m_Grammar.push_back(NODT_LEAF);
   m_Grammar.push_back(NODT_COMPLEX_OBJECT);

   setSize(m_Grammar.size());
}

NODplace* NODTuntypedVector::newChild(int i, const char* Arg1, const char* Arg2)
{
   if (!Arg1)
   {
      COL_ERROR_STREAM_PLAIN(COL_T("type"), NOD_BAD_ARGUMENTS);
   }

   if (Arg1 == NODT_LEAF_NAME)
   {
      m_Grammar.insert(NODT_LEAF, i);
   }
   else if (Arg1 == NODT_COMPLEX_NAME)
   {
      m_Grammar.insert(NODT_COMPLEX_OBJECT, i);
   }
   else if (Arg1 == NODT_LEAF_VECTOR_NAME)
   {
      m_Grammar.insert(NODT_LEAF_VECTOR, i);
   }
   else if (Arg1 == NODT_UNTYPED_VECTOR_NAME)
   {
      m_Grammar.insert(NODT_UNTYPED_VECTOR, i);
   }
   else
   {
      COL_ERROR_STREAM_PLAIN(COL_T("Unrecognized/invalid child type '") << Arg1 << COL_T("'."), COLerror::PreCondition);
   }

   return initChild(i);
}

NODplace* NODTuntypedVector::initChild(int i) const
{
   COLPRECONDITION(i < m_Grammar.size());
   NODplace* pChild = NULL;
   switch (m_Grammar[i])
   {
   case NODT_LEAF:
      return new NODTleaf(NODT_EMPTY_NAME);
   case NODT_LEAF_VECTOR:
      return new NODTleafVector(NODT_EMPTY_NAME);
   case NODT_COMPLEX_OBJECT:
      return new NODTcomplex(NODT_EMPTY_NAME);
   case NODT_UNTYPED_VECTOR:
      return new NODTuntypedVector(NODT_EMPTY_NAME);
   default:
      COLASSERT(false);
      return NULL;
   }
}

bool NODTuntypedVector::onRemoveChild(int i)
{
   // Just to test that this actually gets called.
   m_LastRemovedChild = i;
   return true;
}
