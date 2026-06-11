function TRANScomponentGuid(){ return PAGEhashParameter("component"); }
function TRANScomponentHide(){ return TRANScomponentGuid() ? "" : " TRANShide"; }

function TRANSselectedFile(){ 
   if(!TRANSsourceDirectory) {
      COL_TRC("No TRANSsourceDirectory - returning NULL");
      return;
   }
   let Result = TRANSsourceDirectory + TRANSfile(); 
   // TODO - why is this called so often
   COL_DBG("Selected file:", Result);
   return Result;
}

function TRANSdevelopment(){ return PAGEhashParameter("development") === 'true'; }

function TRANSprojectRoot(){
   return TRANSworkingDirectory + TRANSsourceDirectory;
}

function TRANSfullPathModule(){
   return TRANSprojectRoot() + TRANSfile();
}

// Basically an alternative to alert().
function TRANSwarning(Message) {
   let Id = "warning_"+Message.replace(/[^0-9a-zA-Z_]/g, '');
   if(WNDfocusOnWindow(Id)) return;
   let Html = `<div class="TRANSwarningDiv">${Message}</div>`
   WNDcreateWindow(id = Id, content = Html, title = "Warning", iconClass = "", PosX = "", PosY = "", height = "0px", width = "0px" );
}

function TRANSrenderViewMode(){
   let Label = document.querySelector(".TRANSviewModeLabel");
   if(!Label) return;
   let LabelText = "";
   if(TRANSdevelopment()) LabelText = "Editable — Using: Development";
   else LabelText = "Viewable — Using Commit: " + TRANScomponentData()?.commit;
   if(Label.innerHTML != LabelText) Label.innerHTML = LabelText;
}

function TRANSrenderBranchLabel(){
   let M = TRGTgitRepo();
   let Label = document.querySelector(".TRANSbranchLabel");
   if(!Label) return;
   let LabelText = "";
   let CurrentBranch = M.branch || "";
   if (CurrentBranch != "") LabelText = "Current branch: " + CurrentBranch; 
   else LabelText = "";
   if(Label.innerHTML != LabelText) Label.innerHTML = LabelText;
}

function TRANSviewingTranslator(){
   return PAGEhash() == "translator";
}

let TRANSpromotionRenderKey = null;
let TRANSpromotionVisible = null;
let TRANSpromotionRenderTarget = null;
let TRANSpromotionPostCommitCard = null;
let TRANSpromotionPostCommitBusy = false;
let TRANSpromotionBannerBusy = false;
let TRANSpromotionExitConfirmCard = null;

function TRANSpromotionCurrent(){
   const state = TRGTpromotionState();
   return state?.active ? state : null;
}

function TRANSpromotionPhase(state){
   return state?.state || "pending_commit";
}

function TRANSpromotionForceHide(){
   const target = document.querySelector(".TRANSpromotionBannerTarget");
   if(!target) return;
   target.style.display = "none";
   target.innerHTML = "";
   TRANSpromotionVisible = false;
   TRANSpromotionRenderKey = "";
}

function TRANSpromotionBannerHtml(state){
   if(!state) return "";
   const source = CARDescapeHtml(state.original_source_branch || "");
   const target = CARDescapeHtml(state.target_branch || "");
   if(TRANSpromotionPhase(state) === "pending_push") {
      return /*html*/`
         <div class="TRANSpromotionBanner">
            <div class="TRANSpromotionText"><strong>Promotion Committed (Beta):</strong> <span class="TRANSpromotionBranchBadge">${source}</span> ➔ <span class="TRANSpromotionBranchBadge">${target}</span> (Pending Push)</div>
            <div class="TRANSpromotionActions">
               <div class="BUTTONstandard BUTTONaction TRANSpromotionPushReturn">Push &amp; Return</div>
               <div class="BUTTONstandard BUTTONdefault TRANSpromotionDismiss">Exit Promotion</div>
            </div>
         </div>`;
   }
   return /*html*/`
      <div class="TRANSpromotionBanner">
         <div class="TRANSpromotionText"><strong>Promoting (Beta):</strong> <span class="TRANSpromotionBranchBadge">${source}</span> ➔ <span class="TRANSpromotionBranchBadge">${target}</span> (Pending Commit)</div>
         <div class="TRANSpromotionActions">
            <div class="BUTTONstandard BUTTONaction TRANSpromotionReview">Review &amp; Commit</div>
            <div class="BUTTONstandard BUTTONdefault TRANSpromotionAbort">Exit Promotion</div>
         </div>
      </div>`;
}

