function LPRlibraryDescriptionHtml(){
   return /*html*/`
   <div class="RPOdescription">
      Libraries allow you to share code. Changes made in one spot can be applied across all components that use the library.
      [<a target="_blank" href=${LINKconfigLibraryRepo}>Learn more</a>]
   </div>`;
}

function LPRcontentHtml(GitUrl, Repo){
   let Args = {};
   Args.GitUrl = GitUrl;
   Args.Repo = Repo;
   Args.UpstreamClass = "LPRremoveUpstream";
   return /*html*/`
   <div class="RPOheader">CURRENT REPOSITORY:</div>
   <br>
   <div class="RPOcomponentDiv">${RPOlibraryHtml(LPRlibraryName)} ${RPOregularUpstreamHtml(Args)} <br><br>${LPRlibraryDescriptionHtml()}</div>
   <br>`;
}

function LPRonRequestSuccess(GitUrl, Repo){
   let Content = LPRcontentHtml(GitUrl, Repo);
   let Args = {};
   Args.title = "Configure Library Repository";
   Args.content = Content;
   Args.onSubmit = LPRonSubmit;
   Args.allowConnect = true;
   Args.defaultCustomValue = LPRdefaultCustomValue;
   Args.precondition = LPRprecondition;
   RPOcreateRepoDialog(Args);
}

function LPRprecondition(callback, failCallback){
   let Perm = TRANSpermissionData()?.edit_script;
   if(Perm) callback();
   else     failCallback();
}

function LPRonSubmit(Data, Push) {
   RPOshowProgress();
   let ApiData         = {};
   ApiData.development = true;
   ApiData.component   = PAGEhashParameter("component");
   ApiData.name        = LPRlibraryName;
   ApiData.push        = Push;
   GITcreateUrls(Data, GIT_URL_TYPE.SSH, function(sshUrl) {
      ApiData.git = sshUrl;
      APIcall("component/library/set_upstream", ApiData, function(R) {
         RPOmidOnSubmitRequest = false;
         if(!R.success) { SNCKsnackbar("error", "Could not complete setting upstream of library: " + R?.error || ""); }
         TRANSfileReloadGitModules();
         SNCKsnackbar("success", "Set the library upstream git url to: " + Data, 4000);
         RPOclose();
         MODELforcePoll("TRANSgitModel");
         MODELforcePoll("TRANSstatusModel");
         MODELforcePoll("TRANSmodel");
      });
   })
}
