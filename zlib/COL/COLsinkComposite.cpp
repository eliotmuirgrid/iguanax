//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLcompositeSink
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Fri 01/16/2004 
//---------------------------------------------------------------------------

#include "COLsinkComposite.h"
#include "COLarray.h"

class COLsinkItem
{
public:
   COLsinkItem() 
    : IsOwner(false),
      pSink(NULL)
   {}
   ~COLsinkItem()
   {
      if (IsOwner)
      {
         delete pSink;
      }
   }
   bool IsOwner;
   COLsink* pSink;
};

class COLsinkCompositePrivate
{
public:
   void flush();
   COLuint32 write(const void* cpBuffer, COLuint32 SizeOfBuffer);
   COLarray<COLsinkItem> SinkVector;
};

void COLsinkCompositePrivate::flush()
{
   for (COLuint32 SinkIndex = 0;
   SinkIndex < SinkVector.size();
   SinkIndex++)
   {
      SinkVector[SinkIndex].pSink->flush();
   }
}

COLuint32 COLsinkCompositePrivate::write(const void* cpBuffer, COLuint32 SizeOfBuffer)
{
   for (COLuint32 SinkIndex = 0;
   SinkIndex < SinkVector.size();
   SinkIndex++)
   {
      SinkVector[SinkIndex].pSink->write(cpBuffer, SizeOfBuffer);
   }

   return SizeOfBuffer;
}

COLsinkComposite::COLsinkComposite()
{
   pMember = new COLsinkCompositePrivate;
}

COLsinkComposite::~COLsinkComposite()
{
   delete pMember;
}

COLuint32 COLsinkComposite::write
(
   const void* cpBuffer, 
   COLuint32 SizeOfBuffer
)
{
   return pMember->write(cpBuffer, SizeOfBuffer);
}

COLuint32 COLsinkComposite::writeString(const COLstring& String) {
   return this->write(&String, String.size());
}

void COLsinkComposite::flush()
{
   pMember->flush();
}

void COLsinkComposite::addSink(COLsink* pSink, bool IsOwner)
{
   COLPRECONDITION(pSink != NULL);
   pMember->SinkVector.push_back(COLsinkItem());
   pMember->SinkVector[pMember->SinkVector.size()-1].pSink = pSink;
   pMember->SinkVector[pMember->SinkVector.size()-1].IsOwner = IsOwner;
}

void COLsinkComposite::clearSinkVector()
{
   pMember->SinkVector.clear();
}
