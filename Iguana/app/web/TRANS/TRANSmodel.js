/**
 * Copyright (c) 2010-2022 iNTERFACEWARE Inc.  All rights reserved.
 * Model for translator
 */

function TRANSclearModels(){
   // Add clearing of editor data
   PRJclearModels();
   PRJcomponentListRendered = undefined;
   TRANSopenToFileOnLoad = true;
   MODELremove("LBUmodel");
   LBUreset();
}

function TRANSmodelInit(Callback) {
   COL_TRC("Set up models.");
   TRANSclearModels();
   MODELremove('TRANSmodel');
   if(TRANScomponentGuid()){
      MODELpollerAdd('TRANSmodel', 'SMPLdataObject',         7, 'component/dir',          TRANSsampleDataArgs);
      MODELpollerAdd('TRANSmodel', 'PRJdirTree',             3, 'ide/project_tree',       TRANSprojectTreeArgs);
      MODELpollerAdd('TRANSmodel', 'TRANSrunningComponents', 2, 'component/run_list',     {});
      MODELpollerAdd('TRANSmodel', 'TRANScomponentData',     6, 'component/detail',       TRANScomponentArgs);
      MODELpollerAdd('TRANSmodel', 'TRGTlibraryList',       14, 'component/library/list', TRANScomponentDevArgs);
   }
   MODELpollerAdd('TRANSmodel', 'TRANScomponentList',    5,  'component/list',            {});
   MODELpollerAdd('TRANSmodel', 'TRGTgitRepo',           5,  'component/get_repository',  TRANScomponentArgs);
   MODELpollerAdd('TRANSmodel', 'TRANSpermissionData',   17, 'component/permission',     TRANScomponentArgs);
   MODELpollerAdd('TRANSmodel', 'TRANSpermissionAll',    16, 'component/permission/all', {});
   MODELpollerAdd('TRANSmodel', 'TRANScomponentMapData', 10, 'component/map',            {group: false});
   MODELviewAdd  ('TRANSmodel', 'TRANScomponentListError');
   // MODELviewAdd  ('TRANSmodel', 'TRANSaddLibrary');
   MODELviewAdd  ('TRANSmodel', 'TRANSsaveCurrentModuleCursorPosition');
   MODELviewAdd  ('TRANSmodel', 'SMPLsampleDataManipulation');
   MODELviewAdd  ('TRANSmodel', 'SMPLsampleDataError');
   MODELviewAdd  ('TRANSmodel', 'PRJupdateSwitch');
   MODELviewAdd  ('TRANSmodel', 'PRJupdateLink');
   MODELviewAdd  ('TRANSmodel', 'PRJupdateStatusText');
   MODELviewAdd  ('TRANSmodel', 'PRJrenderSwitchToolTip');
   MODELviewAdd  ('TRANSmodel', 'PRJupdateName');
   MODELviewAdd  ('TRANSmodel', 'TRANSupdateAutoButton');
   MODELviewAdd  ('TRANSmodel', 'HLPupdateHelpButton');
   MODELviewAdd  ('TRANSmodel', 'LIBcheckUnlocked');
   MODELviewAdd  ('TRANSmodel', 'PRJassignAndRenderTree');
   MODELviewAdd  ('TRANSmodel', 'PRJcomponentListUpdate');
   MODELviewAdd  ('TRANSmodel', 'TRGTterminalUpdate');
   // MODELviewAdd  ('TRANSmodel', 'TRANSrenderNewLines');
   MODELviewAdd  ('TRANSmodel', 'SMPLupdateBackArrows');
   MODELviewAdd  ('TRANSmodel', 'SMPLupdateForwardArrows');
   MODELviewAdd  ('TRANSmodel', 'PRJgetFullPath');
   MODELviewAdd  ('TRANSmodel', 'SMPLrenderNavGoto');
   MODELviewAdd  ('TRANSmodel', 'TRANSrenderViewMode');
   MODELviewAdd  ('TRANSmodel', 'TRANSrenderBranchLabel');
   MODELviewAdd  ('TRANSmodel', 'SMPLdialogRenderButtonsWrapper');
   MODELviewAdd  ('TRANSmodel', 'SMPLviewEditButton');
   MODELviewAdd  ('TRANSmodel', 'SMPLrenderAddButton');
   MODELviewAdd  ('TRANSmodel', 'LFDrender');
   MODELviewAdd  ('TRANSmodel', 'SMPLtoolbarRenderHexByte');
   MODELviewAdd  ('TRANSmodel', 'SMPLtoolbarRenderFileType');
   MODELviewAdd  ('TRANSmodel', 'DIFFshowConflicts');
   MODELviewAdd  ('TRANSmodel', 'PRJpopulateSourcesReceivers');
   MODELviewAdd  ('TRANSmodel', 'MENUopenMenu');
   MODELviewAdd  ('TRANSmodel', 'CLDsyncOn');
   MODELviewAdd  ('TRANSmodel', 'PRJupdateButtonGroup');
   MODELviewAdd  ('TRANSmodel', 'TRANSrenderPromotionBanner');
   MODELforcePoll('TRANSmodel', function(){
      TRANSopenToFileOnLoad = false;
      TRANScontrolTranslatorFeatures(!TRANScomponentGuid());
      if(Callback) Callback();
   });
   COL_TRC("Set up the sample data toolbar");
   SMPLsetUpToolbar();
}

