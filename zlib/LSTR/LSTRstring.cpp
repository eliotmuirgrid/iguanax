//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LSTRstring
//
// Description:
//
// Implementation
//
// Author: Akshay Ganeshen
// Date:   Thu  6 Nov 2014 15:23:45 EST
//
//---------------------------------------------------------------------------
#include "LSTRstring.h"

#include <COL/COLauto.h>
#include <COL/COLstring.h>
#include <COL/COLarray.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLsplit.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <LUA/LUAutils.h>
#include <LUA/LUAvar.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAcheck.h>

#include <LUAL/LUALutils.h>
#include <REX/REXfind.h>

// To prevent COL_VAR taking up insane levels of output, use this to truncate
static const int ColVarTruncationLimit = 1024;

// Rewritten to use pointers for the majority of the work (instead of using COLsplit and then converting the COLarray to a COLvar)
// Some reference is provided in t25348 comments
int LSTRsplit(lua_State* L) {
   COL_FUNCTION(LSTRsplit);
   LUAcheckArgCount(L, 2, -1);
   COLstring Data, SplitToken;
   if (!LUAextractString(L, 1, &Data) || !LUAextractString(L, 2, &SplitToken)) {
     luaL_error(L, "Arguments must be either strings or node trees.");
   }
   // Push back values into the return COLvar while moving a char* along the data
   COLvar SplitListVar = COLvar();
   const char* pDataPosition = Data.c_str();
   // Just in case the compiler can't figure it out, calculate the end position for optimization in the loop
   const char* const pEndPosition = Data.c_str() + Data.size();
   const int SplitTokenLength = SplitToken.size();
   if (!SplitTokenLength) {
      luaL_error(L, "Split string is empty.");
   }
   while (true) {
      const char* pNextTokenStart = strstr(pDataPosition, SplitToken.c_str());
      if (!pNextTokenStart) {
         // Go until the end of the string
         SplitListVar.push_back(COLstring(pDataPosition, pEndPosition - pDataPosition /* Remaining bytes */));
         break;
      }
      SplitListVar.push_back(COLstring(pDataPosition, pNextTokenStart - pDataPosition));
      pDataPosition = pNextTokenStart + SplitTokenLength;
   }

#ifdef COL_ENABLE_TRACING
   if (Data.size() > ColVarTruncationLimit) {
      COLstring Truncated = Data.substr(0, ColVarTruncationLimit - 3) + "...";
      COL_VAR3(SplitListVar, Truncated, SplitToken);
   } else {
      COL_VAR3(SplitListVar, Data, SplitToken);
   }
#endif /* COL_ENABLE_TRACING */

   LUApushVar(L, SplitListVar);
   return 1;
}

// This is a helper method that pushes the results of a regex find onto the stack in the same format gmatch does
// Returns the number of values pushed onto the lua stack
static int LSTRpushFinderMatches(lua_State* L, const REXfind& Finder) {
   COL_FUNCTION(LSTRpushFinderMatches);

   // If match groups are present, the individual matches are pushed instead of the global match
   // This is the behaviour of gmatch, so users familiar with the return style of that will find this to be analogous
   if (Finder.numMatchGroups() > 0) {
      for (int i = 1; i <= Finder.numMatchGroups(); i++) {
         if (Finder.matchPresent(i)) {
            REXresult Match = Finder.match(i);
            COL_TRC("Match group " << i << ": " << Match);
            lua_pushlstring(L, Match.pData, Match.Size);
         } else {
            COL_TRC("Match group " << i << " not present");
            lua_pushnil(L);
         }
      }

      return Finder.numMatchGroups();
   } else {
      if (Finder.matchPresent(0)) {
         REXresult Match = Finder.match(0);   // the global match
         COL_TRC("No match groups, so using the global match: " << Match);
         lua_pushlstring(L, Match.pData, Match.Size);
      } else {
         // This isn't necessarily an error, pcre already knows not to return empty matches, so trust its judgement here
         COL_TRC("No global match");
         lua_pushnil(L);
      }

      return 1;
   }
}

