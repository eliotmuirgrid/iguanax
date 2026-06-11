// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTUNrecord
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir
// Date:   Wednesday 14 June 2023 - 11:58AM
// ---------------------------------------------------------------------------
#include "testTUNrecord.h"

#include <TUN/TUNrecord.h>
#include <TUN/TUNstateClient.h>
#include <TUN/TUNkeyLoad.h>
#include <TUN/TUNmsgHello.h>
#include <TUN/TUNrecordProcess.h>
#include <TUN/TUNstateServer.h>
#include <TUN/TUNsetup.h>
#include <TUN/TUNrequest.h>
#include <TUN/TUNsendHello.h>

#include <UNIT/UNITapp.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testRecordWrite(){
   COL_FUNCTION(testRecordWrite);
   COLstring Buffer;
   TUNrecordWriteString(&Buffer, 1, "Hello");
   TUNrecordWriteString(&Buffer, 1, "A quick brown fox jumped over the lazy dog.");
   TUNrecordWriteString(&Buffer, 1, "1234567890123456789");
   TUNrecordWriteString(&Buffer, 1, "An incomplete record.  That I truncate.");
   Buffer.setSize(Buffer.size()-10);  // purposely truncating the data.
   COL_DUMP("Buffer:", Buffer.c_str(), Buffer.size());

   COLstring Result;
   COLuint32 Type;
   int Size = Buffer.size();
   int SizeRead;
   const unsigned char* pNext = (const unsigned char*)Buffer.get_buffer();
   do{
      SizeRead = TUNrecordRead(pNext, Size, &Type, &Result);
      COL_VAR((int)Type);
      if (SizeRead == 0){
         COL_TRC("Reached the end of complete records.");
         break;
      }
      if (SizeRead == -1){
         COL_TRC("We have a corrupted stream.");
         break;
      }
      COL_DUMP("Result:", Result.c_str(), Result.size());
      pNext += SizeRead;
      Size -= SizeRead;
   } while (true);
   COL_TRC("Consumed " << ((const char*)pNext) - Buffer.c_str() << " bytes");
   TUNtruncate(&Buffer, ((const char*)pNext) - Buffer.c_str());
   COL_DUMP("Buffer:", Buffer.c_str(), Buffer.size());
}

static void testRecordTypeLarge(){
   COL_FUNCTION(testRecordTypeLarge);
   COLstring Buffer;
   COLuint32 Type1, Type2;
   Type1 = 0xFFFFFFFF;
   COLstring Data1, Data2;
   Data1 = "Hello";
   TUNrecordWriteString(&Buffer, Type1, Data1);
   const unsigned char* pBuffer = (const unsigned char*)Buffer.data();
   int SizeRead = TUNrecordRead(pBuffer, Buffer.size(), &Type2, &Data2);
   UNIT_ASSERT_EQUALS(Type1, Type2);
   UNIT_ASSERT_EQUALS(Data1, Data2);
}

bool TUNisRequestEmpty(const TUNrequest& Request){
   return Request.pIn->is_null() && Request.pOut->is_null();
}

static void testRecordHello(){
   COL_FUNCTION(testRecordHello);
   TUNstateClient ClientState1;
   TUNstateClient ClientState2;
   TUNstateServer ServerState;
   TUNkeyLoad("client/private1.pem", &ClientState1.m_MyKey);
   TUNkeyLoad("client/private2.pem", &ClientState2.m_MyKey);
   COL_TRC("Loaded key");
   COLstring Client1OutBuffer;
   COLstring Server1OutBuffer;
   COLstring Client2OutBuffer;
   COLstring Server2OutBuffer;

   COLhashmap<COLuint32, COLauto<COLclosure1<TUNrequestItem*>> > m_ServerHandlers;
   COLhashmap<COLuint32, COLauto<COLclosure1<TUNrequestItem*>> > m_Client1Handlers;
   TUNsetupClient(&m_Client1Handlers, &ClientState1);

   COLhashmap<COLuint32, COLauto<COLclosure1<TUNrequestItem*>> > m_Client2Handlers;
   TUNsetupClient(&m_Client2Handlers, &ClientState2);

   TUNsetupServer(&m_ServerHandlers, &ServerState);

   TUNsendHello(&Client1OutBuffer, ClientState1);
   TUNsendHello(&Client2OutBuffer, ClientState2);

   TUNrequest ServerClient1;
   ServerClient1.ConnectionId = 10;
   ServerClient1.pIn  = &Client1OutBuffer;
   ServerClient1.pOut = &Server1OutBuffer;

   TUNrequest ServerClient2;
   ServerClient2.ConnectionId = 20;
   ServerClient2.pIn  = &Client2OutBuffer;
   ServerClient2.pOut = &Server2OutBuffer;

   TUNrequest Client1;
   Client1.pIn  = &Server1OutBuffer;
   Client1.pOut = &Client1OutBuffer;

   TUNrequest Client2;
   Client2.pIn  = &Server2OutBuffer;
   Client2.pOut = &Client2OutBuffer;
   COL_VAR(ServerState);
   do{
      TUNrecordProcess(&ServerClient1, m_ServerHandlers);
      TUNrecordProcess(&ServerClient2, m_ServerHandlers);
      TUNrecordProcess(&Client1, m_Client1Handlers);
      TUNrecordProcess(&Client2, m_Client2Handlers);
   } while (!TUNisRequestEmpty(ServerClient1) || !TUNisRequestEmpty(ServerClient2));

   COL_VAR(ServerState);
}



void testTUNrecord(UNITapp* pApp) {
   pApp->add("record/write", &testRecordWrite);
   pApp->add("record/type/large", &testRecordTypeLarge);
  // pApp->add("record/hello", &testRecordHello);
}
