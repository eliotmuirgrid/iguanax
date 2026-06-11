function AULlisteners(){
   let Card = document.querySelector(".AULmodal");
   if(!Card) return;
   Card.addEventListener("click", AULclickListeners);
   Card.addEventListener("input", AULinputListeners);
   Card.addEventListener("keyup", AULkeyupListeners);
   Card.addEventListener("paste", AULpasteListeners);
}

function AULclickListeners(e){
   if(e.target.closest(".AULclose")) return AULcloseModal();
   if(e.target.closest(".AULadd")) return AULaddComponent();
}

function AULinputListeners(e){
   if(e.target.closest(".AULname")) AULnameTyped = true;
   if(e.target.closest(".AULgit") || e.target.closest(".AULbranch")) {
      AULqueueNameGuess();
      AULsetNameFeedback("");
   }
   if(e.target.closest(".AULname")) AULsetNameFeedback("");
   if(e.target.closest(".AULgit") || e.target.closest(".AULbranch") || e.target.closest(".AULname") || e.target.closest(".AULtagsExtra")) {
      AULrenderFormState();
   }
}

function AULkeyupListeners(e){
   if(e.target.closest(".AULname")) {
      AULnameTyped = true;
      AULsetNameFeedback("");
   }
}

function AULpasteListeners(e){
   if(!e.target.closest(".AULgit")) return;
   setTimeout(function(){
      AULqueueNameGuess();
      AULrenderFormState();
   }, 0);
}

function AULqueueNameGuess(){
   if(AULnameTyped) return;
   clearTimeout(AULguessNameTimer);
   AULguessNameTimer = setTimeout(AULrenderNameGuess, 100);
}

function AULbuildCreateData(){
   let Parsed = AULsyncParsedGitInput();
   return {
      name        : (document.querySelector(".AULname")?.value || "").trim(),
      branch      : Parsed.branch,
      description : (document.querySelector(".AULdescription")?.value || "").trim(),
      git         : Parsed.url,
      tags        : AULtagsForSubmit(),
      editable    : false,
      template    : true
   };
}

function AULcreateErrorText(Response){
   return Response?.error || Response?.data?.error || "Unknown error";
}

function AULhandleCreateError(Response){
   AULinProgress = false;
   let Error = AULcreateErrorText(Response);
   AULrenderFormState();
   if (!LICcheckComponentCreateError(Response)) return;
   SNCKsnackbar("error", Error);
}

function AULhandleCreateSuccess(ComponentData, Response){
   AULcloseModal();
   let Warning = Response?.data?.warning;
   if(Warning) SNCKsnackbar("error", Warning);
   MODELforcePoll("DASHmodel");
   let Guid = AULcreateGuid(Response);
   let Message = GINaddFromUrl(ComponentData.name, ComponentData.git);
   GINaddAndCommit({message: Message, component: Guid, all: true});
}

function AULaddComponent(){
   let Button = document.querySelector(".AULadd");
   if(!Button) return;
   if(Button.classList.contains("BUTTONdisabled")) return;
   if(AULinProgress) return;

   AULclearFeedback();
   AULinProgress = true;
   AULrenderFormState();

   let ComponentData = AULbuildCreateData();
   APIcall("component/create", ComponentData, function(Response){
      if(!AULcreateSuccess(Response)) return AULhandleCreateError(Response);
      return AULhandleCreateSuccess(ComponentData, Response);
   });
}
