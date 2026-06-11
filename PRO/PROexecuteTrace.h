#ifndef __PRO_EXECUTE_TRACE_H__
#define __PRO_EXECUTE_TRACE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PROexecuteTrace
//
// Description:
//
// File to make it easy to trace GIT commands invoked by Iguana.
//
// Author: Eliot Muir 
// Date:   Tuesday 10 October 2023 - 08:47AM
// ---------------------------------------------------------------------------

class COLstring;

void PROexecuteTrace(const COLstring& Command);

#endif // end of defensive include
