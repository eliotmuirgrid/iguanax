let DETranOnceFlag = false;

function DETrowIndex(Class){
   let Div  = document.querySelector(Class);
   if(!Div) return 0;
   const parentElement = Div.parentNode;
   const Rows = Array.from(parentElement.querySelectorAll('.DETmessageRow'));
   const Index = Rows.indexOf(Div);
   return Index;
}

function DETtopIndex()   { return DETrowIndex(".DETtopVisible"); }

function DETbottomIndex(){ return DETrowIndex(".DETbottomVisible"); }

function DETstringViewArg(){
   let Div = document.querySelector(".DETstringView");
   if(!Div) return;
   return Div.value;
}

function DETmessageModelArgs() {
   let Data         = DETdetailArgs();
   Data.chunk       = DETtopIndex();
   Data.chunk_extra = DETbottomIndex();
   Data.view_mode   = DETstringViewArg();
   return Data;
}

function DETrawDataArgs() {
   let Data = DETdetailArgs();
   Data.raw = true;
   return Data;
}

function DETsetViewMode(){
   let Div = document.querySelector(".DETstringView");
   if(!Div) return;
   let SelectedValue = STORElocalRead("DETselectedViewMode");
   if(!SelectedValue) return;
   if(Div.value != SelectedValue) Div.value = SelectedValue;
}

function DETrunOnce(){
   return !DETranOnceFlag;
}

function DETmessageModelInit(){
   let Content = document.querySelector(".DETdetailContentDiv");
   Content.innerHTML = "";
   DETranOnceFlag = false;
   DETsetViewMode();
   MODELpollerAdd("DETmessageModel", "DETrawData",        4, "log/detail",           DETrawDataArgs);
   MODELpollerAdd("DETmessageModel", "DETmessageData",    4, "log/detail",           DETmessageModelArgs);
   MODELpollerAdd("DETmessageModel", "DETpermissionData", 4, "component/permission", DETdetailArgs, DETrunOnce);
   MODELpollerAdd("DETmessageModel", "DETpermissionAll",  4, "component/permission/all", {}, DETrunOnce);
   MODELviewAdd  ("DETmessageModel", "DETrenderMessageSkeleton");
   MODELviewAdd  ("DETmessageModel", "DETrenderMessage");
   MODELviewAdd  ("DETmessageModel", "DETrenderNoPermission");
   // MODELviewAdd  ("DETmessageModel", "DETshutdownMessage");
   MODELviewAdd  ('DETmessageModel', 'DETupdateTags');
   MODELstart    ("DETmessageModel");
   MODELforcePoll("DETmessageModel", function(){
      DETrenderMessageSkeleton();
      DETranOnceFlag = true;
      if(DETviewLogPermission()) DETinitializeFields();
      DETsetRenderListeners();
      DETselectBottom();
   });
}

function DETpermissionData(){
   let data = MODELdata?.DETpermissionData?.data || {};
   return data;
}

function DETpermissionAll(){
   return MODELdata?.DETpermissionAll?.data?.components || {};
}

function DETviewLogPermission(){
   let Data = DETpermissionAll();
   let Guid = DETcomponentId();
   let Exists = Data[Guid];
   if(!Exists) return true;
   return DETpermissionData()?.view_logs;
}

function DETrenderNoPermission(){
   let Perm = DETviewLogPermission();
   if(Perm) return;
   let div = document.querySelector(".DETdetailContentDiv");
   if(!div) return;
   div.innerHTML = "You do not have permission to view this."
}

function DETinitializeFields(){
   let Data = DETmessageData();
   let Type = DETmessageData().type;
   DETresubmitUpdate(Type);
   DETsetMessageSize(parseInt(Data.size));
   DETsetMessageLocation(Data.file);
   DETupdateSampleDataExport();
   DETupdateRelatedMessages(Data);
   DETupdateDownloadButton();
   let typeHtml = LOGtypeButton(Type);
   let I = document.querySelector('.LOGdetailTypeIcon');  // Assuming the class is there - explicit errors okay.
   I.innerHTML = typeHtml;

   // I.src = "LOG/types/" + Data.type + ".svg";
   // I.alt = Data.type;
   document.querySelector('.DETdateTime').innerHTML = LOGyearMonthDateTime({
      epoch: Data.time,
      includeTime: true,
      simpleTime: false,
      shortDayLabel: false
   });
}

function DETrawData(){
   return MODELdata?.DETrawData?.data || {};
}

function DETmessageData(){
   return MODELdata?.DETmessageData?.data || {};
}

function DETmessageNumberOfChunks(){
   let Data = DETmessageData();
   return Data?.TotalChunks; 
}

function DETchunkData(){
   let Data = DETmessageData()?.Chunks;
   if(!Data) return "";
   return Data[0]; 
}

function DETchunkDataExtra(){
   let Data = DETmessageData()?.Chunks;
   if(!Data) return "";
   return Data[1]; 
}

function DETlastLineCount(){
   let Data = DETmessageData();
   return Data?.LinesInLastChunk;
}

function DETmessageRowHtml(LastRow){
   let LineCount = LastRow ? DETlastLineCount() + 1 : ITM_CHUNK_SIZE;
   const ChunkHeight = 16 * LineCount;
   return /*html*/`<div class="DETmessageRow DETmessageLoading" style="height:${ChunkHeight}px;"></div>`;
}

function DETskeletonHtml(){
   let NumberOfChunks = DETmessageNumberOfChunks();
   let Html = /*html*/`<div class="DETmessageDiv">`;
   for(let i=0 ; i < NumberOfChunks; i++) Html += DETmessageRowHtml( i == NumberOfChunks - 1);
   return Html + "</div>";
}

function DETrenderMessageSkeleton(){
   let Div = document.querySelector(".DETdetailContentDiv");
   if(!Div) return;
   let Skeleton = document.querySelector(".DETmessageDiv");
   if(Skeleton) return;
   let Html = DETskeletonHtml();
   Div.innerHTML = Html;
   DETapplyScroll();
}

function DETupdateRow(RowNumber, Content, overRender = false){
   let Rows = document.querySelectorAll(".DETmessageRow");
   if(!Rows || !Rows.length) return;
   let Row = Rows[RowNumber];
   if(!Row) return;
   let Loading = Row.classList.contains("DETmessageLoading");
   if(!Loading && !overRender) return;
   if(Row.innerHTML !== Content) Row.innerHTML = Content;
   Row.classList.remove("DETmessageLoading");
}

function DETrenderMessage(){
   let Div = document.querySelector(".DETdetailContentDiv");
   if(!Div) return;
   if(DETsegmentSelected()) return DETannotationRequest();
   let Data = DETchunkData();
   DETupdateRow(Data?.ChunkNumber, Data?.Value);
   Data = DETchunkDataExtra();
   DETupdateRow(Data?.ChunkNumber, Data?.Value);
}

function DETsegmentSelected(){
   return DETstringViewArg() === 'string_seg';
}
