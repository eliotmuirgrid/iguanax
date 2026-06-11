#ifndef __GITW_COLLECTION_EDIT_H__
#define __GITW_COLLECTION_EDIT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITWcollectionEdit
//
// Description:
//
// Try to initialize a scratch repository using ssh credentials, check for write 
// permission and on success return the contents of collection.json. 
//
// Author: Tyler Brown
 
// Date:   Tuesday 09 May 2023 - 12:25PM
// ---------------------------------------------------------------------------
class COLwebRequest;

void GITWcollectionEdit(const COLwebRequest& Request);


#endif // end of defensive include
