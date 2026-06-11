//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNannotationJob
//
// Description:
//
// Implementation - I put the constructor and destructor in here to keep track of them.
//
// Author: Eliot Muir
// Date:   Tuesday, November 16th, 2010 @ 02:58:17 PM
//---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <TRN/TRNannotationJob.h>
#include <TRN/TRNconstant.h>
#include <TRN/TRNcompile.h>
COL_LOG_MODULE;

TRNannotationJob::TRNannotationJob() : L(0){
   COL_METHOD(TRNannotationJob);
}

TRNannotationJob::~TRNannotationJob() {
   COL_METHOD(~TRNannotationJob);
   TRNdestroyLua(&L);
}
