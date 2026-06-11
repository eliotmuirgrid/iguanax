#ifndef __TRN_IDE_ANNOTATION_H__
#define __TRN_IDE_ANNOTATION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNideAnnotation
//
// Description:
//
// Annotation web API.
//
// Author: Eliot Muir 
// Date:   Tuesday 25 April 2023 - 03:24PM
// ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
class COLwebRequest;
class SCKloop;
struct TRNjobQueue;

void TRNideAnnotation(const COLwebRequest& Input, SCKloop* pLoop, COLhashmap<COLstring, TRNjobQueue*>* pTranslatorJobQueues);

#endif // end of defensive include
