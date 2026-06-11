function SLBtableUpdate(){
   let Data = SLBdata();
   SLBdataObj = Data;
   let div = document.querySelector(".SLBtarget");
   if(!div) return;
   if(DEEPequal(SLBpageDataRendered, Data)) return;
   let Html = SLBempty(Data) ? SLBemptyHtml() : SLBtableHtml(Data);
   if(div.innerHTML == Html) return;
   div.innerHTML = Html;
   SLBpageDataRendered = Data;
}

function SLBheaderHtml(){
   return /*html*/`
   <div class="SLBheading">
      <div>Name</div>
      <div class="SLBahead">Customized</div>
      <div class="SLBbehind">Outdated</div>
      <div>Used in</div>
   </div>`;
}

function SLBrowHtml(DataRow, Key){
   let Name = SLBname(DataRow?.name, Key);
   let Link = SLBlink(DataRow?.urls?.web);
   let Usage = DataRow?.usage;
   return /*html*/`
   <div class="SLBrow">
      <div class="SLBnameDiv">
         ${Link}
         <a class="SLBname" href="#settings/libraries?url=${Key}">${Name}</a>
      </div>
      <div class="SLBdata">${SLBformat(SLBcount(Usage, "ahead"))}</div>
      <div class="SLBdata">${SLBformat(SLBcount(Usage, "behind"))}</div>
      <div class="SLBdata">${SLBformat(Usage?.length || 0)}</div>
   </div>`;
}

function SLBtableHtml(Data){
   return LBUtableHtml(Data, "", true);
}