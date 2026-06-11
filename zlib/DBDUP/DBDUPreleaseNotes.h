#ifndef __DBDUP_RELEASE_NOTES_H__
#define __DBDUP_RELEASE_NOTES_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDUPreleaseNotes
//
// Description:
//
// Checks the release server and gets the changelog.html file
//
// Author: Vismay Shah 
// Date:   Thursday 17 August 2023 - 9:49AM
// ---------------------------------------------------------------------------
class COLwebRequest;

void DBDUPreleaseNotes(const COLwebRequest& Request);

#endif // end of defensive include
