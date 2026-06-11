function RPOupdateSuccess() {
   const Selector = document.querySelector('.RPOworkspaceFeedback');
   if(!Selector) { return; }
   Selector.innerHTML = `<p class="RPOsuccess">Update Successful!</p>`
   setTimeout(function() { Selector.innerHTML = ''; }, 2000);
   RPOrenderWorkspaces();
}

function RPOrenderWorkspaces(){
   APIcall('git/owners', {}, function(R) {
      let Workspaces = document.querySelector('.RPOworkspaces');
      GITutilsUpdateWorkspaces(Workspaces, 'RPOselectedWorkspace', R?.data?.owners);
   });
}

function RPOupdateOwners() {
   GITupdateWorkspaces(RPOupdateSuccess);
}

function RPOtryAgain(){
   GITupdateWorkspaces(RPOmodelInit, true);
}

function RPOupdateGitUrl(){
   let Workspaces = document.querySelector('.RPOworkspaces');
   let Ending     = document.querySelector('.RPOslug');
   let GitUrl     = document.querySelector('.RPOgitUrl');
   if(!Workspaces || !Ending || !GitUrl) { return; }
   let UrlHtml = RPOsshUrl();
   if(Workspaces.value === "") UrlHtml = "";
   if(GitUrl.value !== UrlHtml) GitUrl.value = UrlHtml;
}

function RPOupdateGitUrlError(){
   let GitError = document.querySelector('.RPOfeedback');
   if(!GitError) return;
   if (RPOgitUrlError != "") { GitError.innerHTML = RPOgitUrlError; }
}
