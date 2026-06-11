let ACTuploadData = {Name: '', Content: ''};

// Eliot - this is somewhat of a slow method - escaping the binary file and unescaping on the back end
// There are other ways you can use the
// HTTP procotol to avoid encoding of the file.  You can do fancy things like get callbacks
// from the browser about upload progress so you can do a progress bar etc. etc. etc.
// But this works and is adequate for our purposes.  But for future generations reading this code...
function ACTsetupFileUpload() {
   const FileInput  = document.querySelector('.ACTuploadFile');
   const Filename     = document.querySelector('.ACTuploadedName');
   const UploadButton = document.querySelector('.ACTuploadButton');
   FileInput.addEventListener("change", function() {
      const SelectedFile = FileInput.files[0];
      if (SelectedFile) {
         const Reader = new FileReader();
         Reader.addEventListener('load', function() {
            ACTuploadData.Name    = SelectedFile.name;
            ACTuploadData.Content = Reader.result;
            Filename.value = ACTuploadData.Name;
            UploadButton.classList.remove('BUTTONdisabled');
            UploadButton.classList.add('BUTTONaction');
            // TODO: Include Size restriction ??
            document.querySelector('.ACTuploadFileFeedback').classList.add('ACThide');
         });
         document.querySelector('.ACTuploadFileFeedback').classList.remove('ACThide');
         Reader.readAsBinaryString(SelectedFile);
         FORMfocus(Filename);
      }
   });
}

function ACTupload(NewFileLocation, Popout){
   let FileName = ACTuploadData.Name;
   if (FileName[0] == ".") return SNCKsnackbar('error', "Error - Cannot create a hidden file! (A hidden file is a file starting with a '.')");
   if (FileName.endsWith(".lua") && FileName.split(".").length > 2) return SNCKsnackbar('error', "Filename with lua extension cannot contain '.'");
   let FilePath = (NewFileLocation ? NewFileLocation + FILsep() : "") + FileName;
   const Data = { component: TRANScomponentGuid(), path: FilePath, content: ACTuploadData.Content,  development: TRANSdevelopment() };
   APIcall("component/file/upload", Data, function(Response) {
      if(!Response.success) return SNCKsnackbar('error', Response.error);
      SNCKsnackbar('success', `${Data.path} was uploaded.`, 12000);
      MODELforcePoll('TRANSmodel');
      POPpopoutClose(Popout);
      TRANSideRequestCompile( function(){ TRANSupdateAnnotations(); } );
   });
}