function INITprepare(){
   COL_TRC("Calling INITprepare");
   MODELshutdownAll();
   INITframe(INITinstallerPrepare());
   INITprepareListeners();
   INITprepareModel();
}

function INITprepareListeners(){
   INITbackNextBtns('', "git");
   INITprepareButtons();
}

function INITprepareModel(Callback){   
   COL_TRC("Set up INITprepareModel");
   MODELpollerAdd('INITprepareModel', 'INITexists',    1,   'install/service/exists', {});
   MODELpollerAdd('INITprepareModel', 'INITrunning',   1.1, 'install/service/running', {});
   MODELpollerAdd('INITprepareModel', 'INITexe',       1000,'install/service/path',    {});
   MODELviewAdd  ('INITprepareModel', 'INITupdateRunningStatus');
   MODELviewAdd  ('INITprepareModel', 'INITupdateInstalledStatus');
   MODELviewAdd  ('INITprepareModel', 'INITupdatePrepareProceed');
   MODELviewAdd  ('INITprepareModel', 'INITprepareDone');
   MODELviewAdd  ('INITprepareModel', 'INITupdateExeLocation');
   MODELforcePoll('INITprepareModel', Callback);
   MODELstart('INITprepareModel');
}

function INITremoveCRLF(str) {
   return str.replace(/\r\n/g, '');
}

function INITupdateExeLocation(){
   let E = document.querySelector(".INITexeLocation");
   let Html ='';

   if (MODELdata.INITexe.success){
      let Location = INITremoveCRLF(MODELdata?.INITexe?.data?.filename || "")
      Html = "Located at " + Location;
   }
   if (E.innerHTML !== Html){
      COL_TRC("Changing exe to", Html)
      E.innerHTML = Html;
   }if(E.title !== Html){
      E.title = Html;
   }

   let Exists = INITiguanaInstalled();
   if( Exists &&  E.classList.contains("INIThidden")) E.classList.remove("INIThidden");
   if(!Exists && !E.classList.contains("INIThidden")) E.classList.add   ("INIThidden");
}

function INITprepareDone(){
   let Ready = !INITiguanaRunning() && !INITiguanaInstalled();
   COL_TRC("Next state ready =", Ready);
   INITbuttonNextEnabled(Ready);
}

function INITiguanaInstalled(){
   if (!MODELdata?.INITexists?.success){
      return false;
   }
   COL_DBG(MODELdata?.INITexists?.data?.exists);
   return MODELdata?.INITexists?.data?.exists;
}

function INITiguanaRunning(){
   if (!MODELdata?.INITrunning?.success){
      return false;
   }
   COL_DBG(MODELdata.INITrunning.data.running);
   return MODELdata.INITrunning.data.running;
}

function INITserverButtonStop(){
   return `<div class="INITserverStop BUTTONstandard BUTTONaction">STOP</div>`;
}

function INITserverButtonRemove(){
   return `<div class="INITserverRemove BUTTONstandard BUTTONaction">REMOVE</div>`;
}

function INITprepareButtons(){
    document.querySelector(".INITprepareEvents").addEventListener('click', function(E) {INITonClicked(E);}); 
}

function INITonClicked(E){
   COL_TRC("Clicked", E);
   E.stopPropagation();
        if (E.target.classList.contains('INITserverStop'))   {COL_TRC("Stop");  INITstopServer();   }
   else if (E.target.classList.contains('INITserverRemove')) {COL_TRC("Remove");INITremoveServer(); }
}

// We just let the model display the result.
function INITstopServer()  { 
   APIcall("install/service/stop",      {}, function(R){
      if(R.success) INITremoveServer();
   });
}
function INITremoveServer(){ 
   APIcall("install/service/uninstall", {}, function(R){
      if(R.success) MODELforcePoll("INITprepareModel");
   });}

// TODO - FORMlabel isn't really what this is.
function INITpleaseProceed() { return `<p class="FORMlabel">Please proceed to the next step.</p>`;}

function INITupdateRunningStatus(){
   let Html;
   if(INITiguanaRunning()) Html = INITrunningHtml();
   else                    Html = INITsuccessHtml("IguanaX is not currently running");
   let E = document.querySelector(".INITrunningStatus");
   if(E.innerHTML !== Html) E.innerHTML = Html;
}


function INITrunningHtml(){
   return /*html*/`
   <div class="INITcurrentlyRunning">IguanaX is currently running...</div>
   ${INITserverButtonStop()}`;
}

function INITsuccessHtml(Message){
   return /*html*/`
   <div class="INITcurrentlyRunning">${Message}...</div>
   ${INITcheckmarkHtml()}`;
}

function INITcheckmarkHtml(){
   return /*html*/`<div class="INITstatusCheckmark"></div>`;
}

function INITupdateInstalledStatus(){
   let Html;
   if     (INITiguanaRunning())   Html = INITinstalledHtml(true);
   else if(INITiguanaInstalled()) Html = INITinstalledHtml(false);
   else                      Html = INITsuccessHtml("IguanaX is not currently installed");
   let E = document.querySelector(".INITinstalledStatus");
   if(E.innerHTML !== Html) E.innerHTML = Html;
}

function INITinstalledHtml(Running){
   let Button = Running ? `<div class="BUTTONdisabled BUTTONstandard">REMOVE</div>`: INITserverButtonRemove();
   return /*html*/`
   <div class="INITcurrentlyRunning">IguanaX is currently installed...</div>
   ${Button}`;
}

function INITupdatePrepareProceed(){
   let Proceed = !INITiguanaRunning() && !INITiguanaInstalled();
   let E = document.querySelector(".INITproceed");
   let Html = Proceed ? INITpleaseProceed() : "";
   if(E.innerHTML !== Html) E.innerHTML = Html;
}

function INITinstallerPrepare(){
   return /*html*/`
	${INITsidebarHtml()}
	<div class="INITmodalMainArea INITprepareEvents">
		<h2 class="INITmodalHeading">
			Prepare
		</h2>
      <div>To install IguanaX it cannot be currently running or installed.</div>
      <br>
      <div class="INITprepareContainer">
         <div class="INITrunningStatus">Loading application details...</div>
         <br>
         <div class="INITinstalledStatus"></div>
         <div class="INITexeLocation"></div>
      </div>
      <br>
      <div>Ensure you have closed all other applications viewing IguanaX in particular the <b>Event Viewer</b> and the <b>Services App</b>.</div>
      <div class="INITproceed"></div>
      ${INITcreateBtnGroupAndStatusDivs()}
	</div>`;
}