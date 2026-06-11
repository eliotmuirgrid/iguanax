function TBLnodeHtml(NodeName, FileName){
   let FileData = TBLfileData();
   let Columns = 0;
   if(FileData[FileName]["tables"][NodeName].columns) Columns = FileData[FileName]["tables"][NodeName].columns.length;
   let EditButton = TRANSreadOnly() ? "" : `<div class="TBLeditNodeWindow BUTTONdefault BUTTONstandard">EDIT</div>`;
   return /*html*/`
   <div class="TBLwindow">
      <div class="TBLeditBanner">
         <div class="TBLtitle TBLtable">${ESChtmlEscape(NodeName)}</div>
         <div class="TBLfileName">${ESChtmlEscape(FileData[FileName]["tables"][NodeName].desc)}</div>
         ${EditButton}
      </div>   
      <br>
      <div class="TBLnodeWindow">${TBLcolumnsHtml(NodeName, FileName)}</div>
   </div>`;
}

function TBLcolumnsHtml(NodeName, FileName){
   let FileData = TBLfileData();
   let Html = /*html*/`
   <div class="TBLnodeTable">
      <div class="TBLnodeHeader FORMlabel">NAME</div>
      <div class="TBLnodeHeader FORMlabel">DESCRIPTION</div>
      <div class="TBLnodeHeader FORMlabel">TYPE</div>
      <div class="TBLnodeHeader TBLboolValue FORMlabel">KEY</div>`;
   for(ColIdx in FileData[FileName]["tables"][NodeName].columns) Html += TBLcolumnHtml(FileData[FileName]["tables"][NodeName].columns[ColIdx]);
   Html += "</div>";
   if(!FileData[FileName]["tables"][NodeName].columns || FileData[FileName]["tables"][NodeName].columns.length === 0) Html += TBLnoColumnsHtml();
   return Html;
}

function TBLnoColumnsHtml(){
   return /*html*/`<div class="TBLcolumnBlank">
      <div class="TBLblankRow">No columns are currently configured</div>
   </div>`;
}

function TBLcolumnHtml(ColumnData){
   let Name        = ColumnData.name ? ColumnData.name : "";
   let Description = ColumnData.desc ? ColumnData.desc : "";
   let Type        = ColumnData.type ? ColumnData.type : "";
   if(Type) Type = Type.charAt(0).toUpperCase() + Type.slice(1);
   let Key         = ColumnData.key === 1;
   return /*html*/`
   <div class="TBLvalue">${ESChtmlEscape(Name)}</div>
   <div class="TBLvalueEnd">${ESChtmlEscape(Description)}</div>
   <div class="TBLvalue">${Type}</div>
   <div class="TBLvalue TBLboolValue"><input type="checkbox" disabled ${Key ? "checked" : ""}></div>`;
}