let DBMdatabasePopout; 

function DBMsetFromDatabase(){
   EXPimportModalRender();
}
function DBMdatabaseClick(e){
   if(e.target.closest('.DBMdatabaseCancel')) POPpopoutClose(DBMdatabasePopout);
   if(e.target.closest('.DBMdatabaseQuery'))  DBMdatabaseQueryApi();
}

function DBMdatabaseChange(e){
   if(e.target.closest('.DBMdatabaseSelect')) DBMupdateDatabaseInterface();
}

function DBMdatabaseQueryApi(){
   let DatabaseFields = document.querySelector('.DBMdatabaseInput');
   if(!DatabaseFields) return;
   DBMfromFileData = {};
   DBMfromFile = "";
   let Data = {};
   Data.type       = DatabaseFields.querySelector('.DBMdatabaseSelect')?.value;
   Data.datasource = DatabaseFields.querySelector('.DBMdataSource')    ?.value;
   Data.database   = DatabaseFields.querySelector('.DBMdatabase')      ?.value;
   Data.username   = DatabaseFields.querySelector('.DBMusername')      ?.value;
   Data.password   = DatabaseFields.querySelector('.DBMpassword')      ?.value;
   Data.location   = TRANSprojectRoot();
   Data.development = TRANSdevelopment();
   APIcall("database/read", Data, function(R){
      if(!R.success) return SNCKsnackbar('error', R.error);
      DBMfromFileData = R.data;
      DBMfromDatabase = `${Data.api} Database ${Data.datasource}`;
      MODELforcePoll('DBMmodel');
      POPpopoutClose(DBMdatabasePopout);
   });
}

function DBMupdateDatabaseInterface(){
   let Database = document.querySelector('.DBMdatabaseSelect');
   if(!Database) return;
   let DatabaseSelected = Database.value;
   let DatabaseInterface = document.querySelector('.DBMdatabaseInterface');
   if(!DatabaseInterface) return;
   DatabaseInterface.innerHTML = DBMdatabaseInterfaceHtml(DatabaseSelected);
}

function DBMdatabaseInterfaceHtml(Database){
   if(Database === "SQLite")                return DBMdatabaseSqliteHtml();
   if(Database === "MySQL")                 return DBMdatabaseOneHtml();
   if(Database === "MariaDB")               return DBMdatabaseOneHtml();
   if(Database === "ODBC - PostgreSQL")     return DBMdatabaseOdbcHtml();
   if(Database === "ODBC - MySQL")          return DBMdatabaseOdbcHtml();
   if(Database === "ODBC - Oracle")         return DBMdatabaseOdbcHtml();
   if(Database === "ODBC - MS SQL Server")  return DBMdatabaseOdbcHtml();
   if(Database === "OCI - Oracle")          return DBMdatabaseOneHtml();
}

function DBMdatabaseHtml(Databases){
   let Html = `
   <div class="DBMdatabaseInput">
      <div class="DBMdatabaseSkeleton">
         <div class="FORMlabel">Source:</div>
         <select class="DBMdatabaseSelect FORMselectBox">`
   for (i of Databases) { Html = Html + "<option value='" + i + "'>" + i + "</option>" }
   Html = Html + `
         </select>
      </div>
      <div class="DBMdatabaseInterface"></div>
      <div class="BUTTONgroup DBMqueryButton">
         <div class="DBMdatabaseCancel BUTTONdefault BUTTONstandard">CANCEL</div>
         <div class="DBMdatabaseQuery BUTTONstandard BUTTONaction">QUERY</div>
      </div>
   </div>`;
   return Html;
}

function DBMdatabaseOneHtml(){
   return /*html*/`
      <div class="DBMdatabaseSkeleton">
         <div class="FORMlabel">Data Source:</div>
         <input class="DBMdataSource FORMinput" placeholder="Required">
         <div class="FORMlabel">Username:</div>
         <input class="DBMusername FORMinput" placeholder="Required">
         <div class="FORMlabel">Password:</div>
         <input class="DBMpassword FORMinput" placeholder="Optional" type="password">
      </div>
   `;
}

function DBMdatabaseOdbcHtml(){
   return /*html*/`
      <div class="DBMdatabaseSkeleton">
         <div class="FORMlabel">Data Source:</div>
         <input class="DBMdataSource FORMinput" placeholder="Required">
         <div class="FORMlabel">Database:</div>
         <input class="DBMdatabase FORMinput"   placeholder="Required">
         <div class="FORMlabel">Username:</div>
         <input class="DBMusername FORMinput" placeholder="Required">
         <div class="FORMlabel">Password:</div>
         <input class="DBMpassword FORMinput" placeholder="Optional" type="password">
      </div>
   `;
}

function DBMdatabaseSqliteHtml(){
   return /*html*/`
      <div class="DBMdatabaseSkeleton">
         <div class="FORMlabel">Data Source:</div>
         <input class="DBMdataSource FORMinput" placeholder="Required">
      </div>
   `;
}
