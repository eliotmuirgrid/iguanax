#ifndef __DIR_PATH_OLD_H__
#define __DIR_PATH_OLD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DIRpathOld
//
// Description:
//
// These were all the old paths that we will need to reference when we do migration
//
// Author: Tyler Brown 
// Date:   Friday 27 October 2023 - 10:11AM
// ---------------------------------------------------------------------------
class COLstring;

COLstring DIRoldconfigRootDirDefault(); // returns the iNTERFACEWARE/ directory
COLstring DIRoldconfigDirDefault();     // returns iNTERFACEWARE/IguanaX/
COLstring DIRoldfileConfigPermissions();

#endif // end of defensive include
