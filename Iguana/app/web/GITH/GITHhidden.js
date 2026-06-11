function GITHneedsUsername(Service){
   return Service.includes("Bitbucket") || Service.includes("Azure Devops");
}

function GITHhideShowDiv(div, Show){
   if(!div) return;
   let HideClass = "GITHhidden";
   if( div.classList.contains(HideClass) &&  Show ) div.classList.remove(HideClass);
   if(!div.classList.contains(HideClass) && !Show ) div.classList.add   (HideClass);
}

function GITHupdateHidden(){
   let Bitbucket     = GITHneedsUsername(GITHgitProvider() || "");
   let ProviderError = GITHserverError();
   GITHhideShowDiv(document.querySelector(".GITHusernameTarget"), !ProviderError && Bitbucket );
   GITHhideShowDiv(document.querySelector(".GITHappPassword"),    !ProviderError);
   GITHhideShowDiv(document.querySelector(".GITHkeyFiles"),       !ProviderError);
   GITHhideShowDiv(document.querySelector(".GITHsshModal"),       !ProviderError);
}