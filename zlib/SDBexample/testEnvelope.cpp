// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testFile
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Mon 13 Feb 2023 10:01:00 EST
// ---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>

#include <SDB/SDBcontent.h>
#include <SDB/SDBenvelope.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBmagicMarker.h>
#include <SDB/SDBnumber.h>

#include <COL/COLauto.h>
#include <COL/COLmath.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testWrapper() {
   COL_FUNCTION(testWrapper);
   SDBmessage Msg("envelop", "data");
   // test size calculation
   int PayloadSize = SDBcrcSize() + SDBmessageSize(&Msg);
   int ExpectedSize = SDBmarkerSize() + SDBnumberSize(PayloadSize) + PayloadSize;
   UNIT_ASSERT_EQUALS(ExpectedSize, SDBenvelopeSizeFromContent((SDBcontent*)&Msg));

   // test content pointer
   COLstring Buffer = SDB_ENVELOPE_MARKER;
   SDBwriteNumber(PayloadSize, Buffer.get_buffer() + SDBmarkerSize());
   const char* pContent = SDBenvelopeContent(Buffer.data(), Buffer.size());
   UNIT_ASSERT_EQUALS(0, pContent);    // incomplete envelope returns NULL

   Buffer.append("SCCCCD");   // make buffer long enough. i.e. marker + size + crc checksum + data
   pContent = SDBenvelopeContent(Buffer.data(), Buffer.size());
   UNIT_ASSERT_EQUALS(SDBmarkerSize() + SDBnumberSize(PayloadSize) + SDBcrcSize(), pContent - Buffer.data());
   Buffer.clear();

   // create envelop buffer with a single envelope
   Buffer.setCapacity(SDBmessageSize(&Msg));
   SDBwriteMessage(&Msg, Buffer.get_buffer(), Buffer.capacity()-1);
   Buffer.setSize(SDBmessageSize(&Msg));
   COLstring Envelope;
   SDBenvelopeWrite(&Envelope, Buffer);
   // SDBenvelopNext should return end of the the buffer
   UNIT_ASSERT_EQUALS(Envelope.data()+Envelope.size(), SDBenvelopeNext(Envelope.data(), Envelope.size()));
   // SDBenvelopeComplete should fail when there isn't enough data in input buffer
   UNIT_ASSERT_EQUALS(false, SDBenvelopeComplete(Envelope.data(), 2));
   UNIT_ASSERT_EQUALS(true,  SDBenvelopeComplete(Envelope.data(), Envelope.size()));
   // SDBenvelopeContentType should give us SDB_MESSAGE back
   UNIT_ASSERT_EQUALS(SDB_MESSAGE, SDBenvelopeContentType(Envelope.data(), Envelope.size()));

   // envelope should contain | marker | payload_size | crc | message |
   COLuint64 Size;
   UNIT_ASSERT(SDBisMarker(Envelope.data()));
   int Amount = SDBreadNumber(&Size, Envelope.data() + SDBmarkerSize(), Envelope.size() - SDBmarkerSize());
   UNIT_ASSERT_EQUALS(SDBcrcSize() + SDBmessageSize(&Msg), Size);
}

static void testCorrupt() {
   COL_FUNCTION(testCorrupt);
   COLstring Data(104, '~');  // ensures the payload_size has 2 bytes
   SDBmessage Msg("corrupt", Data);
   COLstring Buffer;
   Buffer.setCapacity(SDBmessageSize(&Msg));
   SDBwriteMessage(&Msg, Buffer.get_buffer(), Buffer.capacity()-1);
   Buffer.setSize(SDBmessageSize(&Msg));
   COLstring Envelope;
   SDBenvelopeWrite(&Envelope, Buffer);
   COL_DUMP("Envelope = ", Envelope.data(), Envelope.size());

   // simulate partial envelop in buffer
   UNIT_ASSERT_EQUALS(-1, SDBenvelopeInvalid(Envelope.data(), 1));                     // partial marker
   UNIT_ASSERT_EQUALS(-1, SDBenvelopeInvalid(Envelope.data(), SDBmarkerSize() + 1));   // partial payload_size
   UNIT_ASSERT_EQUALS(-1, SDBenvelopeInvalid(Envelope.data(), Envelope.size() - 3));   // partial content

   // mess with the message to trigger crc check fail
   Envelope[Envelope.size()-2] += 4;
   UNIT_ASSERT_EQUALS(-5, SDBenvelopeInvalid(Envelope.data(), Envelope.size()));
   Envelope[Envelope.size()-2] -= 4;
   UNIT_ASSERT_EQUALS(0,  SDBenvelopeInvalid(Envelope.data(), Envelope.size()));

   // message with the marker to trigger marker check fail
   Envelope.replace(SDB_ENVELOPE_MARKER, ":(");
   UNIT_ASSERT_EQUALS(-2, SDBenvelopeInvalid(Envelope.data(), Envelope.size()));
   Envelope.replace(":(", SDB_ENVELOPE_MARKER);
   UNIT_ASSERT_EQUALS(0,  SDBenvelopeInvalid(Envelope.data(), Envelope.size()));
}

