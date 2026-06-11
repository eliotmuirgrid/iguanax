function STClisteners(){
  let div = document.querySelector(".SETmainContent");
  div.addEventListener   ("click", function(e){ SETcollectionActions(e); } );
}

function SETcollectionActions(e){
   COL_TRC(e);
   e.stopPropagation();
   if(e.target.closest('.SETrefreshCache'))           SETrefreshCacheClicked();
   if(e.target.closest('.SETcreateAndAddCollection')) SETaddNewCollectionClicked();
   if(e.target.closest('.SETeditCollection'))         SETeditCollectionClicked(e.target);
   if(e.target.closest('.SETeditCatalogue'))          SETcollectionRenderEdit(e.target);
   if(e.target.closest('.SETsaveCatalogue'))          SETsaveCatalogue();
   if(e.target.closest('.SETaddRow'))                 SETcollectionAddRow();
   if(e.target.closest('.SETdeleteRow'))              SETcollectionDeleteRow(e);
   if(e.target.closest('.SETcancelCatalogue'))        SETexitEditMode();
   if(e.target.closest('.SETresetToDefaultCollectionsLink')) STCresetToDefault();
}

function SETeditCollectionClicked(target){
   let CollectionData = MODELdata.SETcollection.data;
   let SelectedCard = target.closest('.SETcatalogueRow');
   let Index = Array.from(document.querySelector('.SETcatalogueTable').children).indexOf(SelectedCard) - 1;
   let GitUrl = CollectionData[Index].git;
   EDCinit(GitUrl);
}

function SETrefreshCacheClicked(){
   let ApiData = {};
   ApiData.force = true;
   ApiData.clear = true;
   APIcall("git/cache/refresh", ApiData, function(Response){ 
      // Don't care about the response here, we just want to trigger a poll in the backend
      MODELforcePoll("STCmodel");
   });
   let Info = document.querySelector('.SETrefreshCacheInfo');
   Info.innerHTML = "- Cache refresh started. [<a class='SETrefreshCacheInfoLink' href='#logs?tags=%23git'>More details</a>]";
}

function SETaddNewCollectionClicked(){
   // boot from edit mode if in edit mode
   let EditMode = document.querySelector('.SETgitCollectionEdit');
   if(EditMode) SETexitEditMode();
   CLCrender();
}

function STCresetToDefault(){
   console.log("STCresetToDefault");
   let Data = STCdefaultData();
   let ExistingData = STCgetData();
   STCupdateEditHtml({...Data, ...ExistingData});
}

function STCdefaultData(){
   return {
      "ssh://git@bitbucket.org/interfaceware/igx-api-collection.git": {},
      "ssh://git@bitbucket.org/interfaceware/igx-business-collection.git": {},
      "ssh://git@bitbucket.org/interfaceware/igx-core-collection.git": {},
      "ssh://git@bitbucket.org/interfaceware/igx-data-transformation.git": {},
      "ssh://git@bitbucket.org/interfaceware/igx-data-validation-collection.git": {},
      "ssh://git@bitbucket.org/interfaceware/igx-devops-collection.git": {},
      "ssh://git@bitbucket.org/interfaceware/igx-devtools-collection.git": {},
      "ssh://git@bitbucket.org/interfaceware/igx-emr-collection.git": {},
      "ssh://git@bitbucket.org/interfaceware/igx-notification-collection.git": {},
      "ssh://git@bitbucket.org/interfaceware/igx-orchestration-collection.git": {},
      "ssh://git@bitbucket.org/interfaceware/igx-security-collection.git": {},
      "ssh://git@bitbucket.org/interfaceware/igx-storage-collection.git": {}
   };
}

