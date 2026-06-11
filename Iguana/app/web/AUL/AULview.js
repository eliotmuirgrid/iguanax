function AULrenderDefaults(){
   let Git = document.querySelector(".AULgit");
   let Branch = document.querySelector(".AULbranch");
   let Name = document.querySelector(".AULname");
   let Description = document.querySelector(".AULdescription");
   let TagsExtra = document.querySelector(".AULtagsExtra");
   if(Git && !Git.value.trim()) Git.value = AUL_OFFICIAL_AUTO_LAYOUT_URL;
   if(Branch && !Branch.value.trim()) Branch.value = "";
   if(Name && !Name.value.trim()) Name.value = AULnextAvailableName("Auto Layout Utility");
   if(Description && !Description.value.trim()) Description.value = "Required utility for Auto Layout.";
   if(TagsExtra && !TagsExtra.value.trim()) TagsExtra.value = "#utility";
}

function AULsetNameFeedback(Message){
   let Div = document.querySelector(".AULnameExistsFeedback");
   if(!Div) return;
   Div.textContent = Message || "";
}

function AULclearFeedback(){
   let Repo = document.querySelector(".AULrepoFeedback");
   if(Repo) Repo.textContent = "";
   AULsetNameFeedback("");
}

function AULrenderNameGuess(){
   let Parsed = AULsyncParsedGitInput();
   let Guess = NRLbuildNameGuess(Parsed);
   Guess = AULnextAvailableName(Guess);
   let Input = document.querySelector(".AULname");
   if(!Input || !Guess) return;
   Input.value = Guess;
   AULrenderFormState();
}

function AULupdateWorkingState(){
   let Working = document.querySelector(".AULworking");
   if(!Working) return;
   if(AULinProgress) Working.classList.add("AULworkingActive");
   else Working.classList.remove("AULworkingActive");
}

function AULcanSubmit(){
   let Parsed = AULsyncParsedGitInput();
   let Name = (document.querySelector(".AULname")?.value || "").trim();
   if(AULinProgress) return false;
   if(!Parsed.url) return false;
   if(!Name) return false;
   return true;
}

function AULrenderFormState(){
   let Button = document.querySelector(".AULadd");
   if(!Button) return;
   let Enabled = AULcanSubmit();
   if(Enabled) {
      Button.classList.add("BUTTONaction");
      Button.classList.remove("BUTTONdisabled");
   }
   else {
      Button.classList.add("BUTTONdisabled");
      Button.classList.remove("BUTTONaction");
   }
   AULupdateWorkingState();
}
