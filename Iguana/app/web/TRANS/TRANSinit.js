let TRANSsourceDirectory  = '';
let TRANSworkingDirectory = '';
let TRANSprojectRootPrior = '';
let TRANSrenderedFileName = '';
let TRANSfileValid;
let TRANSemptyGuid = false;

function TRANSonFileLoad(R){
   COL_TRC(R);
   TRANSloadCode(R.data.Code, TRANSrenderedFileName);
   TRANSsetCode();
   TRANSforceUpdateModels();
}

function TRASinitialAnnotationCall(){
   TRANSstartModels();
   TRANSupdateAnnotations({FirstRequest : true});
   // TRANSupdateAnnotations({FirstRequest : true}, TRANSstartModels);
}

function TRANSsetDirectoriesCallback(InitializePage){
   COL_TRC(InitializePage);
   let Pollers = MODELgetPollers("TRANSmodel")?.length;
   if(TRANSnewSourceDirectory() || Pollers == 0 || InitializePage){
      TRANSemptyGuid = false;
      COL_TRC("Need to restart the models.");
      TRANSideRequestCompile();
      WNDcloseAllWindowsAndDocks(true);
      TRANSresetGlobals(InitializePage);
      SRCHreset();
      TRANSfileLoad(function(Response){
         TRANScreateEditor(Response, function(){ 
            TRANSsetCode();
            TRANSmodelInit(TRASinitialAnnotationCall);
            TRANSattachBrowserEvents();
         });
      });
   } 
   else {
      COL_TRC("Whoopee we can reuse our existing models.");
      TRANSfileSave(TRANSrenderedFileName, TRANSeditor.getValue(), function(){
         TRANSfileLoad(function(R){
            TRANSonFileLoad(R);
         });
     });
   }  
}

function TRANSverifyComponent(){
   LOGgetComponentList(function(R){
      TRANScheckComponent(R);
   });
}

function TRANScheckComponent(Data){
   let Component = TRANScomponentGuid();
   let Valid = LOGcomponentIsValid(Component, Data);
   if(!Valid){
      let Selected = "";
      window.location.hash = "#translator";
      STORElocalCreate("PRJlastSelectedComponent", Selected);
      return;
   }
   STORElocalCreate("PRJlastSelectedComponent", Component);
   TRANStranslatorScreen();
}

function TRANSrender(){
   TRANSverifyComponent();
}

function TRANStranslatorScreen(){
   COL_TRC("Translator screen.");
   CORpreparePage();
   PAGEremovePopouts();
   MENUremoveTooltip();
   TRANSsaveArgs();
   let InitializePage = !document.querySelector('.CodeMirror');
   if(InitializePage) {
      TRANSemptyGuid = false;
      MENUitem = {};
      COL_TRC("Reset page");
      PAGEresetPage();
      COL_TRC("Set up translator HTML");
      CORheader();
      CORsetNavIcon('CORtranslatorIcon');
      document.querySelector('.CORmain').innerHTML = TRANStranslatorHtml();
      TRANSrenderPromotionBanner();
      let State = STORElocalRead('TRANSgitHistoryCollapsed');
      State === 'expanded' ? TRANSexpandedView() : TRANScollapsedView();
      SPLTsetupSplitter('.TRANSwindow', '.PRJresizeControl',   '.PRJprojectManager', 'TRANSratio', 230, function () { return false }, WNDupdateDockDimensions);
      SPLTsetupSplitter('.TRANSwindow', '.TRANSresizeControl', '.TRANSgitHistorySplitter', 'TRANSratio2', 150, function () { return false }, function(){
         WNDupdateDockDimensions();
         COMclearPreviews();
      });
   }
   if(!TRANScomponentGuid()){
      return TRANShandleEmptyComponentGuid();
   }
   TRANSsetDirectories(function(){
      TRANSsetDirectoriesCallback( InitializePage );
      TRANSsetAllListeners();
   });
}

function TRANSstartModels(){
   MODELstart('TRANSmodel');
   if(TRANSdevelopment()) TRANSgitFastInit(); // TODO - double check the if statement makes sense
   TRANSstatusModelInit();
   TRANSslowModelInit();
   TBLmodelInit();
   EDImodelInit();
   HLPdirGlobalInit();
   TCMmodelInit();
}

function TRANSforceUpdateModels(){
   TRANSopenToFileOnLoad = true;
   MODELforcePoll('TRANSmodel', function(){
      TRANSopenToFileOnLoad = false;
   });
   MODELforcePoll('TRANSstatusModel');
   MODELforcePoll('TBLmodel');
   MODELforcePoll('EDImodel');
   HLPdirGlobalInit();
}

function TRANSnewSourceDirectory(){
   let SourceChanged = TRANSprojectRootPrior !== TRANSprojectRoot();
   if(TRANScomponentGuid()) TRANSprojectRootPrior = TRANSprojectRoot();
   return SourceChanged;
}

