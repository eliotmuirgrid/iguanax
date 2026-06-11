// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNrecordProcess
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 14 June 2023 - 02:02PM
// ---------------------------------------------------------------------------

#include <TUN/TUNrecordProcess.h>
#include <TUN/TUNrecord.h>
#include <TUN/TUNrequest.h>
#include <TUN/TUNrecordType.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool TUNrecordProcess(TUNrequest* pRequest, COLhashmap<COLuint32, COLauto<COLclosure1<TUNrequestItem*>>>& HandlerTable){
   COL_FUNCTION(TUNrecordProcess);
   COLstring RecordData;
   COLuint32 Type;
   int Size = pRequest->pIn->size();
   int SizeRead;
   const unsigned char* pNext = (const unsigned char*)pRequest->pIn->get_buffer();
   TUNrequestItem Request;
   Request.ConnectionId = pRequest->ConnectionId;
   do{
      SizeRead = TUNrecordRead(pNext, Size, &Type, &Request.RecordData);
      if (SizeRead == 0) {COL_TRC("Reached the end of complete records."); break;        }
      if (SizeRead == -1){COL_TRC("We have a corrupted stream.");          return false; }
      pNext += SizeRead;
      Size -= SizeRead;
      COL_VAR(Type);
      COL_DUMP("Record Data:", Request.RecordData.c_str(), Request.RecordData.size());
      if (HandlerTable.count(Type) == 0) { COL_ERR("Unrecognized record: " << Type); return false; } else {
         Request.pOut = pRequest->pOut;
         TUNrecordInTrace((TUNrecordType)Type, pRequest->ConnectionId);
         HandlerTable[Type]->run(&Request);
      }
   } while (true);
   COL_TRC("Consumed " << ((const char*)pNext) - pRequest->pIn->c_str() << " bytes");
   TUNtruncate(pRequest->pIn, ((const char*)pNext) - pRequest->pIn->c_str());
   COL_DUMP("Buffer:", pRequest->pIn->c_str(), pRequest->pIn->size());
   return true;
}