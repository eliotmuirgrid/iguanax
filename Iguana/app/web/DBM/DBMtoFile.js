function DBMupdateToFileName(){
   let ToFileName = document.querySelector('.DBMtoFilePath');
   if(!ToFileName) return;
   let ToName = DBMtoFile;
   if(ToName === "") ToName = "Select a VDB destination  →";
   if(ToFileName.innerHTML !== ToName) ToFileName.innerHTML = ToName;
}

function DBMtoFileHtml(Data){
   if(DBMtoFile === undefined || DBMtoFile.trim() === "") return DBMselectFileMessage();
   let MaxName = DBMdetermineMaxNameLength(Data);
   let Html = '';
   for(TableName in Data) Html += DBMeditTableRow(TableName, Data, MaxName); 
   return /*html*/`
      ${Html}
   `;
}

function DBMdetermineMaxNameLength(Data){
   let Max = 0;
   for(TableName in Data) if(Max < TableName.length && TableName.length < 11 ) Max = TableName.length;
   return Max;
}

function DBMeditTableRow(TableName, Data, Max,){
   let SpacePadding = "&nbsp;";
   let SpacesNeeded = Max - TableName.length;
   let Classes = "";
   if(DBMtoFileData["tables"] && DBMtoFileData["tables"][TableName]) Classes = "DBMdisabled";
   if(SpacesNeeded > 0 ) SpacePadding += "&nbsp;".repeat(SpacesNeeded);
   return /*html*/`
   <div class="DBMlistRow">
      <div class="DBMnodeValue DBMtable ${Classes}" value="${TableName}">${TableName}${SpacePadding}${Data[TableName].desc ? ` (${Data[TableName].desc})` : "" }</div>
   </div>`; 
}

function DBMselectFileMessage(){ return /*html*/`<div class="DBMmessage">-- Select a VDB file --</div>`; }
