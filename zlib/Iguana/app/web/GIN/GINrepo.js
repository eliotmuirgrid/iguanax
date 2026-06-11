function GINupdateRepo(){
   let Html = GINrepoHtml();
   let div = document.querySelector(".GINrepo");
   if(div.innerHTML != Html) div.innerHTML = Html;
}

function GINrepoValueHtml(){
   let Repo = GINrepo()?.repo || "";
   let Link = GINrepo()?.link || "";
   if(!Repo) return "<div class='GINvalueEmpty'>Not set</div>";
   return /*html*/`<a href='${Link}' target='_blank'>${Repo}</a>`;
}

function GINrepoHtml(){
   return /*html*/`
   <div class="FORMlabel GINmarginTop">Connected to:</div>
   <div class="GINrepoValue GINmarginTop">${GINrepoValueHtml()}</div>
   <div class="GINpushButton GINpushButtonDiv BUTTONstandard BUTTONdefault"></div>
   <div class="GINeditRepo BUTTONgroup"></div>`;
}

function GINcurrentRepositoryHtml(){
   let Repo = GINrepo()?.repo || "";
   let Link = GINrepo()?.link || "";
   let Args = {};
   Args.GitUrl = Link;
   Args.Repo = Repo;
   Args.UpstreamClass = "GINremoveUpstream";
   return /*html*/`
   <div class="RPOheader">CURRENT REPOSITORY:</div>
   <br>
   <div class="RPOcomponentDiv">${RPOregularUpstreamHtml(Args)}</div>
   <br>`;
}

function GINonSubmit(R, Custom){
   console.log(RPOmidOnSubmitRequest);
   RPOshowProgress();
   let Push = !RPOslugData()?.slug_exists || !Custom;
   let ApiData = {};
   ApiData.git = R;
   ApiData.push = Push;
   APIcall("instance/set_upstream", ApiData, function(R){
      RPOmidOnSubmitRequest = false;
      if(!R.success) SNCKsnackbar("error", "Could not complete setting upstream of instance: " + R?.error || "");
      RPOclose();
      MODELforcePoll("GINmodel");
   });
}

function GINeditRepo(){
   let Data = GINrepo();
   let Args = {};
   Args.title = "Set Instance Repository";
   Args.content = GINcurrentRepositoryHtml(Data);
   Args.onSubmit = GINonSubmit;
   Args.allowConnect = true;
   // Args.precondition = STRprecondition;
   RPOcreateRepoDialog(Args);
}