static void testNext(){
   COL_FUNCTION(testNext);
   COLstring Buffer(SDB_ENVELOPE_MARKER);    // ":)"
   UNIT_ASSERT_EQUALS(0, SDBfindNextEnvelope(Buffer.data(), Buffer.size()));
   COLstring Data("ureikjfdh");
   Buffer.prepend(Data);                     // "ureikjfdh:)"
   UNIT_ASSERT_EQUALS(Buffer.data() + Data.size(), SDBfindNextEnvelope(Buffer.data(), Buffer.size()));
   Buffer[1] = '\0';                         // "u\0eikjfdh:)"
   UNIT_ASSERT_EQUALS(Buffer.data() + Data.size(), SDBfindNextEnvelope(Buffer.data(), Buffer.size()));
   Buffer[0] = '\0';                         // "\0\0eikjfdh:)"
   UNIT_ASSERT_EQUALS(Buffer.data() + Data.size(), SDBfindNextEnvelope(Buffer.data(), Buffer.size()));
   // reduce buffer size so that the entire marker is 2 bytes after the end of buffer
   UNIT_ASSERT_EQUALS(0, SDBfindNextEnvelope(Buffer.data(), Buffer.size() - SDBmarkerSize() - 2));
   // reduce buffer size so that the entire marker is just out of bound
   UNIT_ASSERT_EQUALS(0, SDBfindNextEnvelope(Buffer.data(), Buffer.size() - SDBmarkerSize()));
   // reduce buffer size so only the first byte of the maker is in the buffer, assumes marker is at least two bytes long
   UNIT_ASSERT_EQUALS(0, SDBfindNextEnvelope(Buffer.data(), Buffer.size() - SDBmarkerSize() + 1));
   Buffer.clear();                           // ""
   UNIT_ASSERT_EQUALS(0, SDBfindNextEnvelope(Buffer.data(), Buffer.size()));
   Buffer.append(SDB_ENVELOPE_MARKER);       // ":)"
   Buffer.append(Data);                      // ":)ureikjfdh"
   UNIT_ASSERT_EQUALS(0, SDBfindNextEnvelope(Buffer.data(), Buffer.size()));
   Buffer.append(SDB_ENVELOPE_MARKER);       // ":)ureikjfdh:)"
   UNIT_ASSERT_EQUALS(Buffer.data() + Buffer.size() - SDBmarkerSize(), SDBfindNextEnvelope(Buffer.data(), Buffer.size()));
   int BufSize = 5;
   Buffer = COLstring(BufSize, '\0');        // "\0\0\0\0\0"
   Buffer.append(SDB_ENVELOPE_MARKER);       // "\0\0\0\0\0:)"    marker is 2 bytes after end of buffer
   UNIT_ASSERT_EQUALS(0, SDBfindNextEnvelope(Buffer.data(), Buffer.size() - SDBmarkerSize() - 2));
   Buffer.clear();
   Buffer = SDB_ENVELOPE_MARKER;
   for (int i = 1; i <= 10; ++i) {           // "?:)" to "??????????:)"    IX-2509
      Buffer.prepend("?");
      UNIT_ASSERT_EQUALS(Buffer.data() + i, SDBfindNextEnvelope(Buffer.data(), Buffer.size()));
   }
}

void testEnvelope(UNITapp* pApp){
   COL_FUNCTION(testEnvelope);
   pApp->add("envelope/wrapper",  &testWrapper);
   pApp->add("envelope/corrupt",  &testCorrupt);
   pApp->add("envelope/next",     &testNext);
}
