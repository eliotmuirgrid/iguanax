#ifndef __NOD_ADDRESS_H__
#define __NOD_ADDRESS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODaddress
//
// Description:
//
// The NODe library approach means we do not store parent pointers.  Sometimes
// it's helpful to have the complete path from the top to the bottom.  This is
// what this data-structure is for.
//
// It uses a simple format to maintain the address - just a vector of indexes.
// In the Lua bindings it means that when the customer does something like:
// Message.PID where PID is repeating then there are two address indexes entered - 0 for
// the first child followed by the index of the complex member with that index.
//
// Author: Eliot Muir
// Date:   Wednesday, June 16th, 2010 @ 11:40:58 AM
//
//---------------------------------------------------------------------------

#include <COL/COLlist.h>

#include "NODplace.h"

class NODaddress 
{
public:
   NODaddress();
   // This is non virtual because we are assuming no classes will inherit from this class
   // default copy constructor and assignment operator should work fine.
   ~NODaddress();

   void setParent(NODplace* pNewValue);

   const NODplace* parent() const { return m_pParent.get(); }

   int depth() const;
   int addressIndex(int i) const;
   void pushAddressIndex(int i);

private:
   COLref<NODplace> m_pParent;
   COLarray<int> m_Address;
};

COLostream& operator<<(COLostream& Stream, const NODaddress& Address);

const NODplace& NODoutputAddress(COLostream& Stream, const COLstring& SourceName, const NODaddress& Address);

// Writes the address index to the stream, and returns
// the child specified by the address index.
const NODplace& NODoutputAddressIndex(COLostream& Stream, int AddressIndex, const NODplace& Place);


#endif // end of defensive include
