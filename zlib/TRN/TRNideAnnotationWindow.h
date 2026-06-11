#ifndef __TRN_IDE_ANNOTATION_WINDOW_H__
#define __TRN_IDE_ANNOTATION_WINDOW_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNideAnnotationWindow
//
// Description:
//
// Call to populate an annotation window
//
// Author: Eliot Muir 
// Date:   Tuesday 25 April 2023 - 03:24PM
// ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
class COLwebRequest;
class SCKloop;
struct TRNjobQueue;

void TRNideAnnotationWindow(const COLwebRequest& Request, SCKloop* pLoop, COLhashmap<COLstring, TRNjobQueue*>* pTranslatorJobQueues);

#endif // end of defensive include
