let LPRlibraryName;
let LPRdefaultCustomValue;
let LPRcard;

function LPRrender(FilePath, CustomUrlValue){
   LPRlibraryName        = FilePath;
   LPRdefaultCustomValue = CustomUrlValue;
   MENUremoveTooltip();
   APIcall("component/get_repository", LPRargs(FilePath), function(R){
      if(!R.success) return SNCKsnackbar("error", "Could not get upstream repository details: " + R.error);
      let Data = R?.data || {};
      let Repo = Data?.repo || "";
      let Link = Data?.link || "";
      LPRrenderOnSuccess(Link, Repo);
   });
}

function LPRrenderOnSuccess(GitUrl, Repo){
   TRANSsaveCurrentModuleCursorPosition();
   if(!TRANSdevelopment()) return LPRrenderRunningCommit(GitUrl, Repo);
   LPRonRequestSuccess(GitUrl, Repo);
}

function LPRargs(LibraryName){
   let ApiData = {};
   ApiData.component   = TRANScomponentGuid();
   ApiData.path        = LibraryName;
   ApiData.development = TRANSdevelopment();
   return ApiData;
}

function LPRrenderRunningCommit(GitUrl, Repo){
   let Html = LPRrunningCommitModeHtml(GitUrl, Repo);
   let Args = {};
   Args.content = Html;
   Args.title = "Configure Library Repository";
   Args.width = "800px";
   LPRcard = POPpopoutOpen(Args);
   document.querySelector(".RPOcancel").addEventListener("click", function(){ POPpopoutClose(LPRcard); });
}

function LPRrunningCommitModeHtml(GitUrl, Repo){
   return /*html*/`
   ${LPRcontentHtml(GitUrl, Repo)}
   <div class="RPOinformation">
   You are in read-only mode since you are viewing a library in a repository commit. Repositories in read-only mode cannot be modified.
   </div>
   <br>
   <div class="BUTTONgroup RPObuttons">
      <div class="RPOcancel BUTTONdefault BUTTONstandard">CLOSE</div>
   </div>
   `;
}