// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testMagicMarker
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Fri 24 Feb 2023 10:48:10 EST
// ---------------------------------------------------------------------------
#include "testDir.h"
#include <UNIT/UNITapp.h>

#include <SDB/SDBdir.h>
#include <SDB/SDBnumber.h>
#include <SDB/SDBmagicMarker.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testSimple(){
   COL_FUNCTION(testSimple);
   SDBmagicMarker M1;
   COLstring Buffer;
   Buffer.setCapacity(SDBmagicMarkerSize(&M1));
   int Written = SDBwriteMagicMarker(&M1, Buffer.get_buffer(), Buffer.capacity());
   Buffer.setSize(Written);

   SDBmagicMarker M2;
   int Read = SDBreadMagicMarker(&M2, Buffer.get_buffer(), Buffer.size());
   COL_VAR4(Read, Written, SDBmagicMarkerSize(&M1), SDBmagicMarkerSize(&M2));
   COL_VAR2(M1, M2);
   UNIT_ASSERT_EQUALS(Read, Written);
   UNIT_ASSERT_EQUALS(M1, M2);
}

static void testBufferError(){
   COL_FUNCTION(testBufferError);
   char Buffer[4] = "42";
   SDBmagicMarker M1;
   UNIT_ASSERT_EQUALS(-1, SDBwriteMagicMarker(&M1, Buffer, sizeof(Buffer)));
   UNIT_ASSERT_EQUALS(-1, SDBreadMagicMarker (&M1, Buffer, sizeof(Buffer)));
}

void testMagicMarker(UNITapp* pApp){
   COL_FUNCTION(testMagicMarker);
   pApp->add("magic/read",        &testSimple);
   pApp->add("magic/bufferError", &testBufferError);
}
