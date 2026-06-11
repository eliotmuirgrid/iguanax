let ECNcard;

function ECNscreen(){
   DASHdashboardScreen();
   ECNcard = POPpopoutOpen({
      title: 'Component connections',
      width: '60%', 
      minWidth: '800px',  
      content: ECNcontent(),  
      esc_close : true,
      callback : DASHcloseModal
   });
   ECNmodelInit();
   ECNlisteners();
}
function ECNsourcesTable(){
   return /*html*/`
   <div class="ECNsourcesTable">
      <div class="ECNsourcesHeader">Sources:</div>
      <div class="ECNsourcesList"></div>
      <div class="ECNaddSource"></div>
      <div class="ECNsourcesError"></div>
   </div>`;
}

function ECNtarget(){
   return /*html*/`
   <div class="ECNtitle">SELECTED:</div>
   <div class="ECNtarget">
      <div class="ECNtargetDivInfo"></div>
   </div>`;
}
function ECNreceiversTable(){
   return /*html*/`
   <div class="ECNreceiversTable">
      <div class="ECNreceiversHeader">Destinations:</div>
      <div class="ECNreceiversList"></div>
      <div class="ECNaddReceiver"></div>
      <div class="ECNreceiversError"></div>
   </div>`;
}

function ECNtables(){
   return /*html*/`
   <div class="ECNtitle">CONNECTIONS:</div>
   <div class="ECNtables">
      ${ECNsourcesTable()}
      ${ECNreceiversTable()}
   </div>`;
}

function ECNbuttonRowHtml(){
   return /*html*/`<div class="ECNbuttonRow">
      <div class="ECNerror"></div>
      <div class="BUTTONgroup">
         <div class="ECNclose BUTTONstandard BUTTONdefault">CLOSE</div>
      </div>
   </div>`;
}

function ECNcontent(){
   return /*html*/`
   <div class="ECNmodal">
      ${ECNtarget()}
      ${ECNtables()}
      ${ECNbuttonRowHtml()}
   </div>`;
}