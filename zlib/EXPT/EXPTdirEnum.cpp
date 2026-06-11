// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EXPTdirEnum
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 17 November 2025 - 10:23AM
// ---------------------------------------------------------------------------
#include <EXPT/EXPTdirEnum.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#define MAX_FILE_SIZE 102400   // 100 KB

// Helper function to determine if we should skip this file because it is handled elsewhere
static bool EXPTshouldSkipEarlyHandledFile(const COLstring& Root, const COLstring& FullPath) {
   COL_FUNCTION(EXPTshouldSkipEarlyHandledFile);
   return (FullPath == FILpathAppend(Root, "main.lua") || FullPath == FILpathAppend(Root, "config.json"));
}

// Helper function to determine if we should skip the directory (specifically the Samples dir)
static bool EXPTshouldSkipSamplesDir(const COLstring& Root, const COLstring& FullPath, bool IgnoreSamples) {
   COL_FUNCTION(EXPTshouldSkipSamplesDir);
   return (FullPath == FILpathAppend(Root, "Samples") && IgnoreSamples);
}

// Skip all .git directories, not just the one in root. Adding a library brings in its own .git as an example.
static bool EXPTshouldSkipGitDir(const COLstring& DirName) {
   COL_FUNCTION(EXPTshouldSkipGitDir);
   return (DirName == ".git");
}

// Helper function to check file size, appending any skip reason if too large
static bool EXPTshouldSkipLargeFile(const COLstring& FullPath, const COLstring& CurrentPath, COLvar* pSkipped) {
   COL_FUNCTION(EXPTshouldSkipLargeFile);
   const size_t Size = FILfileSize(FullPath);
   if (Size >= MAX_FILE_SIZE) {
      COL_TRC("Skipping file " << FullPath << " greater than 100kb");
      (*pSkipped)[CurrentPath] = "size greater than 100kb";
      return true;
   }
   return false;
}

// Helper function to read file contents, handling errors and reporting skipped files if needed
static bool EXPTreadFileWithSkip(const COLstring& FullPath, const COLstring& CurrentPath, COLstring* pContent, COLvar* pSkipped) {
   COL_FUNCTION(EXPTreadFileWithSkip);
   try {
      FILreadFile(FullPath, pContent);
      return true;
   } catch (const COLerror& Error) {
      COL_TRC("Skipping file " << FullPath << " error caught when reading file: " << Error.description());
      (*pSkipped)[CurrentPath] = "error when reading";
      return false;
   }
}

// Helper function to append file information to output
static void EXPTappendFileToOutput(const COLstring& CurrentPath, const COLstring& Content, COLstring* pOutput) {
   COL_FUNCTION(EXPTappendFileToOutput);
   pOutput->append("\n-- FILE: " + CurrentPath + "\n");
   pOutput->append(Content);
}

// Root should be the component project directory
void EXPTgetAllFilesStrRecursive(const COLstring& Root, const COLstring& CurrentDir, bool IgnoreSamples, COLstring* pOutput, COLvar* pSkipped){
   COL_FUNCTION(EXPTgetAllFilesStrRecursive);
   COL_VAR2(Root, CurrentDir);
   FILdirEnumerator Enum(FILpathAppend(CurrentDir, FIL_ANYFILE));
   COLstring File;
   while (Enum.getFile(File)) {
      COL_VAR3(File, Enum.isDirectory(), Enum.isHidden());
      if (Enum.isHidden()) { continue; }
      const COLstring FullPath = FILpathAppend(CurrentDir, File);
      COLstring CurrentPath = FullPath;
      CurrentPath.replace(Root, "");
      if (Enum.isDirectory()) {
         if (EXPTshouldSkipGitDir(File)) { continue; } // .git is NOT considered hidden for Windows 
         if (EXPTshouldSkipSamplesDir(Root, FullPath, IgnoreSamples)) { continue; }
         COL_TRC("Descending into folder: " << FullPath);
         EXPTgetAllFilesStrRecursive(Root, FullPath, IgnoreSamples, pOutput, pSkipped);
         continue;
      }
      if (EXPTshouldSkipEarlyHandledFile(Root, FullPath)) { continue; }
      if (EXPTshouldSkipLargeFile(FullPath, CurrentPath, pSkipped)) { continue; }
      COLstring Content;
      if (!EXPTreadFileWithSkip(FullPath, CurrentPath, &Content, pSkipped)) { continue; }
      EXPTappendFileToOutput(CurrentPath, Content, pOutput);
   }
}

void EXPTcaptureSpecificFile(const COLstring& Root, const COLstring& File, COLstring* pOutput, COLvar* pSkipped){
   COL_FUNCTION(EXPTcaptureSpecificFile);
   COLstring FilePath = FILpathAppend(Root, File);
   COLstring Content;
   if (FILfileExists(FilePath)){
      COL_TRC(FilePath + " exists");
      try {
         FILreadFile(FilePath, &Content);
      } catch (const COLerror& Error){
         COL_TRC("Skipping file " << FilePath << " error caught when reading file: " << Error.description());
         (*pSkipped)[FilePath] = "error when reading";
         return;
      }
   }
   pOutput->append("\n-- FILE: " + File + "\n");
   pOutput->append(Content);
}