let PRMCbusy = false;
let PRMCreviewTimer;

function PRMClisteners() {
   let card = document.querySelector(".PRMCmodal");
   card.addEventListener("click", PRMCclick);
   for(const input of document.querySelectorAll(".PRMCsourceInput, .PRMCtargetInput")){
      FORMenterListener(input, PRMCpromote);
      input.addEventListener("input", PRMCbranchInput);
   }
   FORMfocus(document.querySelector(".PRMCtargetInput"));
   PRMCupdateButtonSet();
}

function PRMCbranchInput() {
   PRMCclearError();
   PRMCupdateButtonSet();
}

function PRMCdismiss(){
   if(!PRMCinline) return PRMCclose();
   POPpopoutClose(PRMCcard);
   TRANSfileCurrentReload();
   MODELforcePoll('TRANSmodel');
   MODELforcePoll('TRANSstatusModel');
   MODELforcePoll("TCMmodel");
}

function PRMCclick(e) {
   if(e.target.closest(".PRMCcancel")) return PRMCdismiss();
   if(e.target.closest(".PRMCsubmit")) return PRMCpromote();
}

function PRMCgetSourceBranch() {
   let input = document.querySelector(".PRMCsourceInput");
   return (input?.value || "").trim();
}

function PRMCgetTargetBranch() {
   let input = document.querySelector(".PRMCtargetInput");
   return (input?.value || "").trim();
}

function PRMCbuildPromoteApiData() {
   const apiData = PRMCapiData();
   apiData.action = "start";
   apiData.source_branch = PRMCgetSourceBranch();
   apiData.target_branch = PRMCgetTargetBranch();
   apiData.promotion_type = PRMCgetPromotionType();
   return apiData;
}

function PRMCgetPromotionType() {
   return document.querySelector('input[name="PRMCpromotionType"]:checked')?.value || "squash";
}

function PRMCpromote() {
   if(PRMCbusy || PRMCformInvalid()) return;
   PRMCclearError();
   PRMCsetBusy(true);
   APIcall("component/promote_changes", PRMCbuildPromoteApiData(), PRMConPromoteResponse);
}

function PRMConPromoteResponse(R) {
   PRMCsetBusy(false);
   if(!R?.success) return PRMCsetError(R?.error || "Could not promote changes.");
   const Result = R?.data || {};
   if(PRMCpromoteHasConflict(R, Result)) return PRMCopenConflictReview(Result);
   if(Result?.merge_staged) return PRMCopenCommitReview(Result);
   PRMCfinalizePromoteSuccess(Result);
   PRMCdismiss();
}

function PRMCfinalizePromoteSuccess(result) {
   const source = result?.source_branch || PRMCgetSourceBranch();
   const target = result?.target_branch || PRMCgetTargetBranch();
   const targetCreated = !!result?.target_branch_created;
   let message = "";
   if(result?.no_changes && targetCreated) {
      message = `Created target branch "${target}" because it did not exist yet. No merge commit was needed, and the branch now contains the full Git history from "${source}".`;
   } else if(result?.no_changes) {
      message = `No changes to promote from "${source}" to "${target}".`;
   } else {
      message = `Promoted changes from "${source}" to "${target}".`;
   }
   if(result?.warning) {
      SNCKsnackbar("warning", message + " " + result.warning, 8000);
   } else {
      const duration = (result?.no_changes && targetCreated) ? 9000 : 5000;
      SNCKsnackbar("success", message, duration);
   }
   MODELforcePoll('TRANSmodel');
   MODELforcePoll('TRANSstatusModel');
}

function PRMCopenCommitReview(result) {
   const component = PRMCcomponentId();
   const source = result?.source_branch || PRMCgetSourceBranch();
   const target = result?.target_branch || PRMCgetTargetBranch();
   PRMCcloseForReview();
   SNCKsnackbar("success", `Prepared changes from "${source}" into "${target}". Review the staged files and commit when ready.`, 5000);
   MODELforcePoll('TRANSstatusModel');
   PRMCprepareTranslatorForReview(component);
}

