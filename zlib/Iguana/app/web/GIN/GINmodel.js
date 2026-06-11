function GINmodelInit(){
   MODELpollerAdd("GINmodel", 'GINeditPermission', 10, 'component/permission', {});
   MODELpollerAdd("GINmodel", 'GINrepository',     10, 'instance/repo', {});
   MODELpollerAdd("GINmodel", 'GINuser',           10, 'instance/user', {});
   MODELpollerAdd("GINmodel", 'GINinterval',       10, 'instance/interval', {});
   MODELpollerAdd("GINmodel", 'GINlastPush',       10, 'instance/last_push', {});
   MODELpollerAdd("GINmodel", 'GINtrackKeys',      10, 'instance/track_keys', {});
   MODELpollerAdd("GINmodel", 'GINmoduleWarning',  10, 'git/verify_instance_modules', {});
   MODELviewAdd  ("GINmodel", "GINupdateRepo");
   MODELviewAdd  ("GINmodel", "GINupdateUser");
   MODELviewAdd  ("GINmodel", "GINupdateAuto");
   MODELviewAdd  ("GINmodel", "GINupdateKey");
   MODELviewAdd  ("GINmodel", "GINupdateStatus");
   MODELviewAdd  ("GINmodel", "GINupdateWarning");
   MODELviewAdd  ("GINmodel", "GINupdateModuleWarning");
   MODELviewAdd  ("GINmodel", "GINupdateEditButtons");
   MODELforcePoll("GINmodel");
   MODELstart("GINmodel");
}

function GINeditPermission(){
return MODELdata?.GINeditPermission?.data?.edit_gitservers; 
}

function GINrepo(){
   return MODELdata?.GINrepository?.data || {};
}
function GINrepoRepo(){
   let Repo = GINrepo()?.repo || "";
   return Repo;
}  

function GINuser(){
   return MODELdata?.GINuser?.data?.user || "";
}

function GINautoValue(){
   return MODELdata?.GINinterval?.data?.interval || 0;
}

function GINlastPushAttempted(){
   let T = MODELdata?.GINlastPush?.data?.attempted || 0;
   T = parseInt(T, 10);
   if(isNaN(T)) T = 0;
   return T * 1000;
}

function GINlastPushSuccessful(){
   let T = MODELdata?.GINlastPush?.data?.successful || 0;
   T = parseInt(T, 10);
   if(isNaN(T)) T = 0;
   return T * 1000;
}

function GINlastPushError(){
   let E = MODELdata?.GINlastPush?.data?.last_error || "";
   return E;
}

function GINpushWarningError(){
   return MODELdata?.GINlastPush?.data?.last_warning;
}

function GINtrackKeys(){
   return MODELdata?.GINtrackKeys?.data?.ignored;
}

function GINmoduleWarning(){
   return MODELdata?.GINmoduleWarning?.data?.error || "";
}