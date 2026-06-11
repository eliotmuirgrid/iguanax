let CRPcard;

function CPRrender(){
   POPtooltipRemoveByClass('CLDmodal');
   let ApiData = {};
   ApiData.component = PAGEhashParameter("component");
   ApiData.development = TRANSdevelopment();
   APIcall("component/get_repository", ApiData, function(R){
      if(!R.success) return SNCKsnackbar("error", "Could not get upstream repository details: " + R.error); 
      let Data  = R?.data || {};
      CPRonRequestSuccess(Data);
   });
}

function CPRonSubmit(R, Push){
   RPOshowProgress();
   let Component = PAGEhashParameter("component");
   let ApiData = {};
   ApiData.development = true;
   ApiData.component = Component;
   ApiData.git = R;
   ApiData.push = Push;
   APIcall("component/set_upstream", ApiData, function(R){
      RPOmidOnSubmitRequest = false;
      if(!R.success) SNCKsnackbar("error", "Could not complete setting upstream of component: " + R?.error || "");
      APIcall("component/detail", {component: Component}, function(R){
         let Data = R?.data || {};
         let Message = GINsetUpstream(Data.name, ApiData.git);
         GINaddAndCommit({message: Message, component: Component});
      });
      RPOclose();
      MODELforcePoll('TRANSstatusModel');
   });
}

function CRPcomponentRepositoryHtml(){
   return /*html*/`
   <div class="RPOdescription">
      Each component can be managed as a self-contained git repository. Otherwise, it can be a plain folder. [<a target="_blank" href=${LINKconfigComponentRepo}>Learn more</a>]
   </div>`;
}

function CRPcurrentRepositoryHtml(Data, Development){
   let GitUrl = Data?.link || "";
   let Repo   = Data?.repo || "";
   let Plain  = Data?.plain || false;
   let Args = {};
   Args.GitUrl = GitUrl;
   Args.Repo = Repo;
   Args.Plain = Plain;
   Args.Component = Development;
   Args.Development = Development;
   Args.UpstreamClass = "CRPremoveUpstream";

   return /*html*/`
   <div class="RPOheader">CURRENT REPOSITORY:</div>
   <br>
   <div class="RPOcomponentDiv">${RPOregularUpstreamHtml(Args)}<br>${CRPcomponentRepositoryHtml()}</div>
   <br>`;
}

function CPRonRequestSuccess(Data){
   if(!TRANSdevelopment()) return CPRrenderRunningCommit(CPRrunningCommitModeHtml(Data));
   if(Data.plain) return CPRrenderRunningCommit(CRPcurrentRepositoryHtml(Data, true));
   let Args = {};
   Args.title = "Set Git URL";
   Args.content = CRPcurrentRepositoryHtml(Data, true);
   Args.onSubmit = CPRonSubmit;
   Args.allowConnect = true;
   Args.precondition = CPRprecondition;
   Args.onCancel = CPRonClose;
   RPOcreateRepoDialog(Args);
}

function CPRprecondition(callback, failCallback){
   let Perm = TRANSpermissionData();
   if(Perm) callback();
   else     failCallback();
}

function CPRonClose(){
   MODELforcePoll('TRANSmodel');
   MODELforcePoll('TRANSstatusModel');
}

function CRPclose(){
   POPpopoutClose(CRPcard);
   if(CARDredirectToRemote) CPRrender();
   CARDredirectToRemote = "";
}

function CPRrenderRunningCommit(Content){
   let CloseButton = `   <br>
   <div class="BUTTONgroup RPObuttons">
      <div class="RPOcancel BUTTONdefault BUTTONstandard">CLOSE</div>
   </div>`;
   let Args = {};
   Args.content = Content + CloseButton; 
   Args.title = "Set Git URL";
   Args.width = "800px";
   CRPcard = POPpopoutOpen(Args);
   let div = document.querySelector(".RPOturnToRepo");
   if(div) div.addEventListener("click", function(){ RPOturnRepo(CRPclose); });
   document.querySelector(".RPOcancel").addEventListener("click", CRPclose);
}

function CPRrunningCommitModeHtml(GitUrl){
   return /*html*/`
   ${CRPcurrentRepositoryHtml(GitUrl, false)}
   <div class="RPOinformation">
   You are in read-only mode since you are viewing a repository commit. Repositories in read only mode cannot be modified.
   </div>
   `;
}
