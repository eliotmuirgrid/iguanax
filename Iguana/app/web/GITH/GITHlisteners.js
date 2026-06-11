function GITHlisteners(){
   let GitTable = document.querySelector('.GITSpane');
   GitTable.removeEventListener('click',  GITHclickListeners);
   GitTable.addEventListener   ('click',  GITHclickListeners);
}

function GITHclickListeners(e){
   if(e.target.closest('.GITSappPasswordHelp')) GITSpasswordHelp(e);
   if(e.target.closest(".GITHeditGitCred"))     GITHsetCredentialsPopout();
   if(e.target.closest(".GITHeditService"))     GITHsetServicePopout();
   if(e.target.closest(".GITHserviceUser .GITHedit")) GITHsetUsernamePopout();
   if(e.target.closest(".GITHappPassword .GITHedit")) GITHsetAppPasswordPopout();
   if(e.target.closest(".GITHkeyFiles .GITHedit"))    GITHsetKeyFilesPopout();
   if(e.target.closest(".GITHorganization .GITHedit"))GITHsetOrgsPopout();
   if(e.target.closest(".GITHsshModal .GITHedit"))    GITHsetSshPopout();
   if(e.target.closest(".GITHnotSet .SETlink"))       GITHtryAgain();
}

function GITHtryAgain(){
   MODELforcePoll("GITHmodel");
   MODELforcePoll("GITHslowModel");
}