function TRANSpermissionAll(){
   return MODELdata?.TRANSpermissionAll?.data?.components || {};
}

function TRANScomponentArgs(){
   let Args = {};
   Args.component   = TRANScomponentGuid();
   Args.development = TRANSdevelopment(); 
   return Args;
}

function TRANSlibraryArgs(){
   let Args = {};
   Args.component   = TRANScomponentGuid();
   Args.development = TRANSdevelopment(); 
   Args.type        = "library"; 
   return Args;
}

function TRANScomponentDevArgs(){
   let Args = {};
   Args.component   = TRANScomponentGuid();
   Args.development = TRANSdevelopment();
   return Args;
}

function TRANSsampleDataArgs(){
   let Args = {};
   Args.component   = TRANScomponentGuid();
   Args.path        = SMPLrootFolder();
   Args.development = TRANSdevelopment();
   Args.exclude     = SMPLexcludeFiles();
   return Args;
}

let TRANSopenToFileOnLoad;
let TRANSopenDir;
let TRANSopenMenu;

function TRANSprojectTreeArgs(){
   let Args = {};
   let OpenDirs = [];
   if(TRANSopenToFileOnLoad) OpenDirs.push(TRANSselectedFile());
   if(TRANSopenDir) OpenDirs.push(TRANSsourceDirectory + TRANSopenDir);
   TRANSopenDir = undefined;
   let DirTree = PRJdirTree();
   PRJfindOpenDirsInTree(DirTree, OpenDirs);
   Args.open_dirs = OpenDirs;
   Args.component = TRANScomponentGuid();
   Args.development = TRANSdevelopment();
   return Args;
}

function TRANSpermissionData(){
   return MODELdata?.TRANSpermissionData?.data || {};
}

function TRANScomponentData(){
   return MODELdata?.TRANScomponentData?.data;
}

function TRANScomponentMapData() {
   return MODELdata?.TRANScomponentMapData?.data || {};
}

function TRGTuncommittedChanges(){
   return MODELdata?.TRGTuncommittedChanges?.data || {};
}

function SMPLdataObject(){
   return MODELdata?.SMPLdataObject?.data?.content || [];
}

function SMPLdataCount(){
   return SMPLdataObject().length;
}

function SMPLcomponentId(){
   return MODELdata?.SMPLdataObject?.data?.component || "";
}

function PRJdirTree(){
   return MODELdata?.PRJdirTree?.data || [];
}

function TRGTaheadBehind(){
   return MODELdata?.TRGTaheadBehind?.data || {};
}


// This is a model we want to execute often to give fast git feedback when making changes in the editor 
// Big model has too much going on and results in infinite loops sometimes (since pollers are doing too much etc)
// these are less about polling and more about being called on save
function TRANSgitFastInit(){
   MODELpollerAdd('TRANSgitModel', 'TRGTuncommittedChanges', 55, 'git/uncommitted_changes', TRANScomponentArgs);
   MODELpollerAdd('TRANSgitModel', 'TRGTaheadBehind',        60, 'component/git_ahead_behind',        TRANScomponentArgs);
   MODELviewAdd  ('TRANSgitModel', 'TRGTuncommittedChangesUpdate');
   MODELviewAdd  ('TRANSgitModel', 'TRGTuncommittedChangesAndRepoLinkUpdate');
   MODELviewAdd  ('TRANSgitModel', 'TRGTaheadBehindUpdate');
   MODELstart    ('TRANSgitModel');
}

