let TGITUcommitInfo = {};
let TGITcommitModel = {};
let TGITpromotionCommitContext = null;
const TGITnull = "TGITnull1234_432";

function TGITgitHelpHtml(){
   return /*html*/`Cannot perform operation because of an issue with the git configuration.<br><br> Go to <a href="#settings?page=git_setup">Settings &gt; Git Setup</a> to resolve the issue.`
}

function TGITcapturePromotionContext() {
   const Promotion = (typeof TRANSpromotionCurrent === "function") ? TRANSpromotionCurrent() : null;
   const original = Promotion?.original_source_branch || Promotion?.source_branch || "";
   const target = Promotion?.target_branch || "";
   if(!original || !target) return null;
   return {
      original_source_branch: original,
      target_branch: target
   };
}

function TGITremoveCommitWindow(promotionContext, shouldFinalizePromotion) {
   let GitWindow = document.querySelector('.TGITgitWindow');
   if(GitWindow && GitWindow.parentElement) {
      GitWindow.parentElement.remove();
   }
   if(typeof TRANSsetCode === "function") TRANSsetCode();
   MODELforcePoll('TRANSstatusModel');
   if(shouldFinalizePromotion) {
      const context = promotionContext || TGITpromotionCommitContext;
      try {
         TRANSpromotionFinalizeAfterCommit(context?.original_source_branch || "", context?.target_branch || "");
      } catch(E) {
         SNCKsnackbar("error", "Commit succeeded, but the post-commit promotion dialog could not be opened.");
      }
   }
   TGITpromotionCommitContext = null;
   MODELforcePoll("LBUmodel");
}

function TGITselectFirstFile() {
   let FirstLib = Object.keys(TGITUcommitInfo)[0];
   let FileName = TGITUcommitInfo[FirstLib][0].name;
   TGITfileDiff(FileName, FirstLib);
}

function TGITunselectAll(){
   let Boxes = document.querySelectorAll('.TGITselectFileToCommit .CHKcheckbox');
   if (!Boxes.length) return;
   Boxes.forEach(box => {
      box.checked = false;
   });
   TGITenableCommit();
}

function TGITresolveFileDiff(item) {
   if(!item) { return; }
   let fullPath = item.firstElementChild.id;
   let fileName = item.firstElementChild.value;
   if(fullPath === fileName) { return TGITfileDiff(fileName); } // we are at the top level
   let relativePath = fullPath.substring(0, fullPath.length - fileName.length - 1); // -1 to remove trailing FIL_SEP
   return TGITfileDiff(fileName, relativePath);
}

function TGITfileDiff(File, RelativePath){
   let FileName = TRGTconvertToFileDiffName(File);
   let data = {component: TRANScomponentGuid(), file_name: FileName, development: TRANSdevelopment()};
   COL_TRC(File, RelativePath, data);
   if (RelativePath && RelativePath !== TRANScomponentGuid()) data.relative_path = RelativePath;
   APIcall('htool/file_diff', data, function(Response){
      if(!Response.success) return SNCKsnackbar('error', Response.error);
      Response = Response.data;
      Response.file = File;
      let Html = TGITdiffTemplate(Response);
      document.querySelector('.TGITfileDiff').innerHTML = Html;
      TGITsetViewModeSelector();
      TGITselectDiffView(STORElocalRead('TGITdiffViewSelectValue'), Response);
      let diffViewSelect = document.querySelector('#TGITdiffViewSelect');
      diffViewSelect.addEventListener('change', function(){ TGITdiffViewSelectHandler(Response); });
      FORMfocus(document.querySelector('.TGITcommitMsg'));
   });
}

function TGITdiffViewSelectHandler(Response) {
   let Value = document.querySelector('#TGITdiffViewSelect').value;
   COL_TRC('Select=', Value);
   STORElocalCreate('TGITdiffViewSelectValue', Value);
   TGITselectDiffView(STORElocalRead('TGITdiffViewSelectValue'), Response);
}

function TGITexecuteCommit() {
   const promotionContext = TGITcapturePromotionContext();
   TGITpromotionCommitContext = promotionContext;
   TGITcreateSpinner();
   const commandChain = TGITcreateCommandChain();
   document.querySelector('.TGITcommitDiffRow').style.display = 'none';
   if(PAGEhash() == "documents") return DOCcommit(commandChain);
   if(PAGEhash() == "documents2") return DOCcommit(commandChain);
   TGITopenBranchTerminal(function() {
      TERMexecuteCommand(commandChain, function(){
         TGITremoveCommitWindow(promotionContext, true);
      });
   });
}

