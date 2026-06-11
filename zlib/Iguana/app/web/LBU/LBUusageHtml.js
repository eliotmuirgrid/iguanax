function LBUusageRow(Row){
   let Name = LBUcomponentName(Row?.component || "");
   let Path = Row?.path || "";
   Path = Path + FILsep();
   let link = LBUlink(Row?.component || "", Row?.path || "");
   let Status = Row?.has_changes ? `<div>Contains uncommitted changes.</div>` : LBUzero();
   return /*html*/`
   <div class="LBUdetRow">
      <div></div>
      <div class="LBUdata LBUcomp" title="${Name}">${Name}</div>
      <div class="LBUdata LBUpath" title="${Path}">${Path}</div>
      <div class="LBUdata LBUstatus">${Status}</div>
      <div class="LBUlinkData">${link}</div>
   </div>`;
}

function LBUusageTable(Data){
   LBUbehindData = [];
   let Count = Data?.usage?.length;
   if(Count == 0) return "";
   let Html = "";
   for(Row of Data){
      Html = Html + LBUusageRow(Row);
   }
   return /*html*/`
   <br>
   <div class="LBUdetTable LBUdetTableTop">${LBUdetTableHeader(false, false)}</div>
   <div class="LBUdetTable LBUdetTableBottom">${Html}</div>`;
}
