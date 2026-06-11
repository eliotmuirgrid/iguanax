#ifndef __DBGHELP_H__
#define __DBGHELP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: dbghelp
//
// Description:
//
//   Wrapper around inc\dbghelp.h to allow customization.
//
// Author:   Steven Dodd
// Date:     January 19, 2010
//
//---------------------------------------------------------------------------

#ifdef _MSC_VER
   #if _MSC_VER < 1300 // VC6
      typedef unsigned long ULONG_PTR, *PULONG_PTR;
   #endif

   #include "inc/dbghelp.h"
#endif

#endif // end of defensive include