// This helper method extracts the REXoption enum values from a COLstring
// Throws on an invalid option, but silently allows option respecification and conflicts
//    libpcre has its own specifications on what it does when conflicting options are specified, so leave it to libpcre
static int LSTRrxExtractOptions(const COLstring& OptionString) {
   COL_FUNCTION(LSTRrxExtractOptions);
   COL_VAR(OptionString);

   // Need to work with ints for enum considerations
   int Options = REXfind::REX_DEFAULT;
   // Creating an array would be more efficient, but it's not too important that this be efficient
   for (int i = 0; i < OptionString.size(); i++) {
      switch(OptionString[i]) {
      case 'm':
         Options |= REXfind::REX_MULTILINE;
         break;
      case 'i':
         Options |= REXfind::REX_CASE_INSENSITIVE;
         break;
      case 'x':
         Options |= REXfind::REX_IGNORE_COMMENTS;
         break;
      case 'X':
         Options |= REXfind::REX_EXTRA;
         break;
      case 's':
         Options |= REXfind::REX_DOT_ALL;
         break;
      case 'u':
         Options |= REXfind::REX_UNICODE;
         break;
      case 'U':
         Options |= REXfind::REX_UNGREEDY;
         break;
      case 'A':
         Options |= REXfind::REX_ANCHORED;
         break;
      case 'J':
         Options |= REXfind::REX_ALLOW_DUPLICATE_NAMES;
         break;
      // Feel free to add more here!
      default:
         COL_ERROR_STREAM_PLAIN("Unrecognized option '" << OptionString[i] << "'.", COLerror::PreCondition);
      }
   }

   return Options;
}

// This is a small RAII-style helper class that deallocates the REXfind state for LSTRrxmatchIterator
// Having Lua manage the lifetime of these objects is best, since the user might not even iterate through all the items (or may try to iterate through it twice)
#define LSTRrxmatch_ITERATOR_METANAME  "RxMatchMeta"
class LSTRrxmatchIteratorData {
public:
   LSTRrxmatchIteratorData() {
      COL_METHOD(LSTRrxmatchIteratorData::LSTRrxmatchIteratorData);
      m_Initialized = false;
   }
   ~LSTRrxmatchIteratorData() {
      COL_METHOD(LSTRrxmatchIteratorData::~LSTRrxmatchIteratorData);
#ifdef COL_ENABLE_TRACING
      if (m_pSubject.isOwner() != m_pFinder.isOwner()) {
         COL_ERR("Ownership mismatch - this class should either manage both, or neither...");
         COL_ERR("m_pSubject.isOwner() = " << m_pSubject.isOwner() <<
                 ", m_pFinder.isOwner() = " << m_pFinder.isOwner());
      }
      if (m_pSubject.isOwner()) {
         COL_TRC("Freeing rxmatch subject data: " << m_pSubject.get());
      } else {
         COL_TRC("Temporary - not freeing rxmatch subject data");
      }
      if (m_pFinder.isOwner()) {
         COL_TRC("Freeing rxmatch finder data: " << m_pFinder.get());
      } else {
         COL_TRC("Temporary - not freeing rxmatch finder data");
      }
#endif
   }
   // Warning: This can throw!
   void init(const COLstring& RegexString, int RegexOptions, const COLstring& SubjectString) {
      COL_METHOD(LSTRrxmatchIteratorData::init);
      COL_VAR3(RegexString, RegexOptions, SubjectString);
      m_pSubject = new COLstring(SubjectString);
      m_pFinder = new REXfind();
      COL_TRC("Allocated rxmatch subject data: " << m_pSubject.get());
      COL_TRC("Allocated rxmatch finder data: " << m_pFinder.get());
      m_pFinder->init(RegexString.data(), RegexOptions);
      m_pFinder->matchBegin(m_pSubject->data(), m_pSubject->size());
      m_Initialized = true;
   }

   REXfind* finder() {
      COL_METHOD(LSTRrxmatchIteratorData::finder);
      if (!m_Initialized) {
         COL_ERR("init has not been called!");
      }
      COLASSERT(m_Initialized);
      return m_pFinder.get();
   }

private:
   // NOTE : These must be dynamically allocated - it'll get pushed onto the Lua stack and copied, so the pointers must match up
   bool m_Initialized;
   COLauto<COLstring> m_pSubject;
   COLauto<REXfind> m_pFinder;
};
// Warning: This can throw!
static LSTRrxmatchIteratorData* LSTRrxmatchIteratorDataFromLua(lua_State* L, int i) {
   COL_FUNCTION(LSTRrxmatchIteratorDataFromLua);
   const int StackTop = lua_gettop(L);
   void* UserData = LUAcheckUdata(L, i, LSTRrxmatch_ITERATOR_METANAME, "'rxmatch' iterator data expected");
   COLASSERT(StackTop == lua_gettop(L));
   return (LSTRrxmatchIteratorData*)UserData;
}

