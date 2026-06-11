// Eliot I refactored this code from backend C++

function SMPLfileExtension(FileName) {
   let i = FileName.lastIndexOf(".");
   if (i == -1) { return "";}
   return FileName.substring(i+1);
}

let SMPLfileMap={
   "lua" : "lua",
   "dbs" : "dbs",
   "vmd" :  "vmd",   
   "js"  : "javascript",
   "json": "javascript",  
   "css" : "css",
   "html": "htmlmixed",
   "htm" : "htmlmixed",
   "xml" : "xml",
   "txt" : "txt",
   "sql" : "plsql"
};
// TODO - move this to TRANS
function SMPLfileType(FileName){
   let Ext = SMPLfileExtension(FileName);
   COL_TRC("Ext =", Ext, "FileName =", FileName);
   if (!(Ext in SMPLfileMap)){
      COL_TRC("Could not find the file name extension so return other");
      return "plain";
   }
   let Type = SMPLfileMap[Ext];
   COL_TRC("Type =", Type);
   return Type;
}

function SMPLisSampleData(FileName){
   COL_TRC(FileName);
   return FileName.indexOf("Samples") != -1;
}

// Map of file extension to bool (don't care about the value)
// Making this a map instead of an array makes checking the extension much easier 
// since it avoids an iteration of the exclusion array per file enumerated.
function SMPLexcludeFiles(){
   return {"meta" : true};
}