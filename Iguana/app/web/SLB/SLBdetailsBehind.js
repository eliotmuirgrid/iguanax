function SLBcheckbox(extraClass){
   return `<div class="SLBcheckbox${extraClass ? " " : ""}${extraClass || ""}"><input type="checkbox" class="SLBselectRow"></div>`;
}

function SLBbehindRow(Row){
   if(!Row?.behind) return "";
   return /*html*/`
   <div class="SLBdetRow">
      <div>${SLBcheckbox()}</div>
      <div class="SLBdata SLBcomp">${Row?.component}</div>
      <div class="SLBdata SLBpath">${Row?.path}</div>
   </div>`;
}

function SLBdetTableHeader(){
   return /*html*/`
   <div class="SLBdetHeader">
      <div>${SLBcheckbox("SLBheader")}</div>
      <div>Component behind</div>
      <div>Path</div>
      <div></div>
   </div>`;
}

function SLBupdateButton(){
   return /*html*/`
   <div class="SLBbuttonGroup">
      <div class="BUTTONstandard BUTTONaction SLBupdateComp">UPDATE</div>
   </div>`;
}

function SLBbehindTable(Data){
   let Count = SLBcount(Data, "behind");
   if(Count == 0) return "";
   let Html = SLBdetTableHeader();
   for(Row of Data){
      Html = Html + SLBbehindRow(Row);
   }
   return /*html*/`
   <br>
   <div class="SLBdetTable">${Html}</div>
   <br>
   ${SLBupdateButton()}
   `;
}

function SLBupdateBehind(Data){
   let Usage = Data?.usage || [];
   let Table = SLBbehindTable(Usage);
   SLBupdate(".SLBbehindTarg", Table);
}