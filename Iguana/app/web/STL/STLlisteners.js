function STLlisteners(){
   let div = document.querySelector(".SETcontentBody");
   if(!div) return;
   div.addEventListener("click", STLclickListener);
}

function STLclickListener(e){
   if(e.target.closest(".SETeditLicense"))         return SETlicenseEdit();
   if(e.target.closest(".SETlicenseCancelUpdate")) return STLlicenseScreen();
   if(e.target.closest(".SETlicenseUpdate"))       return SETlicenseUpdate();
   if(e.target.closest(".ITMcopyFlexDivTable"))    return STLlicenseCopy();
}
