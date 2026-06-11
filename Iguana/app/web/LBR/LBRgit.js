function LBRupdateGitError(){
   let Error = document.querySelector('.LBRgitFeedback');
   if(!Error) return;
   let GitError = "";
   if(!LBRgoodGitData()) GitError = GITHerrorHtml("add a library");
   if(Error.innerHTML !== GitError) Error.innerHTML = GitError;
}

function LBRupdateGitDisplay(){
   let Error = document.querySelector('.LBRgitInformation');
   let EditBody = document.querySelector('.LBRmodal');
   if(!Error || !EditBody) return;
   let GitError = false;
   if(!LBRgoodGitData()) GitError = true;
   if(GitError) { Error.classList.remove("LBRhidden"); EditBody.classList.add   ("LBRhidden"); }
   else         { Error.classList.add   ("LBRhidden"); EditBody.classList.remove("LBRhidden"); }
   let NewContainer = document.querySelector('.LBRcomponent');
   let height = "";
   if(!GitError) height = "90%";
   if(NewContainer.style.height !== height) NewContainer.style.height = height;
}

function LBRgoodGitData(){ 
   return LBRgitData()?.installed || false;
}
