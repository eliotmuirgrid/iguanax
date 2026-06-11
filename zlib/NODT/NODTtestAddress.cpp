//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODTtestAddress
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Thursday, September 23rd, 2010 @ 11:54:34 AM
//
//---------------------------------------------------------------------------
#include "NODTprecomp.h"
#pragma hdrstop

#include "NODTtestAddress.h"
#include "NODTplaces.h"

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <NOD/NODaddress.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


static void NODTtestAddressBasic()
{
   // Create a tree
   COLref<NODTcomplex> pRoot = new NODTcomplex("Root");

   NODaddress Address;
   COLstring AddressString;
   COLostream AddressStream(AddressString);

   TST_ASSERT_EQUALS(0, Address.depth());
   TST_ASSERT(!Address.parent());

   // Set parent
   Address.setParent(pRoot.get());
   TST_ASSERT_EQUALS(0, Address.depth());
   TST_ASSERT_EQUALS(pRoot.get(), Address.parent());
   TST_ASSERT_EQUALS(pRoot.get(), &NODoutputAddress(AddressStream, pRoot->name(), Address));
   TST_ASSERT_EQUALS("Root", AddressString);
   AddressString.clear();

   // Push '3' (a Lua '3', which is a C++ '2')
   Address.pushAddressIndex(3);
   TST_ASSERT_EQUALS(1, Address.depth());
   TST_ASSERT_EQUALS(pRoot.get(), Address.parent());
   TST_ASSERT_EQUALS(&pRoot->child(2), &NODoutputAddressIndex(AddressStream, Address.addressIndex(0), *Address.parent()));
   TST_ASSERT_EQUALS(".Field3", AddressString);
   AddressString.clear();
   TST_ASSERT_EQUALS(&pRoot->child(2), &NODoutputAddress(AddressStream, pRoot->name(), Address));
   TST_ASSERT_EQUALS("Root.Field3", AddressString);
   AddressString.clear();

   // Push '2' (a Lua '2', which is a C++ '1').
   Address.pushAddressIndex(2);
   TST_ASSERT_EQUALS(2, Address.depth());
   TST_ASSERT_EQUALS(pRoot.get(), Address.parent());
   pRoot->child(2).add(NULL, NULL);
   pRoot->child(2).add(NULL, NULL);
   TST_ASSERT_EQUALS(&pRoot->child(2).child(1), &NODoutputAddress(AddressStream, pRoot->name(), Address));
   TST_ASSERT_EQUALS("Root.Field3[2]", AddressString);
   AddressString.clear();
}

TSTtestable* NODTtestAddress()
{
   TST_START_COLLECTION("NODTtestAddress")
      TST_ADD_TEST_FUNCTION(NODTtestAddressBasic);
   TST_END_FIXTURE
}
