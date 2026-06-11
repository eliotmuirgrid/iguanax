function ADCcreateRender(){
   let Args = {};
   Args.title = "Create a Collection";
   Args.content = CLCcreateCollectionHtml();
   Args.onSubmit = ADConSubmit;
   Args.allowConnect = false;
   Args.precondition = ADCprecondition;
   RPOcreateRepoDialog(Args);
}

function ADCprecondition(callback, failCallback){
   APIcall("component/permission", {}, function(R){
      if(!R.success) return;
      let data = R?.data || {};
      let Perm = data?.edit_collections;
      if(Perm) callback();
      else     failCallback();
   });
}

function ADConSubmit(GitUrl){
   let ApiData = {};
   ApiData.git = GitUrl;
   APIcall("git/catalogue/add", ApiData, function(R){
      if(R.success) { 
         let Url = R?.data?.git;
         let Message = GINaddCatalogue(Url);
         GINaddAndCommit({message: Message, settings: true}); 
      }
      RPOclose();
      ADCrender(ADCaddedData, R.data.git);
   });
}
