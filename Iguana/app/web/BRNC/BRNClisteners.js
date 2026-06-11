let BRNCconfirm = false;
let BRNCbusy = false;

function BRNClisteners() {
   BRNCsetBusy(false);
   let card = document.querySelector(".BRNCmodal");
   card.addEventListener("click", BRNCclick);
   let input = document.querySelector(".BRNCbranchInput");
   FORMenterListener(input, BRNCsetBranch);
   input.addEventListener("input", BRNCbranchInput);
   FORMfocus(input);
}

function BRNCbranchInput() {
   BRNCclearError();
   BRNCsetConfirmMode(false);
   BRNCupdateButtonSet();
}

function BRNCdismiss(){
   if(!BRNCinline) return BRNCclose();
   POPpopoutClose(BRNCcard);
   TRANSfileCurrentReload();
   MODELforcePoll('TRANSmodel');
   MODELforcePoll('TRANSstatusModel');
   MODELforcePoll("TCMmodel");
}

function BRNCclick(e) {
   if(e.target.closest(".BRNCcancel")) return BRNCdismiss();
   if(e.target.closest(".BRNCsubmit")) return BRNCsetBranch();
}

function BRNCgetBranch() {
   let input = document.querySelector(".BRNCbranchInput");
   return (input.value || "").trim();
}

function BRNCsetBranch() {
   if(BRNCbusy) return;
   BRNCupdateButtonSet();
   let btn = document.querySelector(".BRNCsubmit");
   if(!btn || btn.classList.contains("BUTTONdisabled")) return;
   BRNCclearError();
   BRNCsetBusy(true);
   APIcall("component/set_branch", BRNCbuildSetBranchApiData(), BRNConSetBranchResponse);
}
 
function BRNCbuildSetBranchApiData() {
   const apiData = BRNCapiData();
   apiData.branch = BRNCgetBranch();
   apiData.allow_existing = BRNCconfirm;
   return apiData;
}
 
function BRNConSetBranchResponse(R) {
   BRNCsetBusy(false);
   if (!R?.success) return BRNChandleSetBranchError(R?.error || "");
   const result = R?.data || {}; // { branch, created }
   BRNCfinalizeSetBranchSuccess(BRNCapiData().component, result);
   BRNCsetConfirmMode(false);
   BRNCdismiss();
}
 
function BRNChandleSetBranchError(error) {
   const confirmSwitch = BRNCisConfirmSwitchError(error);
   if (confirmSwitch) {
     BRNCsetConfirmMode(true);
   }
   BRNCsetError(error, confirmSwitch);
}

function BRNCisConfirmSwitchError(error) {
   const text = error || "";
   return text.includes("already exists")
       && text.includes("Switching source branches replaces");
}
 
function BRNCfinalizeSetBranchSuccess(component, result) {
   APIcall("component/detail", { component }, function (R) {
     const data = R?.data || {};
     const message = GINsetBranch(data.name, result?.old_branch, result?.branch, result?.created);
     GINaddAndCommit({ message, component });
     SNCKsnackbar("success", message, 3000);
   });
}

function BRNCsetConfirmMode(on) {
   BRNCconfirm = !!on;
   if(BRNCbusy) return;
   let btn = document.querySelector(".BRNCsubmit");
   if (btn) btn.textContent = BRNCconfirm ? "CONFIRM SWITCH" : "SWITCH SOURCE";
}

function BRNCupdateButtonSet(){
   let Input = document.querySelector('.BRNCbranchInput');
   if(!Input) return;
   let Button = document.querySelector('.BRNCsubmit');
   if(!Button) return;
   let InputValue = Input.value.trim();
   let NotValid = InputValue === "" || InputValue === BRNCbranchName() || BRNCbusy;
   if( Button.classList.contains("BUTTONdisabled") && !NotValid) Button.classList.remove("BUTTONdisabled");
   if(!Button.classList.contains("BUTTONdisabled") &&  NotValid) Button.classList.add   ("BUTTONdisabled");
   if( Button.classList.contains("BUTTONaction") &&  NotValid)   Button.classList.remove("BUTTONaction");
   if(!Button.classList.contains("BUTTONaction") && !NotValid)   Button.classList.add  ("BUTTONaction");
}

function BRNCsetBusy(on){
   BRNCbusy = !!on;
   let Button = document.querySelector('.BRNCsubmit');
   if(Button) Button.textContent = BRNCbusy ? "SWITCHING..." : (BRNCconfirm ? "CONFIRM SWITCH" : "SWITCH SOURCE");
   BRNCupdateButtonSet();
}
