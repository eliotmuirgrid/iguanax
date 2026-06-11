function RPOnoValidWorkspaceCache(){
   if(RPOvalidWorkspaceCache()) return;
   MODELremove("RPOmodel");
   RPOsetUpBitbucket();
}

function RPOsetUpBitbucket(){
   let Div = document.querySelector(".RPOcloudDiv");
   Div.innerHTML = RPOmustSetupCloudCredentialsHtml();
}

function RPOupdateButton(){
   let SlugData = RPOslugData();
   if(!SlugData) return;
   const confirmBtn = document.querySelector('.RPOconfirm');
   if(RPOworkspaceError || RPOslugError || RPOgitUrlError) {
      RPOsetConfirmButtonState(false, confirmBtn);
      return;
   }
   let enabled = !RPOallowConnect ? SlugData?.slug_exists !== true : true;
   RPOsetConfirmButtonState(enabled, confirmBtn);
}

function RPOupdateCustomButton(){
   const confirmCustomBtn = document.querySelector('.RPOconfirmCustom');
   const customUrlInput = document.querySelector('.RPOgitUrlCustom')?.value.trim() || "";
   let enabled = customUrlInput != "";
   RPOsetConfirmButtonState(enabled, confirmCustomBtn);
}

function RPOsetConfirmButtonState(Enable, Button) {
   if(!Button) { return; }
   if (Enable) {
      Button.classList.add('BUTTONaction');
      Button.classList.remove('BUTTONdisabled');
   } else {
      Button.classList.add('BUTTONdisabled');
      Button.classList.remove('BUTTONaction');
   }
}
