function CARDrunningCommitHtml(){
   return /*html*/`
   <div class='CARDgitRow'>
      <div class="FORMlabel">Using Commit:</div>
      <div class="CARDcurrentRunningCommit"></div>
      <div class="CARDgitEditCurrentCommitTarget"></div>
   </div>`;
}

function CARDdevelopmentCommitHtml(attr, Classes){
   let EditPerm = CARDpermissionData()?.edit_script;
   let html = /*html*/`<span class="CARDcurrentCommitIdDiv CARDcurrentCommitIdDevelopment">DEVELOPMENT</span>`;
   if(EditPerm) return html;
   return /*html*/`
   <div class="CARDcurrentCommitDevelopmentGrid">
      ${html}
      <div class="CARDmakeCopyBrackets">- [</div>
      <a class="CARDviewCurrentCommit CARDcurrentRealCommitIdDiv ${Classes}" ${attr}>View</a>
      <div class="CARDmakeCopyBracketsEnd">]</div>
   </div>`;
}

function CARDcommitsMatch(hashA, hashB) {
   if (!hashA || !hashB) return false;
   return hashA.startsWith(hashB) || hashB.startsWith(hashA);
 }

function CARDfindCommitInfo(Commit, Commits){
   for(let i = 0; i < Commits.length; i++){
      if(CARDcommitsMatch(Commits[i].hash, Commit)) return Commits[i];
   }
   for(let Idx in Commits){
      if(CARDcommitsMatch(Commits[Idx].hash, Commit)) return Commits[Idx];
   }
   return null;
}

function CARDcommitMaybeInfo(Commit){
   let Data = CARDcommits();
   let CommitInfo = CARDfindCommitInfo(Commit, Data);
   if(CommitInfo) return CommitInfo;
   let InstCommits = CARDinstCommits();
   CommitInfo = CARDfindCommitInfo(Commit, InstCommits);
   if(CommitInfo) return CommitInfo;
   return null;
}

function CARDshortCommitHtml(Commit, ViewCommit){
   let CommitData = CARDcommitMaybeInfo(Commit);
   if(!CommitData) return Commit;
   let   DateData      = CommitData.date;
   const ConvertedDate = DateData ? CARDconvertedDate(DateData) : "";
   const CommitTime    = CARDformatTime(DateData);
   const CommitDate    = CARDformatDate(DateData);
   let RunComp = CARDrunComp();
   let RunCompHtml = RunComp ? ` ( on "${RunComp}" ) - ` : "";
   return /*html*/`<div class="CARDcommitIdView">${Commit}</div>${RunCompHtml} ${ViewCommit} <br><div class="CARDcommitMessage" title="${CommitDate} at ${CommitTime}">${CommitData.message} by ${CommitData.author} (${ConvertedDate})</div>`;
}

function CARDcurrentRunningCommitHtml(){
   let Commit = CARDcommit();
   let CommitExists = CARDnewCommitExists();
   let Perm = CARDpermissionData()?.view_script;
   let attr = Perm ?`href="${TRANSformatUrl(CARDcomponentId(), false)}"` : `title="You do not have permission to view this."`
   let Classes = Perm ? "" : "CARDdisableLink";
   let Dev = Commit === "DEVELOPMENT";
   if(Dev){
      return CARDdevelopmentCommitHtml(attr, Classes);
   }
   let Html = `[
      <a class="CARDviewCurrentCommit CARDcurrentRealCommitIdDiv ${Classes}" ${attr}>Open in Translator…</a>
      ]`;
   return /*html*/`
   <div class="CARDcurrentCommitGrid">
      <span class="CARDcurrentCommitIdDiv CARDcurrentCommitDescription ${CommitExists? 'CARDnewCommitExists" title="You are not running from the latest commit ' : "" }">${CARDshortCommitHtml(Commit, Html)}</span>
   </div>`;
}

function CARDupdateCurrentCommit(){
   if(!CARDdata()) return;
   if(CARDcommitEdit) return;
   let Commit = CARDcommit();
   if(!Commit) return;
   let CurrentCommitHtml = CARDcurrentRunningCommitHtml();
   let CurrentCommitDiv = document.querySelector('.CARDcurrentRunningCommit');
   if(!CurrentCommitDiv) return;
   let CurrentCommitId = document.querySelector('.CARDcommitIdView');
   if(CurrentCommitId && CurrentCommitId.innerHTML === Commit) return;
   if (CurrentCommitDiv.innerHTML === CurrentCommitHtml) return;
   CurrentCommitDiv.innerHTML = CurrentCommitHtml;
}

function CARDupdateLoadingCommit(){
   let CurrentCommit = document.querySelector('.CARDcurrentRunningCommit');
   if(!CurrentCommit) return;
   let NoCommitHtml = /*html*/`<div class="CARDloadingCommit">loading...</div>`;
   if(!CARDdata()){
      if(CurrentCommit.innerHTML !== NoCommitHtml) CurrentCommit.innerHTML = NoCommitHtml;
   }
}

function CARDeditCommitButtonHtml(){
   return !CARDcomponentOn() ? 
   /*html*/`<div class="CARDgitEditCurrentCommitButton CARDeditBtn"><img src="CARD/CARDiconMoreOptions.svg"></div>` : 
   /*html*/`<div class="CARDgitNoEditButton"></div>`;
}

function CARDupdateEditCommitButton(){
   let ButtonHtml = CARDeditCommitButtonHtml();
   let EditButtonTarget = document.querySelector(".CARDgitEditCurrentCommitTarget");
   if(!EditButtonTarget) return;
   if(EditButtonTarget.innerHTML !== ButtonHtml) EditButtonTarget.innerHTML = ButtonHtml;
}

function CARDupdateRunningCommitView(){
   if(CARDcommitEdit) return;
   if(document.querySelector('.CARDrunningCommit') && document.querySelector('.CARDcurrentRunningCommit') === null) {
      document.querySelector('.CARDrunningCommit').innerHTML = CARDrunningCommitHtml();
   }
   CARDupdateLoadingCommit();
   CARDupdateCurrentCommit();
   CARDupdateEditCommitButton();
}