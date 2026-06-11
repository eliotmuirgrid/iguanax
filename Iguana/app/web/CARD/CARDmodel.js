/*
 * Copyright (c) 2010-2022 iNTERFACEWARE Inc.  All rights reserved.
 * Model for card
 */

let CARDnameEdit        = false;
let CARDdescriptionEdit = false;
let CARDcommitEdit      = false;

function CARDmodelInit(){
   COL_TRC("Begin card model");
   CARDresetEdits();
   MODELpollerAdd('CARDmodel', 'CARDoperationalNotiData', 2,  'notifications/enabled',    {});
   MODELpollerAdd('CARDmodel', 'CARDrunningComponents',   2,  'component/run_list',       {});
   MODELpollerAdd('CARDmodel', 'CARDcomponents',          10,  'component/list',          {});
   MODELpollerAdd('CARDmodel', 'CARDnotificationData',    2,  'notifications',            {});
   MODELpollerAdd('CARDmodel', 'CARDtemplates',           10, 'git/cache',                CARDtemplatesArg);
   MODELpollerAdd('CARDmodel', 'CARDdataModel',           2,  'component/detail',         CARDcomponentArgs);
   MODELpollerAdd('CARDmodel', 'CARDgitRepo',             10, 'component/get_repository', CARDcomponentArgs);
   MODELpollerAdd('CARDmodel', 'CARDfieldData',           3,  'component/fields/get',     CARDcomponentArgs);
   MODELpollerAdd('CARDmodel', 'CARDcommits',             10, 'component/commits',        CARDcommitsArgs);
   MODELpollerAdd('CARDmodel', 'CARDpermissionData',      10, 'component/permission',     CARDargs);
   MODELpollerAdd('CARDmodel', 'CARDroleData',            15, 'role/list',                {});
   MODELviewAdd  ('CARDmodel', 'CARDcomponentExists');
   MODELviewAdd  ('CARDmodel', 'CARDrender');          // this allows us to call it once polls finish and to cancel if page change occurs
   MODELviewAdd  ('CARDmodel', 'CARDupdateTag');
   MODELviewAdd  ('CARDmodel', 'CARDupdateTagEditButton');
   MODELviewAdd  ('CARDmodel', 'CARDupdateRepo');
   MODELviewAdd  ('CARDmodel', 'CARDupdateSourceActions');
   MODELviewAdd  ('CARDmodel', 'CARDupdateNameView');
   MODELviewAdd  ('CARDmodel', 'CARDupdateDescriptionView');
   MODELviewAdd  ('CARDmodel', 'CARDupdateError');
   MODELviewAdd  ('CARDmodel', 'CARDupdateFields');
   MODELviewAdd  ('CARDmodel', 'CARDupdateQueue');
   MODELviewAdd  ('CARDmodel', 'CARDupdateRunningCommitView');
   MODELviewAdd  ('CARDmodel', 'CARDupdateSwitch');
   MODELviewAdd  ('CARDmodel', 'CARDupdateClearLinks');
   MODELviewAdd  ('CARDmodel', 'CARDupdateWriteLinks');
   MODELviewAdd  ('CARDmodel', 'CARDupdateReposition');
   MODELviewAdd  ('CARDmodel', 'CARDupdatePermissionViews');
   MODELviewAdd  ('CARDmodel', 'CARDupdateInjectHtml');
   MODELviewAdd  ('CARDmodel', 'CARDupdateNotificationView');
   MODELviewAdd  ('CARDmodel', 'CARDupdatePrev');
   MODELviewAdd  ('CARDmodel', 'CARDupdateNext');
   COL_TRC("Force poll of the main card model");
   MODELforcePoll('CARDmodel', CARDmainModelStated);
   MODELstart('CARDmodel');
}

function CARDmainModelStated(){
   COL_TRC("Now do the slow stuff");
   CARDfetchRepository();
   CARDslowModelInit();
   CARDcommitUpdateModelInit();
}

