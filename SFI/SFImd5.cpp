//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFImd5
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, January 15th, 2013 @ 02:19:41 PM
//
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

#include "SFImd5.h"
#include "SFImd5Imp.h"
#include "SFIhexEncodeFilter.h"

#include <COL/COLsinkString.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <memory.h>

class SFImd5HasherPrivate{
public:
   SFImd5HasherPrivate() : HashValueCreated(false){}
   ~SFImd5HasherPrivate(){}
   md5_state_s Md5State;
   COLstring Hash;
   bool HashValueCreated;
};

SFImd5Hasher::SFImd5Hasher(){
   pMember = new SFImd5HasherPrivate;
   reset();
}

SFImd5Hasher::~SFImd5Hasher(){
   delete pMember;
}

COLuint32 SFImd5Hasher::write(const void* cpBuffer,COLuint32 SizeOfBuffer){
   COL_METHOD(SFImd5Hasher::write);
   COL_VAR2(COLstring((const char*)cpBuffer, SizeOfBuffer), SizeOfBuffer);
   ::md5_append( &pMember->Md5State, (const md5_byte_t*)cpBuffer, SizeOfBuffer );
   return SizeOfBuffer;
}

COLstring& SFImd5Hasher::hash(){
   if (!pMember->HashValueCreated){
      ::md5_finish( &pMember->Md5State, (md5_byte_t*)pMember->Hash.get_buffer() );
      pMember->HashValueCreated = true;
   }
   return pMember->Hash;
}

void SFImd5Hasher::reset(){
   ::md5_init( &pMember->Md5State );
   pMember->Hash.setCapacity(16);
   pMember->Hash.setSize(16);
   ::memset(pMember->Hash.get_buffer(),0,pMember->Hash.size());
   pMember->HashValueCreated = false;
}

// TODO - I haven't bothered to make this implementation efficient - just
// wanted to get the interface convenient for many common use cases in the code.
// It would make the most sense to use the native interfaces from the MD5 C code
// Eliot
COLstring SFImd5(void* pData, int SizeOfData){
   COL_FUNCTION(SFITmd5);
   SFImd5Hasher Hasher;
   Hasher.write(pData, SizeOfData);

   COLstring Hash;
   COLsinkString Sink(Hash);
   SFIhexEncodeFilter HexMd5Filter(&Sink);
   HexMd5Filter.write( Hasher.hash().data(), Hasher.hash().size());
   HexMd5Filter.flush();
   COL_VAR(Hash);
   return Hash;
}

COLstring SFImd5Raw(void* pData, int SizeofData){
   COL_FUNCTION(SFITmd5);
   SFImd5Hasher Hasher;
   Hasher.write(pData, SizeofData);
   return Hasher.hash();
}

COLstring SFImd5(const COLstring& Data){
   return SFImd5((void*)Data.c_str(), Data.size());
}

COLstring SFImd5Raw(const COLstring& Data){
   return SFImd5Raw((void*)Data.c_str(), Data.size());
}
