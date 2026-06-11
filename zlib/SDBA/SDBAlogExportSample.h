#ifndef __SDBA_LOG_EXPORT_SAMPLE_H__
#define __SDBA_LOG_EXPORT_SAMPLE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBAlogExportSample
//
// Description:
//
// This API exports a message from the log into the translator as a sample.
//
// Author: Eliot Muir 
// Date:   Sunday 14 May 2023 - 05:51PM
// ---------------------------------------------------------------------------

class COLwebRequest;

void SDBAlogExportSample(const COLwebRequest& Request);

#endif // end of defensive include
