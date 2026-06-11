#ifndef __COL_SINK_STANDARD_H__
#define __COL_SINK_STANDARD_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLsinkStandard
//
// Description:
//
// COLsink objects for standard out and standard error and also
// a trace sink for use in _WIN32
//
// Author: Eliot Muir
// Date:   Fri 01/16/2004 
//---------------------------------------------------------------------------

#include "COLsink.h"

#include <stdio.h>

class COLsinkFileHandle : public COLsink{
public:
   COLsinkFileHandle(FILE* pFileHandle) : m_pFileHandle(pFileHandle) {}
   virtual ~COLsinkFileHandle() {}
      
   virtual COLuint32 write(const void* cpBuffer, 
                           COLuint32   SizeOfBuffer);
   virtual COLuint32 writeString(const COLstring& String);

   virtual void flush();

   // We only need this on Windows. Unix does not do translations.
   void setBinaryMode(bool Set=true);
private:
   FILE* m_pFileHandle;
};

class COLsinkStandardOut : public COLsinkFileHandle{
public:
   COLsinkStandardOut() : COLsinkFileHandle(stdout) {}
   virtual ~COLsinkStandardOut() {}

   // Eliot - here for backward compatibility
   static void setBinaryMode(bool Set=true);
};

class COLsinkStandardErr : public COLsinkFileHandle{
public:
   COLsinkStandardErr() : COLsinkFileHandle(stderr) {}
   virtual ~COLsinkStandardErr() {}

   // Eliot - here for backward compatibility
   static void setBinaryMode(bool Set=true);
};

#endif // end of defensive include