function TRANSrenderPromotionBanner(){
   const target = document.querySelector(".TRANSpromotionBannerTarget");
   if(!target) return;
   const targetChanged = TRANSpromotionRenderTarget !== target;
   if(targetChanged) { TRANSpromotionRenderTarget = target; }
   const state = TRANSpromotionCurrent();
   const visible = !!state;
   const key = state
      ? JSON.stringify({
           c: state.component || "",
           s: state.original_source_branch || "",
           t: state.target_branch || "",
           o: state.original_source_branch || "",
           p: state.state || "",
           x: !!state.has_conflict
        })
      : "";

   // Avoid repaint churn from frequent model polls: only mutate when state changed.
   if(!targetChanged && TRANSpromotionVisible === visible && TRANSpromotionRenderKey === key) return;

   if(TRANSpromotionVisible !== visible) {
      target.style.display = visible ? "block" : "none";
      TRANSpromotionVisible = visible;
   }
   if(TRANSpromotionRenderKey !== key) {
      target.innerHTML = TRANSpromotionBannerHtml(state);
      TRANSpromotionRenderKey = key;
   }
}

function TRANSpromotionResume(){
   const state = TRANSpromotionCurrent();
   if(!state) return;
   if(typeof TGITshowGitCommitModal === "function") TGITshowGitCommitModal();
}

function TRANSpromotionAbort(){
   const state = TRANSpromotionCurrent();
   if(!state) return;
   TRANSpromotionOpenExitConfirm(state);
}

function TRANSpromotionFinalizeAfterCommit(originalBranch = "", targetBranch = ""){
   const state = TRANSpromotionCurrent();
   const original = originalBranch || state?.original_source_branch || state?.source_branch || "";
   const target = targetBranch || state?.target_branch || "";
   if(!original || !target) return;
   TRANSpromotionAdvanceState(function(advanceError){
      if(advanceError) {
         SNCKsnackbar("warning", "Commit succeeded, but promotion state could not be advanced. " + advanceError, 8000);
      }
      MODELforcePoll("TRANSstatusModel");
      TRANSpromotionPostCommitDialog(original, target);
   });
}

function TRANSpromotionAdvanceState(callback){
   const payload = {
      component: TRANScomponentGuid(),
      development: TRANSdevelopment(),
      action: "advance"
   };
   APIcall("component/promote_changes", payload, function(R){
      if(!R?.success) return callback(R?.error || "Could not advance promotion state.");
      callback("");
   });
}

function TRANSpromotionHasUnpushedCommits(){
   if(typeof TRGTanythingAhead !== "function") return true;
   return !!TRGTanythingAhead();
}

function TRANSpromotionPostCommitDialog(originalBranch, targetBranch){
   if(TRANSpromotionPostCommitCard) return;
   const source = CARDescapeHtml(originalBranch);
   const target = CARDescapeHtml(targetBranch);
   const hasUnpushed = TRANSpromotionHasUnpushedCommits();
   const actionHtml = hasUnpushed
      ? `<div class="BUTTONstandard BUTTONaction TRANSpromotionPostCommitConfirm">Push &amp; Return to ${source}</div>`
      : "";
   const noPushHtml = hasUnpushed
      ? ""
      : `<div class="TRANSpromotionPostCommitInfo">No unpushed commits were detected for this promotion.</div>`;
   const content = /*html*/`
      <div class="TRANSpromotionPostCommitBody">
         Your code is committed on <strong>${target}</strong>. Push the target branch to the remote repository,
         then return to <strong>${source}</strong>.
      </div>
      ${noPushHtml}
      <div class="TRANSpromotionPostCommitError" style="display:none;"></div>
      <div class="BUTTONgroup TRANSpromotionPostCommitButtons">
         ${actionHtml}
         <div class="BUTTONstandard BUTTONdefault TRANSpromotionPostCommitStay">Finish Later</div>
      </div>`;
   TRANSpromotionPostCommitBusy = false;
   TRANSpromotionPostCommitCard = POPpopoutOpen({
      title: "Complete Promotion (Beta)",
      content: content,
      width: "92%",
      maxWidth: "560px",
      className: "TRANSpromotionPostCommitModal",
      esc_close: true,
      callback: TRANSpromotionPostCommitReset
   });
   const wrapper = TRANSpromotionPostCommitCard?.element;
   if(!wrapper) return;
   wrapper.addEventListener("click", function(e){
      if(e.target.closest(".TRANSpromotionPostCommitConfirm")) return TRANSpromotionPostCommitConfirm(originalBranch, targetBranch);
      if(e.target.closest(".TRANSpromotionPostCommitStay")) return TRANSpromotionPostCommitStay();
   });
}

