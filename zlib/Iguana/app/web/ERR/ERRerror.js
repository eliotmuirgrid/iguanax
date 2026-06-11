function ERRpageCatalogue(){
   let R = '';
   for (K in PAGEtable){
      R+= `<a href="#${K}">#${K}</a><br>`;
   }
   return R; 
}

function ERRerror(){
   PAGEresetPage();
   CORpreparePage();
   CORheader();
   CORsetNavIcon('CORdashboardIcon');

   document.querySelector('.CORmain').innerHTML = `
	<div class='ERRcontainer'>
   <div class='ERRbody'>
   <div class="ERRheading">This page hash is invalid: <b>${location.hash}</b></div>
   <p>These are the known pages:</p>
	<div class='ERRknownPages'>
   ${ERRpageCatalogue()}
	</div>
   </div>
	</div>`;
}
