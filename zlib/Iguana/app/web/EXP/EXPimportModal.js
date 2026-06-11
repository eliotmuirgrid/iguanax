let EXPimportCard;

function EXPimportModalRender(){
   EXPread();
   if(document.querySelector('.EXPmodal')) return;
   let CardConfig = {
      title     : 'Database query',
      content   : EXPimportHtml(),
      className : 'EXPimportWindow',
      width: '750px',
      callback  : function(){ MODELremove("EXPmodel"); }
   }
   EXPimportCard = POPpopoutOpen(CardConfig);
   EXPmodelInit();
   EXPlisteners();
}

function EXPimportHtml(){
   return /*html*/`<div class="EXPmodal">
      ${EXPheaderHtml(false)}
   </div>`;
}

function EXPqueryActions(){
   return /*html*/`
   <br>
   <br>
   <div class="BUTTONgroup EXPbuttons">
      <div class="BUTTONdefault BUTTONstandard">CANCEL</div>
      <div class="BUTTONaction BUTTONstandard">CONFIRM</div>
   </div>`;
}

function EXPimportQuery(){
   DBMfromFileData = {};
   DBMfromFile = "";
   let Args = EXPdatabaseArgs();
   APIcall("database/read", Args, function(R){
      if(!R.success) return SNCKsnackbar('error', R.error);
      DBMfromFileData = R.data;
      DBMfromDatabase = `${Args?.type} Database ${Args?.datasource}`;
      MODELforcePoll('DBMmodel');
      POPpopoutClose(EXPimportCard);
   })
}