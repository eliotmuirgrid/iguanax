let BLKcard;
let BLKargs;
let BLKendVal;
let BLKmiddleData;

function BLKconfirm(){
   let div = document.querySelector(".LOGbulkAction");
   if(!div) return;
   if(div.value === "Resubmit") BLKresubmit();
}

function BLKresubmit(){
   BLKgetStart();
   BLKgetEnd();
   BLKmodelInit();
   let Args = {};
   Args.content = BLKskeletonHtml();
   Args.title = "Resubmit Messages";
   Args.width = "75%";
   Args.height = "82%";
   Args.minHeight = "580px";
   Args.className = "BLKbulkResubmitPopWindow"
   BLKcard = POPpopoutOpen(Args);
   document.querySelector('.BLKbulkResubmitPopWindow .POPpopoutCard-content').classList.add('BLKbulkResubmitPop');
   BLKlisteners();
}

function BLKstartDiv(){ return document.querySelector(".LOGbulkStart"); }
function BLKendDiv()  { return document.querySelector(".LOGbulkEnd"); }

function BLKlogsHtml(){
   return /*html*/`   
   <div class="BLKmiddle">
      <div class="BLKellipsis">Loading logs...</div>
   </div>`;
}

function BLKleftHtml(){
   return /*html*/`
   <div class="BLKleft">
      <div class="BLKtitle">CRITERIA:</div>
      <div class="BLKcriteria"></div>
      <div class="BLKtitle">RESULTS:</div>
      <div class="BLKcount"></div>
      <div></div>
      <div class="BLKcomponent">
         <div class="FORMlabel BLKconfirmText">Resubmit messages to Destination Component:</div>
         <select class="BLKselect FORMselectBox"></select>
      </div>
      <div class="BLKfeedback">
         <div class="BLKerror"></div>
         <div class="BLKsuccess"></div>
      </div>
      <div class="BUTTONgroup BLKbuttons">
         <div class="BUTTONstandard BUTTONcancel BLKclose">CANCEL</div>
         <div class="BUTTONstandard BUTTONaction BLKresubmitConfirm BLKconfirmTarget">RESUBMIT</div>
      </div>
   </div>`;
}

function BLKskeletonHtml(){
   return /*html*/`
   <div class="BLKmodelContent">
      <div class="BLKmodal">
         ${BLKleftHtml()}
         ${BLKlogsHtml()}
      </div>
   </div>`;
}