#ifndef __PARALLELUPLOAD_H__
#define __PARALLELUPLOAD_H__
// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: parallelUpload.h
//
//  Description: parallel workflow for downloading and uploading releases
//
//  Author: Matthew Sobkowski
//  Date:   Friday, September 29, 2023
// ---------------------------------------------------------------------------
class COLstring;

void beginParallelProcess(const COLstring& Hash, const COLstring& Changelog);

#endif //end of defensive include
