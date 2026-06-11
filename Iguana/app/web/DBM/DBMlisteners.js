function DBMsetAllListeners() {
   const Modal = document.querySelector('.DBMmodal');
   if(!Modal) { return; }
   Modal.addEventListener('click', DBMlisteners);
   Modal.addEventListener('keydown', DBMkeyDownListeners);
}

function DBMlisteners(e){
   if(e.target.closest('.DBMfromDatabase div')){ DBMsetFromDatabase(); }
   if(e.target.closest('.DBMfromFile div')){ DBMsetFromFile(); }
   if(e.target.closest('.DBMtoFile div'))  { DBMsetToFile();   }
   if(e.target.closest('.DBMnodeValue')){
      let NodeName = e.target.closest(".DBMnodeValue").getAttribute('value');
      if(!DBMtoFileData["tables"]) return;
      if(DBMtoFileData["tables"][NodeName]) return;
      DBMimportList[NodeName] = true;
      MODELforcePoll('DBMmodel');
   }
   if(e.target.closest('.DBMimportCancel')) POPpopoutClose(DBMimportWindow);
   if(e.target.closest('.DBMimportButton')) DBMimportSave();
   if(e.target.closest('.DBMdeleteListItem')){
      let NodeName = e.target.closest('.DBMlistRow').querySelector(".DBMnodeValue").getAttribute('value').trim();
      delete DBMimportList[NodeName];
      MODELforcePoll('DBMmodel');
   }
}

function DBMkeyDownListeners(e){
   if(e.target.closest('.DBMfromFileSearch')){ MODELforcePoll('DBMmodel'); }
   if(e.target.closest('.DBMtoFileSearch'))  { MODELforcePoll('DBMmodel'); }
}

function DBMcleanPath(Path){ return Path.trim().endsWith(".vdb") ? Path.replace(TRANSprojectRoot(), "") : ""; }

function DBMsetFromFile(){
   FILbrowser({
      new_folder   : true,
      show_hidden  : true,
      dir_only     : false,
      value        : TRANSprojectRoot(),
      file_types   : ["vdb"],
      callback     : function(Path) { 
         DBMfromDatabase = "";
         DBMfromFile = DBMcleanPath(Path);
         MODELforcePoll('DBMmodel');
      }
   });
}

function DBMsetToFile(){
   FILbrowser({
      new_folder   : true,
      show_hidden  : true,
      dir_only     : false,
      file_types   : ["vdb"],
      value        : TRANSprojectRoot(),
      callback     : function(Path) { 
         DBMtoFile = DBMcleanPath(Path);
         MODELforcePoll('DBMmodel');
      }
   });
}

function DBMimportSave(){
   let NewData    = JSON.parse(JSON.stringify(DBMtoFileData));
   let ImportData = JSON.parse(JSON.stringify(DBMfromFileData));
   let ImportList = JSON.parse(JSON.stringify(DBMimportList));
   for(TableName in DBMimportList){ if(ImportList[TableName]) NewData["tables"][TableName] = ImportData["tables"][TableName]; }
   let ApiData = {};
   ApiData.path = DBMtoFile;
   ApiData.component   = TRANScomponentGuid();
   ApiData.content     = JSON.stringify(NewData);
   ApiData.development = TRANSdevelopment();
   APIcall('component/db/set', ApiData, function(R){ 
      if(!R.success) SNCKsnackbar("error", "Error importing:" + R.error);
      TRANSfileCurrentReload();
      MODELremove('DBMmodel');}
   );
}