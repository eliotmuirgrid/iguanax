#ifndef __NOB_MEMOIZER_H__
#define __NOB_MEMOIZER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOBmemoizer
//
// Description:
//
// NOBmemoizer class.  Has the same functionality as NODmemoizer, with
// the additional ability to map node type/protocol ids to strings.
//
// Author: Kevin Senn
// Date:   Thursday, September 15th, 2011 @ 02:16:20 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include <NOD/NODmemoizer.h>

class NODplace;

class NOBmemoizer : public NODmemoizer
{
public:
   NOBmemoizer() {}
   // This is non virtual because we are assuming no classes will inherit from this class
   ~NOBmemoizer() {}

   void addProtocol(int ProtocolId, const COLstring& ProtocolName);
   void addNodeType(int ProtocolId, int NodeTypeId, const COLstring& NodeTypeName);

   const COLstring& protocol(int ProtocolId) const;
   const COLstring& nodeType(int ProtocolId, int NodeTypeId) const;

   // Making these lookups accessible just for serialization.
   const COLhashmap<int, COLstring>& protocolLookup() const { return m_ProtocolLookup; }
   const COLhashmap<int, COLstring>& nodeTypeLookup() const { return m_NodeTypeLookup; }

   // Just for when parsing a serialized NOBmemoizer.
   void addRawNodeType(int Key, const COLstring& NodeTypeName);

   // This tree might be an entry in a Lua table tree.  If so, m_KeyName
   // represents the key name for the entry.
   const COLstring& keyName() const { return m_KeyName; }
   void setKeyName(const char* pKeyName) { m_KeyName = pKeyName; }

private:
   COLhashmap<int, COLstring> m_ProtocolLookup;
   COLhashmap<int, COLstring> m_NodeTypeLookup;

   COLstring m_KeyName;

   NOBmemoizer(const NOBmemoizer& Orig); // not allowed
   NOBmemoizer& operator=(const NOBmemoizer& Orig); // not allowed
};

// Gets the memoizer from a NODplace, if it is a NOB node.
const NOBmemoizer* NOBgetMemoizer(const NODplace& NobNode);

#endif // end of defensive include
