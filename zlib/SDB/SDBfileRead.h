#ifndef __SDB_FILE_READ_H__
#define __SDB_FILE_READ_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBfileRead
//
// Description:
//
// Same as FILreadFile except that it doesn't throw exceptions if the file gets
// larger as it is written to.
//
// Author: Eliot Muir 
// Date:   Thursday 11 May 2023 - 02:54PM
// ---------------------------------------------------------------------------

class COLstring;

bool SDBfileRead(const COLstring& FileName, COLstring* pContent);

#endif // end of defensive include
