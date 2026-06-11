function CLCcreateCollectionHtml(){
   return /*html*/`
   <div class="RPOheader">WHAT IS A COLLECTION?</div>
   <br>
   <div class="RPOdescription RPOdescriptionPadding">${STCcollectionDescription()}</div>
   <br>
   <br>`;
}

function CLCrender(){
   let Args = {};
   Args.title = "Create a Collection";
   Args.content = CLCcreateCollectionHtml();
   Args.onSubmit = CLConSubmit;
   Args.allowConnect = false;
   Args.precondition = CLCprecondition;
   RPOcreateRepoDialog(Args);
}

function CLCprecondition(callback, failCallback){
   let Perm = STCeditPermission();
   if(Perm) callback();
   else     failCallback();
}

function CLConSubmit(GitUrl){
   let ApiData = {};
   ApiData.git = GitUrl;
   APIcall("git/catalogue/add", ApiData, function(R){
      RPOclose();
      if(!R.success) return;
      let Url = R?.data?.git;
      let Message = GINaddCatalogue(Url);
      GINaddAndCommit({message: Message, settings: true}); 
      let Data = R?.data;
      let GitUrl = Data?.git;
      STCeditMode = false;
      RPOclose();
      MODELforcePoll("STCmodel");
      EDCinit(GitUrl);
   });
}