// This is the actual iterator function returned by regex match
// The function is provided as a closure, and follows the requirements for a lua iterator
static int LSTRrxmatchIterator(lua_State* L) {
   COL_FUNCTION(LSTRrxmatchIterator);
   // This is set up as a C closure, and the upvalue at index 1 is a LSTRrxmatchIteratorData
   // This contains all the data required to continue matching (namely, the REXfind object)
   LSTRrxmatchIteratorData* pData = NULL;
   try {
      pData = LSTRrxmatchIteratorDataFromLua(L, lua_upvalueindex(1));
   } LUA_CATCH();

   REXfind* pFinder = pData->finder();
   if (!pFinder->matchNext()) {
      // Return a nil to indicate end of iterator
      COL_TRC("Done iterating");
      lua_pushnil(L);
      return 1;
   }
   return LSTRpushFinderMatches(L, *pFinder);
}

int LSTRrxmatch(lua_State* L) {
   COL_FUNCTION(LSTRrxmatch);
   int CountOfArgument = LUAcheckArgCount(L, 2, 3);
   COLstring Data, Regex, OptionString;
   int RegexOptions = REXfind::REX_NOTEMPTY; // always assume non-empty global matches, or else infinite recursion
   if (!LUAextractString(L, 1, &Data) || !LUAextractString(L, 2, &Regex)) {
      luaL_error(L, "Arguments must be either strings or node trees.");
   }
   if (CountOfArgument == 3 && !lua_isnil(L, 3)) {
      if (!lua_isstring(L, 3)) {
         luaL_error(L, "Regular expression modifiers must be provided as a string.");
      }
      OptionString = LUAtoString(L, 3);
      // Give the ill-formed option error to the translator, since the user was the one who specified these options
      try {
         RegexOptions |= LSTRrxExtractOptions(OptionString);
      } LUA_CATCH();
   }
   COL_VAR2(Data, Regex);
   // Create the regex object first - it will be managed by Lua itself, so we don't have to worry about it
   LSTRrxmatchIteratorData LuaData;
   try {
      COL_TRC("initializing on data: " << Data);
      LuaData.init(Regex, RegexOptions, Data);
   } LUA_CATCH();
   // Create a closure with the iterator object
   LUApushObject<LSTRrxmatchIteratorData>(L, LuaData, LSTRrxmatch_ITERATOR_METANAME);
   lua_pushcclosure(L, &LSTRrxmatchIterator, 1);

   return 1;
}

