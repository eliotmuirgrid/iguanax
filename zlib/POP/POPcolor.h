#ifndef __POP_COLOR_H__
#define __POP_COLOR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: POPcolor
//
// Description:
//
// Functions to produce color ASCII output.  I use them a lot for the linux install
// process - so that is why POPiguanaCommandLine is here.
//
// Author: Eliot Muir 
// Date:   Tuesday 19 September 2023 - 04:25PM
// ---------------------------------------------------------------------------

class COLstring;

COLstring POPpop(const COLstring& In);
COLstring POPwarning(const COLstring& In);


COLstring POPiguanaCommandLine(bool NeedRoot=false);

#endif // end of defensive include
