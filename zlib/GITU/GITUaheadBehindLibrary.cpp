//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITUaheadBehindLibrary.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   03/10/24 2:36 PM
//  ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <COL/COLstring.h>
#include <GITU/GITUaheadBehindLibrary.h>
#include <PRO/PROexecute.h>
COL_LOG_MODULE;

bool GITUselectedBranch(const COLstring& Branch){
   COL_FUNCTION(GITUselectedBranch);
   if(Branch[0] == '*') return true;
   return false;
}

// TODO - rename and unit test - it's more generalized.
bool GITUaheadBehindLibrary(const COLstring& Dir, bool* pAhead, bool* pBehind, COLstring* pError){
   COL_FUNCTION(GITUaheadBehindLibrary);
   COLstring Out;
   COLstring Command = "git branch -v";
   COL_VAR2(Dir, Command);
   int ExitCode = PROexecuteString(Dir, Command, &Out, pError);
   COL_VAR2(Out, *pError);
   COLstring Branch, Remainder;
   Remainder = Out;
   while(Remainder.split(Branch, Remainder, "\n")){
      if(GITUselectedBranch(Branch)){
         COL_TRC("Selected branch: " + Branch);
         // TODO - put in more checks for whether it is from the commit message instead of within the [ ] 
         (*pAhead)  = COLstringContains(Branch, "ahead") || COLstringContains(Branch, "[gone]");
         (*pBehind) = COLstringContains(Branch, "behind");
         break;
      }
   }
   COL_VAR2( *pAhead, *pBehind );
   return ExitCode == 0;
}