// This helper method expands back references in the target string (in the form $n or \\n) using the current match of the REXfind object
// It must therefore be called in every match to be able to expand the back references of the whole string
static void LSTRexpandCurrentBackReferences(COLstring* pResult, const REXfind& Finder, const COLstring& ExpandPattern) {
   COL_FUNCTION(LSTRexpandCurrentBackReferences);
   COL_VAR(ExpandPattern);

   // Expand along the ExpandPattern from left to right, so that the unmatched portions can be appended into the string, all in one pass
   static const int NumBackReferenceChars = 2;
   static const char* BackReferenceSequences[] = { "$", "\\" };  // Has to be strings, as that's what COLstring.find accepts, but the code below assumes each are length 1
   int BackReferenceFoundPositions[NumBackReferenceChars];
   size_t ExpandPatternPosition = 0;   // Has to be size_t since that's what COLstring.find accepts

   while (ExpandPatternPosition < ExpandPattern.size()) {
      COL_TRC("Built so far: \"" << *pResult << "\"");
      COL_TRC("Expand pattern remaining: \"" << (ExpandPattern.c_str() + ExpandPatternPosition) << "\"");
      // Each 'for'-sequence segment will be unrolled, so the performance here will be fine
      for (int i = 0; i < NumBackReferenceChars; i++) {
         COL_TRC("Trying to find " << BackReferenceSequences[i] << " in \"" << (ExpandPattern.c_str() + ExpandPatternPosition) << "\"");
         if (!ExpandPattern.find(&BackReferenceFoundPositions[i], BackReferenceSequences[i], ExpandPatternPosition)) {
            COL_TRC("Not found");
            BackReferenceFoundPositions[i] = -1;   // No match whatsoever, going to work with < 0 (which 'npos' should already be, but just in case)
         } else {
            COL_TRC("Found (position = " << BackReferenceFoundPositions[i] << ")");
         }
      }

      int FirstFoundPosition = -1;
      for (int i = 0; i < NumBackReferenceChars; i++) {
         if (BackReferenceFoundPositions[i] >= 0) {
            // Circumventing the initialization of FirstFoundPosition being negative
            // Sorry, it's a little ugly:
            FirstFoundPosition = (FirstFoundPosition < 0) ? BackReferenceFoundPositions[i] :
                                                            COL_MIN(FirstFoundPosition, BackReferenceFoundPositions[i]);
         }
      }
      COL_VAR(FirstFoundPosition);

      // All potential backreferences may have been completed, so FirstFoundPosition will remain at -1
      // That means we're done, so append the unmatched end-portion of the pattern and return
      if (FirstFoundPosition < 0) {
         pResult->append(ExpandPattern.c_str() + ExpandPatternPosition, ExpandPattern.size() - ExpandPatternPosition);
         COL_TRC("No more matches - finished with: " << *pResult);
         break;
      }
      // else, there's a backreference sequence to take care of
      // Begin by appending the unmatched portion leading up to this find
      pResult->append(ExpandPattern.c_str() + ExpandPatternPosition, (FirstFoundPosition - ExpandPatternPosition));
      // Get the length of the digit in the string, so backreferences like "$12" will still be handled
      int DigitLength = 0;
      // NOTE : The back-reference sequence is assumed to be length 1: substr(FirstFoundPosition, FirstFoundPosition+1)
      //    Feel free to modify this to be dynamic if longer back-reference sequences are required
      while (COLisdigit(ExpandPattern.c_str()[FirstFoundPosition + 1 + DigitLength])) {
         DigitLength++;
      }
      COL_TRC("Back reference with " << DigitLength << " digits: " << COLstring(ExpandPattern.c_str() + FirstFoundPosition, DigitLength + 1));
      if (!DigitLength) {
         COL_TRC("Doesn't seem to have digits, so skipping this back reference");
         // Must be something like $foo, which we are just going to ignore (no replaces on that)
         // So just skip past the escaping character
         pResult->append(ExpandPattern.c_str() + FirstFoundPosition, 1);
         ExpandPatternPosition = FirstFoundPosition + 1;
         // This allows $$ and \\ to be turned into a literal $ or \ (so no back substitutions happen)
         if (ExpandPattern.size() > FirstFoundPosition + 1 && ExpandPattern[FirstFoundPosition] == ExpandPattern[FirstFoundPosition + 1]) {
            COL_TRC("Putting in a literal '" << ExpandPattern[FirstFoundPosition] << "'");
            ExpandPatternPosition++; // Skip over the second escaping character
         }
         continue;
      }

      int BackReferenceNumber = atoi(ExpandPattern.substr(FirstFoundPosition + 1, DigitLength).c_str());
      ExpandPatternPosition = FirstFoundPosition + DigitLength + 1;

      COL_TRC("Handling back reference " << BackReferenceNumber);

      // Now everything is set up, we can finally begin the substitution!
      if (BackReferenceNumber > Finder.numMatchGroups()) {
         throw COLerror("There is no match group for back reference '" + ExpandPattern.substr(FirstFoundPosition, DigitLength + 1) + "'.",
                        COLerror::PreCondition);
      }
      if (Finder.matchPresent(BackReferenceNumber)) {
         pResult->append(Finder.match(BackReferenceNumber).pData, Finder.match(BackReferenceNumber).Size);
      } else {
         // that's fine, it's not an error, a blank substitution is to be used in this case
         COL_TRC("Match group " << BackReferenceNumber << " is not present in this match, so leaving it blank");
      }
   }
}

