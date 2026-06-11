function TBLeditListHtml(Data, FileName){
   let TableCount = Object.keys(Data).length;
   return /*html*/`
   <div class="TBLwindow">
      <div class="TBLeditBanner">
         <div class="TBLtitle">Database Tables (${TableCount})</div>
         <div class="TBLfileName">(${FileName})</div>
         <div class="BUTTONgroup">
            <div class="TBLimport BUTTONaction BUTTONstandard">IMPORT</div>
            <div class="TBLdoneEditList BUTTONdefault BUTTONstandard">DONE</div>
         </div>
      </div>
      <div class="TBLcontent">
         ${TBLeditFolderListHtml(Data)}
      </div>
   </div>`;
}

function TBLeditFolderListHtml(Data){
   let MaxName = TBLdetermineMaxNameLength(Data);
   let Html = /*html*/`
   <div class="TBLlistRow">
      <div class="TBLaddListItem"> + ADD TABLE DEFINITION </div>
   </div>`;
   for(TableName in Data) Html += TBLeditTableRow(TableName, Data, MaxName); 
   return Html;
}

function TBLeditTableRow(TableName, Data, Max){
   let SpacePadding = "&nbsp;";
   let SpacesNeeded = Max - TableName.length;
   if(SpacesNeeded > 0 ) SpacePadding += "&nbsp;".repeat(SpacesNeeded);
   return /*html*/`
   <div class="TBLlistRow">
      <div class="TBLnodeValue TBLtable" value="${ESChtmlEscape(TableName)}">${ESChtmlEscape(TableName)}${SpacePadding}${Data[TableName].desc ? ` (${ESChtmlEscape(Data[TableName].desc)})` : "" }</div>
      <div class="TBLlistRemove" title="Remove table definition">x</div>
   </div>`; 
}