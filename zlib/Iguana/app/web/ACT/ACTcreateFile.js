function ACTfolderWrapperHtml(path, innerHtml){
   return  path ? `
   <div class="ACTfolderNameDiv">
      <div class="ACTfolderNamePrefix">${path + FILsep()}</div>
      ${innerHtml}
   </div>` : innerHtml;
}

function ACTcreateFileHtml(path){
   let filenameInHtml = ACTfolderWrapperHtml(path, `<input class='FORMinput ACTfileName'/>`);
   let filenameUploadHtml = ACTfolderWrapperHtml(path, `<input class='FORMinput ACTuploadedName' placeholder='No file selected' />`);
   return /*html*/`
<div class="ACTtab">
   <div class='ACTtabLink ACTcreateTab ACTselectedTab'> New File </div>
   <div class='ACTtabLink ACTuploadTab'>                Upload File     </div>
</div>
<div class="ACTnewFileContent">
   <div class='FORMlabel'>File Name:</div>
   <br>
   <div class='FORMinputGroup'>
      ${filenameInHtml}
      <select class="FORMselectBox ACTfileExt">
         <option value=".lua">Lua file (*.lua)</option>
         <option value=".help">Help file (*.help)</option>
         <option value=".vmd">HL7/X12 schema file (*.vmd)</option>
         <option value=".vdb">Database schema file (*.vdb)</option>
         <option value="">Custom (blank)</option>
      </select>
   </div>
<br>
   <label class="ACTaddRequire">
      <input type="checkbox" class="ACTaddRequireCheck" checked/>
      Add require statement to main.lua
   </label>
   <br/>
   <br/>

   <div class="ACTfileInfo"></div>
   <br>
   <p class='ACTcreateFileFeedback hint' ></p>
   <br>
   <div class='BUTTONgroup ACTbuttons'>
      <div class='ACTnewFileCancel BUTTONstandard BUTTONcancel'>CANCEL</div>
      <div class='ACTnewFileButton BUTTONstandard BUTTONaction'>CREATE</div>
   </div>
</div>
<div class="ACTuploadContent ACThide">
   <div class='FORMlabel'>File Name:</div>
   <br>
   <div class='FORMinputGroup'>
      ${filenameUploadHtml}
      <label for="ACTchooseFile" class="BUTTONstandard BUTTONcancel ACTuploadFileButton">
         <input type="file" id="ACTchooseFile" class="ACTuploadFile">BROWSE...
      </label>
   </div>
   <p class='ACTcreateFileFeedback hint' ></p>
   <br>
   <br>
   <div class='BUTTONgroup ACTbuttons'>
      <div class="ACTuploadFileFeedback ACThide" >Processing selected file...</div>
      <div class='ACTuploadCancel BUTTONstandard BUTTONcancel'>CANCEL</div>
      <div class='ACTuploadButton BUTTONstandard BUTTONdisabled'>UPLOAD</div>
   </div>
</div>
`;
}

function ACTcreateFileWindow(CurrentTreeNode){
   let Popout = POPpopoutOpen({
      title:'New File',
      content: ACTcreateFileHtml(CurrentTreeNode.path),
      width: '700px',
      esc_close : true,
      callback : TRANSsetCode
   });
   UTLdisableButtonEl(document.querySelector('.ACTnewFileButton'));
   document.querySelector('.ACTfileName')     .addEventListener("keyup", function(e){ ACTcheckFileName(e, CurrentTreeNode.path, Popout); });
   document.querySelector('.ACTfileName')     .addEventListener("input", function(e){ ACTcheckFileName(e, CurrentTreeNode.path, Popout); });
   document.querySelector('.ACTnewFileButton').addEventListener("click", function() { ACTcreateFile(CurrentTreeNode.path, Popout); });
   document.querySelector('.ACTnewFileCancel').addEventListener("click", function() { POPpopoutClose(Popout); });
   document.querySelector('.ACTuploadCancel') .addEventListener("click", function() { POPpopoutClose(Popout); });
   document.querySelector('.ACTcreateTab')    .addEventListener("click", function() { ACTselectTab('Create'); });
   document.querySelector('.ACTuploadTab')    .addEventListener("click", function() { ACTselectTab('Upload'); });
   document.querySelector('.ACTuploadButton') .addEventListener("click", function() { ACTupload(CurrentTreeNode.path, Popout); });
   document.querySelector('.ACTfileExt')      .addEventListener("change", function() { document.querySelector('.ACTfileName').focus(); });
   ACTsetupFileUpload(Popout)
   document.querySelector('.ACTfileName').focus();
   FORMfocus(document.querySelector('.ACTfileName'));
   FORMenterListener(document.querySelector('.ACTfileName'), function() { ACTcreateFile(CurrentTreeNode.path, Popout); });
   FORMenterListener(document.querySelector('.ACTuploadedName'), function() { ACTupload(CurrentTreeNode.path, Popout); });
   MENUremoveTooltip();
   ACTcreateFileInfo();
}

