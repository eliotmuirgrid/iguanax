#ifndef __HTTP_PARSE_RESULT_H__
#define __HTTP_PARSE_RESULT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPparseResult
//
// Description:
//
// HTTPparseResult enumeration - used to return the results from the
// parsers in this library.
//
// Author: Eliot Muir
// Date:   Monday, December 11th, 2006 @ 04:57:54 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

enum HTTPparseResult
{
   HTTP_PARSE_ERROR,
   HTTP_PARSE_NEED_MORE_DATA,
   HTTP_PARSE_CHUNK_COMPLETE, // Only applicable to HTTPbodyParser
   HTTP_PARSE_COMPLETE
};

#endif // end of defensive include
