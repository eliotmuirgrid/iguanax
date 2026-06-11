//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLutils
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, February 11th, 2009 @ 09:49:46 AM
//
//---------------------------------------------------------------------------
#include <COL/COLwindows.h>
#include <COL/COLutils.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


#include <stdlib.h>
#include <ctype.h>

#ifndef _WIN32
// #   ifndef CHM#_HPUX          // JULIAN CHM_XXX
// #      include <sys/select.h>
// #   endif
#   include <sys/time.h>
#   include <unistd.h>
#   include <sched.h>
#endif

void COLsleep(int TimeOutInMilliSeconds ){
   COL_FUNCTION(COLsleep);
   COL_VAR(TimeOutInMilliSeconds);
#ifdef _WIN32
   // no return code from Sleep
   ::Sleep( TimeOutInMilliSeconds );
#else
   // The POSIX implementation does not have a hard guarantee on sleep duration.
   // It may be interrupted and return early from time to time.

   double Seconds = TimeOutInMilliSeconds * 0.001;
   if (Seconds < 0.0){
      // Do not return immediately - sleeping for 0 seconds is legitimate to
      // yield the thread.
      Seconds = 0.0;
   }

   struct timeval tv;
   tv.tv_sec = (COLuint64)Seconds;
   tv.tv_usec = (COLuint64)((Seconds - tv.tv_sec) * 1000000);

   // no need to check return code and errno, as we don't care
   // and will not throw an exception.
   ::select(0, NULL, NULL, NULL, &tv);
#endif
}

static int getVarNameLength(const char* Position){
   int VarLength = 0;
   while (Position[0] && Position[0] != '}'){
      ++VarLength;
      ++Position;
   }
   return Position[0] == 0 ? -1 :  VarLength;
}

COLstring COLexpandEnvironmentVariables(const COLstring& Original){
   COLstring Expanded;
   Expanded.setCapacity(Original.size());
   if (const char* Position = Original.c_str()){
      while (char Ch = Position[0]){
         if (Ch == '$'){
            int VarNameLength = 0;
            if (Position[1] == '{' && ((VarNameLength = getVarNameLength(Position+2)) >= 0)) {
               COLstring VarName = COLstring(Position+2, VarNameLength);
               if (const char* VarValue = ::getenv(VarName.c_str())){
                  Expanded += VarValue;
               }
               Position += 2 + VarNameLength + 1; // skip over ${VAR}
               continue;
            }
         }
         Expanded += Ch;
         ++Position;
      }
   }
   return Expanded;
}

COLstring COLexpandEnvironmentVariablesUsingCache(const COLstring& Original, const COLvar& CachedVars){
   COLstring Expanded;
   if (!CachedVars.isMap() || 0 == CachedVars.size()) { return ""; }
   Expanded.setCapacity(Original.size());
   if (const char* Position = Original.c_str()){
      while (char Ch = Position[0]){
         if (Ch == '$'){
            int VarNameLength = 0;
            if (Position[1] == '{' && ((VarNameLength = getVarNameLength(Position+2)) >= 0)) {
               COLstring VarName = COLstring(Position+2, VarNameLength);
               COL_VAR(VarName);
               if (CachedVars.exists(VarName)){
                  Expanded += CachedVars[VarName].asString();
               }
               Position += 2 + VarNameLength + 1; // skip over ${VAR}
               continue;
            }
         }
         Expanded += Ch;
         ++Position;
      }
   }
   return Expanded;
}

// pattern is text, except that it contains question marks that match any character.
static bool subpatternMatches(const char* Pattern, const char* Module, int Length){
   for (; Length && *Pattern && *Module; ++Pattern, ++Module, --Length){
      if (*Pattern != *Module && *Pattern != '?'){
         return false;
      }
   }
   return Length == 0;
}

static bool COLisNullByte(const char* pChar) { return *pChar == 0; }

static bool COLisChar(const char* pPattern) { return *pPattern && !COLisspace(*pPattern); }

static bool COLisWildcard(const char* pPattern) { return *pPattern == '*'; }

