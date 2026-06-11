function LBUaheadRow(Row){
   if(!Row?.ahead) return "";
   let Name = LBUcomponentName(Row?.component || "");
   let Path = Row?.path || "";
   Path = Path + FILsep();
   let link = LBUlink(Row?.component || "", Row?.path || "");
   return /*html*/`
   <div class="LBUdetRow">
      <div></div>
      <div class="LBUdata LBUcomp">${Name}</div>
      <div class="LBUdata LBUpath">${Path}</div>
      <div class="LBUdata LBUstatus">Open in Development Editor and push.</div>
      <div class="LBUlinkData">${link}</div>
   </div>`;
}

function LBUaheadTable(Data){
   LBUbehindData = [];
   let Count = LBUcount(Data, "ahead");
   if(Count == 0) return "";
   let Html = "";
   for(Row of Data){
      Html = Html + LBUaheadRow(Row);
   }
   return /*html*/`
   <br>
   <div class="LBUdetTable LBUdetTableTop">${LBUdetTableHeader(false, true)}</div>
   <div class="LBUdetTable LBUdetTableBottom">${Html}</div>`;
}
