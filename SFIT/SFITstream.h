#ifndef __SFIT_STREAM_H__
#define __SFIT_STREAM_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFItestStream
//
// Description:
//
// A set of tests of functions in the SFI library.
//
// Author: Matt Lawrence
// Date:   Monday, August 28th, 2006 @ 08:55:35 AM
//
//---------------------------------------------------------------------------

class SFITstream
{
public:
   SFITstream(COLsink& Sink);
   ~SFITstream();

   COLostream& stream();

private:
   COLostream Stream;
};

#endif // end of defensive include
