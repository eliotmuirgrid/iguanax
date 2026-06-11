// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITbranch
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Tuesday 28 February 2023 - 12:40PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <COL/COLstring.h>
#include <COL/COLvarUtils.h>
#include <GIT/GITbranch.h>
#include <GIT/GITexecute.h>
#include <GIT/GITfetch.h>
#include <COL/COLsplit.h>
#include <PRO/PROexecute.h>

COL_LOG_MODULE;

static void GITtrimCRLF(COLstring& s){
   s.stripAll('\n');
   s.stripAll('\r');
}

static bool GITrun(const COLstring& Dir, const COLstring& Command, COLstring* pOut, COLstring* pError){
   COL_FUNCTION(GITrun);
   COL_VAR2(Dir, Command);
   COLstring LocalOut;
   COLstring* OutPtr = pOut ? pOut : &LocalOut;
   COLstring Err;
   const int ExitCode = GITexecuteString(Dir, Command, OutPtr, &Err);
   COL_VAR3(ExitCode, *OutPtr, Err);
   if(ExitCode == 0) { return true; }
   if(pError) { *pError = Err.isWhitespace() ? *OutPtr : Err; }
   if(pError && !pError->isWhitespace()) { COL_ERR(*pError); }
   return false;
}

// Returns true if Out contains at most one "meaningful" line.
// i.e., either no '\n', or only whitespace after the first '\n'.
static bool GITisSingleLineOutput(const COLstring& Out){
   COL_FUNCTION(GITisSingleLineOutput);
   int NewlinePos = -1;
   if(!Out.find(&NewlinePos, '\n')) { return true; }
   for(int i = NewlinePos + 1; i < Out.size(); ++i){
      const char c = Out[i];
      if(c != '\r' && c != '\n' && c != ' ' && c != '\t'){
         return false;
      }
   }
   return true;
}

// Quietly resolve a ref to a SHA. Returns false if it doesn't exist / can't be resolved.
static bool GITresolveShaQuiet(const COLstring& Dir, const COLstring& Ref, COLstring* pSha){
   COL_FUNCTION(GITresolveShaQuiet);
   COL_VAR2(Dir, Ref);

   COLstring Out, Err;
   const COLstring Command = "git rev-parse --verify --quiet " + Ref;
   const int ExitCode = GITexecuteString(Dir, Command, &Out, &Err);
   COL_VAR3(ExitCode, Out, Err);

   if(ExitCode != 0) { return false; }
   GITtrimCRLF(Out);
   if(Out.isWhitespace()) { return false; }
   if(pSha) { *pSha = Out; }
   return true;
}

// Returns up to 2 short refs from Namespace, optionally excluding a full ref name.
// If output is a single line, there was exactly 1 match.
static bool GITforEachRefShortCount2(const COLstring& Dir,
                                    const COLstring& Namespace,
                                    const COLstring& ExcludeFullRef,
                                    COLstring* pOut){
   COL_FUNCTION(GITforEachRefShortCount2);
   COL_VAR3(Dir, Namespace, ExcludeFullRef);

   COLstring Out;

   const COLstring Command =
      ExcludeFullRef.isWhitespace()
         ? ("git for-each-ref --count=2 --format=%(refname:short) " + Namespace)
         : ("git for-each-ref --count=2 --format=%(refname:short) --exclude=" + ExcludeFullRef + " " + Namespace);

   const bool Ok = GITrun(Dir, Command, &Out, nullptr);
   if(!Ok) { return false; }
   if(pOut) { *pOut = Out; }
   return true;
}

COLstring GITbranchCurrent(const COLstring& Dir){
   COL_FUNCTION(GITbranchCurrent);
   COLstring Out, Err;
   GITrun(Dir, "git rev-parse --abbrev-ref HEAD", &Out, &Err);
   GITtrimCRLF(Out);
   return Out;
}

// 1) Preferred (new init): origin/HEAD is a SYMBOLIC ref.
static bool GITdefaultBranchFromSymbolicOriginHead(const COLstring& Dir, COLstring* pDefault){
   COL_FUNCTION(GITdefaultBranchFromSymbolicOriginHead);
   if(pDefault) { pDefault->clear(); }
   COLstring Out, Err;
   const bool Ok = GITrun(Dir, "git symbolic-ref --quiet --short refs/remotes/origin/HEAD", &Out, &Err);
   COL_VAR(Ok);
   GITtrimCRLF(Out);
   if(!Ok) { return false; }
   if(!COLstringHasPrefix(Out, "origin/")) { return false; }
   if(pDefault) { *pDefault = COLstringCutPrefix(Out, "origin/"); }
   return !pDefault->isWhitespace();
}