// Helper method that does a replace on the current Finder match with the entry of the match in a table
static void LSTRrxsubReplaceMatchWithTableValue(COLstring* pResult, const REXfind& Finder, const COLvar& SubstitutionMap) {
   COL_FUNCTION(LSTRrxsubReplaceMatchWithTableValue);

   // NOTE : What if there is more than one match group? Could use nested tables, append matches into one string, etc. Not sure which makes the most sense
   if (Finder.numMatchGroups() > 1) {
      // For now, just don't allow it, though the implementation below handles it like gsub does
      throw COLerror("Cannot have more than one match group if using a table to make substitutions.");
   }

   COLstring Match;  // default to empty, which will substitute to empty if not defined in the user's provided map
   if (Finder.numMatchGroups() == 0) {
      if (Finder.matchPresent(0)) {
         Match = COLstring(Finder.match(0).pData, Finder.match(0).Size);
      } else {
         COL_TRC("No global match present");
      }
   } else if (Finder.numMatchGroups() >= 1) {
       // This behaviour copies gsub: use the first non-empty match
      for (int i = 1; i <= Finder.numMatchGroups(); i++) {
         if (Finder.matchPresent(i)) {
            COL_TRC("Found match for group i=" << i << ": " << Finder.match(i));
            Match = COLstring(Finder.match(i).pData, Finder.match(i).Size);
         } else {
            COL_TRC("No match found for group i=" << i);
         }
      }
   }
   COL_VAR(Match);

   if (SubstitutionMap.exists(Match)) {
      if (SubstitutionMap[Match].isBool()) {
         if (SubstitutionMap[Match].asBool()) {
            // This is how lua itself behaves, which is a bit strange, but replicated here
            throw COLerror("Error in key '" + Match + "': invalid replacement value (a boolean)");
         } else {
            pResult->append(Match);
         }
      } else if (SubstitutionMap[Match].isInt()) {
         pResult->append(COLintToString(SubstitutionMap[Match].asInt()));
      } else if (SubstitutionMap[Match].isString()) {
         pResult->append(SubstitutionMap[Match].str());
      } else {
         throw COLerror("Corresponding value for key '" + Match + "' must be either a string, integer, or boolean.", COLerror::PreCondition);
      }
   } else {
      pResult->append(Match); // just like gsub, don't do any substitution
   }
}

// Helper method that does a replace on the current Finder match with the provided lua function (state + index)
static void LSTRrxsubReplaceMatchWithFunction(COLstring* pResult, const REXfind& Finder, lua_State* L, int SubstitutionFunctionIndex) {
   COL_FUNCTION(LSTRrxsubReplaceMatchWithFunction);

   // Set up the substitution function to be called
   lua_pushvalue(L, SubstitutionFunctionIndex);
   // Set up the function arguments - Same semantics as LSTRrxmatch in terms of # of arguments
   int PushedArguments = LSTRpushFinderMatches(L, Finder);
   // Not doing a pcall - if it errors, that's fine
   lua_call(L, PushedArguments, 1 /* single return value is assumed */);

   COLstring ReturnedString;
   if (lua_isnumber(L, -1)) {
      ReturnedString = COLintToString(lua_tointeger(L, -1));
   } else if (lua_isboolean(L, -1)) {
      // TODO : Find out if gsub even supports functions that returns bools
      if (lua_toboolean(L, -1)) {
         luaL_error(L, "Substitution function returned true.");
      } else if (Finder.matchPresent(0)) {
         // Do not substitute
         ReturnedString = COLstring(Finder.match(0).pData, Finder.match(0).Size);
      }
   } else if (!LUAextractString(L, -1, &ReturnedString)) {
      luaL_error(L, "Substitution function did not return a string.");
   }
   // Take the return value off the stack
   lua_pop(L, 1);
   // Everything went fine, so the substitution is finished
   pResult->append(ReturnedString);
}

