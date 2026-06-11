#ifndef __SFI_MD5_H__
#define __SFI_MD5_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFImd5
//
// Description:
//
// SFImd5 - utilities for calculating MD5 hashes
//
// Author: Eliot Muir
// Date:   Tuesday, January 15th, 2013 @ 02:19:41 PM
//
//---------------------------------------------------------------------------

#include <COL/COLsink.h>

class SFImd5HasherPrivate;

class SFImd5Hasher : public COLsink{
public: 
   SFImd5Hasher();
   virtual ~SFImd5Hasher();
   //updates the hash value
   virtual COLuint32 write(const void* cpBuffer, COLuint32 SizeOfBuffer);
   COLstring& hash(); //the resulting hash value. This is all 0s if no hash value has been made
   void reset();    //resets the hash object
private:
   SFImd5HasherPrivate* pMember; 
   SFImd5Hasher(const SFImd5Hasher&);
   SFImd5Hasher operator=(const SFImd5Hasher&);
};

// Returns a hex encoded MD5 hash of the buffer of data passed in
// This is intended to be a convenient simple interface rather than one which
// supports streaming.
COLstring SFImd5(void* pData, int SizeOfData);
COLstring SFImd5(const COLstring& Data);
COLstring SFImd5Raw(void* pData, int SizeofData);
COLstring SFImd5Raw(const COLstring& Data);

#endif // end of defensive include