// 2) Legacy (old init): origin/HEAD is a DIRECT ref (SHA). Match it to known names.
static bool GITdefaultBranchFromLegacyOriginHeadSha(const COLstring& Dir, COLstring* pDefault){
   COL_FUNCTION(GITdefaultBranchFromLegacyOriginHeadSha);
   if(pDefault) { pDefault->clear(); }
   COLstring OriginHeadSha;
   if(!GITresolveShaQuiet(Dir, "refs/remotes/origin/HEAD", &OriginHeadSha)) { return false; }
   const COLstring Candidates[] = { "main", "master", "trunk", "develop" };
   for(const auto& Name : Candidates){
      COLstring Sha;
      if(GITresolveShaQuiet(Dir, "refs/remotes/origin/" + Name, &Sha) && Sha == OriginHeadSha){
         if(pDefault) { *pDefault = Name; }
         return true;
      }
   }
   return false;
}

// 3) If exactly one remote-tracking branch exists under origin (excluding origin/HEAD), use it.
static bool GITdefaultBranchFromOnlyRemoteTrackingBranch(const COLstring& Dir, COLstring* pDefault){
   COL_FUNCTION(GITdefaultBranchFromOnlyRemoteTrackingBranch);
   if(pDefault) { pDefault->clear(); }
   COLstring Out;
   const bool Ok = GITforEachRefShortCount2(Dir, "refs/remotes/origin", "refs/remotes/origin/HEAD", &Out);
   if(!Ok) { return false; }
   if(Out.isWhitespace()) { return false; }
   if(!GITisSingleLineOutput(Out)) { return false; }
   GITtrimCRLF(Out); // "origin/<name>"
   if(!COLstringHasPrefix(Out, "origin/")) { return false; }
   if(pDefault) { *pDefault = COLstringCutPrefix(Out, "origin/"); }
   return !pDefault->isWhitespace();
}

// 4) Last resort: if exactly one local branch exists and it is "main", assume default is "main".
static bool GITdefaultBranchFromOnlyLocalMain(const COLstring& Dir, COLstring* pDefault){
   COL_FUNCTION(GITdefaultBranchFromOnlyLocalMain);
   if(pDefault) { pDefault->clear(); }
   COLstring Out;
   const bool Ok = GITforEachRefShortCount2(Dir, "refs/heads", "", &Out);
   if(!Ok) { return false; }
   if(Out.isWhitespace()) { return false; }
   if(!GITisSingleLineOutput(Out)) { return false; }
   GITtrimCRLF(Out); // "<name>"
   if(Out != "main") { return false; }
   if(pDefault) { *pDefault = "main"; }
   return true;
}

static bool GITremoteDefaultBranch(const COLstring& Dir, COLstring* pOut, COLstring* pError){
   COL_FUNCTION(GITremoteDefaultBranch);
   COL_TRC("Calling remote to get HEAD");
   COLstring Out, Error;
   bool Ok = GITrun(Dir, "git ls-remote --quiet --symref origin HEAD", &Out, &Error);
   COL_VAR2(Out, Error);
   if(!Ok) { return false; }
   if(Out.isWhitespace()) { return false; }
   if(!COLstringHasPrefix(Out, "ref:")) { return false; }
   COLarray<COLstring> List;
   COLsplit(&List, Out, "\n");
   const size_t SlashPos = List[0].rfind("/");
   const size_t TabPos = List[0].find('\t');
   if (TabPos == npos || SlashPos == npos) { return false; }
   *pOut = List[0].substr(SlashPos+1,TabPos - SlashPos);
   return true;
}

bool GITsetLocalDefaultBranch(const COLstring& Dir, const COLstring& Branch, COLstring* pOut, COLstring* pError){
   COL_FUNCTION(GITsetLocalDefaultBranch);
   COLstring Command = "git symbolic-ref refs/remotes/origin/HEAD refs/remotes/origin/" + Branch;
   bool Ok = GITrun(Dir, Command, pOut, pError);
   return Ok;
}

