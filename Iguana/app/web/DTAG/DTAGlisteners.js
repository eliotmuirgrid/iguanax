function DTAGlisteners(){
   let div = document.querySelector(".DTAGmodal");
   if(!div) return;
   div.addEventListener("click", DTAGclick);
}

function DTAGclick(e){
   if(e.target.closest(".DTAGsave"))     return DTAGsaveCall();
   if(e.target.closest(".BUTTONcancel")) return DTAGclose();
}

function DTAGtagsValue(){
   let TagInput = document.querySelector(".DTAGinput").value || "";
   return TagInput.split(/\s+/);
}

function DTAGerror(Error){
   let div = document.querySelector(".DTAGerrorFeedback");
   if(!div) return;
   div.innerHTML = Error || "could not save user filter tags. (Reason unknown)";
}

function DTAGsaveCall(){
   let Data = {};
   Data.filters = DTAGtagsValue();
   APIcall("filter_tags/save", Data, function(R){
      if(!R.success) return DTAGerror(R.error);
      let Message = GINsetTags("filter", R?.data || []);
      GINaddAndCommit({message: Message, users: true});
      MODELforcePoll("DASHmodel");
      DTAGclose();
   });
}

function DTAGclose() {
   DASHcloseModal();
   POPpopoutClose(DTAGcard);
}