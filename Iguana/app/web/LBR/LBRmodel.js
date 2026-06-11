let LBRnameError = false;
let LBRtemplatesRendered = null;
let LBRcollectionsRendered = null;
let LBRrepoError = "";
function LBRclearGlobals(){
   LBRnameError = false;
   LBRtemplatesRendered = null;
   LBRcollectionsRendered = null;
   LBRrepoError = "";
}

function LBRmodelInit(){
   LBRclearGlobals();
   MODELpollerAdd('LBRmodel', 'LBRnameExists',     2,  'component/dir', LBRlibraryNameArg);
   MODELpollerAdd('LBRmodel', 'LBRgitData',        2,  'git/check_install', {});
   MODELpollerAdd('LBRmodel', 'LBRcollections',    25, 'git/catalogue', {});
   MODELviewAdd  ('LBRmodel', 'LBRupdateGitDisplay');
   MODELviewAdd  ('LBRmodel', 'LBRupdateGitError');
   MODELviewAdd  ('LBRmodel', 'LBRupdateNameExists');
   MODELviewAdd  ('LBRmodel', 'LBRupdateNameError');
   MODELviewAdd  ('LBRmodel', 'LBRupdateTemplateLoading');
   MODELviewAdd  ('LBRmodel', 'LBRupdateTemplateError');
   MODELviewAdd  ('LBRmodel', 'LBRupdateCollections'); 
   MODELviewAdd  ('LBRmodel', 'LBRupdateTemplates');
   MODELviewAdd  ('LBRmodel', 'LBRfilterTemplates');
   MODELviewAdd  ('LBRmodel', 'LBRupdateRepo');
   MODELviewAdd  ('LBRmodel', 'LBRupdateSelectedTemplate');
   MODELviewAdd  ('LBRmodel', 'LBRupdateButton');
   MODELforcePoll('LBRmodel');
   MODELstart    ('LBRmodel');
   LBRlisteners();
}

function LBRlibraryNameArg(){
   let NameDiv = document.querySelector('.LBRname');
   let Name = NameDiv ? NameDiv.value.trim() : "";
   let Args = {};
   Args.path        = Name;
   Args.component   = TRANScomponentGuid();
   Args.development = TRANSdevelopment();
   return Args;
}

function LBRnameExists(){
   return MODELdata.LBRnameExists.data.exists;
}

function LBRnameExistsError(){
   return MODELdata.LBRnameExists.error;
}

function LBRgitData(){
   return MODELdata?.LBRgitData?.data || {};
}

function LBRcollections(){
   let data = MODELdata?.LBRcollections?.data || [];
   return data;
}

function LBRmodelDestroy(){
   MODELremove('LBRmodel');
   MODELremove('LBRslowModel');
}

function LBRslowModelInit(){
   MODELpollerAdd('LBRslowModel', 'LBRtemplates', 6, 'git/cache', LBRargs);
   MODELviewAdd  ('LBRslowModel', 'LBRupdateTemplateLoading');
   MODELviewAdd  ('LBRslowModel', 'LBRupdateTemplateError');
   MODELviewAdd  ('LBRslowModel', 'LBRupdateTemplates');
   MODELviewAdd  ('LBRslowModel', 'LBRfilterTemplates');
   MODELforcePoll('LBRslowModel');
   MODELstart    ('LBRslowModel');
}

function LBRargs(){
   let ApiData = {};
   ApiData.type = "library";
   return ApiData;
}

function LBRtemplates(){
   let Templates = MODELdata?.LBRtemplates?.data;
   if(!Templates) Templates = {};
   return Templates;
}

function LBRtemplatesError(){
   let Error = MODELdata?.LBRtemplates?.error;
   if(!Error) Error = "";
   return Error;
}
