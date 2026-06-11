#ifndef __TST_EXCEPTION_H__ 
#define __TST_EXCEPTION_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTexception
//
// Description:
//
// An exception class that represents when a test fails. We use this to differentiate
// between an expected failure, and an unexpected one (COLerror, or ... )
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------
#include <COL/COLerror.h>

class TSTexception : public COLerror
{
public:
   enum ETSTerrorCode
   {
      eAssertionFailure
   };

   TSTexception(const COLstring& Message, 
                COLuint32 Line, 
                const char* File, 
                ETSTerrorCode Code)
      : COLerror(Message,Line,File,(COLuint32)Code){};

   TSTexception(TSTexception& Exception)
      : COLerror(Exception){};

   virtual ~TSTexception(){};
};

#endif // end of defensive include
