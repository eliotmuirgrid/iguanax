let LBRupdateModelTimer; 

function LBRnameField(){
   return document.querySelector('.LBRname');
}

function LBRlisteners(){
   let card = document.querySelector('.LBRmodal');
   card.addEventListener('click',    function(e) { LBRclickListeners(e); });
   card.addEventListener('dblclick', function(e) { LBRdoubleClickListeners(e); });
   card.addEventListener('keyup',    function(e) { LBRkeyupListeners(e); });
   // card.addEventListener('change',   function(e){  LBRchangeListeners(e); });
   let gitcard = document.querySelector('.LBRgitInformation');
   gitcard.addEventListener('click', function(e) { LBRclickListeners(e); });
   let div = document.querySelector('.LBRfilterContainer');
   FORMenterListener(div, LBRcreateRequest);
}

function LBRclickListeners(e){
   if(e.target.closest('.LBRcancel'))       LBRclose();
   if(e.target.closest('.LBRcreate'))       LBRcreateRequest();
   if(e.target.closest('.LBRrepoLink'))     LBRopenRepoClick();
   if(e.target.closest('.LBRrefreshCache')) LBRrefreshCache();
   if(e.target.closest('.LBRcached'))       LBRtemplateSelected(e.target);
   if(e.target.closest('.LBRcollectionCard')) LBRcardClicked(e.target);
}

function LBRdoubleClickListeners(e){
   if(e.target.closest('.LBRnotCached')) LBRnoCachePopoutClicked(e.target);
}

function LBRkeyupListeners(e){
   if(e.target.closest('.LBRfilter') || e.target.closest('.LBRname') || e.target.closest('.LBRrepoLinkInput')) { 
      clearTimeout(LBRupdateModelTimer);
      LBRupdateModelTimer = setTimeout(function() { MODELforcePoll('LBRmodel'); }, 100);
   }
}

function LBRchangeListeners(e) {
   if(e.target.closest('.LBRcollectionsSelect')) { return LBRsaveCollectionSelected(); }
}

function LBRsaveCollectionSelected() {
   let Div = document.querySelector('.LBRcollectionsSelect');
   if(!Div) return;
   let Value = Div.value;
   STORElocalCreate("LBRselectedCollections", Value);
   MODELforcePoll("LBRmodel");
}

function LBRopenRepoClick(){
   let Div = document.querySelector('.LBRrepoLinkInput');
   if(!Div) return;
   if(!Div.value) return SNCKsnackbar('error', 'Link could not be resolved.');
   APIcall("git/all_urls", {url : Div.value}, function(R){
      if (R.success){
         return window.open(R?.data?.web);
      } else {
         return SNCKsnackbar('error', 'Link could not be resolved.');
      }
   });
}

function LBRtemplateSelected(target){
   let SelectedCard = target.closest('.LBRtemplateCard');
   let Index = Array.from(document.querySelector('.LBRcardContent').children).indexOf(SelectedCard);
   let Templates = LBRtemplates();
   let Template = Templates[Index];
   if(!Template) return;
   let Link = Template.git;
   document.querySelector('.LBRrepoLinkInput').value = Link;
   document.querySelector('.LBRname').value = Template.name;
   LBRupdateRepo();
   MODELforcePoll('LBRmodel');
   FORMfocus(LBRnameField());
}

function LBRoldName(UrlValue){
   let Idx = LBRselectedTemplateNumber(UrlValue);
   if(Idx == -1) return "";
   let Templates = LBRtemplates();
   return Templates[Idx]?.name || "";
}

function LBRcreateRequest(){
   MODELforcePoll('LBRmodel', function(){
      if(LBRnameError || LBRnameExists() || LBRrepoError) return;
      let UrlValue = document.querySelector('.LBRrepoLinkInput').value.trim();
      let OldName = LBRoldName(UrlValue);
      let NewName = document.querySelector('.LBRname').value.trim();
      let Plain = document.querySelector('.LBRkeepGitTrackingCheck')?.checked || false;
      if(TRANSplainFolder()) Plain = true;
      let ApiData = {};
      ApiData.component   = TRANScomponentGuid();
      ApiData.name        = NewName;
      ApiData.git         = UrlValue;
      ApiData.plain       = Plain;
      ApiData.development = TRANSdevelopment();
      LBRcreateLibraryApi(ApiData, OldName, NewName);
   });
}

function LBRcreateLibraryApi(ApiData, OldName, NewName){
   APIcall("component/library/add", ApiData, function(R){ 
      if(!R.success) return SNCKsnackbar('error', "There was a problem importing library : " + R.error);
      LBRclose();
      TRANSfileCurrentReload();
      if(!ApiData.plain) LIBUrlApiLock(ApiData.git, ApiData.name);
      if(NewName == OldName || OldName == "") return;
      RNMrender(NewName, OldName);
   });
}

function LBRrefreshCache(){
   let ApiData = {};
   ApiData.force = true;
   ApiData.clear = true;
   APIcall('git/cache/refresh', ApiData, function(R){
      MODELforcePoll('LBRmodel');
   });
   let Info = document.querySelector('.LBRrefreshCacheInfo');
   Info.innerHTML = " - Cache refresh started. [<a class='LBRrefreshCacheInfoLink' href='#logs?tags=%23git'>More details</a>]";
}
