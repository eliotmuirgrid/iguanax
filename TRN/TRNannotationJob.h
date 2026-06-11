#ifndef __TRN_TRNOTATION_JOB_H__
#define __TRN_TRNOTATION_JOB_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNannotationJob
//
// Description:
//
// TRNannotationJob class.
//
// Author: Eliot Muir
// Date:   Tuesday, November 16th, 2010 @ 02:58:17 PM
//---------------------------------------------------------------------------

#include "TRNannotationState.h"
#include "TRNfile.h"
#include <CFG/CFGmap.h>

class SCKloop;
struct CFGconfig;
struct lua_State;

class  TRNannotationJob {
public:
   TRNannotationJob();
   ~TRNannotationJob();

   lua_State* L;
   TRNannotationState m_AnnotationState;
   bool m_CompileNeeded = true;

private:
   TRNannotationJob(const TRNannotationJob& Orig); // not allowed
   TRNannotationJob& operator=(const TRNannotationJob& Orig); // not allowed
};

bool TRNcompileAnnotationProgram(TRNannotationJob* pJob, COLstring* pCompileError);

void TRNideBoilerPlate(COLvar P, TRNannotationJob* pJob);

#endif // end of defensive include
