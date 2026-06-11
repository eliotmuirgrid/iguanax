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
#include "COLsinkStandard.h"
#include "COLstring.h"

#ifdef _WIN32
   #include <io.h>
   #include <FCNTL.h>
#endif

COLuint32 COLsinkFileHandle::write(const void* cpBuffer, COLuint32 SizeOfBuffer)
{
   if (SizeOfBuffer) { fwrite(cpBuffer, 1, SizeOfBuffer, m_pFileHandle); }
   return SizeOfBuffer;
}

COLuint32 COLsinkFileHandle::writeString(const COLstring& String) {
   return this->write(&String, String.size());
}

void COLsinkFileHandle::flush()
{
   fflush(m_pFileHandle);
}

void COLsetBinaryModeOnFileHandle(FILE* pHandle, bool Set)
{
#ifdef _WIN32  // We only need this on Windows. Unix does not do translations.
   _setmode(_fileno(pHandle), Set ? _O_BINARY : _O_TEXT);
#endif 
}

void  COLsinkFileHandle::setBinaryMode(bool Set) { COLsetBinaryModeOnFileHandle(m_pFileHandle, Set); }
void COLsinkStandardOut::setBinaryMode(bool Set) { COLsetBinaryModeOnFileHandle(stdout, Set);        }
void COLsinkStandardErr::setBinaryMode(bool Set) { COLsetBinaryModeOnFileHandle(stderr, Set);        }

