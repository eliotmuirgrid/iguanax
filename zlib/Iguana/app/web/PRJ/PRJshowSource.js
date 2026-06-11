function PRJisStructuredFile(Path){
   return Path.endsWith(".vmd") || 
   Path.endsWith(".vdb") || 
   Path.endsWith(".help")|| 
   Path === "config.json";
}

function PRJshowSource(Filepath){
   TRANSfileChange(Filepath)
}

function PRJisImportantFile(Path){
   return Path === "main.lua" || Path === ".gitmodules" ||  Path === "config.json";
}