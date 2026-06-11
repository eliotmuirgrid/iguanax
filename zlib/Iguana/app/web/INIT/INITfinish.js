function INITfinish(){
   COL_TRC("Set up finish screen.");
   MODELshutdownAll();
   INITheartbeatClearInterval();
   INITframe(INITinstallerFinish());
   INITfinishListeners();
   INITfinishModel();
}

function INITfinishListeners(){
   document.querySelector('.INITnextBtn').addEventListener('click', function() {
      INITexitServer();
   });
}

function INITexitServer(){
   COL_TRC("INITexitServer");
   APIcall("mode/normal/start", {}, APIdoNothingCallback);
   window.location = MODELdata.INITurl.data.url;
}

function INITfinishModel(Callback){   
   COL_TRC("Set up INITfinishModel");
   MODELpollerAdd('INITfinishModel', 'INITexists',    1,    'install/service/exists', {});
   MODELpollerAdd('INITfinishModel', 'INITrunning',   1.1,  'install/service/running', {});
   MODELpollerAdd('INITfinishModel', 'INITurl',       1000, 'install/url', {});
   MODELviewAdd  ('INITfinishModel', 'INITfinishStatus');
   MODELviewAdd  ('INITfinishModel', 'INITfinishButton');
   MODELforcePoll('INITfinishModel', Callback);
   MODELstart('INITfinishModel');
}

function INITfinishButton(){
   if (INITiguanaInstalled() && INITiguanaRunning()){
      INITenableNext();
   }
}

function INITfinishStatus(){
   let E = document.querySelector(".INITfinishView");
   let Html = INITfinishStatusHtml();
   if (E.innerHTML !== Html){
      E.innerHTML = Html;
   }
}

function INITfinishStatusHtml(){
   if (!INITiguanaInstalled()){
      return "Failed to install IguanaX";
   } else {
      if (INITiguanaRunning()){
         return INITinstalledAndRunningHtml();
      } else {
         return INITinstalledDoneHtml();
      }
   }
}

function INITinstalledDoneHtml(){
   return /*html*/`
   <div class="INITfinishDiv">
      <div class="INITcheckmarkDiv"></div>
      <p class="INITappTypelabel INITfinished">The installation was successful.</p>
      <p>IguanaX was installed but it is not currently running.</p>
   </div>`;
}

function INITinstalledAndRunningHtml(){
   return /*html*/`
   <div class="INITfinishDiv">
      <div class="INITcheckmarkDiv"></div>
      <p class="INITappTypelabel INITfinished">The installation was successful and IguanaX is running.</p>
      <p>Click Finish to open the dashboard.</p>
   </div>`;
}

function INITinstallerFinish(){
   return `
   ${INITsidebarHtml()}
	<div class="INITmodalMainArea">
		<h2 class="INITmodalHeading">
			Finish
		</h2>
      <div class="INITfinishView">Detecting if we have started the server...</div>
	   <div class="INITbtnGroup INITfooter">
         <div></div>
         <div class="INITnextBtn BUTTONstandard BUTTONdisabled">FINISH</div>
      </div>
	</div>`;
}