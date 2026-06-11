function DBMexportHtml(DatabaseApis, Datasource){
   let ExportWindowHtml = `
   <div class='DBMexportContainer'>
      <div class='DBMexportExplain'>This window will generate the SQL to create all the tables found in the vdb file. Tables that already exist in the database will be dropped first. You can edit the generated SQL to exclude columns or for other custom behaviour.</div>
      <div class='DBMexportOverwriteWarning'>Note that changing the Api value will regenerate the SQL and you will lose any changes you have made.</div>
      <div class='DBMapiAndDatasource'>
         <div class='FORMlabel DBMalignMiddle'>Api:     </div>
         <select class='DBIMexportApi FORMselectBox' placeholder='Choose database'>`
      for (i of DatabaseApis) { ExportWindowHtml = ExportWindowHtml + "<option>" + i + "</option>" }
      ExportWindowHtml = ExportWindowHtml + `
         </select>
         <div></div> 
         <div class='FORMlabel DBMalignMiddle'>Datasource:</div>
         <input class='DBMexportDataSource FORMinput' placeholder='Required', value='${Datasource}'/>
         <div class='DBMdatasourceExistsInfo DBMalignMiddle'></div>
      </div>
      <div class='DBMexportInputContainer'>
      </div>
      <div class="DBMexportSqlContainer">
      </div>
      <div class="DBMexportError"></div>
      <div class="DBMexportButtonRight">
         <div class="DBMexportExecute BUTTONaction BUTTONstandard">EXECUTE</div>
      </div>
   </div>
   `;
   return ExportWindowHtml;
}

function DBMexporterDbParamsHtml(Api){
   if(Api === "SQLite")                return "";
   if(Api === "MySQL")                 return DBMexporterBasicParams();
   if(Api === "MariaDB")               return DBMexporterBasicParams();
   if(Api === "ODBC - PostgreSQL")     return DBMexporterOdbcParams();
   if(Api === "ODBC - MySQL")          return DBMexporterOdbcParams();
   if(Api === "ODBC - Oracle")         return DBMexporterOdbcParams();
   if(Api === "ODBC - MS SQL Server")  return DBMexporterOdbcParams();
   if(Api === "OCI - Oracle")          return DBMexporterBasicParams();
}

function DBMexporterBasicParams(){
   return `
   <div class='FORMlabel'>Username:</div><input class='DBMexportUser FORMinput' placeholder='Required'/>
   <div class='FORMlabel'>Password:</div><input class='DBMexportPassword FORMinput' placeholder='Optional'/>`;
}

function DBMexporterOdbcParams(){
   return `
   <div class='FORMlabel'>Database:</div><input class='DBMexportDbInput  FORMinput' placeholder='Required'/>
   <div class='FORMlabel'>Username:</div><input class='DBMexportUser FORMinput' placeholder='Required'/>
   <div class='FORMlabel'>Password:</div><input class='DBMexportPassword FORMinput' placeholder='Optional'/>`;
}
