#ifndef __NOB_NODE_H__
#define __NOB_NODE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOBnode
//
// Description:
//
// NOBnode class.  LEGACY.  Use NOBcomposite and NOBsimple instead.
//
// Author: Kevin Senn
// Date:   Wednesday, November 3rd, 2010 @ 11:44:42 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLauto.h>

#include <NOD/NODplaceComposite.h>

class NOBnode : public NODplaceComposite
{
public:
   NOBnode()
    : m_GrammarType(NOD_SIMPLE),
      m_NodeType(-1),
      m_ProtocolType(-1),
      m_IsNamed(true),
      m_ShouldEscapeValue(true),
      m_IsHomogeneous(true),
      m_FavorIndexByName(false),
      m_GracefulNoSuchMember(false),
      m_IsNull(false)
   {}

   // This is non virtual because we are assuming no classes will inherit from this class
   ~NOBnode() {}

   NODgrammarType grammarType() const { return m_GrammarType; }
   int protocolType() const { return m_ProtocolType; }
   int nodeType() const { return m_NodeType; }
   const COLstring& protocolName() const { return m_ProtocolName; }
   const COLstring& nodeTypeName() const { return m_NodeTypeName; }

   const COLstring& name() const { return m_Name; }

   bool isNull() const { return m_IsNull; }
   void setIsNull(bool IsNull) { m_IsNull = IsNull; }

   bool isReadOnly() const { return true; }
   bool isNamed() const { return m_IsNamed; }
   bool isHomogeneous() const { return m_IsHomogeneous; }
   bool favorIndexByName() const { return m_FavorIndexByName; }
   bool gracefulNoSuchMember() const { return m_GracefulNoSuchMember; }

   void setValue(const COLstring& Value) { COLASSERT(!isReadOnly()); }
   const COLstring& value() const { return m_Value; }
   bool shouldEscapeValue() const { return m_ShouldEscapeValue; }

   // These functions are not part of the NODplace interface.
   void setGrammarType(NODgrammarType GrammarType)         { m_GrammarType = GrammarType; }
   void setNodeType(int NodeType)                          { m_NodeType = NodeType; }
   void setProtocolType(int ProtocolType)                  { m_ProtocolType = ProtocolType; }
   void setNodeTypeName(const COLstring& NodeTypeName)     { m_NodeTypeName = NodeTypeName; }
   void setProtocolName(const COLstring& ProtocolName)     { m_ProtocolName = ProtocolName; }
   void setName(const COLstring& Name)                     { m_Name = Name; }
   void setIsNamed(bool IsNamed)                           { m_IsNamed = IsNamed; }
   void setCachedValue(const COLstring& Value)             { COLASSERT(isSimple()); m_Value = Value; }
   void setShouldEscapeValue(bool ShouldEscapeValue)       { COLASSERT(isSimple()); m_ShouldEscapeValue = ShouldEscapeValue; }
   void setIsHomogeneous(bool IsHomogeneous)               { COLASSERT(isVector()); m_IsHomogeneous = IsHomogeneous; }
   void setFavorIndexByName(bool FavorIndexByName)         { COLASSERT(isComplex()); m_FavorIndexByName = FavorIndexByName; }
   void setGracefulNoSuchMember(bool GracefulNoSuchMember) { COLASSERT(isComplex()); m_GracefulNoSuchMember = GracefulNoSuchMember; }
   NOBnode* addChild();
   NOBnode* initDummyItem();

protected:
   // An index of -1 indicates that a "dummy" item is being created for
   // a homogeneous vector (see NODplace::createDummyItem()).
   NODplace* newChild(int i, const char* Arg1, const char* Arg2);

   NODplace* initChild(int i) const { return new NOBnode(); }

private:
   NODgrammarType m_GrammarType;
   int m_NodeType;
   int m_ProtocolType;
   COLstring m_NodeTypeName;
   COLstring m_ProtocolName;
   COLstring m_Name;
   bool m_IsNamed;
   COLstring m_Value;
   bool m_ShouldEscapeValue;
   bool m_IsHomogeneous;
   bool m_FavorIndexByName;
   bool m_GracefulNoSuchMember;
   bool m_IsNull;

   // A dummy item node, if this node has one.
   COLref<NOBnode> m_pDummyItem;

   NOBnode(const NOBnode& Orig); // not allowed
   NOBnode& operator=(const NOBnode& Orig); // not allowed
};

#endif // end of defensive include
