function RPOlisteners(){
   let Div = document.querySelector(".RPOlistenerDiv");
   Div.addEventListener("click",  function(e){ RPOclickListener(e); });
   Div.addEventListener("change", function(e){ RPOchangeListeners(e); });
   Div.addEventListener("input",  function(e){ RPOinputListeners(e); });
}

function RPOclickListener(e){
   if(e.target.closest(".RPOconfirm"))       { RPOconfirmSubmit();       }
   if(e.target.closest(".RPOconfirmCustom")) { RPOconfirmCustomSubmit(); }
   if(e.target.closest(".RPOtryAgain"))      { RPOtryAgain(); }
   if(e.target.closest(".RPOupdateOwners"))  { RPOupdateOwners(); }
   if(e.target.closest(".RPOredirectBitbucket")) { window.location.hash = "#settings/git"; }
   if(e.target.closest(".RPOredirectGit"))       { window.location.hash = "#settings"; }
   if(e.target.closest(".RPOcancel"))        { POPpopoutClose(RPOcard); }
   if(e.target.closest(".RPOturnToPlain"))   { RPOturnPlain(RPOclose); }
   if(e.target.closest(".CRPremoveUpstream")) { CRPremoveUpstream(); }
   if(e.target.closest(".GINremoveUpstream")) { GINremoveUpstream(); }
   if(e.target.closest(".LPRremoveUpstream")) { LPRremoveUpstream(); }
   if(e.target.closest(".RPOturnToRepo"))    { RPOturnRepo(RPOclose); }
}

function RPOchangeListeners(e){
   if(e.target.closest('.RPOslug'))       { return RPOgitUrlError = ""; }
   if(e.target.closest('.RPOworkspaces')) { return RPOsaveWorkspaceSelected(); }
}

function RPOinputListeners(e){
   if(e.target.closest('.RPOslug'))         { return RPOgitUrlError = ""; }
   if(e.target.closest('.RPOgitUrlCustom')) { 
      MODELupdate("RPOmodel");
      RPOupdateCustomButton();
   }
}

function RPOsaveWorkspaceSelected(){
   let Div = document.querySelector('.RPOworkspaces');
   if(!Div) return;
   let Value = Div.value;
   STORElocalCreate("RPOselectedWorkspace", Value);
   MODELforcePoll("RPOmodel");
}

function RPOgetApiData() {
   let ApiData        = {};
   ApiData.owner      = document.querySelector('.RPOworkspaces').value.trim();
   ApiData.repository = document.querySelector('.RPOslug').value.trim();
   return ApiData;
}

function RPOconfirmSubmit(){
   if(RPOisDisabled()) return;
   RPOmidOnSubmitRequest = true;
   const SlugExists = RPOslugData().slug_exists;
   if(SlugExists) {
      let Git = document.querySelector('.RPOgitUrl').value;
         RPOmidOnSubmitRequest = false;
         return RPOonsubmitCallback(Git, !SlugExists);
   }
   APIcall("git/create/repo", RPOgetApiData(), function(R){ 
      if (!R.success){
         let ErrorMessage = R.error;
         RPOgitUrlError = ErrorMessage;
         RPOmidOnSubmitRequest = false;
         return;
      }
      RPOonsubmitCallback(R.data, true);
   });
}

function RPOonsubmitCallback(Data, Push){
   if(RPOonSubmit) RPOonSubmit(Data, Push);
   else RPOclose();
}

function RPOconfirmCustomSubmit(){
   if(RPOisCustomDisabled()) return;
   let Div = document.querySelector(".RPOgitUrlCustom");
   if(!Div) return;
   let Div2 = document.querySelector(".RPOfeedbackCustom");
   if(!Div2) return;
   if(Div.value == "") return Div2.innerHTML = "Provided git url must not be empty.";
   RPOmidOnSubmitRequest = true;
   RPOonsubmitCallback(Div.value.trim(), false);
}