// initialize these maybe after the file is loaded
function TRANSstatusModelInit() {
   DEPreset();
   MODELpollerAdd('TRANSstatusModel', 'TRANSgitModulesWarning', 9, 'git/verify_component_modules', TRANScomponentArgs);
   MODELpollerAdd('TRANSstatusModel', 'TRGTuncommittedChanges', 50, 'git/uncommitted_changes', TRANScomponentArgs);
   MODELpollerAdd('TRANSstatusModel', 'TRGTaheadBehind',        20, 'component/git_ahead_behind', TRANScomponentArgs);
   MODELpollerAdd('TRANSstatusModel', 'PRJgitInfo',       10, 'component/ide/commit/info',    TRANScomponentDevArgs); // long
   MODELpollerAdd('TRANSstatusModel', 'TRGTgitRepo',      12, 'component/get_repository',     TRANScomponentArgs);
   MODELpollerAdd('TRANSstatusModel', 'TRGTlibraryList', 14, 'component/library/list',      TRANScomponentDevArgs);
   MODELpollerAdd('TRANSstatusModel', 'TRGTfetchRepos',   16, 'component/fetch_repositories', TRANScomponentArgs);
   MODELpollerAdd('TRANSstatusModel', 'PRJconflicts',     17, 'component/merge_conflicts',    TRANScomponentArgs);
   MODELpollerAdd('TRANSstatusModel', 'TRGTpromotionState', 8, 'component/promotion_state',    TRANScomponentArgs);
   MODELpollerAdd('TRANSstatusModel', 'TRGTallLibraries', 18, 'git/cache',                    TRANSlibraryArgs);
   MODELpollerAdd('TRANSstatusModel', 'DEPdata',          19, 'deprecated/lua',              {});
   MODELpollerAdd('TRANSstatusModel', 'DEPhelpIndex',     20, 'help/index',                   TRANShelpArgs);
   MODELviewAdd  ('TRANSstatusModel', 'PRJassignAndRenderTree');
   MODELviewAdd  ('TRANSstatusModel', 'CLDupdate');
   // MODELviewAdd  ('TRANSstatusModel', 'TRANSaddLibrary');
   MODELviewAdd  ('TRANSstatusModel', 'TRGTuncommittedChangesUpdate');
   MODELviewAdd  ('TRANSstatusModel', 'TRGTrepoLinkUpdate'); 
   MODELviewAdd  ('TRANSstatusModel', 'TRGTuncommittedChangesAndRepoLinkUpdate'); 
   MODELviewAdd  ('TRANSstatusModel', 'TRGTaheadBehindUpdate'); 
   MODELviewAdd  ('TRANSstatusModel', 'DEPupdate');
   MODELviewAdd  ('TRANSstatusModel', 'DEPtooltips');
   MODELviewAdd  ('TRANSstatusModel', 'DEPtitles');
   MODELviewAdd  ('TRANSstatusModel', 'TRANSrenderPromotionBanner');
   MODELforcePoll('TRANSstatusModel');
   // MODELforcePoll('TRANSstatusModel', function(){ LBUmodelInit(); });
   MODELstart    ('TRANSstatusModel');
}

function TRANShasError(){
   return TRANScomponentData()?.light == "RED";
}

function TRANSslowModelInit() {
   MODELpollerAdd('TRANSslowModel', 'TRANSlastError', 4, 'logs/retrieve', LGEargs, TRANShasError);
   MODELviewAdd  ('TRANSslowModel', 'PRJupdateSwitchTooltip');
   MODELforcePoll('TRANSslowModel');
   MODELstart    ('TRANSslowModel');
}

function TRANShelpArgs(){
   return { 
      component: TRANScomponentGuid(), 
      development: TRANSdevelopment()
   };
}

function DEPhelpIndexData(){
   return MODELdata?.DEPhelpIndex?.data?.list || {};
}

function PRJgitInfo(){
   return MODELdata?.PRJgitInfo?.data || {};
}

function PRJconflicts(){
   let Data = MODELdata?.PRJconflicts?.data || {};
   let ConflictData = Data[TRANScomponentGuid()] || {};
   return ConflictData?.output || {};
}

function TRANSplainFolder(){
   return MODELdata?.TRGTgitRepo?.data?.plain || false;
}

function TRGTgitRepo(){
   return MODELdata?.TRGTgitRepo?.data || {};
}

function TRGTlibraryList(){
   return MODELdata?.TRGTlibraryList?.data || {};
}

function TRGTallLibraries(){
   return MODELdata?.TRGTallLibraries?.data || [];
}

function TRGTpromotionState(){
   return MODELdata?.TRGTpromotionState?.data || { active: false };
}

function TRANSlastError(){
   return MODELdata?.TRANSlastError?.data || {};
}

function TRANSgitModulesWarning(){
   return MODELdata?.TRANSgitModulesWarning?.data?.error || "";
}