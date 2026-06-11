let SMPLfileListArray = [];

function SMPLfileNameUnqiue(FileName) {
   for ( file of SMPLfileListArray) {
      if (file === FileName) { return false; }
   }
   return true;
}

function SMPLimportName(Value) {
   let Name = document.querySelector('.SMPLimportName');
   if(!Name) return "";
   if (Value)  Name.value = Value;
   else return Name.value.trim();
}

function SMPLbadFileName() { 
   return !SMPLfileNameUnqiue(SMPLimportName()) || document.querySelector('.SMPLimportName').value.trim() === ""; 
}

function SMPLpopulateFileName(FileName) { document.querySelector('.SMPLimportName').value = FileName; }

function SMPLzeroPad(num, length) {
   var str = num.toString();
   while (str.length < length) {
     str = "0" + str;
   }
   return str;
}

function SMPLgenerateUniqueName() {
   let FileNameBase = 'Sample_';
   let FileNumber = 1;
   while (!SMPLfileNameUnqiue(FileNameBase + SMPLzeroPad(FileNumber, 3)) && FileNumber++ <= 100) {}
   return FileNameBase + SMPLzeroPad(FileNumber, 3);
}

function SMPLinitializeFilename() {
   SMPLpopulateFileName(SMPLgenerateUniqueName());
}

function SMPLpopulateFileList(Response) {
   SMPLfileListArray = [];
   for (file of Response) { SMPLfileListArray.push(file.name); }
}

function SMPLgetSampleFileList(Callback) {
   let Params = {};
   Params.component   = TRANScomponentGuid();
   Params.path        = SMPLrootFolder();
   Params.development = TRANSdevelopment();
   APIcall('component/dir', Params, function(R){
      if (SMPLhandleError(R)) return;
      SMPLfileListArray = [];
      if (R.data.exists) SMPLpopulateFileList(R.data.content);
      if(Callback) Callback();
   });
}

function SMPLdialogRenderName() {
   const errorElement = document.querySelector('.SMPLimportNameError');
   if (SMPLbadFileName()) errorElement.classList.remove('SMPLhidden');
   else                   errorElement.classList.add('SMPLhidden');
}

function SMPLdialogSetUpName() {
   SMPLgetSampleFileList(SMPLinitializeFilename);
   const nameInput = document.querySelector(".SMPLimportName");
   nameInput.addEventListener('change', function(){ SMPLdialogRenderName(); SMPLdialogRenderButtons(); });
   nameInput.addEventListener('keyup',  function(){ SMPLdialogRenderName(); SMPLdialogRenderButtons(); });
}