static void GITsetDefaultBranchFromRemoteImpl(const COLstring& Dir){
   COL_FUNCTION(GITsetDefaultBranchFromRemoteImpl);
   COLstring Out, Error;
   if(!GITremoteDefaultBranch(Dir, &Out, &Error)) { return; }
   COL_TRC("Remote default branch: " + Out);
   int ExitCode = GITsetLocalDefaultBranch(Dir, Out, &Out, &Error);
   COL_VAR3(ExitCode, Out, Error);
   return;
}

void GITupdateDefaultBranch(const COLstring& Dir){
   COL_FUNCTION(GITupdateDefaultBranch);
   if(GITrepositoryDefaultBranch(Dir) != "") { return; }
   GITsetDefaultBranchFromRemoteImpl(Dir);
}

COLstring GITrepositoryDefaultBranch(const COLstring& Dir){
   COL_FUNCTION(GITrepositoryDefaultBranch);
   COLstring Default;
   if(GITdefaultBranchFromSymbolicOriginHead(Dir, &Default)) { return Default; }
   if(GITdefaultBranchFromLegacyOriginHeadSha(Dir, &Default)) { return Default; }
   if(GITdefaultBranchFromOnlyRemoteTrackingBranch(Dir, &Default)) { return Default; }
   if(GITdefaultBranchFromOnlyLocalMain(Dir, &Default)) { return Default; }
   return "";
}  

COLstring GITbranchCurrentDev(const COLstring& Dir){
   COL_FUNCTION(GITbranchCurrentDev);
   const COLstring Branch        = GITbranchCurrent(Dir);
   const COLstring DefaultBranch = GITrepositoryDefaultBranch(Dir);
   COL_VAR2(Branch, DefaultBranch);
   if(Branch == "HEAD"){ return Branch; }
   if(!DefaultBranch.isWhitespace() && Branch == DefaultBranch){ return ""; }
   return Branch;
}

bool GITbranchValidateName(const COLstring& Dir, const COLstring& BranchName, COLstring* pError){
   COL_FUNCTION(GITbranchValidateName);
   if(BranchName.isWhitespace()){
      if(pError) { *pError = "Branch name cannot be empty"; }
      return false;
   }
   // Git-native validation; avoids ad-hoc rules.
   COLstring Out, Err;
   if(GITrun(Dir, "git check-ref-format --branch " + BranchName, &Out, &Err)) { return true; }
   if(pError) {
      *pError = !Err.isWhitespace() ? Err : (!Out.isWhitespace() ? Out : "Invalid branch name: " + BranchName);
   }
   return false;
}

bool GITbranchExistsLocal(const COLstring& Dir, const COLstring& BranchName, const COLstring& Username, bool* pExists, COLstring* pError){
   COL_FUNCTION(GITbranchExistsLocal);
   if(pExists) { *pExists = false; }
   // show-ref returns:
   //   0 => exists
   //   1 => does not exist
   // else => error
   COLstring Out, Err;
   COLstring Command = "git show-ref --verify --quiet refs/heads/" + BranchName;
   COL_VAR2(Dir, Command);
   int ExitCode = GITexecuteString(Dir, Command, &Out, &Err);
   if (ExitCode != 0 ){
      COLstring FetchOut, FetchErr;
      GITfetch(Username, Dir, "", &FetchOut, &FetchErr);
      COL_VAR2(FetchOut, FetchErr);
      Command = "git show-ref --verify --quiet refs/remotes/origin/" + BranchName;
      COL_VAR2(Dir, Command);
      ExitCode = GITexecuteString(Dir, Command, &Out, &Err);   
   }
   COL_VAR3(ExitCode, Out, Err);
   if(ExitCode == 0) { if(pExists) { *pExists = true; } return true; }
   if(ExitCode == 1) { if(pExists) { *pExists = false; } return true; }
   if(pError) { *pError = Err.isWhitespace() ? Out : Err; }
   return false;
}

bool GITbranchCreateAndCheckout(const COLstring& Dir, const COLstring& BranchName, COLstring* pError){
   COL_FUNCTION(GITbranchCreateAndCheckout);
   COLstring Out;
   return GITrun(Dir, "git checkout -b " + BranchName, &Out, pError);
}

bool GITbranchCheckoutExistingOrTrack(const COLstring& Dir, const COLstring& BranchName, COLstring* pError){
   COL_FUNCTION(GITbranchCheckoutExistingOrTrack);
   COLstring Out;
   if(GITrun(Dir, "git checkout " + BranchName, &Out, nullptr)) { return true; }
   return GITrun(Dir, "git checkout -t origin/" + BranchName, &Out, pError);
}
