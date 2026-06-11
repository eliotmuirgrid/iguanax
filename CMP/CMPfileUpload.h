#ifndef __CMP_FILE_UPLOAD_H__
#define __CMP_FILE_UPLOAD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPfileUpload
//
// Description:
//
// Upload a file to the component repository.
//
// Author: Orkhan Amikishiyev 
// Date:   Thursday 02 March 2023 - 04:30PM
// ---------------------------------------------------------------------------
class COLwebRequest;

void CMPfileUpload(const COLwebRequest& Request);

#endif // end of defensive include