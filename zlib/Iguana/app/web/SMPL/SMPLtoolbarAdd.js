let SMPLeditor;

const SMPLdialogAddSampleHtml = /*html*/`
<div class="SMPLimportModal">
   <div class="SMPLpane">
      <div class="SMPLresizeControl">
         <div class="SMPLresizeShadow"></div>
      </div>
      <div class="SMPLimportSideBar">
         <div class="SMPLimportFromUploadControls">
            <div class='FORMlabel'>Upload File:</div>
            <div class="SMPLimportFromFileControls">
               <div class="SMPLfileSelectButtonContainer">
                  <label for="SMPLchooseFile" class="SMPLfileSelectButton">
                     <input type="file" id="SMPLchooseFile" class="SMPLfileUploadSelect">BROWSE...
                  </label>
               </div>
               <div class="SMPLfileUploadSelected" style="display: none;">Processing selected file...</div>
            </div>
         </div>
         <div class="SMPLimportFromLogControls">
            <div class='FORMlabel'>From Component Log:</div>
            <select class='SMPLcomponentList FORMselectBox'>
               <option value="null" disabled>Select a Component</option>
            </select>
            <div class="SMPLcomponentLogs">
               <div class="SMPLsearchFilterCell">
                  <input class='SMPLsearchFilter FORMinput' placeholder="Search filter">
               </div>
               <div class="SMPLtagFilterCell">
                  <input class='SMPLtagFilter FORMinput' placeholder="Tag filter">
               </div>
               <div class='SMPLimportLogMessages'></div>
            </div>
         </div>
      </div>
   </div>
   <div class="SMPLimportMain">
		<div class="SMPLsectionHeader">SAMPLE MESSAGE</div>
      <div class='FORMlabel'>Name:</div>
      <div class='SMPLimportNameField'>
         <input class='SMPLimportName FORMinput' placeholder='Sample'>
         <div class='SMPLimportNameError SMPLhidden'>The name is already taken or empty. Please choose a different name.</div>
      </div>
      <div class='FORMlabel'>Data:</div>
      <div class='SMPLimportSampleEditor'>
         <textarea class='SMPLimportData FORMinput' placeholder='Message'></textarea>
      </div>
      <div class="SMPLbuttonGroup">
         <div class='BUTTONstandard BUTTONdefault SMPLdiscardSample'>CANCEL</div>
         <div class='BUTTONstandard BUTTONaction SMPLsaveSample'>ADD</div>
      </div>
   </div>
</div>`;

function SMPLcontainsSamplesFile(FileList) {
   for (file of FileList) {
      if (file.name === "Samples" && file.dir === false) { return true; }
   }
   return false;
}

function SMPLdialogSetup(){
   PRJexpandSamplesDir();
   let CardConfig = {
      title     : 'Add Sample Data',
      content   : SMPLdialogAddSampleHtml,
      className : 'SMPLaddDataDialog',
      width     : '85%',
      Zindex    : 1005,
      esc_close : true,
      callback : TRANSsetCode
   }
   let Card = POPpopoutOpen(CardConfig);
   SMPLdialogSetUpEditor();
   SMPLdialogSetUpName();
   SMPLdialogSetUpImport();
   SMPLdialogSetUpUpload();
   SMPLdialogSetUpButtons(Card);
   SPLTsetupSplitter( '.SMPLimportModal', '.SMPLresizeControl', '.SMPLpane', 'SMPLratio')
}

function SMPLaddSampleData() {
   if (TRANSreadOnly()) { return; }  // no-op for readonly mode
   let Params = {
      component  : TRANScomponentGuid(),
      path       : '/',
      development: TRANSdevelopment(),
   };
   APIcall('component/dir', Params, function(R){
      if (SMPLhandleError(R)) return;
      if (SMPLcontainsSamplesFile(R.data.content)) {
         SNCKsnackbar('error', 'To import sample data you cannot have a file with the name "Samples" at the root folder', 6000);
      } else {
         SMPLdialogSetup();
      }
   });
   MENUremoveTooltip();
}