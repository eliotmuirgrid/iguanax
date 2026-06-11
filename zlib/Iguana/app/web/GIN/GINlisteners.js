function GINlisteners(){
   let div = document.querySelector(".GINmodal");
   if(!div) return;
   div.removeEventListener("click", GINclickListeners);
   div.addEventListener   ("click", GINclickListeners);
}

function GINclickListeners(e){
   if(!GINeditPermission()) return;
   if(e.target.closest(".GINpushButton")) return GINpushTo();
   if(e.target.closest(".GINeditRepo")) return GINeditRepo();
   if(e.target.closest(".GINeditUser")) return GINeditUser();
   if(e.target.closest(".GINeditAuto")) return GINeditAuto();
   if(e.target.closest(".GINrestore"))  return GINrestore();
   if(e.target.closest(".GINrevert"))   return GINrevert();
   if(e.target.closest(".GINeditKey")) return GINeditKeyTracking();
   if(e.target.closest(".GINwarningDismiss")) return GINdismissWarning();

}

function GINdismissWarning(){
   STORElocalCreate("GINpushWarningDismissed", GINpushWarningError());
   GINupdateWarning();
   MODELforcePoll("CORmodel");
}
