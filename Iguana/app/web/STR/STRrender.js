function STRrender(){
   DASHdashboardScreen();
   if(document.querySelector('.STRmodal')) { return; }
   let ApiData = {};
   ApiData.component = PAGEhashParameter("component");
   ApiData.development = true;
   APIcall("component/get_repository", ApiData, function(R){
      if(!R.success){ 
         window.location.hash = "dashboard";
         return SNCKsnackbar("error", "Could not get upstream repository details: " + R.error); 
      }
      STRonRequestSuccess(R?.data || {});
   });
}

function STRonSubmit(R, Push){
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
      APIcall("component/detail", {component: ApiData.component}, function(R){
         let Data = R?.data || {};
         let Message = GINsetUpstream(Data.name, ApiData.git);
         GINaddAndCommit({message: Message, component: ApiData.component});
      });
      RPOclose();
   });
}

function STRprecondition(callback, failCallback){
   let ApiData = {};
   ApiData.component = PAGEhashParameter("component");
   APIcall("component/permission", ApiData, function(R){
      if(!R.success) return;
      let data = R?.data || {};
      let Perm = data?.edit_upstream;
      if(Perm) callback();
      else     failCallback();
   });
}

function STRonRequestSuccess(Data){
   let Args = {};
   Args.title = "Set Git URL";
   Args.onCancel = STRclose;
   Args.content = CRPcurrentRepositoryHtml(Data, true);
   Args.onSubmit = STRonSubmit;
   Args.allowConnect = true;
   Args.precondition = STRprecondition;
   RPOcreateRepoDialog(Args);
}

function STRclose() {
   let Data = {};
   Data.component = PAGEhashParameter("component");
   Data.filter    = PAGEhashParameter("filter");
   let Url = "dashboard/details" + PAGEformatParams(Data);
   window.location.hash = Url;
}