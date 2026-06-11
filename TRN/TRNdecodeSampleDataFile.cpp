//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNdecodeSampleDataFile
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, November 16th, 2010 @ 02:58:17 PM
//---------------------------------------------------------------------------
#include <TRN/TRNdecodeSampleDataFile.h>
#include <TRN/TRNideFileLoad.h>
#include <TRN/TRNfile.h>

#include <COL/COLstring.h>
#include <COL/COLlog.h>
#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>
#include <SMPL/SMPLencoding.h>
COL_LOG_MODULE;

static bool TRNisPathSample(const COLstring& FullPath){
   COL_FUNCTION(TRNisPathSample);
   COLstring SampleDir = COL_T("Samples") + FIL_DIR_SEPARATOR; // Samples/
   COL_VAR2(FullPath, SampleDir);
   return (FullPath.find(SampleDir) != npos);
}

bool TRNisAllowedExtension(const COLstring& Extension) {
   COL_FUNCTION(TRNisAllowedExtension);
   const COLstring Lower = COLlowerCase(Extension);
   return (
      Lower == ".css"        ||
      Lower == ".csv"        ||
      Lower == ".db"         ||
      Lower == ".dbs"        ||
      Lower == ".gitignore"  ||
      Lower == ".gitmodules" ||
      Lower == ".help"       ||
      Lower == ".hl7"        ||
      Lower == ".html"       ||
      Lower == ".http"       ||
      Lower == ".info"       ||
      Lower == ".js"         ||
      Lower == ".json"       ||
      Lower == ".lua"        ||
      Lower == ".md"         ||
      Lower == ".pdf"        ||
      Lower == ".prj"        ||
      Lower == ".txt"        ||
      Lower == ".vdb"        ||
      Lower == ".vmd"        ||
      Lower == ".xml"
   );
}

static bool TRNcheckOpenableFile(const COLstring& NewFile){
   COL_FUNCTION(TRNcheckOpenableFile);
   if (TRNisPathSample(NewFile)) { 
      COL_TRC("Switching to sample data file, ignoring binary check");
      return true;
   }
   COLstring Extension = FILpathExtLast(NewFile, true);
   return TRNisAllowedExtension(Extension);
}

void TRNdecodeSampleDataFile(const COLstring& FullPath, const COLstring& ComponentId, COLstring* pCode){
   COL_FUNCTION(TRNdecodeSampleDataFile);
   if (!TRNisPathSample(FullPath)) {
      COL_TRC("Not sample data. No-op");
      return;
   }
   COLstring RawCode;
   SMPLdecode(*pCode, &RawCode);
   RawCode.swap(pCode);
}

bool TRNloadTranslatorFile(const COLstring& FileName, COLstring* pContent){
   COL_FUNCTION(TRNloadTranslatorFile);
   if (!FILfileExists(FileName)){
      *pContent = TRNblankFile();
      return false;
   }
   if (!TRNcheckOpenableFile(FileName)){
      *pContent = TRNunsupportedFile();
      return false;
   }
   FILreadFile(FileName, pContent);
   // if (TRNisPathSample(FileName)){
   //    COLstring Out;
      // SMPLencode(*pContent, &Out);
      // pContent->swap(&Out);
   // }
   return true;
}