function CARDslowModelInit(){
   COL_TRC("Initalizing the slow model");
   MODELpollerAdd('CARDslowModel', 'CARDlastLog',   2, 'logs/retrieve', LGEargs);
   MODELpollerAdd('CARDslowModel', 'CARDlastError', 2, 'logs/retrieve', CARDlastErrorArgs);
   MODELpollerAdd('CARDslowModel', 'CARDinstCommits',   10, 'instance/git_history',     {});
   MODELviewAdd  ('CARDslowModel', 'CARDupdateLastActivity');
   MODELviewAdd  ('CARDslowModel', 'CARDupdateLastErrorPopover');
   MODELviewAdd  ('CARDslowModel', 'CARDupdateCurrentCommit');
   MODELforcePoll('CARDslowModel');
   MODELstart    ('CARDslowModel');
}

function CARDcommitUpdateModelInit(){
   MODELpollerAdd('CARDcommitModel', 'CARDcommits', 10, 'component/commits', CARDargs);
}

function CARDresetEdits(){
   CARDnameEdit        = false;
   CARDdescriptionEdit = false;
   CARDcommitEdit      = false;
   CARDrenderedFields  = null;
   CARDrenderedRunning = false;
   CARDrenderedError   = "";
}

function CARDcomponentRoleData(){
   let Data = CARDroleData();
   let Ans = {};
   for(role in Data){
      if(Data[role].component_role) Ans[role] = Data[role];
   }
   return Ans;
}

function CARDroleData(){
   return MODELdata?.CARDroleData?.data || {};
}

function CARDpermissionData(){
   return MODELdata?.CARDpermissionData?.data;
}

function CARDdata(){
   return MODELdata?.CARDdataModel?.data;
}

function CARDdescription(){
   let Data = CARDdata();
   return Data.description;
}

function CARDname(){
   let Data = CARDdata();
   return ESChtmlEscape(Data.name);
}

function CARDlight(){
   let Data = CARDdata();
   return DASH_LIGHTS[Data.light];
}

function CARDstatus(){
   let Data = CARDdata();
   return Data?.light;
}

function CARDerror(){
   return MODELdata.CARDdataModel.error;
}

function CARDcomponentArgs(){
   let Args = {};
   Args.component = CARDcomponentId();
   Args.development = true;
   return Args;
}

function CARDcommitsArgs(){
   let Args = {};
   Args.component = CARDcomponentId();
   return Args;
}

function CARDargs(){
   let Args = {};
   Args.component = CARDcomponentId();
   Args.development = CARDwritable();
   return Args;
}
function CARDgitRepoSsh(){
   let Data = CARDgitRepoData();
   return Data?.urls?.ssh || "";
}

function CARDgitRepoData(){
   return MODELdata?.CARDgitRepo?.data;
}

function CARDgitBranch(){
   let Data = CARDgitRepoData();
   return Data?.branch || "";
}

function CARDgitRepo(){
   let Data = CARDgitRepoData();
   if(Data?.plain) return RPOplainHtml();
   return Data?.repo || "";
}

function CARDgitLink(){
   let Data = CARDgitRepoData();
   return Data?.link || "";
}

function CARDgitPlain(){
   let Data = CARDgitRepoData();
   return Data?.plain || false;
}

function CARDtemplates(){
   return MODELdata.CARDtemplates.data;
}

function CARDrunComp(){
   let Data = CARDdata();
   return Data?.run_comp || "";
}

function CARDcommit(){
   let Data = CARDdata();
   return Data?.commit || "DEVELOPMENT";
}

function CARDfieldData(){
   return MODELdata?.CARDfieldData?.data || [];
}

function CARDfieldError(){
   let FieldError = MODELdata?.CARDfieldData?.error || "";
   if(FieldError == "Error: View Script permission required to perform this action"){
      FieldError = "You need View Script permission to view custom fields";
   }
   return FieldError;
}

function CARDtemplatesArg(){
   let ApiData = {};
   ApiData.type = "component";
   return ApiData;
}

function CARDfetchRepository(){
   APIcall('component/fetch_repositories', CARDcomponentArgs(), function(){ });
}

function CARDcommits(){
   let Commits = MODELdata?.CARDcommits?.data || [];
   return Commits;
}

function CARDcommitsRelevant(){
   let Commits = MODELdata?.CARDcommits?.data || [];
   return Commits;
}

function CARDinstCommits(){
   let Commits = MODELdata?.CARDinstCommits?.data?.commits || {};
   return Commits;
}

function CARDcomponents(){
   let Components = MODELdata?.CARDcomponents?.data?.component_data || [];
   return Components;
}