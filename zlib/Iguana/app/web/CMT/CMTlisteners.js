let CMTcachedCheckbox = "CMTselectedComponent12345$";

function CMTeditCommitHelpClick(Event){
   let Row = Event.target.closest(".CMTcommitHelpRow");
   if(Row) {
      let Hash = Row.querySelector(".CMTcommitHelpHash").innerText;
      document.querySelector(".CMTcommitHelpValue").value = Hash;
      return;
   }
   if(Event.target.closest(".CMTokAction"))     return CMTconfirmCommit();
   if(Event.target.closest(".CMTcancelAction")) POPpopoutClose(CMTcommitEditPop);
}

function CMTconfirmCommit(){
   let Hash = document.querySelector(".CMTcommitHelpValue").value?.trim();
   if(Hash.length) CMTsetCommit(Hash);
   let RunFromComponent = document.querySelector(".CMTcommitHelpRunFromComponent")?.checked;
   if(RunFromComponent) CMTsetRunFromComponent(CMTcomponentSelected);
   POPpopoutClose(CMTcommitEditPop);
}

function CMTcheckboxDiv(){
   return document.querySelector(".CMTcommitHelpRunFromComponent");
}

function CMTcacheComponentCheckBox() {
   let RunFromComponent = CMTcheckboxDiv();
   if(!RunFromComponent) return;
   let isChecked = RunFromComponent.checked ? 'true' : 'false';
   STORElocalCreate(CMTcachedCheckbox, isChecked);
}

function CMTsetRunFromComponent(Component){
   let div = document.querySelector(".CARDcommitComponentInput");
   let Value = Component;
   if(Component === CMTshowAllOptionId) Value = "";
   if(div) div.value = Value;
}

function CMTsetCommit(Hash){
   let div = document.querySelector(".CARDcommitInputValue");
   if(div) div.value = Hash;
}

function CMTsetListeners(){
   let div = document.querySelector(".CMTcommitHelpModal");
   if(!div) return;
   div.addEventListener("change", CMTchangeEvent);
   div.addEventListener("click",  CMTeditCommitHelpClick);
}

function CMTchangeEvent(e){
   console.log("CMTchangeEvent");
   if(e.target.closest(".CMTcomponentSelectBox")) {
      CMTcomponentSelected = e.target.value;
      MODELforcePoll('CMTmodel');
   }
   if(e.target.closest(".CMTcommitHelpRunFromComponent")) CMTcacheComponentCheckBox();
}