static bool COLendOfModule(const char*& Pattern, const char*& Module, bool* pSkipRestOfGlob) {
   if(!COLisNullByte(Module)) { return false; }
   // We have reached the end of the Module string, but current glob may not yet be finished.
   // The only valid remaining pattern at this point is "*".
   while(COLisWildcard(Pattern)) { ++Pattern; }
   if(COLisChar(Pattern)) { *pSkipRestOfGlob = true; } // failed since there is still more of the glob to process
   return true;
}

static bool COLsubpatternNotFound(const char* pPattern, bool TargetFound, bool* pSkipRestOfGlob) {
   if(!TargetFound) {
      *pSkipRestOfGlob = true;
      return true;
   } else if(!COLisChar(pPattern)) { return true; } // at end of glob, so break out of the glob loop
   return false; // still must process rest of this glob
}

static bool COLtargetFound(const char*& pTarget, const char*& pModule, int TargetLength, bool IsTargetAtEnd) {
   while(*pModule) {
      if(subpatternMatches(pTarget, pModule, TargetLength) && (!IsTargetAtEnd || COLisNullByte(&pModule[TargetLength]))) {
         // Target pattern found in Module
         pModule += TargetLength;
         return true;
      }
      ++pModule;
   }
   return false;
}

static bool COLhasWildcardMatch(const char*& pPattern, const char*& pModule) {
   // have glob advance past all contiguous '*' characters.
   while(COLisWildcard(pPattern)) { ++pPattern; }
   // *Pattern is now after the * and can be a letter, whitespace or NIL.
   if(COLisChar(pPattern)) { return false; }
   // end of glob reached - a successful match.
   pModule = ""; // point to any NIL - faster than advancing Module pointer.
   return true;
}

static bool COLmatchWildCard(const char*& Pattern, const char*& Module, bool* pSkipRestOfGlob) {
   if(COLhasWildcardMatch(Pattern, Module)) { return true; }
   // Handle characters immediately after the '*' in the glob.
   const char* Target = Pattern;
   int TargetLength = 0;
   char Ch;
   while((Ch = Target[TargetLength]) && !COLisspace(Ch) && Ch != '*') { TargetLength++; }
   Pattern += TargetLength;
   if(TargetLength) {
      // search for Target string in Module
      bool IsTargetAtEnd = !COLisChar(&Ch);
      bool TargetFound   = COLtargetFound(Target, Module, TargetLength, IsTargetAtEnd);
      return COLsubpatternNotFound(Pattern, TargetFound, pSkipRestOfGlob);
   }
   return false;
}

static bool COLisPositiveMatch(const char*& pPattern) {
   if(*pPattern != '-') { return true; }
   ++pPattern; // skip the leading '-'
   return false;
}

bool COLglobMatch(const char* Pattern, const char* ModuleToMatch) {
   bool MatchCandidate = false;
   // the glob list loop
   while(*Pattern) {
      while(COLisspace(*Pattern)) { ++Pattern; }
      if(COLisNullByte(Pattern)) { break; }

      // Pattern pointer now points to the beginning of a glob pattern.
      // A glob ends when first whitespace character or NIL is reached.
      const char* Module = ModuleToMatch;
      bool IsPositiveMatchMode = COLisPositiveMatch(Pattern);
      bool SkipRestOfGlob      = false;

      while(COLisChar(Pattern)) {
         if(COLisWildcard(Pattern)) {
            if(COLmatchWildCard(Pattern, Module, &SkipRestOfGlob)) { break; }
            continue;
         }
         if(*Pattern == *Module || *Pattern == '?') { // glob character matches, so advance pointers
            ++Pattern;
            ++Module;
            if(COLendOfModule(Pattern, Module, &SkipRestOfGlob)) { break; }
         } else {
            // glob does not match or is longer than the module - no match.
            SkipRestOfGlob = true;
            break;
         }
      }
      if(SkipRestOfGlob) {
         while(COLisChar(Pattern)) { ++Pattern; }
      }
      else if(COLisNullByte(Module)) {
         MatchCandidate = IsPositiveMatchMode; // reached end of glob pattern and end of Module - successful match
      }
   }
   return MatchCandidate;
}

