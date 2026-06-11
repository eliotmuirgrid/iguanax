//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UNITlistTests
//
// Author: Eliot Muir
//
// Date: Wed 17 Mar 2021 13:18:26 EDT
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------
#include "UNITlistTests.h"

#include <FIL/FILutils.h>

#include <COL/COLmap.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void UNITlistTests(const COLmap<COLstring, COLauto<COLclosure0> >& Tests){
   COL_FUNCTION(UNITlistTests);
   COLcout << "Available tests:" << newline;
   for (auto i = Tests.cbegin(); i != Tests.cend(); i++){
      COLcout << i->first << newline;

   } 
}