function PRMCopenConflictReview(result) {
   const component = PRMCcomponentId();
   const source = result?.source_branch || PRMCgetSourceBranch();
   const target = result?.target_branch || PRMCgetTargetBranch();
   const conflictFile = PRMCfirstConflictFile(result);
   PRMCcloseForReview();
   SNCKsnackbar("warning", "Merge conflict detected. Resolve the conflict markers in the editor, then use Review & Commit from the banner.", 9000);
   MODELforcePoll('TRANSstatusModel');
   if(conflictFile) PRMCopenConflictFile(component, conflictFile);
}

function PRMCpromoteHasConflict(response, result) {
   if(result?.merge_conflicts || result?.has_conflict) return true;
   const text = ((result?.conflict_output || "") + "\n" + (response?.error || "")).toLowerCase();
   return text.includes("merge conflict") || text.includes("conflict");
}

function PRMCfirstConflictFile(result) {
   const fromList = result?.conflict_files;
   if(Array.isArray(fromList) && fromList.length > 0) {
      const first = (fromList[0] || "").trim();
      if(first) return first;
   }
   const output = (result?.conflict_output || "").trim();
   if(!output) return "";
   const lines = output.split(/\r?\n/);
   for(const line of lines) {
      const inMatch = line.match(/CONFLICT.* in (.+)$/);
      if(inMatch?.[1]) return inMatch[1].trim();
      const bothModifiedMatch = line.match(/both modified:\s*(.+)$/);
      if(bothModifiedMatch?.[1]) return bothModifiedMatch[1].trim();
   }
   return "";
}

function PRMCopenConflictFile(component, fileName) {
   if(!PRMCtranslatorContext() || TRANScomponentGuid() !== component) return;
   const currentFile = (typeof TRANSfile === "function") ? TRANSfile() : "";
   if(currentFile === fileName) {
      TRANSfileCurrentReload();
      return;
   }
   TRANSfileChange(fileName);
   // Force a reload so merge-conflict markers appear immediately.
   TRANSfileCurrentReload();
}

function PRMCcloseForReview() {
   const card = PRMCcard;
   PRMCcard = null;
   PRMCinline = false;
   if(card) POPpopoutClose({id: card.id});
}

function PRMCprepareTranslatorForReview(component) {
   if(PRMCreviewTimer) clearTimeout(PRMCreviewTimer);
   if(!PRMCtranslatorContext() || TRANScomponentGuid() !== component) {
      SNCKsnackbar("warning", "The merge is staged, but the review modal can only open from the editable translator view.", 8000);
      return;
   }
   TRANSfileCurrentReload();
   PRMCwaitForCommitReview(component, 0);
}

function PRMCwaitForCommitReview(component, attempt) {
   const Ready = PAGEhash() === "translator" &&
                 TRANScomponentGuid() === component &&
                 TRANSdevelopment() &&
                 document.querySelector(".TRANSwindowEdit") &&
                 typeof TGITshowGitCommitModal === "function";
   if(Ready) {
      TGITshowGitCommitModal();
      return;
   }
   if(attempt >= 80) {
      SNCKsnackbar("warning", "The merge is staged, but the review modal could not be opened automatically. Open Commit Changes from the translator toolbar.", 8000);
      return;
   }
   PRMCreviewTimer = setTimeout(function(){ PRMCwaitForCommitReview(component, attempt + 1); }, 250);
}

function PRMCsetBusy(on) {
   PRMCbusy = !!on;
   let btn = document.querySelector(".PRMCsubmit");
   if(btn) btn.textContent = PRMCbusy ? "Preparing..." : "Review & Promote";
   PRMCupdateButtonSet();
}

function PRMCformInvalid() {
   const Source = PRMCgetSourceBranch();
   const Target = PRMCgetTargetBranch();
   return Source === "" || Target === "" || Source === Target;
}

function PRMCupdateButtonSet(){
   let Button = document.querySelector('.PRMCsubmit');
   if(!Button) return;
   let NotValid = PRMCformInvalid() || PRMCbusy;
   if( Button.classList.contains("BUTTONdisabled") && !NotValid) Button.classList.remove("BUTTONdisabled");
   if(!Button.classList.contains("BUTTONdisabled") &&  NotValid) Button.classList.add   ("BUTTONdisabled");
   if( Button.classList.contains("BUTTONaction") &&  NotValid)   Button.classList.remove("BUTTONaction");
   if(!Button.classList.contains("BUTTONaction") && !NotValid)   Button.classList.add  ("BUTTONaction");
}
