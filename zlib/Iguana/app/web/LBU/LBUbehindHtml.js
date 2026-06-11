function LBUcheckbox(extraClass){
   return `<div class="LBUcheckbox${extraClass ? " " : ""}${extraClass || ""}"><input type="checkbox" class="LBUselectRow"></div>`;
}

function LBUbehindRow(Row){
   if(!Row?.behind) return "";
   LBUbehindData.push(Row);
   let Name = LBUcomponentName(Row?.component || "");
   let Path = Row?.path || "";
   Path = Path + FILsep();
   let Status = Row?.has_changes ? `<div class="LBUwarningIcon">Uncommitted changes—fix, then update.</div>` : LBUzero();
   let link = LBUlink(Row?.component || "", Row?.path || "");
   return /*html*/`
   <div class="LBUdetRow">
      <div>${LBUcheckbox()}</div>
      <div class="LBUdata LBUcomp" title="${Name}">${Name}</div>
      <div class="LBUdata LBUpath" title="${Path}">${Path}</div>
      <div class="LBUdata LBUstatus">${Status}</div>
      <div class="LBUlinkData">${link}</div>
   </div>`;
}

function LBUdetTableHeader(behind, instr){
   let checkbox = behind ? `${LBUcheckbox("LBUheader")}` : "";
   let status = instr ? "Row instruction" : "Status";
   return /*html*/`
   <div class="LBUdetHeader">
      <div>${checkbox}</div>
      <div>Component</div>
      <div>Location</div>
      <div>${status}</div>
      <div></div>
   </div>`;
}

function LBUupdateButton(){
   return /*html*/`
   <div class="LBUbuttonGroup">
      <div class="LBUerrorFeedback"></div>
      <div class="BUTTONstandard BUTTONdisabled LBUupdateComp">UPDATE</div>
   </div>`;
}

function LBUbehindTable(Data){
   LBUbehindData = [];
   let Count = LBUcount(Data, "behind");
   if(Count == 0) return "";
   let Html = "";
   for(Row of Data){
      Html = Html + LBUbehindRow(Row);
   }
   return /*html*/`
   <br>
   <div class="LBUdetTable LBUdetTableTop">${LBUdetTableHeader(true, false)}</div>
   <div class="LBUdetTable LBUdetTableBottom">${Html}</div>
   <br>
   ${LBUupdateButton()}`;
}
