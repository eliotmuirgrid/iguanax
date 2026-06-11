let INITgitActionInProgress = false;

function INITgit(){
   COL_TRC("Initialize GIT screen");
   MODELshutdownAll();
   INITframe(INITgitHtml());
   INITbackNextBtns("prepare", "location");
   INITgitButtons();
   INITgitModel();
}

function INITgitButtons(){
   document.querySelector(".INITgitView").addEventListener('click', function(E) {INITgitOnClicked(E);}); 
}

function INITgitOnClicked(E){
  COL_TRC("Clicked", E);
  E.stopPropagation();
  if      (E.target.classList.contains('INITinstallGit'))   {COL_TRC("Install Git");  INITinstallGit(); }
  else if (E.target.classList.contains('INITdownloadGit'))  {COL_TRC("Download Git"); INITdownloadGit();}
}

function INITgitModel(){
   COL_TRC("Setting up INITgitModel model");
   MODELpollerAdd('INITgitModel', 'INITgit',        1, 'git/exists', {});
   MODELpollerAdd('INITgitModel', 'INITgitInstall', 1, 'git/download/present', {});
   MODELviewAdd('INITgitModel', 'INITgitShowStatus');
   MODELviewAdd('INITgitModel', 'INITgitDone');
   MODELforcePoll('INITgitModel');
   MODELstart('INITgitModel');
}

function INITgitDone(){
   let Ready = INITgitInstalled();
   COL_TRC("Next state ready =", Ready);
   INITbuttonNextEnabled(Ready);
}

function INITgitInstalled(){ 
   // TODO this can fail - should take account of this.
   COL_DBG(MODELdata.INITgit);
   return MODELdata.INITgit.data.exists;
}

function INITgitInstalledView(){
   let E = document.querySelector(".INITproceed");
   let Html = INITgitInstalled() ? INITpleaseProceed() : '';
   if(E.innerHTML !== Html) E.innerHTML = INITpleaseProceed();
   return `<p>Git is installed.</p>`;
}

function INITgitInstallView(){
   return`
   <div class="INITgitInfoView">
      <div class="INITgitRowDownload">
         <div>Install git before proceeding...</div>
         <div class="INITinstallGit BUTTONaction BUTTONstandard">INSTALL</div>
      </div>
   </div>
    <div class="INITsetupContainer">${INITgitSpinner()}</div>`;
}

function INITgitDownloadView(){
   return`
   <div class="INITgitInfoView">
      <div class="INITgitRowDownload">
         <div>Git install needs to be downloaded...</div>
         <div class="INITdownloadGit BUTTONaction BUTTONstandard">DOWNLOAD</div>
      </div>
   </div>
   <p class="FORMlabel">Download it before proceeding.</p>
   <div class="INITsetupContainer">${INITgitSpinner()}</div>
   `;
}

function INITgitInstallerDownloaded(){
   return MODELdata.INITgitInstall.data.present;
}

function INITgitShowStatus(){
   COL_DBG("Showing GIT status");
   let Html;
   if      (INITgitInstalled())           Html = INITgitInstalledView();
   else if (INITgitInstallerDownloaded()) Html = INITgitInstallView();
   else                                   Html = INITgitDownloadView();
   let E = document.querySelector('.INITgitView');
   if (E.innerHTML !== Html){
      COL_TRC("Change GIT status");
      E.innerHTML = Html;
   }
}

function INITdownloadGit(){
   if(INITgitActionInProgress) { return; }
   COL_TRC("Download GIT");
   INITgitActionInProgress = "Downloading Installer...";
   INITgitShowStatus();
   APIcall('git/download', {}, function(R) { INIThandleDownloadResponse(R);} );
}

function INIThandleDownloadResponse(R) {
   INITgitActionInProgress = false;
   MODELforcePoll("INITgitModel");
   if (!R.success) {
      const error = R.error.length ? R.error : 'Ran into unexpected error -- please try installing git manually and ensure it is on the system path';
      return INITshowError(error);
   }
}

function INITinstallGit() {
   if(INITgitActionInProgress) { return; }
   INITgitActionInProgress = "Running Git Installer...";
   APIcall('git/install', {}, INIThandleInstallResponse);
}

function INIThandleInstallResponse(R) {
   INITgitActionInProgress = false;
   if (!R.success){
      INITshowError(R.error);
   }
}

function INITgitSpinner(){
   if (!INITgitActionInProgress) {
      return "";
   }
   return `<div class="INITspinner"></div><div class="INITinstall-text">${INITgitActionInProgress}</div>`;
}

function INITgitHtml() {
   COL_TRC("Git screen HTML");
   return `
   ${INITsidebarHtml()}
	<div class="INITmodalMainArea">
		<h2 class="INITmodalHeading">
			Install Git
		</h2>
		<p>Git is required for IguanaX to run.</p>
      <div class="INITgitView">Checking for it's presence...</div>
      <div class="INITproceed"></div>
		${INITcreateBtnGroupAndStatusDivs()}
	</div>`;
}