function TRANSpromotionPostCommitReset(){
   TRANSpromotionPostCommitBusy = false;
   TRANSpromotionPostCommitCard = null;
}

function TRANSpromotionPostCommitSetBusy(on){
   TRANSpromotionPostCommitBusy = !!on;
   const button = document.querySelector(".TRANSpromotionPostCommitConfirm");
   if(!button) return;
   if(TRANSpromotionPostCommitBusy) {
      button.textContent = "Pushing...";
      button.classList.remove("BUTTONaction");
      button.classList.add("BUTTONdisabled");
   } else {
      const state = TRANSpromotionCurrent();
      const source = CARDescapeHtml(state?.original_source_branch || "");
      button.innerHTML = `Push &amp; Return to ${source}`;
      button.classList.remove("BUTTONdisabled");
      button.classList.add("BUTTONaction");
   }
}

function TRANSpromotionPostCommitSetError(text){
   const div = document.querySelector(".TRANSpromotionPostCommitError");
   if(!div) return;
   const msg = (text || "").trim();
   div.style.display = msg ? "block" : "none";
   div.textContent = msg;
}

function TRANSpromotionRunPush(callback){
   const payload = {
      component: TRANScomponentGuid(),
      development: TRANSdevelopment(),
      action: "push"
   };
   APIcall("component/promote_changes", payload, function(R){
      callback(R?.success ? "" : (R?.error || "Push command failed."), R?.data || {});
   });
}

function TRANSpromotionRunSetBranch(branch, callback){
   const payload = {
      component: TRANScomponentGuid(),
      development: TRANSdevelopment(),
      branch: branch,
      allow_existing: true
   };
   APIcall("component/set_branch", payload, function(R){
      callback(R?.success ? "" : (R?.error || "Could not switch back to original branch."));
   });
}

function TRANSpromotionCompleteAndRefresh(callback){
   const payload = {
      component: TRANScomponentGuid(),
      development: TRANSdevelopment(),
      action: "complete"
   };
   APIcall("component/promote_changes", payload, function(R){
      if(!R?.success) return callback(R?.error || "Could not clear promotion state.");
      TRANSpromotionForceHide();
      MODELforcePoll("TRANSstatusModel");
      MODELforcePoll("TRANSmodel");
      callback("");
   });
}

function TRANSpromotionPostCommitClose(){
   if(!TRANSpromotionPostCommitCard) return;
   POPpopoutClose(TRANSpromotionPostCommitCard);
}

function TRANSpromotionPushSuccessSnackbar(originalBranch, targetBranch, pushResult){
   const source = CARDescapeHtml(originalBranch);
   const target = CARDescapeHtml(pushResult?.target_branch || targetBranch || "target branch");
   const url = (pushResult?.web_branch_url || "").trim();
   if(!url) {
      SNCKsnackbar("success", `Pushed changes and returned to "${originalBranch}".`, 5000);
      return;
   }
   const href = CARDescapeAttr(url);
   SNCKsnackbar(
      "success",
      `<span>Pushed changes to <a target="_blank" rel="noopener noreferrer" href="${href}">${target}</a> and returned to "${source}".</span>`,
      8000,
      false
   );
}

function TRANSpromotionPostCommitConfirm(originalBranch, _targetBranch){
   if(TRANSpromotionPostCommitBusy) return;
   if(!TRANSpromotionHasUnpushedCommits()) {
      TRANSpromotionPostCommitSetError("No unpushed commits were detected.");
      return;
   }
   TRANSpromotionPostCommitSetError("");
   TRANSpromotionPostCommitSetBusy(true);
   TRANSpromotionRunPush(function(pushError, pushResult){
      if(pushError) {
         TRANSpromotionPostCommitSetError(pushError);
         return TRANSpromotionPostCommitSetBusy(false);
      }
      TRANSpromotionRunSetBranch(originalBranch, function(branchError){
         if(branchError) {
            TRANSpromotionPostCommitSetError(branchError);
            return TRANSpromotionPostCommitSetBusy(false);
         }
         TRANSpromotionCompleteAndRefresh(function(doneError){
            if(doneError) {
               TRANSpromotionPostCommitSetError(doneError);
               return TRANSpromotionPostCommitSetBusy(false);
            }
            TRANSpromotionPushSuccessSnackbar(originalBranch, _targetBranch, pushResult);
            TRANSpromotionPostCommitClose();
         });
      });
   });
}

