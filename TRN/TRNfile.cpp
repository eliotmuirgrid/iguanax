//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNfile
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, November 19th, 2010 @ 10:34:07 AM
//---------------------------------------------------------------------------
#include <TRN/TRNfile.h>
#include <TRN/TRNfunction.h>
#include <TRN/TRNloadFile.h>
#include <TRN/TRNtargetSet.h>

#include <FIL/FILpathUtils.h>
#include <LUA/LUAlineParser.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void TRNcleanupFunctions(COLmap<int, TRNfunction*>& List){
   COL_FUNCTION(TRNcleanupFunctions);
   for (COLmapPlace i=List.first(); i; i=List.next(i)){
      delete List[i];
   }
}

TRNfile::TRNfile(const COLstring& FilePath, const COLstring& Label) : m_FilePath(FilePath), m_Label(FILpathSepToPosix(Label)) {
   COL_METHOD(TRNfile::TRNfile);
   COL_VAR2(FilePath, Label);   
   m_FileMissing = !TRNloadFile(m_FilePath, &m_Source);
   if (m_FileMissing) {
      m_Source = TRNblankFile();       
   }
   COL_VAR2(m_FilePath, m_Label);   
}

TRNfile::~TRNfile(){
   COL_METHOD(TRNfile::~TRNfile);
   COL_VAR(m_FilePath);
   TRNcleanupFunctions(m_FunctionList);
}

const TRNfunction* TRNfileFunctionAtLine(const TRNfile& File, int Start){
   COL_FUNCTION(TRNfileFunctionAtLine);
   COLmapPlace i = File.m_FunctionList.find(Start);
   return i ? File.m_FunctionList[i] : NULL;
}

void TRNclearFunctionMap(TRNfile* pFile){
   COL_FUNCTION(TRNclearFunctionMap);
   pFile->m_FunctionList.clear();
}

// Several functions defined on one line will be stored in the same TRNfunction object,
// but annotations will show the correct name/content for each iteration.
TRNfunction* TRNfindOrCreateFunction(TRNfile* pFile, int Start){
   COL_FUNCTION(TRNfindOrCreateFunction);
   COL_VAR(Start);
   COLmapPlace i = pFile->m_FunctionList.find(Start);
   if (!i) {
      COL_TRC("Adding function starting on line " << Start << " for file " << pFile->m_Label);
      i = pFile->m_FunctionList.add(Start, new TRNfunction(pFile, Start));
   } 
   return (pFile->m_FunctionList)[i];
}


TRNfunction* TRNfunctionAtLine(TRNfile* pFile, int Start){
   COL_FUNCTION(TRNfunctionAtLine);
   COL_VAR(Start);
   COLmapPlace i = pFile->m_FunctionList.find(Start);
   COL_VAR(pFile->m_FunctionList.size());
   TRNfunction* pResult = i ? (pFile->m_FunctionList)[i] : NULL;
   COL_VAR(pResult);
   return pResult;
}

bool TRNcheckUnreachableBlocks(TRNfile* pFile){
   COL_FUNCTION(TRNcheckUnreachableBlocks);
   bool Result = false;
   for (auto i = pFile->m_FunctionList.cbegin(); i; ++i){
      TRNfunction* pFunc = i->second;
      if (LUAXscopeTargetIteration(pFunc->m_Annotations) > 1 && pFunc->m_Annotations.m_CurrentIteration < LUAXscopeTargetIteration(pFunc->m_Annotations)){
         pFunc->m_Annotations.m_TargetIteration = COL_MAX(pFunc->m_Annotations.m_CurrentIteration, 1);
         Result = true;
      }
   }
   return Result;
}

void TRNcalcMaxLineLength(TRNfile* pFile){
   COL_FUNCTION(TRNcalcMaxLineLength);
   COLarray<int> LineLengthVector;
   LUAparseLineLengths(pFile->m_Source, &LineLengthVector);
   for (auto  i=pFile->m_FunctionList.cbegin(); i; ++i){
      TRNfunction* pFunc = i->second;
      pFunc->m_MaxLine = LUAmaxLineLengthInBlock(LineLengthVector, pFunc->m_Start-1, pFunc->m_End-1);      
   }
}

COLstring TRNunsupportedFile(){
   COL_FUNCTION(TRNunsupportedFile);
   COLstring Result = "-- File type is not supported. Empty content loaded as a placeholder. --\n"
                      "--                  Please edit or replace the file.                  --";   
   return Result;
}

COLstring TRNblankFile(){
   COL_FUNCTION(TRNblankFile);
   COLstring Result = "-- File not found. Empty content loaded as a placeholder. --\n"
                      "--            Please edit or replace the file.            --";   
   return Result;
}

bool TRNisSampleData(const COLstring& FileName){
   COL_FUNCTION(TRNisSampleData);
   return FileName.find("Samples") != npos;
}

COLostream& operator<<(COLostream& Stream, const TRNfile& File){
   Stream << "This file has " << File.m_FunctionList.size() << " function(s)." << indent;
   for (auto i=File.m_FunctionList.cbegin(); i != File.m_FunctionList.cend(); ++i){
      TRNfunction* pFunc = i->second;
      Stream << newline << "line = " << i->first << " " << *pFunc; 
   }
   Stream << unindent;
   return Stream;
}

COLostream& operator<<(COLostream& Stream, const COLhashmap<COLstring, TRNfile*>& FileList){
   Stream << "File List size = " << FileList.size();
   for (auto i=FileList.cbegin(); i != FileList.cend(); i++){
      Stream << newline << i->first << " " << *i->second;
   }
   return Stream;
}