function INITwelcome(){
   COL_TRC("Doing welcome splash screen")
   MODELshutdownAll();
   let Content  = INITinstallerWelcome();
   INITframe(Content);
	INITwelcomeStartListener();
}

function INITwelcomeStartListener() {
   let StartBtn = document.querySelector('.INITstartBtn');
   StartBtn.addEventListener('click', function() {
      location.hash = `prepare`;
   });
}

function INITinstallerWelcome() {
   return /*html*/`
	${INITsidebarHtml()}
	<div class="INITmodalMainArea">
		<h2 class="INITmodalHeading">
			Welcome to IguanaX
		</h2>
		<div class="INITfeatureGrid">
			<div><img src="INIT/INITiconCreate.svg"></div>
			<div>
				<div class="INITfeature">Take control of your data</div>
				<div class="INITfeatureDesc">Build interfaces with modular components</div>
			</div>
			<div><img src="INIT/INITiconScale.svg"></div>
			<div>
				<div class="INITfeature">Eliminate bottlenecks</div>
				<div class="INITfeatureDesc">There are no limitations</div>
			</div>
			<div><img src="INIT/INITiconShare.svg"></div>
			<div>
				<div class="INITfeature">Grow your understanding</div>
				<div class="INITfeatureDesc">Helping you learn and apply first principles</div>
			</div>
		</div>
		<div class="INITfooter">
			<div class="INITstartBtn BUTTONstandard BUTTONaction">BEGIN &#10132;</div>
		</div>
	</div>`;
}
