function SMPLcurrentError() {
   let Editor = SMPLeditor;
   if(!Editor) return "";
   return SMPLbadFileName();
}

function SMPLsaveData(FilePath, Content, Card) {
   COL_TRC("SMPLsaveData", Content);
   let Params = {};
   Params.component = TRANScomponentGuid();
   Params.path      = FilePath;
   Params.content   = Content;
   Params.development = TRANSdevelopment();
   APIcall("component/file/add", Params, function(Response) {
      if (SMPLhandleError(Response)) return;
      SNCKsnackbar('success', "Added sample data: " + FilePath + "'.", 3000);
      POPpopoutClose(Card);
      SMPLonSaveSuccess(FilePath);
   });
}

function SMPLonSaveSuccess(FilePath){
   if (SMPLeditingSampleData()) PAGEsetHashParameter("edit", FilePath);
   MODELforcePoll('TRANSmodel', function() {
      SMPLgetSampleFileList(function(){
         SMPLsetIndexWithFileName(FilePath);
         TRANSupdateAnnotations();
      });
   });
}

function SMPLsaveButtonClicked(Card) {
   const Content = LFDsaveEncoded(SMPLeditor.getValue());
   if (SMPLcurrentError()) return;
   let FilePath = SMPLrootFolder() + SMPLimportName();
   SMPLsaveData(FilePath, Content, Card);
}

function SMPLdialogRenderButtons() {
   COL_TRC("SMPLdialogRenderButtons");
   let SmplCard = document.querySelector('.SMPLimportModal');
   if(!SmplCard) return;
   if (SMPLcurrentError()) {
      COL_TRC("Bad file name so we disable the save button");
      document.querySelector(".SMPLsaveSample").classList.add("BUTTONdisabled");
   } else {
      COL_TRC("Good file name so we enable the save button");
      document.querySelector(".SMPLsaveSample").classList.remove("BUTTONdisabled");
   }
}
function SMPLdialogRenderButtonsWrapper() { // to get tracing.
   SMPLdialogRenderButtons();
}

function SMPLdialogSetUpButtons(Card) {
   document.querySelector(".SMPLsaveSample")        .addEventListener('click',  function(){ SMPLsaveButtonClicked(Card); });
   document.querySelector(".SMPLdiscardSample")     .addEventListener('click',  function(){ POPpopoutClose(Card); });
   document.querySelector(".SMPLimportSampleEditor").addEventListener('keyup',  function(){ SMPLdialogRenderButtons(); });
   document.querySelector(".SMPLimportSampleEditor").addEventListener('change', function(){ SMPLdialogRenderButtons(); });
   document.querySelector(".SMPLimportSampleEditor").addEventListener('click',  function(){ SMPLdialogRenderButtons(); });
}
