let NWCupdateModelTimer; 

function NWClisteners(){
   let card    = document.querySelector('.NWCmodal');
   card.addEventListener('click',    function(e){ NWCsingleClickListeners(e); });
   card.addEventListener('dblclick', function(e){ NWCdoubleClickListeners(e); });
   card.addEventListener('keyup',    function(e){ NWCkeyupListeners(e); });
   card.addEventListener('change',   function(e){ NWCchangeListeners(e); });
   FORMenterListener(document.querySelector(".NWClistContainer"), NWCcreateRequest);
}

function NWCsingleClickListeners(e){
   if(e.target.closest('.NWCcancel'))                 DASHcloseModal();
   if(e.target.closest('.NWCremoveComponent'))        NWCremoveComponent(e.target);
   if(e.target.closest('.NWCrefreshCache'))           NWCrefreshCache();
   if(e.target.closest('.NWCconnect'))                NWCconnectListRequest();
   if(e.target.closest('.NWCadd'))                    NWCcreateRequest();
   if(e.target.closest('.NWCcached'))                 NWCtemplateSelected(e.target);
   // if(e.target.closest('.NWCconnectComponentsCheck')) NWCconnectComponentsCheckBox();
   // if(e.target.closest('.NWCkeepGitTrackingCheck'))   NWCplainFolderCheckBox();
   if(e.target.closest('.NWCblankCard'))              NWCaddComponentBlankRow();
   if(e.target.closest('.NWCcollectionCard'))         NWCcardClicked(e.target);
}

function NWCdoubleClickListeners(e){
   if(e.target.closest('.NWCnoCache'))         NWCnoCachePopoutClicked(e.target);
}

function NWCkeyupListeners(e){
   if(e.target.closest('.NWCfilter') || e.target.closest('.NWCname') || e.target.closest('.NWCrepoLinkInput')) { 
      clearTimeout(NWCupdateModelTimer);
      NWCupdateModelTimer = setTimeout(function() { MODELforcePoll('NWCmodel'); }, 100);
   }
   if(e.target.closest('.NWCname')){
      NWCnameInput = true;
      if(e.target.closest('.NWCname').value === "") NWCnameInput = false;
   }
   if(e.target.closest('.NWCdescription')){
      NWCdescInput = true;
      if(e.target.closest('.NWCdescription').value === "") NWCdescInput = false;
   }
}

function NWCchangeListeners(e) {
   if(e.target.closest('.NWCcollectionsSelect')) { return NWCsaveCollectionSelected(); }
}

function NWCsaveCollectionSelected() {
   let Div = document.querySelector('.NWCcollectionsSelect');
   if(!Div) return;
   let Value = Div.value;
   STORElocalCreate("NWCselectedCollection", Value);
   MODELforcePoll("NWCmodel");
}

function NWCtemplateSelected(target){
   let SelectedCard = target.closest('.NWCtemplateCard');
   let Cards = Array.from(document.querySelector('.NWCcardContent').children).slice(1);
   let Index = Cards.indexOf(SelectedCard);
   let Templates = NWCtemplates();
   let Template = Templates[Index];
   if(!Template) return;
   let Link = Template.git;
   let Urls = Template.urls;
   let NewName = Template.name;
   if(NWCnameExistsBool(NewName, NWCnameData())) NewName = NWCnextAvailableName(NewName, NWCnameData());
   let ApiData = {};
   ApiData.name = NewName;
   ApiData.description = Template.description;
   ApiData.git = Link;
   ApiData.urls = Urls;
   ApiData.branch = Template.branch;
   NWCaddComponentRow(ApiData);
   MODELforcePoll('NWCmodel');
}

function NWCnextAvailableName(Name, Data){
   let int = 1;
   while(NWCnameExistsBool(`${Name} (${int})`, Data) && int < 20){
      int++;
   }
   return Name + ` (${int})`;
}

function NWCnameData(){
   return MODELdata?.NWCcomponentNamesData?.data;
}

function NWCnameExistsBool(Name, Data){
   let ComponentNames = NWCcomponentNames().length ? NWCcomponentNames() : EDCcollectionNames();
   let ComponentNamesData = Data || {};
   if(ComponentNamesData[Name] == true){
      return true;
   }
   for(let i=0; i<ComponentNames.length; i++){
      if(ComponentNames[i] == Name) return true;
   }
   return false;
}

function NWCcreateFinished(R){
   COL_TRC(R);
   if(!R.success) {
      if (!LICcheckComponentCreateError(R)) { return; } // This function will take care of creating the error snackbar
      else                                  { return SNCKsnackbar('error', R.error); }
   }
   let ComponentsData = R.data?.components || [];
   let Components = DASHgetComponentList(ComponentsData);
   GINaddAndCommit({message: GINcommitMessage("Added", ComponentsData), components: Components, all : true});
   if (location.hash !== '#dashboard'){
      COL_ERR("User has navigated away from the dashboard.");
      return;
   }
   let Warning  = R.data?.warning || "";
   NWCselectComponents(Components);
   DASHenableCheckOnlyRows();
   let Filter = PAGEhashParameter("filter");
   let Args = "";
   if(Components.length == 1){
      let Guid = Components[0];
      if(Guid)   Args = "/details?component=" + Guid;
      if(Guid && Filter) Args = Args + "&filter=" + Filter;
   }
   else{
      if(Filter) Args = "?filter=" + Filter;
   }
   if(Warning)  SNCKsnackbar("error",   Warning);
   location.hash = 'dashboard' + Args;
   MODELforcePoll("DASHmodel");
}

function NWCcreateRequest(){
   let Button = document.querySelector('.NWCadd');
   if(!Button) return;
   if(Button.classList.contains('BUTTONdisabled')) return; 
   let ApiData = {};
   ApiData.components = NWCgetComponentListData();
   ApiData.connected  = document.querySelector('.NWCconnectComponentsCheck')?.checked;
   ApiData.plain      = document.querySelector('.NWCkeepGitTrackingCheck')?.checked;
   if(PAGEhashParameter("source")) ApiData.source = PAGEhashParameter("source");
   COL_TRC("Shutdown all the models so we minimize GIT contention");
   location.hash = 'dashboard';
   APIcall('component/create', ApiData, function(R){ NWCcreateFinished(R); });
}

function NWCselectComponents(Response){
   DASHcheckedComponents = {};
   for(Component of Response) {
      DASHcheckedComponents[Component.guid] = true;
   }
}

function NWCrefreshCache(){
   let Args = {};
   Args.force = true;
   Args.clear = true;
   APIcall('git/cache/refresh', Args, function(R){});
   let Info = document.querySelector('.NWCrefreshInfo');
   Info.innerHTML = " - Cache refresh started. [<a class='NWCrefreshInfoLink' href='#logs?tags=%23git'>More details</a>]";
}

function NWCupdateSelectedOptions(){
   let ConnectComponent = document.querySelector('.NWCconnectComponentsCheck');
   if(ConnectComponent) ConnectComponent.checked = STORElocalRead("NWCcheckedConnectComponent") === 'true';
   let PlainFolder = document.querySelector('.NWCkeepGitTrackingCheck');
   if(PlainFolder) PlainFolder.checked = STORElocalRead("NWCcheckedPlainFolder") === 'true';
}
