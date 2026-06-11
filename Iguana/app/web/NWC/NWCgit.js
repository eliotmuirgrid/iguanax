function NWCupdateGitError(){
   let Error = document.querySelector('.NWCgitFeedback');
   if(!Error) return;
   let GitError = "";
   if(!NWCcheckGitData()) GitError = GITHerrorHtml("add a component");
   if(Error.innerHTML !== GitError) Error.innerHTML = GitError;
}

function NWCupdateGitDisplay(){
   let Error = document.querySelector('.NWCgitInformation');
   let EditBody = document.querySelector('.NWCmodal');
   if(!Error || !EditBody) return;
   let GitError = false;
   if(!NWCcheckGitData()) GitError = true;
   if(GitError) { Error.classList.remove("NWChidden"); EditBody.classList.add   ("NWChidden"); }
   else         { Error.classList.add   ("NWChidden"); EditBody.classList.remove("NWChidden"); }
   let NewContainer = document.querySelector('.NWCcomponent');
   let height = "";
   if(!GitError) height = "90%";
   if(NewContainer.style.height !== height) NewContainer.style.height = height;
}
function NWCcheckGitData(){
   let GitData = NWCgitData();
   return GitData?.installed || false;
}
