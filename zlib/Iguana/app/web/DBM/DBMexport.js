let DBMexportDatabaseTableEditor = null;
let DBMexportFilename = "";
let DBMexportDatasource = "";
let DBMexportPopout = "";

function DBMexportClearGlobals(){   
   DBMexportDatabaseTableEditor = null;
   DBMexportFilename = "";
   DBMexportDatasource = "";
   DBMexportPopout = "";
}
function DBIMexportDisableEditor() {
   DBMexportDatabaseTableEditor.setOption("readOnly", "nocursor");
   DBMexportDatabaseTableEditor.display.wrapper.style.opacity = 0.5;
   DBMexportDatabaseTableEditor.display.wrapper.style.background = "var(--grey-400)";
   DBMexportDatabaseTableEditor.display.wrapper.style.filter = "grayscale(90%)";
}

function DBIMexportEnableEditor() {
   DBMexportDatabaseTableEditor.setOption("readOnly", false);
   DBMexportDatabaseTableEditor.display.wrapper.style.opacity = 1;
   DBMexportDatabaseTableEditor.display.wrapper.style.background = "white";
   DBMexportDatabaseTableEditor.display.wrapper.style.filter = "grayscale(0%)";
}

function DBMrenderDatasourceExists(Result){
   const DataSource = document.querySelector('.DBMexportDataSource');
   const Target = document.querySelector('.DBMdatasourceExistsInfo');
   if (!Target) { return; }
   if (DataSource.value.trim() == ""){
      DBIMexportDisableEditor();
      Target.innerHTML="";
      return;
   }
   DBIMexportEnableEditor();
   if (!Result.data.dbExists) { Target.innerHTML = "This database does not exist. It will be created"; }
   else                       { Target.innerHTML = "This database already exists. Its tables will be modified."; }
}
function DBMexporterRender(){
   let Content = "";
   APIcall("database/list", {}, function(R){
      if(R["success"]){
         Content = DBMexportHtml(R["data"], DBMexportDatasource);
         DBMexportPopout = POPpopoutOpen({title:'SQL', content: Content, width: '85%', height: '85%', className: 'DBMexporter', callback: function(){
            DBMexportClearGlobals();
         }});
         DBMexporterListeners();
         DBMexporterUpdateSql(DBMexportSetUpCodeMirror);
      } else {
         // should never reach this outside of testing as we always support sqlite
         SNCKsnackbar("error", "Unable to determine which databases are supported."); 
      }
   });
}

function DBMexporterListeners(){
   let DatabaseInput = document.querySelector('.DBMexportContainer');
   if(!DatabaseInput) return;
   DatabaseInput.addEventListener("click",  DBMexporterDatabaseClick);
   DatabaseInput.addEventListener("change", DBMexporterDatabaseChange);
   const Datasource =  document.querySelector('.DBMexportDataSource');
   if (!Datasource) return;
   Datasource.addEventListener("input", DBMexporterDatasourceChange);
}

function DBMexporterDatasourceChange(e){
   DBMexportDatasource = e.target.value;
   DBMclearFeedback();
   DBMexporterUpdateSql(DBMexportSetUpCodeMirror);
}

function DBMexporterDatabaseClick(e){
   if(e.target.closest('.DBMexportExecute'))  DBMexporterExecute();
}

function DBMexporterDatabaseChange(e){
   if(e.target.closest('.DBIMexportApi')) {
      DBMclearFeedback();
      DBMexporterUpdateSql(DBMexportSetUpCodeMirror);
   }
}

function DBMexporterUpdateSql(Callback){
   let Type = document.querySelector('.DBIMexportApi') ? document.querySelector('.DBIMexportApi').value : "";
   if(!DBMexportFilename.length && !document.querySelector('.DBMexportInputContainer')) { 
      let Result = { success : false, error : "Could not find this vdb file on disk." }
      return Callback(Result); 
   }
   let ParamsArea = document.querySelector('.DBMexportInputContainer');
   ParamsArea.innerHTML = DBMexporterDbParamsHtml(Type);
   let Data = { 
      "filepath"    : DBMexportFilename,  // This is the vdb file
      "datasource"  : DBMexportDatasource,
      "component"   : TRANScomponentGuid(), 
      "development" : TRANSdevelopment(), 
      "type"         : Type,
      "location"    : TRANSprojectRoot() 
   }
   APIcall("database/generate_sql", Data, function(Response){
      Callback(Response);
   });
}

function DBMexportSetUpCodeMirror(Result){
   if(Result.success) {
      if (document.querySelector('.DBMexportSqlContainer .CodeMirror.cm-s-default')){
         DBMexportDatabaseTableEditor.setValue(Result.data.sql);
      } else { // editor doesn't exist, create
         let EditorConfig = {
            mode           : "text/x-sql",
            theme          : 'default',
            lineNumbers    : true,
            autofocus      : true,
            tabSize        : 3,
            value          : Result.data.sql,
            autofocus      : false
         };
         DBMexportDatabaseTableEditor = new CodeMirror(document.querySelector('.DBMexportSqlContainer'), EditorConfig);
      }
      DBMrenderDatasourceExists(Result);
   } else { // This is if we fail to list any databases - ie nothing is configured 
      let ErrDiv = document.createElement("div");
      let ErrText = document.createTextNode(Result.error);
      ErrDiv.appendChild(ErrText);
      document.querySelector('.DBMexportSqlContainer').appendChild(ErrDiv);
   }
}

function DBMshowFeedback(Type, Message){
   let SqlBox = document.querySelector('.DBMexportSqlContainer');
   if (SqlBox) {
      let Feedback = document.createElement("div");
      Feedback.classList.add('DBMactionFeedback');
      if (Type == "error") {
         Feedback.innerHTML = Message;
         Feedback.classList.add('DBMerrorFeedback');
      } else {
         Feedback.innerHTML = Message;
         Feedback.classList.add('DBMsuccessFeedback');
      }
      SqlBox.after(Feedback);
   }
}

function DBMclearFeedback(){
   let Feedback = document.querySelector('.DBMactionFeedback');
   if (Feedback) Feedback.remove();
}

function DBMexporterExecute(){
   let DatabaseParams = {
      type:       document.querySelector('.DBIMexportApi')        ? document.querySelector('.DBIMexportApi').value        : "",
      datasource: document.querySelector('.DBMexportDataSource')  ? document.querySelector('.DBMexportDataSource').value  : "",
      database:   document.querySelector('.DBMexportDbInput')     ? document.querySelector('.DBMexportDbInput').value     : "",
      username:   document.querySelector('.DBMexportUser')        ? document.querySelector('.DBMexportUser').value        : "",
      password:   document.querySelector('.DBMexportPassword')    ? document.querySelector('.DBMexportPassword').value    : "",
      location:   TRANSprojectRoot(), // to resolve sqlite filepaths
      sql:        DBMexportDatabaseTableEditor.getValue(),
      vdb:        DBMexportFilename,
      component:  TRANScomponentGuid(),
      development: TRANSdevelopment()
   }
   APIcall("database/export", DatabaseParams, function(Response){
      if (Response.success){
         let ExecuteButton = document.querySelector('.DBMexportExecute');
         ExecuteButton.remove();
         POPpopoutClose(DBMexportPopout);
         SNCKsnackbar("success", "Export complete.", 20000);
         TRANSfileCurrentReload();
      } else {
         DBMclearFeedback();
         DBMshowFeedback("error", Response.error);
      }
   });
}
