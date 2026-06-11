function STAmodelInit(){
   MODELpollerAdd('STAmodel', 'STArunningComponents', 2,    'component/run_list', {});
   MODELpollerAdd('STAmodel', 'STAreleaseData',       1000, 'release/info',       {});
   MODELpollerAdd('STAmodel', 'STAisService',         1000, 'release/is_service', {});
   MODELpollerAdd('STAmodel', 'STAautoUpgradeTime',   1000, 'upgrade/get_auto',   {});
   MODELpollerAdd('STAmodel', 'STAcrashPermission',   1000, 'component/permission',   {});
   MODELpollerAdd('STAmodel', 'STAgitSshDetectData',  1000, 'git/check_install',   {});
   MODELpollerAdd('STAmodel', 'STAsystemPathData',    1000, 'system/path',   {});
   MODELviewAdd  ('STAmodel', 'STAupdateUpdateText');
   MODELviewAdd  ('STAmodel', 'STAcloseUpdateWindowIfRunning');
   MODELviewAdd  ('STAmodel', 'STAupdateAutoUpgrade');
   MODELviewAdd  ('STAmodel', 'STArenderGitSshInfo');
   MODELviewAdd  ('STAmodel', 'STAupdateWarnings');
   MODELviewAdd  ('STAmodel', 'STApathDisplay');
   MODELforcePoll('STAmodel');
   MODELstart    ('STAmodel');
}

function STAgitSshDetectionData(){ return MODELdata?.STAgitSshDetectData?.data; }

function STAsystemPathData(){ return MODELdata?.STAsystemPathData?.data || []; }

function STAupgradePermission() { return MODELdata?.STAcrashPermission?.data?.upgrade  || false; }
function STAcrashPermission() { return MODELdata?.STAcrashPermission?.data?.crash_file || false; }
function STAnoVersionInformation(){ return MODELdata?.STAreleaseData?.data?.code != 200; }
function STAonNewerVerion()   { return  MODELdata?.STAreleaseData?.data?.newer; }
function STAonCurrentVersion(){ return !MODELdata?.STAreleaseData?.data?.upgrade_available; }
function STArunningAsService(){ return  MODELdata?.STAisService?.data?.is_service; }
function STAversionNumber()   { return  MODELdata?.STAreleaseData?.data?.version; }

function STAnoVersionError()  { 
   let Error = MODELdata?.STAreleaseData?.data?.error || "";
   let Reasons = MODELdata?.STAreleaseData?.data?.reasons || [];
   let ReasonsList = Reasons.map(reason => `<li>${reason}</li>`).join('');
   return `${Error.trim()}${ReasonsList ? `<ul class="STAnoSpaceList">${ReasonsList}</ul>` : ''}`;
}

function STAcomponentsRunning(){ 
   let RunningComponents = MODELdata?.STArunningComponents?.data || {}; 
   let Count = Object.keys(RunningComponents).length;
   return Count != 0;
}

function STAupdateUpdateText(){
   let d = document.querySelector(".STAversionUpgradable");
   if(!d) return;
   let html = STAupdateTextHtml();
   if(d.innerHTML != html) d.innerHTML = html;
}

function STAupdateTextHtml(){
   if( STAnoVersionInformation() )return STAaccessUnavailableHtml();
   if( STAonNewerVerion()    )    return STAonNewerVersionHtml();
   if( STAonCurrentVersion() )    return STAonCurrentVersionHtml();
   if( STAcomponentsRunning())    return STAcomponentsRunningHtml();
   if(!STArunningAsService() )    return STArunningAsServiceHtml();
   return STAupdateAvailableHtml();
}

function STAupdateWarning(){
   return STAnoVersionInformation() || MODELdata?.STAreleaseData?.data?.upgrade_available;
}

function STAaccessUnavailableHtml() { return /*html*/`
   <div class="STAwarningText">${STAnoVersionError()} Access required to get updates including critical security patches. [<a class="SETlink" href=${LINKwarningSecurityPatch} target="_blank">Learn more</a>]`;
}

function STAdownloadLatestHtml()    { 
   let Permission = STAupgradePermission();
   if(!Permission) return `- [<div class="STAlinkDisabled" title="You do not have permission to do this.">Get the Latest Beta</div>]`;
   return `- [<a class="STAgetLatestLink SETlink">Get the Latest Beta</a>]`
}

function STAupdateAnywaysHtml(){
   let Permission = STAupgradePermission();
   if(!Permission) return `<div class="STAlinkDisabled" title="You do not have permission to do this.">Update anyways?</div>`;
   return `<a class="STAupdateLink SETlink">Update anyways?</a>`;
}

function STAonNewerVersionHtml()    { return `You're on a version of Iguana more recent than the latest official release ${STAdownloadLatestHtml()}`; }
function STAonCurrentVersionHtml()  { return `You're on the latest version ${STAdownloadLatestHtml()}`; }
function STArunningAsServiceHtml()  { return `Iguana is not currently running as a service. It must be run as a service to safely update to ${STAversionNumber()} - [${STAupdateAnywaysHtml()}]`; }
function STAcomponentsRunningHtml() { return `[<a class="SETlink STAstopAllComponents">Stop components</a>] before updating to ${STAversionNumber()}`; }
function STAupdateAvailableHtml()   { return `[<a class="STAupdateLink SETlink">Update</a>] to ${STAversionNumber()}`; }

function STAcloseUpdateWindowIfRunning(){
   let modal = document.querySelector(".STAupdateModal");
   if(!modal) return;
   let Running = STAcomponentsRunning();
   if(Running && !STAtestChanglog) POPpopoutClose(STAupdateContainer);
}

function STAupdateWarnings(){
   let Data = STAgitSshDetectionData();
   let GitWarning    = !Data?.installed;
   let KeygenWarning = !Data?.sshkeygen_path;
   let SshWarning    = !Data?.ssh_path;
   let UpdateWarning = STAupdateWarning();
   STAupdateTargetClass(".STAgitDiv",            "STAgitWarning",   GitWarning);
   STAupdateTargetClass(".STAgitDetected",       "SETwarningLabel", GitWarning);
   STAupdateTargetClass(".STAsshKeygenDetected", "SETwarningLabel", KeygenWarning);
   STAupdateTargetClass(".STAsshDetected",       "SETwarningLabel", SshWarning);
   STAupdateTargetClass(".STAupdateLabel",       "SETwarningLabel", UpdateWarning);
}