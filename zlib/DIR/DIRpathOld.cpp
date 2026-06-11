// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DIRpathOld
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Friday 27 October 2023 - 10:11AM
// ---------------------------------------------------------------------------

#include <DIR/DIRpathOld.h>
#include <DIR/DIRexpandPath.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring DIRoldconfigRootDirDefault() {
   COLstring Result;
#ifdef _WIN32
   Result = "C:\\ProgramData\\iNTERFACEWARE";
#elif __linux__
   // TODO - if we run as root then make the default be /var/lib/
   // Need to look HOME variable
   Result = "~/.iNTERFACEWARE";
#elif __APPLE__
   Result = "~/Library/iNTERFACEWARE";
#else
   COLMSGPRECONDITION(false, "Unsupported Operating System.");
#endif
   Result = DIRexpandPath(Result); // determines the first ~ expansion
   COL_VAR(Result);
   Result = FILpathResolveEnvironment(Result);
   return Result;
}

COLstring DIRoldconfigDirDefault() {
   COLstring ConfigDir = FILpathAppend(DIRoldconfigRootDirDefault(), "IguanaX");
   FILaddPathSeparator(ConfigDir);
   return ConfigDir;
}

COLstring DIRoldconfigFile(const COLstring& Path){
   COL_FUNCTION(DIRoldconfigFile);
   COLstring FileName = Path;
   FILcorrectPathSeparators(FileName);
   FileName = FILpathAppend(FILworkingDir(), "pdb/");
   COL_VAR2(FileName, Path);
   return FileName;
}

COLstring DIRoldfileConfigPermissions() { return DIRoldconfigFile("configurations/users/permissions.json"); }
