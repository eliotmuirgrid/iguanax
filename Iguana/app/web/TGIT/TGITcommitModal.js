function TGITshowGitCommitModal() { 
   TRANSsaveCurrentModuleCursorPosition();
   APIcall("git/credentials", {}, function(Response){ 
      TGITupdateCommitView();
   });
}

function TGITupdateCommitView() {
   APIcall('git/uncommitted_changes', {component: TRANScomponentGuid(), development : TRANSdevelopment()}, function(Response) {
      if(!Response.success) return TGITremoveCommitWindow();
      TGITUcommitInfo = Response.data;
      if(!Object.keys(TGITUcommitInfo).length) { return TGITremoveCommitWindow(); }
      APIcall("component/get_repository", {component: TRANScomponentGuid(), development: TRANSdevelopment()}, function(Repo){
         const Data = Repo?.success ? (Repo?.data || {}) : {};
         TGITcommitModel = {
            branch: Data?.branch || Data?.default_branch || ""
         };
         TGITrenderModal();
         TGITselectFirstFile();
         TGITsetupListeners();
      });
   });
}

function TGITrenderModal() {
   TGITcheckCommitModalExists();
   TGITrenderCommitContext();
   let CommitMsg = document.querySelector('.TGITcommitMsg');
   if(!CommitMsg) return;
   CommitMsg.value  = TGITdefaultCommitMessage();
   document.querySelector('.TGITselectFileToCommit').innerHTML = TGITcreateTreeHtml();
   // Keep COMMIT button state in sync when message is prefilled.
   TGITenableCommit(false);
   FORMfocus(CommitMsg);
}

function TGITcheckCommitModalExists() {
   TGITclearSpinner();
   if(document.querySelector('.TGITgitWindow')) { return; }
   POPpopoutOpen({
      title    : 'Commit Changes',
      content  : TRANSgitCommitWindow(),
      Zindex   : 1005,
      className: 'TGITgitWindow',
      esc_close : true,
      callback : TRANSsetCode
   });
}
// IX-947 Pull into shape with prefixs
function TRANSgitCommitWindow() {
   return /*html*/`
   <div class="TGITcommitContext"></div>
   <div class='TGITdiffView'>
      <div class="TGITpane">
         <div class='TGITselectFileToCommit'></div>
         <div class="TGITresizeControl">
            <div class="TGITresizeShadow"></div>
         </div>
         <div class='TGITselectGroup'>
            <div class='TGITunselectAll'>Unselect All</div>
         </div>
      </div>
      <div class='TGITfileDiff TGITdiffTank'></div>
   </div>
   <div class="TGITcommitDiffRow">
      <textarea class='TGITcommitMsg FORMinput' type='text' placeholder='Enter a commit message'></textarea>
      <div class="TGITcommitButtonArea">
         <div class='TGITcommitMsgFeedback'></div>
         <div class='BUTTONgroup'>
            <div class='TGITcloseCommitModal BUTTONstandard BUTTONcancel'>CANCEL</div>
            <div class='TGITmodalCommit BUTTONstandard BUTTONdisabled'>COMMIT</div>
         </div>
      </div>
   </div>`;
}

function TGITrenderCommitContext(){
   const div = document.querySelector(".TGITcommitContext");
   if(!div) return;
   const Promotion = (typeof TRANSpromotionCurrent === "function") ? TRANSpromotionCurrent() : null;
   if(Promotion?.original_source_branch && Promotion?.target_branch) {
      const source = ESChtmlEscape(Promotion.original_source_branch);
      const target = ESChtmlEscape(Promotion.target_branch);
      div.innerHTML = `<span class="TGITcommitPromotionText"><strong>Promotion Review:</strong> <span class="TGITcommitBranchBadge">${source}</span> ➔ <span class="TGITcommitBranchBadge">${target}</span> (Pending Commit)</span>`;
      return;
   }
   const branch = ESChtmlEscape(TGITcommitModel?.branch || "");
   div.innerHTML = branch
      ? `<span class="TGITcommitBranchLabel">On branch:</span> <code class="TGITcommitBranchName">${branch}</code>`
      : "";
}

function TGITdefaultCommitMessage(){
   const Promotion = (typeof TRANSpromotionCurrent === "function") ? TRANSpromotionCurrent() : null;
   if(Promotion?.original_source_branch && Promotion?.target_branch) {
      return `Promoted '${Promotion.original_source_branch}' to '${Promotion.target_branch}'.`;
   }
   return "";
}


function TGITclearSpinner() {
   const spinner = document.querySelector('.SPINspinner');
   if(spinner) {spinner.remove();}
}

function TGITcreateTreeHtml() {
   let treeHtml = ['<ul class="TGITcommitList">']
   for(const [path, statusArray] of Object.entries(TGITUcommitInfo)) { treeHtml.push(TGITcreateBlock(path, statusArray)) }
   treeHtml.push('</ul>')
   return treeHtml.join('');
}

function TGITcreateBlock(path, statusArray) {
   let isLibrary = TRGTlibraryList()[path] !== undefined;
   let iconClass = isLibrary ? "TGITicon-openDir-library-unlocked" : "TGITicon-openDir";
   if(!path) { path = TRANSdevelopment() ? "dev" : "run"; }
   let rowHtml = [`<li class="${iconClass} TGITcommitTitle" title="${TGITresolveRepo(path)}">${path}<ul class="TGITcommitList">`];
   statusArray.map(file => {
      const path       = file.path ? file.path : file.name; // if path exists it is a submodule, else normal folder
      const fileStatus = TGITsetStatus(file.status);
      rowHtml.push(`<li>${TGITrowHtml(file.name, path, fileStatus)}</li>`);
   });
   rowHtml.push('</ul></li>');
   return rowHtml.join('');
}

function TGITresolveRepo(name) {
   if(!name) { return 'Root of component repo'; }
   let Libraries = TRGTlibraryList();
   return Libraries[name] ? `Commits to library` : 'Commits to component';
}

function TGITrowHtml(name, path, gitStatus) {
   if(!name || !gitStatus.length) { return; }
   return /*html*/`
         <div class='TGITfileSelectRow'>
            <input type="checkbox" class="CHKcheckbox" id="${path}" value="${name}" name="file" checked ${gitStatus[1].charAt(0) === 'S' ? 'disabled' : 's'}>
            <label for="${path}"></label>
            <div class='TGITstatusLetter ${gitStatus[0]}' title='${gitStatus[2]}'>${gitStatus[1]}</div>
            <div class="TGITselectFile">${ESChtmlEscape(name)}</div>
         </div>`;
}

function TGITsetStatus(status) {
   let StatusStaged   = status.charAt(0);
   let StatusUnstaged = status.charAt(1);
   switch(StatusStaged) {
      case 'R':
      case 'M':
      case 'A':
      case 'D': return ['TGITstatusLetter-added', 'S' + StatusStaged, 'Staged file'];
   }
   switch(StatusUnstaged) {
      case 'M': return ['TGITstatusLetter-modified', 'M', 'Modified file'];
      case '?': return ['TGITstatusLetter-added',    'A', 'Added new file'];
      case 'A': return ['TGITstatusLetter-added',    'A', 'Added by remote'];
      case 'D': return ['TGITstatusLetter-deleted',  'D', 'Deleted file'];
      case 'U': return ['TGITstatusLetter-modified', 'U', 'Updated file'];
      case 'R': return ['TGITstatusLetter-modified', 'R', 'Renamed file'];
   }
}
