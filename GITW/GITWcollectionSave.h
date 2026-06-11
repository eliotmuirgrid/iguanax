#ifndef __GITW_COLLECTION_SAVE_H__
#define __GITW_COLLECTION_SAVE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITWcollectionSave
//
// Description:
//
// Check if folder exists, if not try cloning it.
// Then reset the folder.
// Save collection data to collection.json.
// Add file and commit or do commit --all. 
// Then push.
//
// Author: Tyler Brown 
// Date:   Thursday 11 May 2023 - 12:19PM
// ---------------------------------------------------------------------------
class COLwebRequest;

void GITWcollectionSave(const COLwebRequest& Request);


#endif // end of defensive include
