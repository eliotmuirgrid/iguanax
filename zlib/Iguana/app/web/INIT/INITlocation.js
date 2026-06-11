function INITlocation(){
   MODELshutdownAll();
   INITframe(INITcopyExeHtml());
   INITsetBackBtn("git");
   INITenableNext();
   INITdefaultLocation(INITlocationModel);
}

function INITlocationListeners(){
   INITcopyExeBrowserListener();
   INITworkingDirBrowserListener();
   INITinputListener();
   INITlocationNextListener();
}

function INITinputListener(){
   let Div = document.querySelector(".INITexeLocationDir");
   if(!Div) return;
   Div.addEventListener("change", function(){ MODELforcePoll("INITlocationModel");} );
}

function INITlocationNextListener(){
   let Div = document.querySelector(".INITnextBtn ");
   if(!Div) return;
   Div.addEventListener("click", function(){
      INITcopyExecutable();
   } );
}

function INITlocationModel(){
   COL_TRC("Setting up INITlocationModel model");
   MODELpollerAdd("INITlocationModel", "INITexists",      2,    "install/exists", INITdirectoryValue);
   MODELviewAdd("INITlocationModel",   "INITcheckForIguana");
   MODELviewAdd("INITlocationModel",   "INITstorePath");
   MODELforcePoll('INITlocationModel', function(){
      INITlocationListeners();
   });
   MODELstart("INITlocationModel");
}

let INIT_INSTALL_PATH = null;

function INITstorePath(){
   INIT_INSTALL_PATH = INITpath();
   COL_TRC(INIT_INSTALL_PATH);
}

function INITcheckForIguana(){
   let Exists = INITpathExists();
   let Path   = INITpath();
   let P = document.querySelector(".INITexePathValue");
   let W = document.querySelector(".INITlocationWarning");
   if( !P || !W ) return;
   if(P.title !== Path)  P.title = Path;
   if(P.innerHTML !== Path) {
      P.innerHTML = Path;
      INITupdateWorkingDir(Path);
   }

   if( Exists &&  W.classList.contains("INIThidden")) W.classList.remove("INIThidden");
   if(!Exists && !W.classList.contains("INIThidden")) W.classList.add   ("INIThidden");
}

function INITpathExists(){ return MODELdata?.INITexists?.data?.exists; }
function INITpath()      { return MODELdata?.INITexists?.data?.path || ""; }

function INITdirectoryValue(){
   let Div = document.querySelector(".INITexeLocationDir");
   if(!Div) return;
   return { "dir" : Div.value.trim() };
}

function INITupdateWorkingDir(ExePath){
   let Div = document.querySelector(".INITworkingDir");
   if(!Div) { return; }
   Div.value = "Loading...";
   APIcall("install/working_dir", {install_path : ExePath}, function(R){
      let Default = R?.data || "";
      Div.value = Default;
   });
}

function INITdefaultLocation(Callback){
   APIcall("install/default_path", {}, function(R){
      let Div = document.querySelector(".INITexeLocationDir");
      if(!Div) {
         if(Callback) Callback();
         return;
      }
      let Default = R?.data || "";
      if(!Default.includes("%ProgramFiles%")) Div.value = Default;
      if(Callback) Callback();
   });
}

function INITcopyExecutable(){
   INIThideStatusDivs();
   let ExeDiv  = document.querySelector('.INITexeLocationDir');
   let DataDiv = document.querySelector('.INITworkingDir');
   if(!ExeDiv || !DataDiv) return;
   const app_location  = ExeDiv.value.trim();
   const data_location = DataDiv.value.trim();
   INITcopyRequestUi();
   APIcall('install/copy', {app_dir: app_location, data_dir: data_location}, INIThandleCopyResponse);
}

function INITcopyRequestUi(){
   INITdisableNext();
   INITinstallingMessage(true);
}

function INITinstallingMessage(show){
   let D = document.querySelector(".INITattemptingInstall");
   if(!D) return;
   if( show &&  D.classList.contains("INIThidden")) D.classList.remove("INIThidden");
   if(!show && !D.classList.contains("INIThidden")) D.classList.add   ("INIThidden");
}

function INIThandleCopyResponse(R) {
   INITenableNext();
   INITinstallingMessage(false);
   if(!R.success) { return INITshowError(R.error); }
   window.location.hash = "config";
}

function INITcopyExeBrowserListener() {
   let Browse = document.querySelector('.INITexeBrowse');
   if(!Browse) { return; }
   Browse.addEventListener('click', INITopenBrowser);
}

function INITworkingDirBrowserListener() {
   let Browse = document.querySelector('.INITworkingDirBrowse');
   if(!Browse) { return; }
   Browse.addEventListener('click', INITopenWorkingDirBrowser);
}

function INITopenBrowser() {
   let Location = document.querySelector('.INITexeLocationDir');
   COL_TRC(Location.value);
   FILbrowser({
      value      : Location.value,
      dir_only   : true,
      new_folder : true,
      show_hidden: true,
      callback   : function(T) { 
         Location.value = T;
         MODELforcePoll("INITlocationModel");
      }
   });
}

function INITopenWorkingDirBrowser() {
   let Location = document.querySelector('.INITworkingDir');
   COL_TRC(Location.value);
   FILbrowser({
      value      : Location.value,
      dir_only   : true,
      new_folder : true,
      show_hidden: true,
      callback   : function(T) { 
         Location.value = T;
      }
   });
}

function INITcopyExeHtml() {
   return /*html*/`
   ${INITsidebarHtml()}
   <div class="INITmodalMainArea">
      <h2 class="INITmodalHeading">
         Select Location
      </h2>
      <p class="FORMlabel">Specify directory to install under:</p>
      <div class="INITinstallLocation FORMinputGroup">
         <input class="FORMinput INITexeLocationDir" disabled>
         <div class="INITexeBrowse BUTTONstandard BUTTONcancel">BROWSE...</div>
      </div>
      <p class="FORMlabel">IguanaX data directory:</p>
      <div class="INITinstallLocation FORMinputGroup">
         <input class="FORMinput INITworkingDir" disabled>
         <div class="INITworkingDirBrowse BUTTONstandard BUTTONcancel">BROWSE...</div>
      </div>
      <br>
      <div>Installing to:</div>
      <div class="INTexeAction">
         <div class="INITexePathValue" ></div>
      </div>
      <br>
      <div class="INITlocationWarning">
         <div class="INITwarningLabel">Warning:</div>
         <div class="INITwarningText">Choosing this location will <b>overwrite an existing file</b> when clicking "Next".</div>
      </div>
      <div class="INIThidden INITattemptingInstall">
         <div class="INITinstall-text">Attempting to install ...</div>
         <div class="INITspinner"></div>
      </div>
      ${INITcreateBtnGroupAndStatusDivs()}
   </div>`;
}