function TGITcreateCommandChain() {
   let commandChain = [];
   let commandMap = TGITmapGitAddRm();
   for(const [path, fileList] of Object.entries(commandMap)) {
      if(path !== TGITnull) {
         let Add = fileList['add'];
         let Rem = fileList['rm'];
         if(Add.length)               commandChain.push(TGITcreateCommands(path, 'add', Add));
         if(Rem.length)               commandChain.push(TGITcreateCommands(path, 'rm', Rem )); 
         if(Add.length || Rem.length) commandChain.push(TGITcreateCommitCommand(path)); 
      }
   }
   // component commits need to happen after submodules otherwise you will have to commit twice
   if(commandMap[TGITnull]) {
      let Add = commandMap[TGITnull]['add'];
      let Rem = commandMap[TGITnull]['rm'];
      if(Add.length)               commandChain.push(TGITcreateCommands(TGITnull, 'add', Add)); 
      if(Rem.length)               commandChain.push(TGITcreateCommands(TGITnull, 'rm', Rem)); 
      if(Add.length || Rem.length) commandChain.push(TGITcreateCommitCommand(TGITnull)); 
   }
   return commandChain;
}

function TGITescapeQuotationMarks(str){
   return str.replace(/"/g, '\\"');
}

//  TODO - Rewrite this to not look up the values in the DOM
function TGITmapGitAddRm() {
   let commandMap   = {};
   let commitList   = document.querySelectorAll('.TGITfileSelectRow');  
   for(let i = 0; i < commitList.length; i++) {
      const File = commitList[i];
      let ParentDir    = File.parentElement.parentElement.parentElement.firstChild.nodeValue;
      if(File.closest(".TGITicon-openDir")) { ParentDir = TGITnull; }
      const Checked      = File.children[0].checked;
      if(!Checked) { continue; }
      const StatusLetter = TGITescapeQuotationMarks(File.children[2].innerHTML);
      let Name           = File.children[3].innerHTML;
      let FileNew        = TRGTconvertToGitName(Name);
      const FileName     = TGITescapeQuotationMarks(FileNew);
      let OrigFileName   = TRGTconvertToGitPreviousName(Name);
      if(!commandMap[ParentDir]) { commandMap[ParentDir] = {add: [], rm:[]};}
      StatusLetter.toLowerCase() === 'd' ? commandMap[ParentDir]['rm'].push(`"${FileName}"`) : commandMap[ParentDir]['add'].push(`"${FileName}"`);
      if(OrigFileName) commandMap[ParentDir]['rm'].push(`"${OrigFileName}"`);
   }
   return commandMap;
}

function TGITcreateCommands(path, command, fileList) {
   if(!path || !fileList) { return []; }
   const location = path === TGITnull ? '' : ` -C "${path}"`;
   return `git${location} ${command} ${fileList.join(' ')}`;
}

function TGITcreateCommitCommand(path){
   const Message = document.querySelector('.TGITcommitMsg').value;
   const location = path === TGITnull ? '' : ` -C "${path}"`;
   return `git${location} commit -m "${Message.replace(/"/g, '\'')}"`;
}

function TGITcreateSpinner() {
   let SpinnerDiv = document.createElement('div');
   SpinnerDiv.classList.add('SPINspinner');
   document.querySelector('.TGITmodalCommit').parentElement.after(SpinnerDiv);
}

function TGITsetViewModeSelector(){
   COL_TRC('TGITsetViewModeSelector');
   let StoredViewMode = STORElocalRead('TGITdiffViewSelectValue');
   if(StoredViewMode !== 'condensed' && StoredViewMode !== 'vertical') STORElocalCreate('TGITdiffViewSelectValue', 'vertical');
   document.querySelector('#TGITdiffViewSelect').value = STORElocalRead('TGITdiffViewSelectValue');
}

function TGITscrollToFirstDiffLine(){ 
   const Left         = document.querySelector('.TGITdiffFrom')
   const ChangeOffset = document.querySelector('.TGITchangeDiff') ? document.querySelector('.TGITchangeDiff').offsetTop : Infinity
   const BlankOffset  = document.querySelector('.TGITdumDiff')    ? document.querySelector('.TGITdumDiff').offsetTop    : Infinity
   const FirstDiffTop = Math.min(ChangeOffset, BlankOffset)
   TGITscrollSource = Left
   Left.scrollTo({
      top:       FirstDiffTop,
      behavior: 'smooth'
   });
}

function TGITselectDiffView(ViewMode, diffResponse) {
   COL_TRC('ViewMode=', ViewMode, 'diffResponse=', diffResponse);
   if(ViewMode === 'condensed') {
      if(!diffResponse.condensed) { return document.querySelector('.TGITdiff').innerHTML = `<div class="TGITdiffBlock">${diffResponse.status}</div>`; }
      document.querySelector('.TGITdiff').innerHTML = `<div class='TGITcondensedDiff'>${diffResponse.condensed}</div>`;
   }
   else {
      if(!diffResponse.full) { return document.querySelector('.TGITdiff').innerHTML = `<div class="TGITdiffBlock" '>${diffResponse.status}</div>`; }
      document.querySelector('.TGITdiff').innerHTML = TGITfullDiff(diffResponse.full.from, diffResponse.full.to);
      TGITremoveExtraLineNumbers()
      TGITsynchronizeScroll()
      TGITscrollToFirstDiffLine()
   }
}

let TGITscrollSource;
function TGITsetScrollSource() { TGITscrollSource = this }

function TGITsynchronizeScroll() {
   const Left  = document.querySelector('.TGITdiffFrom')
   const Right = document.querySelector('.TGITdiffTo')
   if(!Left || !Right) { return }
   Left.addEventListener ('wheel',     TGITsetScrollSource)
   Right.addEventListener('wheel',     TGITsetScrollSource)
   Left.addEventListener ('mouseover', TGITsetScrollSource)
   Right.addEventListener('mouseover', TGITsetScrollSource)
   Left.addEventListener ('scroll', function(){
      if(this !== TGITscrollSource) return
      Right.scrollTop  = Left.scrollTop
      Right.scrollLeft = Left.scrollLeft
   })
   Right.addEventListener('scroll', function(){
      if(this !== TGITscrollSource) return
      Left.scrollTop  = Right.scrollTop
      Left.scrollLeft = Right.scrollLeft
   })
}

function TGITremoveExtraLineNumbers() {
   const AllRows = document.querySelectorAll('tr')
   for(let Row of AllRows){
      if(Row.children[1].classList.contains('TGITdumDiff')){
         Row.children[0].innerText = ''
      }
   }
}

function TGITCheckFilesAndMessage() {
   let CommitMessage = document.querySelector('.TGITcommitMsg').value;
   let FilesToCommit = document.querySelectorAll('.TGITselectFileToCommit .CHKcheckbox:checked').length;
   return (FilesToCommit > 0) && (CommitMessage.trim().length > 0);
}

function TGITenableCommit(showFeedback = true) {
   let Enabled = TGITCheckFilesAndMessage();
   let RemoveClass = Enabled ? 'BUTTONdisabled' : 'BUTTONaction';
   let AddClass    = Enabled ? 'BUTTONaction' : 'BUTTONdisabled';
   let Feedback    = Enabled || !showFeedback ? '' : 'Check that the commit message is not empty and at least one file is selected';
   let div = document.querySelector('.TGITmodalCommit');
   if(!div) return false;
   div.classList.remove(RemoveClass);
   div.classList.add(AddClass);
   let FeedbackDiv = document.querySelector('.TGITcommitMsgFeedback');
   if(!FeedbackDiv) return false;
   FeedbackDiv.innerText = Feedback;
   return Enabled;
}

function TGITfullDiff(fromBlock, toBlock) {
   return /*html*/`
   <div class='TGITfullDiff'>
      <div class='TGITdiffTank TGITvrt'>
         <div class='TGITdiffFrom TGITdiffCling'>${fromBlock}</div>
      </div>
      <div class='TGITdiffTank TGITvrt'>
         <div class='TGITdiffTo TGITdiffCling'>${toBlock}</div>
      </div>
   </div>`;
}

function TGITdiffTemplate(Params){ 
   let Added = Params?.summary?.added;
   if(!Added) Added = "";
   let Deleted = Params?.summary?.deleted;
   if(!Deleted) Deleted = "";
   let Changed = Params?.summary?.changed;
   if(!Changed) Changed = "";
   return /*html*/`
   <div class="TGITdiffToolbar">
      <div class="TGITtoolbarFilename">${Params.file}</div>
      <div class="TGITtoolbarLines">
         <span class="TGITlinesInfo">Added:
            <span class="TGITlinesAdded">${Added}</span>
         </span>
         <span class="TGITlinesInfo">Deleted:
            <span class="TGITlinesDeleted">${Deleted}</span>
         </span>
         <span class="TGITlinesInfo">Changed:
            <span class="TGITlinesChanged">${Changed}</span>
         </span>
      </div>
      <div class="TGITtoolbarDisplay">
         <span class="TGITdiffToolLabel">DISPLAY:</span>
         <div>
            <select id="TGITdiffViewSelect" class="FORMselectBox" title="Change display view">
               <option value="vertical">Side by Side</option>
               <option value="condensed">Changes Only</option>
            </select>
         </div>
      </div>
   </div>
   <div class='TGITdiff'></div>`;
}