function TRANSsetDirectories(Callback){
   APIcall("directories", { 'component': TRANScomponentGuid(), development: TRANSdevelopment() }, function(Response){
      if (!Response.success){
         TRANSsourceDirectory   = "";
         TRANSworkingDirectory  = "";
         MODELremove("TRANSgitModel");
         MODELremove("TRANSstatusModel");
         MODELremove("TRANSmodel");
         TRANShandleEditorLaunchError(Response.error);
         return;
      }
      TRANSsourceDirectory  = Response.data.SourceDir;
      TRANSworkingDirectory = Response.data.WorkingDir;
      if(Callback) Callback();
   });
}

function TRANSsetAllListeners(){
   TRGTsetListeners();
   LFDsetListeners();
   SMPLsetListeners();
   TRANSsetListeners();
   HLPsetListeners();
   DEPsetListeners();
   LISsetTranslatorListeners();
   TCMsetListeners();
   TRANSdisableSwipeToPrevious();
}

function TRANStranslatorHtml(){
   let Hide = TRANScomponentHide();
   return /*html*/`
   <div class="TRANSmodal">
      <div class="TRANSfullScreen">
         <div class="TRANSwindow">
            <div class="PRJprojectManager SRCHsidebarGitButtonGroup">
               ${PRJprojectPaneComponentBlock()}
            </div>
            ${TRANSgitHistorySplitter()}
            <div class="TRANSwindowEditGrid">
               <div class="TRANSwindowEdit">
                  <div class="TRANStoolbarWrapper${Hide}">
                     <div class="TRANStoolbarMain">
                        ${TRANStoolbarHtml()}
                     </div>
                     <div class="TRANSpromotionBannerTarget"></div>
                  </div>
                  <div class="TRANSscriptingWindow">
                     <div id="TRANSeditorDiv"></div>

                  </div>
                  <div id="WNDdockBottomPhantom" class="WNDdockPhantom WNDdockBottomPhantom"></div>
                  <div id="WNDdockRightPhantom" class="WNDdockPhantom WNDdockRightPhantom"></div>
               </div>
               <div class="TRANSstatus_bar">
                  <input class="TRANSbottomBarCurrentFile" tabindex="-1" readonly />
                  <span class="TRANSannotationStatus TRANSstatus_annotations"></span>
                  <span class="TRANSviewModeLabel"></span>
                  <span class="TRANSbranchLabel"></span>
               </div>
            </div>
         </div>
      </div>
   </div>`;
}

function TRANSgitHistorySplitter(){
   return /*html*/`
   <div class="TRANSgitHistorySplitter">
      <div class="TRANSresizeControl">
         <div class="TRANSresizeShadow"></div>
         <div class="SPLTresizeButtonContainer TRANSresizeButtonContainer" title="Git history">
            <div class="SPLTresizeButton TRANSresizeButton">
               <svg class="SPLTresizeIcon TRANSresizeIcon" width="16" height="16">
                  <use href="SPLT/SPLTiconCollapseSplitter.svg#SPLTiconCollapseSplitter"></use>
               </svg>
            </div>
         </div>
      </div>
      <div class="TRANSheader">
         <div class="TRANSheaderText">Commit History</div>
      </div>
      <div class="TRANSgitHistoryTarget">
         <div class="TRANSgitHistoryTargetScroller COMlist"></div>
      </div>
   </div>`
}
function TRANStoolbarHtml(){
   COL_TRC('TRANStoolbarHtml');
   return /*html*/`         
   ${SMPLtoolbarHtml()}
   <div class="TRANSsection TRGTmodal">
      <a class="TRANStoolbarButton TRGTcommit"       ></a>
      <a class="TRANStoolbarButton TRGTrepoButton"></a>
      <a class="TRANStoolbarButton TRGTgitPullRemote"></a>
      <a class="TRANStoolbarButton TRGTgitPushRemote"></a>
   </div>
   <div class="TRANSsection TRANStoolbarModal">
      <a class=" TRANStoolbarButton TRANSrunButton"           title="Execute the script to generate annotations"></a>
      <a class=" TRANStoolbarButton TRANSstopButton disabled" title="Stop the execution of the script"></a>
      <a class=" TRANStoolbarButton TRANSautoRunButton"       title="Automatically execute the script while editing"></a>
   </div>
   <div class='TRANSsection HLPmodal'>
      <a class="TRANStoolbarButton HLPhelpButton" title="Open the help files"></a>
   </div>
   <div class='TRANSsection SMPLtoolbarFileType'>
      <!-- [Select] -->
   </div>
   <div class='TRANSsection LFDmodal'>
      ${LFDselectHtml()}
   </div>
   <div>
   </div>`;
}