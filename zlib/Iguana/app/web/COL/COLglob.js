function COLcurrentChar(Obj) { return Obj.value.charAt(Obj.index); }
function COLendOfValue(Obj)  { return Obj.index >= Obj.value.length || COLisNull(Obj); }
function COLisWildcard(Obj)  { return COLcurrentChar(Obj) === '*'; }
function COLisNull(Obj)      { return COLcurrentChar(Obj) === ''; }
function COLisChar(Obj)      { return !COLisNull(Obj) && !COLisspace(COLcurrentChar(Obj)); }
function COLisspace(char)    { return char === ' ' || char === '\t' || char === '\n' || char === '\r'; }

function COLsubPatternMatches(PatternObj, ModuleObj, Length) {
   for(; Length > 0 && !COLisNull(PatternObj) && !COLisNull(ModuleObj); ++PatternObj.index, ++ModuleObj.index, --Length) {
      const CurrentChar = COLcurrentChar(PatternObj);
      if(CurrentChar !== COLcurrentChar(ModuleObj) && CurrentChar !== '?') {return false; }
   }
   return Length === 0;
}

function COLendOfModule(PatternObj, ModuleObj, SkipRestOfGlobObj) {
   if(!COLisNull(ModuleObj)) { return false; }
   while(COLisWildcard(PatternObj)) { ++PatternObj.index; }
   if(COLisChar(PatternObj)) { SkipRestOfGlobObj.value = true; }
   return true;
}

function COLsubpatternNotFound(PatternObj, TargetFound, SkipRestOfGlobObj) {
   if(!TargetFound) {
      SkipRestOfGlobObj.value = true;
      return true;
   }else if(!COLisChar(PatternObj)) { return true; }
   return false;
}

function COLtargetFound(TargetObj, ModuleObj, TargetLength, IsTargetAtEnd) {
   while(!COLendOfValue(ModuleObj)) {
      const IsEnd = !IsTargetAtEnd || ModuleObj.value.charAt(ModuleObj.index + TargetLength) === '';
      // these copies are necessary because we do not want to modify the index's if SubpatternMatch returns false
      let TargetCopy = {value: TargetObj.value, index: TargetObj.index};
      let ModuleCopy = {value: ModuleObj.value, index: ModuleObj.index};
      const SubpatternMatch = COLsubPatternMatches(TargetCopy, ModuleCopy, TargetLength);
      if(SubpatternMatch && IsEnd) {
         ModuleObj.index += TargetLength;
         return true;
      }
      ++ModuleObj.index;
   }
   return false;
}

function COLhasWildcardMatch(PatternObj, ModuleObj) {
   while(COLisWildcard(PatternObj)) { ++PatternObj.index; }
   if(COLisChar(PatternObj)) { return false; }
   ModuleObj.value = "";
   ModuleObj.index = 0;
   return true;
}

function COLmatchWildCard(PatternObj, ModuleObj, SkipRestOfGlobObj) {
   if(COLhasWildcardMatch(PatternObj, ModuleObj)) { return true; }
   let TargetObj    = {value: PatternObj.value.substring(PatternObj.index), index: 0};
   let TargetLength = 0;
   let Ch = TargetObj.value.charAt(TargetLength);
   while(Ch && !COLisspace(Ch) && Ch !== '*') {
      TargetLength++;
      Ch = TargetObj.value.charAt(TargetLength);
   }
   PatternObj.index += TargetLength;
   if(TargetLength !== 0) {
      let CharObj = {value: Ch, index: 0};
      let IsTargetAtEnd = !COLisChar(CharObj);
      let TargetFound   = COLtargetFound(TargetObj, ModuleObj, TargetLength, IsTargetAtEnd);
      return COLsubpatternNotFound(PatternObj, TargetFound, SkipRestOfGlobObj);
   }
   return false;
}

function COLisPositiveMatch(Obj) {
   if(COLcurrentChar(Obj) !== '-') { return true; }
   ++Obj.index;
   return false;
}

function COLglobMatch(Pattern, ModuleToMatch) {
   let PatternObj = {value: Pattern, index: 0};
   let MatchCandidate = false;
   // the glob list loop
   while(!COLendOfValue(PatternObj)) {
      while(COLisspace(COLcurrentChar(PatternObj))) { ++PatternObj.index; }
      if(COLendOfValue(PatternObj)) { break; }

      let ModuleObj           = {value: ModuleToMatch, index: 0};
      let IsPositiveMatchMode = COLisPositiveMatch(PatternObj);
      let SkipRestOfGlobObj   = {value: false};
      while(COLisChar(PatternObj)) {
         if(COLisWildcard(PatternObj)) {
            if(COLmatchWildCard(PatternObj, ModuleObj, SkipRestOfGlobObj)) { break; }
            continue;
         }
         if(COLcurrentChar(PatternObj) === COLcurrentChar(ModuleObj) || COLcurrentChar(PatternObj) === '?') { // glob character matches, so advance pointers
            ++PatternObj.index;
            ++ModuleObj.index;
            if(COLendOfModule(PatternObj, ModuleObj, SkipRestOfGlobObj)) { break; }
         } else {
            // glob does not match or is longer than the module - no match.
            SkipRestOfGlobObj.value = true;
            break;
         }
      }
      if(SkipRestOfGlobObj.value === true) {
         while(COLisChar(PatternObj)) { ++PatternObj.index; }
      }
      else if(COLendOfValue(ModuleObj)) {
         MatchCandidate = IsPositiveMatchMode; // reached end of glob pattern and end of Module - successful match
      }
   }
   return MatchCandidate;
}