let LOGupdateFields;
let LOGreceivedData;
let LOGcomponentRendered; 
let LOGparamsPast;

function LOGcomponentIsValid(Component, Data){
   let List = Data?.component_data || [];
   if(Component == "") return true;
   for(i of List){
      if( i.guid == Component ) return true;
   }
   return false;
}

function LOGverifyComponent(){
   LOGgetComponentList(function(R){
      LOGcheckComponent(R);
   });
}

function LOGgetComponentList(Callback){
   APIcall("component/list", {}, function(R){
      if(!R.success) return;
      if(Callback) Callback(R.data);
   });
}

function LOGcheckComponent(Data){
   let Component = LOGcomponentId();
   let Valid = LOGcomponentIsValid(Component, Data);
   if(!Valid){
      let Selected = "";
      window.location.hash = LOGformatUrl(Selected)
      STORElocalCreate("PRJlastSelectedComponent", Selected);
      return;
   }
   STORElocalCreate("PRJlastSelectedComponent", Component);
   LOGfinishRenderingScreen();
}

function LOGcheckParameters(){
   let Args = LOGsimpleArgs();
   let To = Args.to;
   if(To) To = parseInt(To);
   let From = Args.from;
   if(From) From = parseInt(From);
   if(!To || !From) return;
   if(To < From){
      Args.to = String(From);
      Args.from = String(To);
   }
   window.location.hash = "#logs" + PAGEformatParams(Args);
}

function LOGcheckTimeParameter(){
   let Time =  PAGEhashParameter("time");
   if (!Time) return;
   let Start = PAGEhashParameter("to");
   let End   = PAGEhashParameter("from");
   let Upper = Math.max(Start, End);
   let Lower = Math.min(Start, End);
   if (Start && Time > Upper) return PAGEsetHashParameter("time", "");
   if (End   && Time < Lower) return PAGEsetHashParameter("time", "");
}

function LOGscreenInit(){
   CORpreparePage();
   CORheader();
   CORsetNavIcon("CORlogIcon");
   if(!LOGhtmlRendered()){
      document.querySelector('.CORmain').innerHTML = LOGscreenHtml();
      LOGsearchCriteria = "";
      LOGcomponentRendered = undefined;
      LOGsetListeners();
   }
   LOGhandleShowList();
}

function LOGhandleShowList(){
   let Logs = document.querySelectorAll('.LOGlogEntry');
   if(Logs.length != LOGshowListData.length) LOGshowListData = [];
}

function LOGfinishRenderingScreen(){
   LOGcheckParameters();
   LOGcheckTimeParameter();
   let Init = LOGhtmlRendered();
   PAGEresetPage();
   LOGscreenInit();
   if(Init){
      LOGupdateFilterCheckbox();
      LOGupdateWholeWordCheckbox();
      LOGsetStartTime();
      LOGsetEndTime();
      LOGsetTopBanner();
      LOGsetWarningBanner();
   }
   LOGupdateInputFields();
   LOGinputControl();
   LOGnewComponentCheck();
   LOGmodelInit();
   LOGvisibility();
   if(LOGalreadyRendered()) {
      LOGinitUpdateLatestModel();
      return;
   }
   LOGresetGlobals();
   LOGinitUpdateLatestModel();
   LOGupdateBottomBanner("Oldest results");
   LOGscrollbarModelInit();
   LOGinit(LOGbeginSearchIfNeeded);
   LOGparamsPast = PAGEhashParameters();
}

function LOGnewComponentCheck(){
   if(LOGcomponentRendered !== LOGcomponentId()) LOGresetBulk();
   LOGcomponentRendered = LOGcomponentId();
}

function LOGscreen(){
   LOGverifyComponent();
}

function LOGvisibility(){
   LOGswitchVisibility(".LOGswitchTarget", !LOGisServer());
}

function LOGresetGlobals(){
   LOGcomponentListRendered = undefined;
   LOGshowListData = [];
   LOGreceivedData = false;
   LOGupdateInProgress = false;
   // LOGsearchResultsData = [];
   // LOGsearchIndex = -1;
   LOGscrollbarLineSelectedPosition = undefined;
   LOGstartDateTimeInFocus = false;
   LOGendDateTimeInFocus = false;
   LOGstartOfLogs = false;
   LOGendOfLogs = false;
   clearTimeout(LOGscrollTimerUp);
   clearTimeout(LOGscrollTimerDown);
   clearTimeout(LOGflagResetTimer);
   clearInterval(LOGupdateTimer);
   clearTimeout(LOGupdateTimer);
}

function LOGhtmlRendered(){ return document.querySelector('.LOGbody'); }

function LOGcomponentExists(){
   let ComponentId = LOGcomponentId();
   if(!ComponentId) return true;
   let ComponentList = LOGcomponentListData();
   if(ComponentList === undefined) { return true; }
   for(let i=0; i < ComponentList.length; i++){
      if(ComponentList[i].guid == ComponentId) return true;
   }
   return false;
}

function LOGcomponentExistsUpdate(){
   if(LOGcomponentExists()) return;
   window.location.hash = "#logs"
}

function LOGformatUrl(component) {
   return "#logs" + PAGEformatParams({
      search     : PAGEhashParameter("search"),
      tags       : PAGEhashParameter("tags"),
      filter     : PAGEhashParameter("filter"),
      exact      : PAGEhashParameter("exact"),
      component  : component
   });
}