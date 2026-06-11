let NWCnameError = false;
let NWCtemplatesRendered = null;
let NWCcollectionsRendered = null;
let NWCrepoError = "";
let NWCnameInput = false;
let NWCdescInput = false;
let NWCgitCredentials = {};
let NWClastComponentAdded = "";
let NWCnameMap = {};

function NWCclearGlobals(){
   NWCnameError = false;
   NWCtemplatesRendered = null;
   NWCcollectionsRendered = null;
   NWCrepoError = "";
   NWCnameInput = false;
   NWCdescInput = false;
   NWCgitCredentials = {};
   NWClastComponentAdded = "";
   NWCnameMap = {};
   NWCsourceComponentName= "";
}

function NWCmodelInit(){
   NWCclearGlobals();
   MODELpollerAdd('NWCmodel', 'NWCgitData',            2,  'git/check_install', {});
   MODELpollerAdd('NWCmodel', 'NWCcollections',        25, 'git/catalogue', {});
   MODELpollerAdd('NWCmodel', 'NWCcomponentList',      5, 'component/list',   {});
   MODELpollerAdd('NWCmodel', 'NWCcomponentNamesData', 2,  'component/names', {});
   MODELpollerAdd('NWCmodel', 'NWCtemplates',          20, 'git/cache', NWCcomponentsArgs);
   MODELviewAdd  ('NWCmodel', 'NWCupdateGitDisplay');
   MODELviewAdd  ('NWCmodel', 'NWCupdateGitError');
   MODELviewAdd  ('NWCmodel', 'NWCcomponentNameCount');
   MODELviewAdd  ('NWCmodel', 'NWCupdateNameFeedback');
   MODELviewAdd  ('NWCmodel', 'NWCupdateTemplateLoading');
   MODELviewAdd  ('NWCmodel', 'NWCupdateTemplateError');
   MODELviewAdd  ('NWCmodel', 'NWCupdateCollections'); 
   MODELviewAdd  ('NWCmodel', 'NWCupdateTemplates');
   MODELviewAdd  ('NWCmodel', 'NWCupdateAccessWarning');
   MODELviewAdd  ('NWCmodel', 'NWCfilterTemplates');
   MODELviewAdd  ('NWCmodel', 'NWCupdateButtonAdd');
   MODELviewAdd  ('NWCmodel', 'NWCupdateSourceComponent');
   MODELforcePoll('NWCmodel');
   MODELstart    ('NWCmodel');
   NWClisteners();
}

function NWCbitbucketAccessData(){
   return MODELdata?.NWCbitbucketAccess?.data;
}

function NWCcollections(){
   let data = MODELdata?.NWCcollections?.data || [];
   return data;
}

function NWCbitbucketAccess(){
   return true;
   return NWCbitbucketAccessData()?.code == 200;
}

function NWCgitData(){
   return MODELdata?.NWCgitData?.data;
}
 
// TODO - needs a rethink (not strictly bitbucket access)
function NWCslowModelInit(){
   // MODELpollerAdd('NWCslowModel', 'NWCbitbucketAccess',   20, 'bitbucket/access', {});
   // MODELpollerAdd('NWCslowModel', 'NWCtemplates', 20, 'git/cache', NWCcomponentsArgs);
   MODELviewAdd  ('NWCslowModel', 'NWCupdateTemplateLoading');
   MODELviewAdd  ('NWCslowModel', 'NWCupdateTemplateError');
   MODELviewAdd  ('NWCslowModel', 'NWCupdateTemplates');
   MODELviewAdd  ('NWCslowModel', 'NWCupdateAccessWarning');
   MODELviewAdd  ('NWCslowModel', 'NWCfilterTemplates');
   MODELforcePoll('NWCslowModel');
   MODELstart    ('NWCslowModel');
}

function NWCcomponentsArgs(){
   let ApiData = {};
   ApiData.type = "component";
   return ApiData;
}
