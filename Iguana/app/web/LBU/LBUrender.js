let LBUpopoutCard;
let LBUdataRendered = undefined;

function LBUdetailsHtml(){
   return /*html*/`
   <div class="LBUcontainer">
      <div class="LBUcontents">
         <div class="LBUloading">
            <div class="CARDloading-spinner"></div>
            <div class="LBUgathering">Gathering library usage info...</div>
            <div></div>
         </div>
      </div>
      <div class="LBUdetailsDiv"></div>
      <div class="LBUbuttons">
         <div class="BUTTONstandard BUTTONdefault LBUclose">CLOSE</div>
      </div>
   </div>`;
}

function LBUrender(){
   let Popout = LBUpopout();
   if(Popout) return;
   LBUdataRendered = undefined;
   LBUpopoutCard = POPpopoutOpen({
      title:'Library usage & version drift',
      content: LBUdetailsHtml(),
      width: '950px',
      height: '850px',
      className: 'LBUpopout',
      esc_close : true
   });
   LBUmodelInit();
   LBUlisteners();
}

function LBUpopout(){ return document.querySelector(".LBUpopout"); }

function LBUnoLibsHtml(){
   return `<div class="LBUnoLibs">This component does not use any libraries so there is to nothing show.</div>`;
}

function LBUupdatePopout(){
   let Data = LBUdata();
   if(!DEEPequal(LBUdataRendered, Data)) LBUupdateTable();
   LBUdataRendered = Data;
}

function LBUupdateTable(){
   let div = document.querySelector(".LBUcontents");
   if(!div) return;
   let Html = "";
   if(!LBUlibsExist()) Html = LBUnoLibsHtml();
   else if(LBUblankData()) return;
   else Html = LBUhtml();
   if(Html != div.innerHTML) div.innerHTML = Html;
}