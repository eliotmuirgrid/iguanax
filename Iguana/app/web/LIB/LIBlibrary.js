function LIBremoveBeginning(str, subStr){
   if (str.startsWith(subStr)) {
      return str.slice(subStr.length);
   } else {
      return str;
   }
}

function LIBisInLibrary(FilePath) {
   let LibraryList = TRGTlibraryList();
   for(const library of Object.keys(LibraryList)) {
      let LibraryPath = TRANSsourceDirectory + library;
      if(FilePath === LibraryPath) return library;
      let str1 = FilePath    + FILsep();
      let str2 = LibraryPath + FILsep();
      if(str1.includes(str2)) { return library; }
   }
   return false;
}

function LIBisLibraryRoot(FilePath) {
   let RelativePath = LIBremoveBeginning(FilePath, TRANSsourceDirectory);
   let LibraryList = TRGTlibraryList();
   return LibraryList[RelativePath] !== undefined;
}

function LIBlibraryUpstreamSet(FilePath){
   let LibMap = TRGTlibraryList();
   if(!LibMap) {return false; }
   return LibMap.hasOwnProperty(FilePath) && LibMap[FilePath].url !== "";
}

function LIBlibraryUnlocked(FilePath){;
   let LibMap = TRGTlibraryList();
   if(!LibMap) return false; 
   let Locked = LibMap[FilePath]?.locked;
   return !Locked;
}

function LIBlibraryGitData(FilePath){
   let LibMap = TRGTlibraryList();
   if(!LibMap) return {};
   let RelativePath = LIBremoveBeginning(FilePath, TRANSsourceDirectory);
   return LibMap[RelativePath]?.git || {};
}