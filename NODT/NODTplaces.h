#ifndef __NODT_PLACES_H__
#define __NODT_PLACES_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODTplaces
//
// Description:
//
// NODTplaces class.
//
// Author: Kevin Senn
// Date:   Wednesday, September 22nd, 2010 @ 12:07:49 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include <NOD/NODsimple.h>
#include <NOD/NODvector.h>
#include <NOD/NODcomplex.h>
#include <NOD/NODoperation.h>

const int NODT_TEST_PROTOCOL = 0;

static const COLstring NODT_EMPTY_NAME("");

static const COLstring NODT_PROTOCOL_NAME("nodt");

static const COLstring NODT_LEAF_NAME("leaf");
static const COLstring NODT_COMPLEX_NAME("complex_object");
static const COLstring NODT_LEAF_VECTOR_NAME("leaf_vecor");
static const COLstring NODT_UNTYPED_VECTOR_NAME("untyped_vector");

enum NODTnodeType
{
   NODT_LEAF = 0,
   NODT_COMPLEX_OBJECT = 1,
   NODT_LEAF_VECTOR = 2,
   NODT_UNTYPED_VECTOR = 3
};


class NODTleaf : public NODsimple
{
public:
   NODTleaf(const COLstring& Name, bool IsNull = true) : m_Name(Name), m_IsNull(IsNull) {}
   ~NODTleaf() {}

   int protocolType() const { return NODT_TEST_PROTOCOL; }
   int nodeType() const { return NODT_LEAF; }
   const COLstring& protocolName() const { return NODT_PROTOCOL_NAME; }
   const COLstring& nodeTypeName() const { return NODT_LEAF_NAME; }

   const COLstring& name() const { return m_Name; }

   bool isReadOnly() const { return false; }

   bool isNull() const { return m_IsNull; }
   void setIsNull(bool IsNull) { m_IsNull = IsNull; }

   void setValue(const COLstring& Value) { setIsNull(false); m_Value = Value; }
   const COLstring& value() const { return m_Value; }

   bool setTreeValue(const COLstring& TreeValue) { m_Value = TreeValue; return true; }

private:
   COLstring m_Value;
   COLstring m_Name;
   bool m_IsNull;

   NODTleaf(const NODTleaf& Orig); // not allowed
   NODTleaf& operator=(const NODTleaf& Orig); // not allowed
};


class NODTleafVector : public NODvector
{
public:
   NODTleafVector(const COLstring& Name) : m_Name(Name), m_LastRemovedChild(-1) {}
   ~NODTleafVector() {}

   int protocolType() const { return NODT_TEST_PROTOCOL; }
   int nodeType() const { return NODT_LEAF_VECTOR; }
   const COLstring& protocolName() const { return NODT_PROTOCOL_NAME; }
   const COLstring& nodeTypeName() const { return NODT_LEAF_VECTOR_NAME; }

   const COLstring& name() const { return m_Name; }

   bool isReadOnly() const { return false; }
   bool isHomogeneous() const { return true; }

   int m_LastRemovedChild;

protected:
   NODplace* newChild(int i, const char* Arg1, const char* Arg2);
   bool onRemoveChild(int i);
   // Only applies to vectors that start with some items.
   NODplace* initChild(int i) const { COLASSERT(false); return NULL; }

private:
   COLstring m_Name;

   NODTleafVector(const NODTleafVector& Orig); // not allowed
   NODTleafVector& operator=(const NODTleafVector& Orig); // not allowed
};

// Fictional grammar class for NODTcomplex.
class NODTcomplexField
{
public:
   NODTcomplexField() {}
   ~NODTcomplexField() {}

   COLstring m_Name;
   int m_Type;
};

class NODTcomplex : public NODcomplex
{
public:
   NODTcomplex(const COLstring& Name, bool IsNull = true);
   ~NODTcomplex() {}

   int protocolType() const { return NODT_TEST_PROTOCOL; }
   int nodeType() const { return NODT_COMPLEX_OBJECT; }
   const COLstring& protocolName() const { return NODT_PROTOCOL_NAME; }
   const COLstring& nodeTypeName() const { return NODT_COMPLEX_NAME; }

   const COLstring& name() const { return m_Name; }

   bool isReadOnly() const { return false; }
   bool favorIndexByName() const { return true; }

   bool isNull() const { return m_IsNull; }
   void setIsNull(bool IsNull) { m_IsNull = IsNull; }

   int m_LastRemovedChild;
   int m_LastNullifiedChild;

protected:
   NODplace* newChild(int i, const char* Arg1, const char* Arg2);
   NODplace* initChild(int i) const;
   bool onRemoveChild(int i);
   void onNullifyChild(int i);

private:
   COLstring m_Name;
   COLarray<NODTcomplexField> m_Grammar;
   bool m_IsNull;

   NODTcomplex(const NODTcomplex& Orig); // not allowed
   NODTcomplex& operator=(const NODTcomplex& Orig); // not allowed
};


class NODTuntypedVector : public NODvector
{
public:
   NODTuntypedVector(const COLstring& Name);
   ~NODTuntypedVector() {}

   int protocolType() const { return NODT_TEST_PROTOCOL; }
   int nodeType() const { return NODT_UNTYPED_VECTOR; }
   const COLstring& protocolName() const { return NODT_PROTOCOL_NAME; }
   const COLstring& nodeTypeName() const { return NODT_UNTYPED_VECTOR_NAME; }

   const COLstring& name() const { return m_Name; }

   bool isReadOnly() const { return false; }
   bool isHomogeneous() const { return false; }

   int m_LastRemovedChild;

protected:
   NODplace* newChild(int i, const char* Arg1, const char* Arg2);
   NODplace* initChild(int i) const;
   bool onRemoveChild(int i);

private:
   COLstring m_Name;
   COLarray<int> m_Grammar;

   NODTuntypedVector(const NODTuntypedVector& Orig); // not allowed
   NODTuntypedVector& operator=(const NODTuntypedVector& Orig); // not allowed
};

#endif // end of defensive include
