function TBLupdateErrorOccured(){
   let FileData = TBLfileData();
   let ErrorFiles = [];
   for(FileName in FileData){
      let Error = FileData[FileName]["error"];
      if(Error) {
         ErrorFiles.push(FileName + " (" + Error + ")");
      }
   }
   if(ErrorFiles.length === 0) return;
   let Message = `The following files could not be loaded: ${ErrorFiles.join(", ")}`;
   SNCKsnackbar("error", Message);
}
