let EXPcard;

function EXPrender(FileName){
   EXPfile = FileName;
   EXPread();
   if(document.querySelector('.EXPmodal')) return;
   let CardConfig = {
      title     : 'Export database table',
      content   : EXPhtml(),
      className : 'EXPwindow',
      width     : '80%',
      Zindex    : 805,
      callback  : function(){ MODELremove("EXPmodel"); }
   }
   EXPcard = POPpopoutOpen(CardConfig);
   EXPmodelInit();
   EXPinitEditor();
   EXPlisteners();
   EXPgenerateSql();
   SPLTsetupSplitter( '.EXPmodal', '.EXPresizeControl', '.EXPpane', 'EXPratio');
}

function EXPpane(IncludeFile){
   return /*html*/`
   <div class="EXPpane">
      ${IncludeFile = EXPsplitter()}
      ${IncludeFile = EXPheaderHtml(true)}
   </div>`;
}

function EXPheaderHtml(IncludeFile){
   return /*html*/`
   <div class="EXPheader">
      <div class="EXPheaderFont">TYPE:</div>
      <select class="FORMselectBox EXPdatabaseType">
      </select>
      <div class="EXPinputDivs"></div>
      <div class="EXPgenerateFeedback"></div>
      ${IncludeFile ? EXPgenerateRow() : EXPqueryActions()}
      ${IncludeFile ? /*html*/`
      <div class="EXPfileDiv">
         <div class="EXPheaderFont">FROM FILE:</div>
         <div class="EXPfile"></div>
      </div>` : ""}
   </div>`;
}

{/* <div class="BUTTONcancel BUTTONstandard" title="Generate from VDB file">...</div> */} // realized this assumes we are always in the component folder


function EXPgenerateRow(){
   return /*html*/`
   <div class="EXPgenerateRow">
      <div class="EXPgenerateTitle"></div>
      <div class="BUTTONaction BUTTONstandard">GENERATE SQL</div>
   </div>`;
}

function EXPcodeHtml(){
   return /*html*/`
   <div class="EXPcode">
      <div class="EXPheaderFont">EXPORT SQL:</div>
      <div class="EXPsql">
         <textarea class='EXPsqlEditor FORMinput' placeholder='Message'></textarea>
      </div>
      <div class="EXPexportFeedback">
      </div>
      <div class="BUTTONgroup EXPbuttons">
         <div class="BUTTONdefault BUTTONstandard EXPcancel">CANCEL</div>
         <div class="BUTTONstandard BUTTONaction EXPexport">EXPORT</div>
      </div>
   </div>`;
}

function EXPsplitter(){
   return /*html*/`
   <div class="EXPresizeControl">
      <div class="EXPresizeShadow"></div>
   </div>`;
}

function EXPhtml(){
   return /*html*/`
   <div class="EXPmodal">
      ${EXPpane(true)}
      ${EXPcodeHtml()}   
   </div>`;
}