int LSTRrxsub(lua_State* L) {
   COL_FUNCTION(LSTRrxsub);
   int CountOfArgument = LUAcheckArgCount(L, 3, 5);
   COLstring Data, Regex, Substitution, OptionString;
   int RegexOptions = REXfind::REX_NOTEMPTY; // always assume non-empty global matches, or else infinite recursion
   int MaxSubs = 0;
   bool LimitSubs = false; // kept false to perform all the substitutions in the string
   if (!LUAextractString(L, 1, &Data)) {
      luaL_error(L, "Data argument must be a string or a node tree.");
   }
   if (!LUAextractString(L, 2, &Regex)) {
      luaL_error(L, "Regular expression must be a string or a node tree.");
   }
   if (CountOfArgument >= 4 && !lua_isnil(L, 4)) {
      if (lua_isnumber(L, 4)) {
         MaxSubs = lua_tointeger(L, 4);
         LimitSubs = true;
         COL_TRC("Limiting substitutions to " << MaxSubs);
         COL_VAR2(MaxSubs, LimitSubs);
      } else if (lua_isstring(L, 4)) {
         OptionString = LUAtoString(L, 4);
         // Give the ill-formed option error to the translator, since the user was the one who specified these options
         try {
            RegexOptions |= LSTRrxExtractOptions(OptionString);
         } LUA_CATCH();
      } else {
         luaL_error(L, "Fourth argument must be a number (maximum substitutions) or a string (modifiers).");
      }
   } else {
      COL_TRC("No max substitutions or modifiers specified - going to try to substitute all matches with defaults");
   }
   if (CountOfArgument == 5 && !lua_isnil(L, 5)) {
      if (!lua_isstring(L, 5)) {
         luaL_error(L, "Regular expression modifiers must be provided as a string.");
      }
      OptionString = LUAtoString(L, 5);
      // Give the ill-formed option error to the translator, since the user was the one who specified these options
      try {
         RegexOptions |= LSTRrxExtractOptions(OptionString);
      } LUA_CATCH();
   }

   COL_VAR3(Data, Regex, MaxSubs);
   // Figure out which kind of substitution argument we're dealing with
   int SubstitutionFunctionIndex = -1;
   COLvar Map = COLvar().reset();
   if (LUAextractString(L, 3, &Substitution)) {
      COL_TRC("Going to use a string for substitutions");
      COL_VAR(Substitution);
   } else if (lua_isfunction(L, 3)) {
      // This seems a bit redundant, but it's used below to check for which type of substitution to perform
      SubstitutionFunctionIndex = 3;
      COL_TRC("Going to use a function for substitutions");
   } else if (lua_istable(L, 3)) {
      try {
         Map.fromLua(L, 3);   // this can throw
      } LUA_CATCH();
      COL_TRC("Going to use a table for substitutions");
      COL_VAR(Map);
   } else {
      luaL_error(L, "Substitution argument must be a string, node tree, function, or table.");
   }

   // All the substitutions have the same general scheme: Go through each match, collect the unmatched portions, apply the
   // substitution on each of the matched portions, and append that as it goes
   COLstring Result;
   int MatchesMade = 0;
   try {
      REXfind Finder;
      // TODO : Allow the user to specify their own regular expression options
      Finder.init(Regex.c_str(), RegexOptions); // this can throw
      Finder.matchBegin(Data.c_str(), Data.size());

      while (Finder.matchNext()) {
         // Capture the unmatched portion leading up to this match
         const REXresult& Unmatched = Finder.unmatched();
         Result.append(Unmatched.pData, Unmatched.Size);

#ifdef COL_ENABLE_TRACING
         // Give a nice, thorough trace of each of the matches
         for (int i = 0; i <= Finder.numMatchGroups(); i++) {
            if (Finder.matchPresent(i)) {
               COL_TRC("Match group " << i << ": " << Finder.match(i));
            } else {
               COL_TRC("Match group " << i << " not present");
            }
         }
         COL_TRC("Unmatched: " << Finder.unmatched());
#endif

         COLstring CurrentSubstitution;   // Don't really need to use a temporary, can use &Result directly, but this is for tracing
         if (LimitSubs && MatchesMade >= MaxSubs) {
            // Take the global match, and throw it into the result as-is
            if (Finder.matchPresent(0)) {
               CurrentSubstitution = COLstring(Finder.match(0).pData, Finder.match(0).Size);
            }
            COL_TRC("Already made maximum number of substitutions - Skipping " << CurrentSubstitution);
         } else {
            // Here's where the discernment is made as to which substitution to use
            // Since the function index is only set once, branch predict will catch on rather quickly, so do this check first
            if (SubstitutionFunctionIndex >= 0) {
               LSTRrxsubReplaceMatchWithFunction(&CurrentSubstitution, Finder, L, SubstitutionFunctionIndex);
            } else if (Map.isNull()) {
               // Replace the match with the result, while expanding back references
               LSTRexpandCurrentBackReferences(&CurrentSubstitution, Finder, Substitution);
            } else {
               LSTRrxsubReplaceMatchWithTableValue(&CurrentSubstitution, Finder, Map);
            }
            MatchesMade++;
         }
         COL_VAR(CurrentSubstitution);
         Result.append(CurrentSubstitution);
      }
      // Finish with any unmatched portion at the end
      const REXresult& Unmatched = Finder.unmatched();
      Result.append(Unmatched.pData, Unmatched.Size);
   } LUA_CATCH();

   COL_VAR2(Result, MatchesMade);

   LUApushString(L, Result);
   lua_pushnumber(L, MatchesMade);

   return 2;
}

