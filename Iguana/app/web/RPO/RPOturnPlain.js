function RPOturnPlain(){
   if(RPOcomponentRunning()) return SNCKsnackbar("error", "Cannot turn to a plain folder while the component is running.")
   const Action = "<b>Confirm:</b><br><br>Do you want to turn this component into a plain folder?<br><br><b>Warning:</b><br><br>This will remove the git repository and any other git related files including the entire history of the repository.<br><br>It will also convert libraries into plain folders.<br><br>";
   CONFIRMaction(Action, RPOturnPlainConfirm, "CONFIRM", RPOturnPlainCancel);
}

function RPOturnPlainCallback(){
   let div = document.querySelector('.RPOsetupWithRemote');
   div.addEventListener('change', function(){
      STORElocalCreate("RPOsetupWithRemoteVar", div.checked);
   });
}

function RPOturnRepo(callback){
   let SetupWithRemote = STORElocalRead("RPOsetupWithRemoteVar") === "true";
   const Action = `
      <b>Confirm:</b>
      <br><br>
      Do you want to turn this component into a repo?
      <br><br>
      <div class='RPOsetupWithRemoteGrid'>
         <input type='checkbox' class='RPOsetupWithRemote' ${SetupWithRemote ? "checked" : ""}>
         <div class='RPOsetupWithRemoteText'>Setup with remote repository</div>
      </div>
      <br>
   `;
   CONFIRMaction(Action, function(){ RPOturnRepoConfirm(callback); }, "CONFIRM", RPOturnRepoCancel, RPOturnPlainCallback);
}

function RPOcomponent(){
   return PAGEhashParameter("component");
}

function RPOcomponentName(Component){
   let List = RPOlistData();
   for(let Item of List){
      if(Item.guid === Component) return Item.name;
   }
   return Component;
}

function RPOturnPlainConfirm(){
   let Component = RPOcomponent();
   if(!Component) return;
   APIcall("component/turn_plain", {component : Component}, function(Data){
      if(!Data.success) SNCKsnackbar("error", "Failed to turn component to plain: " + Data.error);
      let Name = RPOcomponentName(Component);
      GINaddAndCommit({message : 'Turned "' + Name + '" into a plain folder.', component : Component, all : true, callback : RPOclose});
   });
}

function RPOturnRepoConfirm(callback){
   let Component = RPOcomponent();
   CARDredirectToRemote = document.querySelector('.RPOsetupWithRemote')?.checked;
   if(!Component) return;
   APIcall("component/turn_repo", {component : Component}, function(Data){
      if(!Data.success) SNCKsnackbar("error", "Failed to turn component to a repo: " + Data.error);
      let Name = RPOcomponentName(Component);
      GINaddAndCommit({message : 'Turned "' + Name + '" into a repo.', component : Component, all : true, callback : callback});
   });
}

function RPOturnPlainCancel(){
   console.log("Cancel");
}

function RPOturnRepoCancel(){
   console.log("Cancel");
}