function ACTselectTab(TabId){
   let CreateTab     = document.querySelector('.ACTcreateTab')
   let UploadTab     = document.querySelector('.ACTuploadTab')
   let CreateContent = document.querySelector('.ACTnewFileContent')
   let UploadContent = document.querySelector('.ACTuploadContent')
   if(TabId === 'Create'){
      CreateContent.classList.remove('ACThide');
      UploadContent.classList.add('ACThide');
      CreateTab.classList.add('ACTselectedTab')
      UploadTab.classList.remove('ACTselectedTab')
   } else if(TabId === 'Upload'){
      CreateContent.classList.add('ACThide');
      UploadContent.classList.remove('ACThide');
      UploadTab.classList.add('ACTselectedTab')
      CreateTab.classList.remove('ACTselectedTab')
   }
   FORMfocus(document.querySelector('.ACTfileName'));
   FORMfocus(document.querySelector('.ACTuploadedName'));
}

function ACTcheckFileName(e, Path, Popout){
   let el = document.querySelector('.ACTfileName');
   if(el.value.trim() === "") UTLdisableButtonEl(document.querySelector('.ACTnewFileButton'));
   if(el.value.trim() !== "") UTLenableButtonEl (document.querySelector('.ACTnewFileButton'));
   if( e.key === "Enter" && document.querySelector('.ACTnewFileButton.BUTTONaction')) ACTcreateFile(Path, Popout);
}

function ACTvalidBlankFileName(File) {
   let FileLength = File.length;
   if (File.substr(FileLength-4, FileLength) == ".lua" && File.split(".").length > 2) return false;
   return true;
}

function ACTopenFile(Path, Extension){
   if      (Extension === ".vmd") EDIopenListWindow(Path);
   else if (Extension === ".vdb") TBLopenListWindow(Path);
   else if (Extension === ".help") {
      HLPlistAll(Path.substring(0, Path.length - 5));
      HLPopenListWindow(Path);
   }
   else return TRANSfileChange(Path);
   MODELforcePoll('TRANSmodel');
}

function ACTcreateFile(NewFileLocation, Popout){
   if(document.querySelector('.ACTnewFileButton.BUTTONdisabled')) return;
   let FileName = document.querySelector('.ACTfileName').value.trim();
   let FileExtension = document.querySelector('.ACTfileExt').value;
   if (FileName[0] == "." && FileName !== ".gitignore") return document.querySelector('.ACTcreateFileFeedback').innerHTML = "Error - Cannot create a hidden file! (A hidden file is a file starting with a '.')";
   if (FileName.includes(".") && FileExtension == ".lua" || !ACTvalidBlankFileName(FileName) ) return document.querySelector('.ACTcreateFileFeedback').innerHTML = "Error - Filename with lua extension cannot contain '.'";
   let Path = (NewFileLocation ? NewFileLocation + FILsep() : "") + FileName + FileExtension;
   let Content = "";
   if(FileExtension == ".help") Content = HLPblankFile(Path);
   if(FileExtension == ".vmd")  Content = EDIblankVmd();
   if(FileExtension == ".vdb")  Content = TBLblankFile();
   let data = { path: Path, component: TRANScomponentGuid(), content: Content , development : TRANSdevelopment() };
   APIcall('component/file/add', data, function(Response){
      if (!Response.success ){
         document.querySelector('.ACTcreateFileFeedback').innerHTML = "Error - " + Response.error;
         return;
      }
      if(ACTisRequireChecked()) { ACTaddRequireToMain(data.path); }
      POPpopoutClose(Popout);
      TRANSopenDir = Path;
      ACTopenFile(Path, FileExtension);
      TRANSideRequestCompile( function(){ TRANSupdateAnnotations(); } );
      MODELforcePoll("TRANSstatusModel");
   });
}

function ACTaddRequireToMain(path) {
   APIcall('component/add/require', {path: path, component: TRANScomponentGuid(), development : TRANSdevelopment()}, function(R) {
      if(!R.success) { return SNCKsnackbar('error', R.error); }
   });
}

function ACTcreateFileInfo() {
   const infoDiv       = document.querySelector('.ACTfileInfo');
   const fileSelection = document.querySelector('.ACTfileExt');
   const addRequire    = document.querySelector('.ACTaddRequire');
   const fileInfo      = {
      ".lua" : `The translator runs using the Lua scripting language.  This is most common file type in most projects.
      [<a target="_blank" href="${LINKtransCreateFileLua}">Learn&nbsp;more</a>]`,
      ".help": `Be a team player.  Document your code. Help files also make it simple to support auto suggestion of arguments.
      [<a target="_blank" href="${LINKtransCreateFileHelp}">Learn&nbsp;more</a>]`,
      ".vmd" : `Make HL7/X12 easy.  VMD files make HL7 and X12 easy to consume and produce.
      [<a target="_blank" href="${LINKtransCreateFileVmd}">Learn&nbsp;more</a>]`,
      ".vdb" : `VDB files make it simple to work with inserting and updating data in databases.  Also you can create databases tables with these.
      [<a target="_blank" href="${LINKtransCreateFileVdb}">Learn&nbsp;more</a>]`,
      ""     : `Custom file with no extension.  You can easily add custom files like PDFs and others to your projects.
      [<a target="_blank" href="${LINKtransCreateFileCustom}">Learn&nbsp;more</a>]`
   };
   infoDiv.innerHTML   = fileInfo[".lua"]; // default
   fileSelection.addEventListener('change', function() {
      infoDiv.innerHTML = fileInfo[this.value];
      addRequire.style.display = this.value === '.lua' ? 'inline-block' : 'none';
   });
}

function ACTisRequireChecked() {
   let checkbox = document.querySelector('.ACTaddRequireCheck');
   let div      = document.querySelector('.ACTaddRequire');
   return checkbox?.checked && div?.style.display !== 'none';
}
