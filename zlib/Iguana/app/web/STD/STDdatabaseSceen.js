// TODO - Could use the MODEL framework for this page

let SETdatabaseObject = {};

function STDdatabaseScreen(){
   SETrender();
	document.querySelector('.STDdatabase').classList.add('selected');
   STDmodelInit();
   STDlisteners();
}

function STDrenderDatabases(){
   let Response = STDdatabaseData();
   if (Response["success"]){
      SETdatabaseObject = Response.data;
      SETdatabaseUpdateViewMode();

   } else {
      document.querySelector('.SETdatabaseActions').remove();
      document.querySelectorAll('.SETdatabaseTableRow').forEach((item)=>{
         item.remove();
      });
      document.querySelector('.SETdatabaseTableGeneralError').innerHTML="Error: Failed to get database driver paths. Details: " + Response.error;
   }
}

function SETdatabaseUpdateViewMode(){
   document.querySelector('.SETdatabaseTableDataMysql').innerHTML=ESChtmlEscape(SETdatabaseObject.mysql);
   document.querySelector('.SETdatabaseTableDataOracle').innerHTML=ESChtmlEscape(SETdatabaseObject.oracle);
   document.querySelector('.SETdatabaseTableDataOdbc').innerHTML=ESChtmlEscape(SETdatabaseObject.odbc);
}

function SETdatabaseScreenEdit(){
   document.querySelector('.SETcontentBody').innerHTML = SETdatabaseEditHtml(SETdatabaseObject);
}

function SETdatabaseSave(){
   let Data = {
      "mysql"  : document.querySelector('.SETdatabaseMysqlEdit').value,
      "oracle" : document.querySelector('.SETdatabaseOracleEdit').value,
      "odbc"   : document.querySelector('.SETdatabaseOdbcEdit').value
   }
   APIcall('settings/db/set', Data, function(Response){
      if (Response.success && !Response.data.issue){
         let Message = GINsaveDatabase();
         GINaddAndCommit({message: Message, settings: true});
         STDdatabaseScreen();
         CONFIRMaction("To apply database changes, Iguana must restart. Restart now?", function(){ GINrestart(); }, "CONFIRM");
      }
      else { SETdatabaseShowFileErrors(Response.data.issue); }
   });
}

function SETdatabaseShowFileErrors(FileErrors){
   if (typeof FileErrors == "string"){
      document.querySelector('.SETdatabaseTableGeneralError').innerHTML = "Error: Encountered a problem while saving. Details: " + FileErrors;
   } else {
      if (FileErrors["mysql"]) { document.querySelector('.SETdatabaseMysqlFeedback') .innerHTML=FileErrors["mysql"];   }
      if (FileErrors["oracle"]){ document.querySelector('.SETdatabaseOracleFeedback').innerHTML=FileErrors["oracle"]; }
      if (FileErrors["odbc"])  { document.querySelector('.SETdatabaseOdbcFeedback')  .innerHTML=FileErrors["odbc"];     }
   }
}

function STDeditButtonHtml(){
   let Edit = STDeditPermission();
   let Classes =  Edit ? "SETeditDatabase BUTTONaction" : "BUTTONdisabled";
   let title   = !Edit ? `title="You do not have permission to edit this."` : "";
   return /*html*/`<div class="${Classes} BUTTONstandard" ${title}>EDIT</div>`;
}

function SETdatabaseHtml(){
   return /*html*/`
   <div class="SETsettingsPaneLayout SETsettingsPane">
      <div class="SETdatabaseTitle">
         <div class="SETdatabaseLabel">Database</div>
         <div class="SETdatabaseActions BUTTONgroup">
            ${STDeditButtonHtml()}
         </div>
      </div>
      <div class="SETpageDescription">
         <div>View and configure the <a href="${LINKsettingsDatabaseHeader}" target="_blank">database client libraries</a> used by Iguana to communicate with various databases.</div>
      </div>
      <div class="SETdatabaseTable">
         <div class="SETdatabaseHeading">
            <div class="SETdatabaseTableHeading">Database API</div>
            <div class="SETdatabaseTableHeading">Path to Shared Library</div>
         </div>
         <div class="SETdatabaseTableRow">
            <div class="SETdatabaseTableData">MySQL Compliant</div>
            <div class="SETdatabaseTableData SETdatabaseTableDataMysql">Not Configured</div>
         </div>
         <div class="SETdatabaseTableRow">
            <div class="SETdatabaseTableData">OCI Oracle</div>
            <div class="SETdatabaseTableData SETdatabaseTableDataOracle">Not Configured</div>
         </div>
         <div class="SETdatabaseTableRow">
            <div class="SETdatabaseTableData">ODBC</div>
            <div class="SETdatabaseTableData SETdatabaseTableDataOdbc">Not Configured</div>
         </div>
      </div>
      <div class="SETdatabaseTableGeneralError"></div>
   </div>`;
}

function SETdatabaseEditHtml(Response){
   return /*html*/`
   <div class="SETsettingsPaneLayout SETsettingsPane">
      <div class="SETdatabaseTitle">
         <div class="SETdatabaseLabel">Database</div>
         <div class="SETdatabaseActions BUTTONgroup">
            <div class="SETsaveDatabase BUTTONaction BUTTONstandard">SAVE</div>
            <div class="SETcancelDatabase BUTTONcancel BUTTONstandard">CANCEL</div>
         </div>
      </div>
      <div class="SETpageDescription">
      <div>View and configure the <a href=${LINKsettingsDatabaseHeader} target="_blank">database client libraries</a> used by Iguana to communicate with various databases.</div>
      </div>
      <div class="SETdatabaseTable">
         <div class="SETdatabaseHeading">
            <div class="SETdatabaseTableHeading">Database API</div>
            <div class="SETdatabaseTableHeading">Path to Shared Library</div>
         </div>
         <div class="SETdatabaseTableRow">
            <div class="SETdatabaseTableData">MySQL Compliant</div>
            <div class="SETdatabaseTableData">
               <div class="SETdatabaseTableValue FORMinputGroup">
                  <input class="FORMinput SETdatabaseMysqlEdit" type="text" value="${ESChtmlEscape(Response.mysql)}">
                  <div class="SETmysqlBrowse BUTTONstandard BUTTONcancel">BROWSE...</div>
                  <div class="SETdatabaseFileFeedback SETdatabaseMysqlFeedback"></div>
               </div>
            </div>
         </div>
         <div class="SETdatabaseTableRow">
            <div class="SETdatabaseTableData">OCI Oracle</div>
            <div class="SETdatabaseTableData">
               <div class="SETdatabaseTableValue FORMinputGroup">
                  <input class="FORMinput SETdatabaseOracleEdit" type="text" value="${ESChtmlEscape(Response.oracle)}">
                  <div class="SEToracleBrowse BUTTONstandard BUTTONcancel">BROWSE...</div>
                  <div class="SETdatabaseFileFeedback SETdatabaseOracleFeedback"></div>
               </div>
            </div>
         </div>
         <div class="SETdatabaseTableRow">
            <div class="SETdatabaseTableData">ODBC</div>
            <div class="SETdatabaseTableData">
               <div class="SETdatabaseTableValue FORMinputGroup">
                  <input class="FORMinput SETdatabaseOdbcEdit" type="text" value="${ESChtmlEscape(Response.odbc)}">
                  <div class="SETodbcBrowse BUTTONstandard BUTTONcancel">BROWSE...</div>
                  <div class="SETdatabaseFileFeedback SETdatabaseOdbcFeedback"></div>
               </div>
            </div>
         </div>
      </div>
      <div class="SETdatabaseTableGeneralError"></div>
   </div>`;
}