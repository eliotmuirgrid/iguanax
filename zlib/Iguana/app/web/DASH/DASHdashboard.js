const DASH_LIGHTS = Object.freeze({'OFF': 'DASHswitchOff', 'ON': 'DASHswitchOn', 'YELLOW': 'DASHswitchInProg', 'RED': 'DASHswitchErr'});
let DASHpreviousFilter = {};
let DASHrunningComponents = {};
let DASHlastOnStatus = {};
let DASHshowOnlyChecked = false;

function DASHcheckMapErrors() {
   APIcall('component/check_invalid', {}, function(R) {
      if(!R.success) return SNCKsnackbar('error', "Unable to verify whether there were errors loading any of the components: " + R.error);
      if(R.data.invalid === true) SNCKsnackbar('error', 'One or more components could be not be loaded, check the system <a href="#logs">logs</a> for more information');
   });
}

function DASHdashboardScreen(ResetPage = true) {
   DASHdashboardScreenImp(ResetPage);  // Call this to make it traceable because DASHdashboardScreen is called as an anonymous function
}

function DASHdashboardScreenImp(ResetPage){
   COL_TRC("Setting up dashboard");
   COL_TRC("Reset...");
   if(ResetPage) PAGEresetPage();
   DASHmodelInit();
   clearTimeout(DASHsearchTimeout);
   DASHsaveArgs();
   DASHupdate();
   if(document.querySelector('.DASHdashboard')) {
      COL_TRC("Dashboard scaffold already set up.");
      COMmodelInit();
      return;
   }
   CORpreparePage();
   CORheader();
   CORsetNavIcon('CORdashboardIcon');
   COL_TRC("Set up header");
   document.querySelector('.CORmain').innerHTML = DASHlayout(DASHshowOnlyChecked);
   COMrender();
   COMmodelInit();
   DASHresetGlobalVariables();
   DASHsetListenersInit();
}

function DASHshowOnlyCheckedRows(){
   let checkedBoxes = document.querySelector('.DASHshowOnlyCheckedSwitchBox');
   if(!checkedBoxes) { return; }
   let visible = checkedBoxes.style.display !== 'none';
   return visible && document.querySelector('.DASHonlyChecked').checked;
}

function DASHisComponentChecked(ComponentId) { return DASHcheckedComponents[ComponentId]; }

function DASHisValidHttpUrl(string) {
   let regexp = /(ftp|http|https):\/\/(\w+:{0,1}\w*@)?(\S+)(:[0-9]+)?(\/|\/([\w#!:.?+=&%@!\-\/]))?/;
   return regexp.test(string);
}

function DASHresetGlobalVariables() {
   DASHrenderedComponentData = {};
   DASHrenderedRows          = ["DASHtableHeader"];
   DASHrenderedColumns       = undefined;
}

function DASHisComponentRunning(ComponentId){
   let ComponentData = DASHrunningComponentsData();
   if(!ComponentData) return;
   return ComponentData[ComponentId];
}

function DASHsaveArgs(){
   let Args = PAGEhashParameters();
   let Data = {};
   Data.filter     = Args?.filter     || "";
   Data.sort       = Args?.sort       || "";
   Data.order      = Args?.order      || "";
   Data.whole_word = Args?.whole_word || "";
   DASHpreviousFilter = Data;
}

function DASHformatUrl(Component){
   let Url = !Component ? "#dashboard" : "#dashboard/details";
   DASHpreviousFilter.component = Component ? Component : "";
   return Url + PAGEformatParams(DASHpreviousFilter);
}

function DASHcloseModal(){
   window.location.hash = DASHformatUrl();
}

function DASHopenComponent(Guid){
   DASHsaveArgs();
   window.location.hash = DASHformatUrl(Guid);
}