// ---------------------------------------------------------------------------
// Copyright (C) 2024 Eliot Muir.  All Rights Reserved
//
// Module: CNVgit
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Sunday 15 December 2024 - 04:58PM
// ---------------------------------------------------------------------------

#include <CNV/CNVgit.h>

#include <PRO/PROexecute.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void CNVclean(COLostream& Stream){
   COL_FUNCTION(CNVclean);
   if (FILfileExists("IguanaMainRepo")){
      COLcout << "Removing IguanaMainRepo...";
      COLstring Error;
      if (!FILremoveFullDirNewSafe("IguanaMainRepo", &Error)){
         COLcout << "Removal of IguanaMainRepo failed " << Error << newline;
      }
      COLcout << "DONE" << newline;
   }
}

void CNVclone(const COLstring& Dir, COLostream& Out){
   COL_FUNCTION(CNVclone);
   COLstring OutString, ErrString;
   Out << "Cloning...";
   int Result = PROexecuteString(".", "git clone " + Dir + "/IguanaMainRepo/", &OutString, &ErrString);
   COL_VAR3(Result, OutString, ErrString);
   Out << "DONE" << newline;
}