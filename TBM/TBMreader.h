#ifndef __TBM_READER_H__
#define __TBM_READER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TBMreader
//
// Description:
//
// TBMreader class.
//
// This class parses the binary stream produced by the TBMwriter
// It parses in a progressive manner which supports streaming data to it.
//
// My planned application of this code is for RPC communications in Titan. 
//
// The manner in which parse is called is important.  If the data
// stream which is passed to TBMreader is incomplete the ParsedUntil
// parameter will have the position where parsing stopped.  It's up to
// the caller to figure out the buffering strategy.  So most likely the
// application will need to take the unparsed data, add extra data when
// it is available and get TBMreader to resume the parse starting from
// where it left off.
//
// This could potentially lead to a problem in a case where there is a
// object with a large string attributes - each string can be up 
// to 64k long.  So thought should be put into each application perhaps
// to make sure this situation doesn't happen.  I guess if the buffer for
// receiving the RPC feed is dynamically resizeable it should be okay. 
//
// In most cases however the application receiving buffer shouldn't need
// to get that large which is nice advantage of streaming parsing.  One
// other annoyance though for an application using this parser for network
// RPC is that if the network connection is terminated during the parse
// it will leave the populated objects in a half set up undefined state.  
//
// The application logic will need to have thought put in to determine
// what to do in those situations - it might involve for instance clearing
// the model etc.
//
// Author: Eliot Muir
// Date:   Friday, February 9th, 2007 @ 11:22:14 PM
//
//---------------------------------------------------------------------------

#include "TBMresult.h"

class TSMhandlerStack;
class TBMreaderPrivate;

class TBMreader 
{
public:
   TBMreader();
   // This is non virtual because we are assuming no classes will inherit from this class
   ~TBMreader();

   TBMresult parse(const char* pBuffer, COLuint32 SizeOfBuffer, COLuint32& ParsedUntil, COLuint32& AmountNeeded); 
   
   // To reset the parser, init the handlerStack
   TSMhandlerStack& handlerStack();
private:
   TBMreaderPrivate* pMember;
   TBMreader(const TBMreader& Orig); // not allowed
   TBMreader& operator=(const TBMreader& Orig); // not allowed
};

#endif // end of defensive include
