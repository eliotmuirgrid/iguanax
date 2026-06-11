function EXPlisteners(){
   let div = document.querySelector(".EXPmodal");
   if(!div) return;
   div.addEventListener("click", EXPclick);
   div.addEventListener("change", EXPchange);
}

function EXPclick(e){
   if(e.target.closest(".EXPfileDiv     .BUTTONstandard")) return EXPfileButtonClicked();
   if(e.target.closest(".EXPgenerateRow .BUTTONstandard")) return EXPgenerateSqlClick();
   if(e.target.closest(".EXPcancel"))                      return POPpopoutClose(EXPcard);
   if(e.target.closest(".EXPexport"))                      return EXPexport();
   if(e.target.closest(".EXPlocationRow .BUTTONstandard")) return EXPdatabaseLocation();

   if(e.target.closest(".EXPimportWindow .BUTTONdefault")) return POPpopoutClose(EXPimportCard);
   if(e.target.closest(".EXPimportWindow .BUTTONaction"))  return EXPimportQuery();
}

function EXPchange(e){
   if(e.target.closest(".EXPdatabaseType")) return EXPdatabaseSelected();
}

function EXPargs(){
   let Args = {};
   Args.filepath    = EXPfile;
   Args.datasource  = EXPget(".EXPdatasourceIn") ? EXPget(".EXPdatasourceIn") : EXPlocation; //  EXPdatasource ? EXPdatasource : EXPlocation;
   Args.database    = EXPget(".EXPdatabaseIn");
   Args.username    = EXPget(".EXPusernameIn");
   Args.password    = EXPget(".EXPpasswordIn");
   Args.type        = EXPdatabaseTypeSelected;
   Args.component   = TRANScomponentGuid();
   Args.development = TRANSdevelopment();
   return Args;
}

function EXPupdateGenerateFeedback(message){
   let div = document.querySelector(".EXPgenerateFeedback");
   if(!div) return;
   if(div.innerHTML !== message) div.innerHTML = message;
}

function EXPgenerateSqlClick(){
   EXPhideRegenerateSqlMessage();
   EXPstore();
   EXPgenerateSql();
}

function EXPgenerateSql(){
   let Args = EXPargs();
   EXPupdateGenerateFeedback("");
   if(!EXPdatabaseTypeSelected) return;
   APIcall("database/generate_sql", Args, function(R){
      if(!R?.success) return EXPupdateGenerateFeedback(R.error || "Error generating SQL");
      EXPpopulateCode(R?.data?.sql);
   });
}

function EXPdefaultLocation(){
   return EXPlocation ? EXPlocation : TRANSprojectRoot();
}

function EXPupdateFileLocation(Path){
   let div = document.querySelector(".EXPlocationIn");
   if(!div) return;
   div.value = Path;
}

function EXPpopulateFile() {
   setTimeout(function() {
      let div = document.querySelector(".EXPlocationIn");
      FILentry(div.value);
   }, 200);
}

function EXPdatabaseLocation(){
   FILbrowser({
      new_folder   : true,
      show_hidden  : true,
      dir_only     : false,
      value        : EXPdefaultLocation(),
      callback     : function(Path) {
         EXPlocation = Path;
         EXPupdateFileLocation(Path);
         EXPstore();
      }
   });
   EXPpopulateFile();
}

function EXPfileButtonClicked(){
   FILbrowser({
      new_folder   : true,
      show_hidden  : true,
      dir_only     : false,
      value        : TRANSprojectRoot(),
      file_types   : ["vdb"],
      callback     : function(Path) {
         EXPfile = EXPcleanPath(Path);
         MODELforcePoll('EXPmodel');
      }
   });
}

function EXPcleanPath(Path){ return Path.trim().endsWith(".vdb") ? Path.replace(TRANSprojectRoot(), "") : ""; }

function EXPdatabaseSelected(){
   EXPhideRegenerateSqlMessage();
   EXPstore();
   MODELforcePoll('EXPmodel');
   EXPgenerateSqlClick();
}

function EXPdatabaseArgs(){
   let Args = EXPargs();
   Args.username = EXPget(".EXPusernameIn");
   Args.password = EXPget(".EXPpasswordIn");
   return Args;
}

function EXPexportArgs(){
   let Args = EXPdatabaseArgs();
   Args.sql = EXPeditor.getValue();
   return Args;
}

function EXPexportFeedback(Message){
   let div = document.querySelector(".EXPexportFeedback");
   if(!div) return;
   if(div.innerHTML !== Message) div.innerHTML = "Could not export: <br><br>" + ESChtmlEscape(Message) + "<br>";
}

function EXPexport(){
   EXPstore();
   let Args = EXPexportArgs();
   APIcall("database/export", Args, function(R){
      if(!R?.success) return EXPexportFeedback(R?.error || "Error with exporting sql.");
      POPpopoutClose(EXPcard);
      SNCKsnackbar("success", "Export complete.", 20000);
      TRANSfileCurrentReload();
   });
}
