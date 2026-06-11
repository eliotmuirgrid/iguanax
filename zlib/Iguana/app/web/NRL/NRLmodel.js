let NRLnameError;
let NRLrepoError;
let NRLinProgress;

function NRLmodelInit(){
   NRLclearGlobals();
   MODELpollerAdd('NRLmodel', 'NRLgitData',            2, 'git/check_install', {});
   MODELpollerAdd('NRLmodel', 'NRLcomponentNamesData', 2, 'component/names', {});
   MODELpollerAdd('NRLmodel', 'NRLcomponentRepoValid', 2, 'git/url/parse', NRLgetRepoString);
   MODELviewAdd  ('NRLmodel', 'NRLupdateNameFeedback');
   MODELviewAdd  ('NRLmodel', 'NRLupdateRepo');
   MODELviewAdd  ('NRLmodel', 'NRLupdateButtonAdd');
   MODELviewAdd  ('NRLmodel', 'NRLworkingFeedback');
   MODELforcePoll('NRLmodel', function(){
      if(NRLgoodGitData()) return;
      MODELremove("NRLmodel");
      NRLrenderGitError();
   });
   MODELstart    ('NRLmodel');
}

function NRLclearGlobals(){
   NRLnameError = false;
   NRLrepoError = false;
   NRLinProgress = false;
   NRLnameTyped = false;
}

function NRLgoodGitData(){
   return NRLgitData()?.installed || false;
}

function NRLgitData(){
   return MODELdata?.NRLgitData?.data || {};
}

function NRLupdateButtonAdd(){
   let Button = document.querySelector('.NRLadd');
   if(!Button) return;
	if( NRLnameError || NRLrepoError || NRLinProgress) {
      Button.classList.add('BUTTONdisabled');
		Button.classList.remove('BUTTONaction');
	}
   else {
      Button.classList.add('BUTTONaction');
		Button.classList.remove('BUTTONdisabled');
	}
}