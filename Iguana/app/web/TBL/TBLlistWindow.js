function TBLlistHtml(Data, FileName){
   let TableCount = Object.keys(Data).length;
   let EditButton   = TRANSreadOnly() ? "" : `<div class="TBLeditListWindow BUTTONdefault BUTTONstandard">EDIT</div>`;
   return /*html*/`
   <div class="TBLwindow">
      <div class="TBLeditBanner">
         <div class="TBLtitle">Database Tables (${TableCount})</div>
         <div class="TBLfileName">(${FileName})</div>
         <div class="TBLexport BUTTONaction BUTTONstandard">EXPORT</div>
         ${EditButton}
      </div>   
      <div class="TBLcontent">
         ${TBLtableListHtml(Data)}
      </div>
   </div>`;
}

function TBLtableListHtml(Data){
   let MaxName = TBLdetermineMaxNameLength(Data);
   let Html = ``;
   for(TableName in Data) Html += TBLtableRow(TableName, Data, MaxName); 
   return Html;
}

function TBLtableRow(TableName, Data, Max){
   let SpacePadding = "&nbsp;";
   let SpacesNeeded = Max - TableName.length;
   if(SpacesNeeded > 0 ) SpacePadding += "&nbsp;".repeat(SpacesNeeded);
   return /*html*/`
   <div class="TBLlistRow">
      <div class="TBLnodeValue TBLtable" value="${ESChtmlEscape(TableName)}">
         ${ESChtmlEscape(TableName)}${SpacePadding}${Data[TableName].desc ? `(${ESChtmlEscape(Data[TableName].desc)})` : "" }
      </div>
      ${Data[TableName].columns && Data[TableName].columns.length !== 0 ? "" :  /*html*/`<div class="TBLwarningMessage" title="No columns set"><img src="TBL/images/TBLwarning.svg" alt="⚠"></div>`}
   </div>`; 
}

function TBLdetermineMaxNameLength(Data){
   let Max = 0;
   for(TableName in Data) if(Max < TableName.length && TableName.length < 11 ) Max = TableName.length;
   return Max;
}