let SLBdetailPopout;
let SLBdetailLib = "";
let SLBdataRendered = undefined;
let SLBdataDetailsRendered = undefined;

function SLBdetailsHtml(libdata){
   return /*html*/`
   <div class="LBUcontents">
      ${LBUtableHtml(libdata, "", true)}
   </div>
   <div class="LBUdetailsDiv"></div>
   <div class="LBUbuttons">
      <div class="BUTTONstandard BUTTONdefault LBUclose">CLOSE</div>
   </div>`;
}

function SLBsetData(data){
   SLBdetailData = data;
}

function SLBresetDetailsData(){
   SLBdataRendered = undefined;
   SLBdataDetailsRendered = undefined;
}

function SLBopenDetails(){
   SLBresetDetailsData();
   SLBdetailModelInit();
   let data = SLBdetData();
   let Key    = SLBkey();
   let Popout = SLBpopout();
   if(!Key || Popout) return;
   SLBdetailPopout = POPpopoutOpen({
      title:'Library details',
      content: SLBdetailsHtml(data),
      className: "LBUdetails",
      width: '950px',
      esc_close : true,
      callback : SLBclosePopout
   });
   SLBupdateDetails();
   SLBdetailListeners();
}

function SLBclosePopout(){ window.location.hash = "#settings/libraries"; }

function SLBkey(){ return PAGEhashParameter("url"); }

function SLBpopout(){ return document.querySelector(".SLBpopout"); }

function SLBupdatePopout(){
   let Data = SLBdetData();
   if(!DEEPequal(SLBdataRendered, Data)) SLBupdateTable();
   SLBdataRendered = Data;
}

function SLBupdateDetails(){
   let focus = PAGEhashParameter("focus");
   let key = SLBkey();
   let libdata = SLBdetData();
   let empty = true; // SLBdetailsUpdate();
   // console.log(empty);
   if(focus == "ahead"  && empty) return LBUaheadUpdate(libdata[key], key);
   if(focus == "behind" && empty) return LBUbehindUpdate(libdata[key], key);
   if(focus == "usage"  && empty) return LBUusageUpdate(libdata[key], key);; 
}

function SLBdetailsUpdate(){
   let div = document.querySelector(".LBUdetailsDiv");
   if(!div) return false;
   if(div.innerHTML == "") return true;
   return false;
}

function SLBdetClear(){
   let div = document.querySelector(".LBUdetailsDiv");
   if(!div) return;
   div.innerHTML = "";
}