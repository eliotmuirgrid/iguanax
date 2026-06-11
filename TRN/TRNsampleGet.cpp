//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNsampleGet
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, November 17th, 2010 @ 10:38:43 AM
//---------------------------------------------------------------------------
#include "TRNsampleGet.h"

#include <DIR/DIRpath.h>
#include <FIL/FILatomic.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILfile.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// TODO - this algorithm will get progressively slower with N files.
// It would make more sense to just pass the file name of the sample data as a file name and separate the concern of selecting the
// next and previous file.
static void TRNsampleFind(const COLstring& ComponentId, int Index, COLstring* pData, COLstring* pName) {
   COL_FUNCTION(TRNsampleFind);
   int Count = 0;
   COLstring SampleDir = DIRcomponentSampleDir(ComponentId);
   COLstring FileName;
   FILdirEnumerator Enum(FILpathAppend(SampleDir, FIL_ANYFILE));
   while (Enum.getFile(FileName)){
      if ( Enum.isDirectory() || Enum.isHidden() || FILpathExtLast(FileName, false) == "meta") { 
         continue;
      }
      Count++;
      COL_VAR2(Count, FileName);
      if (Count == Index) { 
         *pName = FILpathAppend(SampleDir, FileName);
         FILreadFile(SampleDir + FileName, pData);
         COL_TRC("Found sample data.");
         COL_DUMP("Sample data:", pData->c_str(), pData->size());
         return;
      }
   };
   COL_TRC("Unable to find sample data for index = " << Index);
}

COLstring TRNsampleGet(const COLstring& ComponentId, int Id, COLstring* pData, COLstring* pError){
   COL_FUNCTION(TRNsampleGet);
   COL_VAR(Id);
   pData->clear();
   COLstring FullPath;
   try{
      TRNsampleFind(ComponentId, Id, pData, &FullPath);
   } catch(const COLerror& Error){
      *pError = Error.description();
      COL_TRC(*pError);
   }
   COL_VAR(*pData);
   return FullPath;
}

COLstring TRNsampleGet(const COLstring& ComponentId, const COLstring& SampleFile, COLstring* pData, COLstring* pError){
   COL_FUNCTION(TRNsampleGet-File);
   COL_VAR(SampleFile); // this is the file relative to the samples dir
   COLstring SampleDir = DIRcomponentSampleDir(ComponentId);
   COLstring Fullpath = FILpathAppend(SampleDir, SampleFile);
   COL_VAR(Fullpath);
   if (!FILfileExists(Fullpath)) {
      COL_TRC("Sample data file doesn't exist: " << SampleFile);
      *pError = COL_T("The sample data file ") + SampleFile + COL_T(" does not exist.");
      return COL_T("");
   }
   if (FILisDirectory(Fullpath)) {
      *pError = COL_T("The sample data file specified is a directory.");
      return COL_T("");
   }
   if (FILpathExtLast(Fullpath, false) == "meta"){
      *pError = COL_T("The sample data file specified is a meta file.");
      return COL_T("");
   }
   COL_TRC("Found sample data file");
   pData->clear();
   try{
      FILreadFile(Fullpath, pData);
   } catch(const COLerror& Error){
      *pError = Error.description();
      COL_TRC(*pError);
   }
   COL_VAR(*pData);
   return Fullpath;
}

void TRNsampleGetMeta(const COLstring& SampleFile, COLstring* pFunction){
   COL_FUNCTION(TRNsampleGetMeta);
   COLstring Root = FILparentDir(SampleFile);
   COLstring MetaFileName = "_" + FILpathName(SampleFile) + ".meta"; //prefixing with _ sends them to the bottom of the file list, makes the highlighting behaviour better
   COLstring MetaFile = FILpathAppend(Root, MetaFileName);
   COL_VAR(MetaFile);
   if (!FILfileExists(MetaFile)){ 
      COL_TRC("No meta file for this sample data, setting function: main");
      *pFunction = COL_T("main"); 
      return; 
   }
   COLvar MetaData;
   try{
      FILserializeLoadConfig(&MetaData, MetaFile);
      if (MetaData.exists("function")){
         COL_TRC("Setting function: " << MetaData["function"]);
         *pFunction = MetaData["function"]; 
      }
   } catch(const COLerror& Error){
      COL_ERR(Error.description()); // TODO - handle me?
   }
}