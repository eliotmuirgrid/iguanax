function STAaboutScreen(){
   SETrender();
   STAmodelInit();
	document.querySelector('.STAabout').classList.add('selected');
   APIcall('product/version', {}, function(Response){
		let ContentBody = document.querySelector('.SETcontentBody');
		if(!ContentBody) return;
		ContentBody.innerHTML = STAversionCardInfoHtml(Response.data);
      SETbrowserReport();
      STAgetDebugSymbols();
      STAsetListeners();
   });
}

function STAgetDebugSymbols(){
   COL_TRC("Get the debug symbols!");
   APIcall("debug/info/pdb", {}, STAshowDebugSymbols);
}

function STAshowDebugSymbols(R){
   COL_TRC(R);
   let E = document.querySelector(".SETdebugSymbols");
   if (E){
      if (R.success){
         E.innerHTML = R.data.pdb;
      } else {
         E.innerHTML = R.error;
      }
   }
}

function STAcrashFilesHtml(){
   let Crash = STAcrashPermission();
   if(Crash) return /*html*/`<div class="SETversionDetail SETdebug">[<a class="DBGshowList SETlink">Crash Files</a>]</div>`;
   return /*html*/`<div class="SETversionDetail">[<div class="STAlinkDisabled" title="You do not have permission to view this.">Crash Files</div>]</div>`
}

function STAversionCardInfoHtml(Data){ 
	return /*html*/`
	<div class="SETsettingsPaneLayout SETsettingsPane">
		<div class="SETversionAboutIguana">
			<div class="SETversionContainer">About</div>
		</div>
      <div class="SETversionPanel">
         <div class="SETversionRow">
            <div class="SETversionLabel">Version:</div>
            <div class="SETversionDetail SETversion">${Data.version}</div>
         </div>
         <div class="SETversionRow">
            <div class="SETversionLabel STAupdateLabel">Update:</div>
            <div class="SETversionDetail STAversionUpgradable SETupdate">...</div>
         </div>
         <div class="SETversionRow">
            <div class="SETversionLabel">Auto Updates:</div>
            <div class="SETversionDetail STAautoUpgrade SETautoUpdate"></div>
         </div>
      </div>
		<div class="SETversionPanel">
			<div>
            <div class="SETversionRow">
					<div class="SETversionLabel">Commit:</div>
					<div class="SETversionDetail">${Data.commit_id}</div>
				</div>
				<div class="SETversionRow">
					<div class="SETversionLabel">Message:</div>
					<div class="SETversionDetail SETmessage">${Data.commit_message}</div>
				</div>
            <div class="SETversionRow">
					<div class="SETversionLabel">Time:</div>
					<div class="SETversionDetail SETtime">${Data.timestamp}</div>
				</div>
            <div class="SETversionRow">
					<div class="SETversionLabel">Branch:</div>
					<div class="SETversionDetail">${Data.branch}</div>
				</div>
            <div class="SETversionRow">
					<div class="SETversionLabel">Running As:</div>
					<div class="SETversionDetail">${Data.user}</div>
				</div>
            <div class="SETversionRow">
               <div class="SETversionLabel">Iguana Executable:</div>
               <div class="SETversionDetail">${Data.executable}</div>
            </div>
            <div class="SETversionRow">
               <div class="SETversionLabel">Data Directory:</div>
               <div class="SETversionDetail">${Data.working_dir}</div>
            </div>
            <div class="SETversionRow">
               <div class="SETversionLabel">Debug Symbols:</div>
               <div class="SETversionDetail SETdebugSymbols">...</div>
            </div>
            <div class="SETversionRow">
               <div class="SETversionLabel">Debug:</div>
               ${STAcrashFilesHtml()}
            </div>
			</div>
		</div>
      <div class="SETversionPanel STAgitDiv">
         ${STAgitDetectHtml()}
         ${STAsshDetectHtml()}
         <div class="SETversionRow">
            <div class="SETversionLabel">System Path:</div>
            <div class="STAsystemPath">...</div>
         </div>
      </div>
      <div class="SETversionPanel SETbrowserPanel">
			<div class="SETbrowserInformation"></div>
		</div>
	</div>`;
}