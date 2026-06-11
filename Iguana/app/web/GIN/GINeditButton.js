function GINupdateEditButton(Selector){
   let div = document.querySelector(Selector);
   if(!div) return;
   let Html = GINeditButton();
   if(div.innerHTML !== Html) div.innerHTML = Html;
}

function GINupdateEditButtons(){
   GINupdateEditButton(".GINeditRepo");
   GINupdateEditButton(".GINeditUser");
   GINupdateEditButton(".GINeditAuto");
   GINupdateEditButton(".GINeditKey");
   GINupdateEditButton(".GINeditRestore");
   GINupdateEditButton(".GINeditRevert");
   GINupdatePushButton();
}

function GINeditButton(){
   let Edit    = GINeditPermission();
   let Classes =  Edit ? "BUTTONdefault" : "BUTTONdisabled";
   let title   = !Edit ? ` title="You do not have permission to edit this."` : "";
   return /*html*/`<a class="${Classes} BUTTONstandard"${title}>EDIT</a>`;
}

function GINupdatePushButton(){
   let Args = {};
   Args.enabled = GINeditPermission();
   Args.target = document.querySelector(`.GINpushButtonDiv`);
   Args.disabledTitle = "You do not have permission to do this.";
   BUTTONcontrol(Args);
}