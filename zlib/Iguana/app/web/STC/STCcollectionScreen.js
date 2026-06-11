function STCcollectionScreen(){
   SETrender();
	document.querySelector('.STCcollection').classList.add('selected');
   STClisteners();
   SETcollectionModelInit();
}

function SETexitEditMode(){
   STCeditMode = false;
   MODELforcePoll('STCmodel');
}

function STCrefreshCacheInfo(){
   let Info = document.querySelector('.SETrefreshCacheInfo');
   if(!Info) return "";
   return Info.innerHTML;
}

function SETcollectionRenderView(){
   let CollectionData = MODELdata.SETcollection.data;
   let Target = document.querySelector(".SETcontentBody");
   if(!Target || STCeditMode) return;
   if(DEEPequal(CollectionData, STCcollectionRendered)) return; 
   Target.innerHTML = SETtemplateViewHtml(STCrefreshCacheInfo());
   STCcollectionRendered = JSON.parse(JSON.stringify(CollectionData));
}

function STCprocessData(InData){
   let OutData = {};
   for(let i in InData){
      let GitUrl = InData[i]?.git.trim();
      OutData[GitUrl] = {};
   }
   return OutData;
}

function SETcollectionRenderEdit(target){
   if(target.closest(".BUTTONdisabled")) return;
   let CollectionData = STCprocessData(MODELdata.SETcollection.data);
   STCupdateEditHtml(CollectionData);
}

function STCupdateEditHtml(Data){
   STCcollectionRendered = undefined;
   STCeditMode = true;
   let div = document.querySelector(".SETcontentBody");
   div.innerHTML = SETtemplateEditHtml(Data);
   if (Data.length === 0) SETcollectionAddRow();
   SETfocusLast(".SETcatalogueEditValue");
}

function SETrefreshTemplates(Callback){
   let Args = {};
   Args.force = true;
   Args.clear = true;
   APIcall("git/cache/refresh", Args, function(Response){ 
      // Don't care about the response here, we just want to trigger a poll in the backend
      if(Callback) Callback();
      return; 
   });
}

function STCgetData(){
   let Data = {};
   let CatalogueRows = document.querySelectorAll('.SETcatalogueEditValue');
   for(let i = 0; i < CatalogueRows.length; i++){
      let GitUrl = CatalogueRows[i].value.trim();
      if(GitUrl) Data[GitUrl] = {};
   }
   return Data;
}

function SETsaveCatalogue(){
   let Data = STCgetData();
   APIcall("git/catalogue/save", { content : Data }, function(R){
      if(R.success) { 
         let Message = GINsaveCatalogue(Data);
         GINaddAndCommit({message: Message, settings: true}); 
      }
      SETrefreshTemplates(); 
      STCeditMode = false;
      MODELforcePoll("STCmodel");
   });
}

function SETcollectionAddRow(){
   COL_TRC("Adding new row");
   document.querySelector(".SETcatalogueTable").insertAdjacentHTML('beforeend', SETtemplateNewRowHtml);
   SETfocusLast(".SETcatalogueEditValue");
}

function SETcollectionDeleteRow(e){
   COL_TRC("Deleting row");
   e.target.closest(".SETcatalogueRow").remove();
}

/////// helper functions ///////
function SETtargetClassClicked(e, ClassName){
   return e.target.closest(`.${ClassName}`);
}

