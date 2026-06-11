function DBMimportListHtml(Data){
   let ErrorMessage = "";
   if(Object.keys(DBMfromFileData).length === 0) ErrorMessage = "Select a VDB source";
   if(Object.keys(DBMtoFileData).length === 0)   ErrorMessage = "Select a VDB destination";
   if((Object.keys(DBMtoFileData).length === 0) && (Object.keys(DBMfromFileData).length === 0)) ErrorMessage = "Select a VDB source and destination";
   if(ErrorMessage !== "") return DBMselectFileErrorMessage(ErrorMessage);
   if(Data === undefined) return "";
   let MaxName = DBMdetermineMaxNameLength(Data);
   let Html = '';
   for(TableName in Data) Html += DBMeditTableRowList(TableName, Data, MaxName); 
   return /*html*/`
      ${Html}
   `;
}

function DBMselectFileErrorMessage(ErrorMessage){ return /*html*/`<div class="DBMmessage">-- ${ErrorMessage} --</div>`; }

function DBMeditTableRowList(TableName, Data, Max,){
   let SpacePadding = "&nbsp;";
   let SpacesNeeded = Max - TableName.length;
   let Classes = "";
   if(SpacesNeeded > 0 ) SpacePadding += "&nbsp;".repeat(SpacesNeeded);
   return /*html*/`
   <div class="DBMlistRow">
      <div class="DBMnodeValue DBMtable ${Classes}" value="${TableName}">${TableName}${SpacePadding}${Data[TableName].desc ? ` (${Data[TableName].desc})` : "" }</div>
      <div class="DBMdeleteListItem" title="Remove ${TableName}">x</div>
   </div>`; 
}
