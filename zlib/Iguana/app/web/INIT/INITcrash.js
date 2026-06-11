function INITcrash() {
   COL_TRC("Crash");
   document.body.innerHTML =
      `<div class="INITbackground">
			<div class="INITcontainer">
				<div class="INITinstallerContents">
					<div class="INITiguanaLogo">${SESSiguanaWordmarkWhite}</div>
					<div class="INITiguanaTagline">Where integration meets innovation.</div>
              <div class="INITerror-container">
                 <div class="INITerror-text">Something went wrong</div>
                 <div class="INITinstructions">Please restart the installer and try again.</div>
              </div>
				</div>
			</div>
 		</div>`;
}

