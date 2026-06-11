//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFITstream
//
// Description:
//
// Implementation
//
// Author: Matt Lawrence
// Date:   Monday, August 28th, 2006 @ 08:55:35 AM
//
//---------------------------------------------------------------------------
#include "SFITprecomp.h"
#pragma hdrstop

#include "SFITstream.h"

#include <COL/COLsink.h>

SFITstream::SFITstream(COLsink& Sink) : Stream(Sink) 
{
}

SFITstream::~SFITstream()
{
   Stream.sink()->onEndStream();
}


COLostream& SFITstream::stream() 
{ 
   return Stream; 
}