function TRANSpromotionPostCommitStay(){
   if(TRANSpromotionPostCommitBusy) return;
   TRANSpromotionPostCommitClose();
}

function TRANSpromotionPushReturnFromBanner(){
   const state = TRANSpromotionCurrent();
   if(!state || TRANSpromotionPhase(state) !== "pending_push") return;
   const originalBranch = state?.original_source_branch || state?.source_branch || "";
   const targetBranch = state?.target_branch || "";
   if(!originalBranch) return SNCKsnackbar("error", "Could not determine original source branch.");
   if(!targetBranch) return SNCKsnackbar("error", "Could not determine target branch.");
   TRANSpromotionPostCommitDialog(originalBranch, targetBranch);
}

function TRANSpromotionDismissPendingPush(){
   const state = TRANSpromotionCurrent();
   if(!state || TRANSpromotionPhase(state) !== "pending_push") return;
   TRANSpromotionOpenExitConfirm(state);
}

function TRANSpromotionOpenExitConfirm(state){
   if(TRANSpromotionExitConfirmCard) return;
   const phase = TRANSpromotionPhase(state);
   const target = CARDescapeHtml(state?.target_branch || "the target branch");
   const bodyText = phase === "pending_push"
      ? `You are on <strong>${target}</strong> with unpushed promotion commits. If you exit now, Iguana will stop tracking this promotion and you will need to push and switch branches manually.`
      : `You are in the middle of promoting changes to <strong>${target}</strong>. If you exit now, Iguana will abort the in-progress promotion and restore your original source branch.`;
   const content = /*html*/`
      <div class="TRANSpromotionPostCommitBody">
         ${bodyText}
      </div>
      <div class="BUTTONgroup TRANSpromotionPostCommitButtons">
         <div class="BUTTONstandard BUTTONdefault TRANSpromotionExitCancel">Cancel</div>
         <div class="BUTTONstandard BUTTONcancel BUTTONwarning TRANSpromotionExitConfirm">Exit Anyway</div>
      </div>`;
   TRANSpromotionExitConfirmCard = POPpopoutOpen({
      title: "Exit Promotion? (Beta)",
      content: content,
      width: "650px",
      className: "TRANSpromotionExitConfirmModal",
      esc_close: true,
      callback: function(){ TRANSpromotionExitConfirmCard = null; }
   });
   const wrapper = TRANSpromotionExitConfirmCard?.element;
   if(!wrapper) return;
   wrapper.addEventListener("click", function(e){
      if(e.target.closest(".TRANSpromotionExitCancel")) return TRANSpromotionCloseExitConfirm();
      if(e.target.closest(".TRANSpromotionExitConfirm")) return TRANSpromotionConfirmExit(state);
   });
}

function TRANSpromotionCloseExitConfirm(){
   if(!TRANSpromotionExitConfirmCard) return;
   POPpopoutClose(TRANSpromotionExitConfirmCard);
}

function TRANSpromotionConfirmExit(state){
   if(TRANSpromotionBannerBusy) return;
   const phase = TRANSpromotionPhase(state);
   TRANSpromotionBannerBusy = true;
   if(phase === "pending_push") {
      TRANSpromotionCompleteAndRefresh(function(doneError){
         TRANSpromotionBannerBusy = false;
         if(doneError) return SNCKsnackbar("error", doneError, 9000);
         TRANSpromotionCloseExitConfirm();
         SNCKsnackbar("success", "Promotion state dismissed. Staying on the target branch.", 5000);
      });
      return;
   }
   const payload = {
      component: TRANScomponentGuid(),
      development: TRANSdevelopment(),
      action: "abort"
   };
   APIcall("component/promote_changes", payload, function(R){
      TRANSpromotionBannerBusy = false;
      if(!R?.success) return SNCKsnackbar("error", R?.error || "Could not abort promotion.");
      TRANSpromotionCloseExitConfirm();
      TRANSpromotionForceHide();
      MODELforcePoll("TRANSstatusModel");
      SNCKsnackbar("success", "Promotion aborted. Restored source branch.", 5000);
   });
}