int LSTRstringCapitalize(lua_State* L){
   COL_FUNCTION(LSTRstringCapitalize);
   COLstring StringValue, ResultString;
   if (!LUAextractString(L, 1, &StringValue)){
      luaL_error(L, "Cannot extract string from this type.");
   }
   ResultString = StringValue.substr(0, 1).toUpperCase() + StringValue.substr(1).toLowerCase();
   COL_VAR(ResultString);
   try{
      LUApushString(L, ResultString);
      return 1;
   } LUA_CATCH();
}

int LSTRstringTrimWS(lua_State* L){
   COL_FUNCTION(LSTRstringTrimWS);
   COLstring StringValue;
   if (!LUAextractString(L, 1, &StringValue)){
      luaL_error(L, "Cannot extract string from this type.");
   }
   COL_VAR(StringValue);
   StringValue = COLstripWhiteSpace(StringValue);
   COL_VAR(StringValue);
   try{
      LUApushString(L, StringValue);
      return 1; // number of results
   } LUA_CATCH();
}

int LSTRstringTrimRWS(lua_State* L){
   COL_FUNCTION(LSTRstringTrimRWS);
   COLstring StringValue;
   if (!LUAextractString(L, 1, &StringValue)){
      luaL_error(L, "Cannot extract string from this type.");
   }
   COL_VAR(StringValue);
   StringValue = COLstripWhiteSpace(StringValue, false);
   COL_VAR(StringValue);
   try{
      LUApushString(L, StringValue);
      return 1; // number of results
   } LUA_CATCH();
}

int LSTRstringTrimLWS(lua_State* L){
   COL_FUNCTION(LSTRstringTrimLWS);
   COLstring StringValue;
   if (!LUAextractString(L, 1, &StringValue)){
      luaL_error(L, "Cannot extract string from this type.");
   }
   COL_VAR(StringValue);
   StringValue = COLstripWhiteSpace(StringValue, true, false);
   COL_VAR(StringValue);
   try{
      LUApushString(L, StringValue);
      return 1; // number of results
   } LUA_CATCH();
}

int LSTRstringCompactWS(lua_State* L){
   COL_FUNCTION(LSTRstringCompactWS);
   COLstring StringValue;
   if (!LUAextractString(L, 1, &StringValue)){
      luaL_error(L, "Cannot extract string from this type.");
   }
   COL_VAR(StringValue);
   int ReplacementCount = COLreplaceAllWhiteSpaceCount(StringValue, ' ');
   try{
      LUApushString(L, StringValue);
      lua_pushnumber(L, ReplacementCount);
      return 2; // number of results
   } LUA_CATCH();
}

void LSTRaddStringNamespace(lua_State* L){
   COL_FUNCTION(LSTRaddStringNamespace);
   lua_getglobal(L, "string");
   if (!lua_istable(L, -1)) {
      COL_TRC("Couldn't get the string table onto the stack");
      lua_pop(L, 1);
   } else {
      LUAaddMethod(L, "split",      &LSTRsplit);           // Returns an array of strings
      LUAaddMethod(L, "rxmatch",    &LSTRrxmatch);         // Note that this returns an iterator function, and not a string
      LUAaddMethod(L, "rxsub",      &LSTRrxsub);           // Returns the substituted string (with up to max)
      LUAaddMethod(L, "capitalize", &LSTRstringCapitalize);// Sets string to sentence case
      LUAaddMethod(L, "trimWS",     &LSTRstringTrimWS);    // Trims outer whitespace from string
      LUAaddMethod(L, "trimRWS",    &LSTRstringTrimRWS);   // Trims right whitespace from string
      LUAaddMethod(L, "trimLWS",    &LSTRstringTrimLWS);   // Trims left whitespace from string
      LUAaddMethod(L, "compactWS",  &LSTRstringCompactWS); // Removes whitespace from string
      lua_setglobal(L, "string");
   }
}
