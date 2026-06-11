 #ifndef __CMD_TEST_STRING_ARRAY_H__
#define __CMD_TEST_STRING_ARRAY_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMDtestStringArray.h
//
// Description:
//
// A string vector that we can use to create string arrays to get
// argv (a const char**) to pass to CMD::parseArgs()
//
// Author: Roman Leifer
// Date:   Tue 08/18/2004
//
//
//---------------------------------------------------------------------------

#include <COL/COLarray.h>

class CMDtestStringArray
{
public:
   CMDtestStringArray(){};
   CMDtestStringArray(const CMDtestStringArray& Rhs);
   virtual ~CMDtestStringArray(){};

   const char** getArray();
   void addCommandLine(const COLstring& CommandLine);
   bool has(const COLstring& String);

   COLarray<COLstring> Vector;
private:
   COLarray<const char*> Array;
};

#endif      //   end of defensive include
