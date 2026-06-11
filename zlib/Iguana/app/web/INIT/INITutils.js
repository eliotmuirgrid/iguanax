function INITsetFrame(Content){
   COL_TRC("Create scaffolding.");
   document.body.innerHTML =
      `<div class="INITbackground">
			<div class="INITcontainer">
				<div class="INITinstallerContents">
					<div class="INITiguanaLogo">${SESSiguanaWordmarkWhite}</div>
					<div class="INITiguanaTagline">Where integration meets innovation.</div>
					<div class="INITmodal">
						${Content}
					</div>
				</div>
			</div>
 		</div>`;
}

function INITframe(Content){
   let M = document.querySelector('.INITmodal');
   if (M){
      COL_TRC("Scaffolding already present.");
      M.innerHTML = Content;
      INITmenuModelInit();
      return;
   }
   INITsetFrame(Content);
   INITmenuModelInit();
}

function INITsidebarHtml(){
   return `
   <div class="INITsidebarDiv">
		<div class="INITmodalSidebar"></div>
		<div class="INITversionDiv"></div>
	</div>`;
}

function INITshowSuccess(successString) {
   let SuccessDiv = document.querySelector('.INITsuccess');
   if(!SuccessDiv) { return; }
   SuccessDiv.style.display = 'block';
   SuccessDiv.innerHTML = `<pre>${successString}</pre>`;
   SuccessDiv.title = successString;
}

function INITshowError(errorString) {
   COL_TRC(errorString);
   let ErrorDiv = document.querySelector('.INITerror');
   if(!ErrorDiv) { return; }
   ErrorDiv.style.display = 'block';
   ErrorDiv.innerHTML = `<pre>${errorString}</pre>`;
   ErrorDiv.title = errorString;
}

function INIThideStatusDivs() {
   let SuccessDiv = document.querySelector('.INITsuccess');
   let ErrorDiv   = document.querySelector('.INITerror');
   if(SuccessDiv) { SuccessDiv.style.display = 'none'; }
   if(ErrorDiv)   { ErrorDiv.style.display = 'none'; }
}