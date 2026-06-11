function SMPLhide(Selector) {
   document.querySelector(Selector).classList.add("SMPLhidden");
}

function SMPLshow(Selector) {
   document.querySelector(Selector).classList.remove("SMPLhidden");
}

function SMPLloadPreview(Editor, Data, Name) {
   if (Name) {
      const SampleName = document.querySelector('.SMPLimportName');
      SampleName.value = Name;
   }
   Editor.setValue(Data);
   SMPLdialogRenderButtons(Editor);
}

function SMPLdialogSetUpUpload() {
   COL_TRC("SMPLdialogSetUpUpload");
   const FileInput  = document.querySelector('.SMPLfileUploadSelect');
   FileInput.addEventListener("change", function() {
      const SelectedFile = FileInput.files[0];
      if (SelectedFile) {
         const reader = new FileReader();
         reader.addEventListener('load', function() {
            SMPLloadPreview(SMPLeditor, reader.result, SelectedFile.name);
            SMPLhide(".SMPLfileUploadSelected");
         });
         // visual clue that js is working hard to SMPLloadPreview()
         SMPLshow(".SMPLfileUploadSelected");
         reader.readAsBinaryString(SelectedFile);
      }
   });
}