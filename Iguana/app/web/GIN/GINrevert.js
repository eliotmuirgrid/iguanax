const GINrevertPagIncrement = 10;

function GINrevertHtml(){
   return /*html*/`
   <div class="FORMlabel GINmarginTop">Revert Instance:</div>
   <div class="GINrevertValue GINmarginTop">This will revert the instance to a specific commit. All changes and commit history after the selected commit will be permanently lost.</div>
   <div class="GINeditRevert BUTTONgroup"></div>`;
}

function GINrevert() {
   POPpopoutOpen({
      title: "Revert Instance",
      width: "650px",
      content: `<div class="GINrevertModal"></div>`,
      callback: GINrevertClose
   });
   let commit = PAGEhashParameter("rollback");
   GINrevertListeners();
   GINrevertSetModalHtml(GINrevertSearchHtml(commit));
   GINupdateRevertBtn();
}
function GINrevertClose() {
   PAGEsetHashParameter("rollback", "");
}

function GINrevertListeners(){
   let div = document.querySelector('.GINrevertModal');
   if(!div) { return; }
   div.addEventListener('click', GINrevertModalClick);
   div.addEventListener('input', GINrevertModalInput);
}

function GINrevertModalClick(e) {
   if(e.target.closest('.GINcommitSearch')) GINrevertCommits();
   if(e.target.closest('.GINrevertBtn'))    GINrevertCall();
   if(e.target.closest('.GINrevertBack'))   GINrevertBack();
   if(e.target.closest('.GINrevertNext'))   GINrevertNext();
   if(e.target.closest('.GINcommitTable'))  GINselectCommit(e);
   if(e.target.closest('.GINcommitChangesBtn')) GINrevertCommitAll();
}

function GINrevertModalInput(e) {
   if(e.target.closest('.GINcommitHash'))  GINupdateRevertBtn();
   if(e.target.closest('.GINcommitInput')) GINtoggleCommitBtn();
}

function GINrevertSetModalHtml(html) {
   const modal = document.querySelector('.GINrevertModal');
   if(modal) { modal.innerHTML = html; }
}

function GINrevertCommits() {
   GINrevertSetModalHtml(GINrevertCommitTableHtml());
   GINrevertLoadCommits();
}

function GINupdateRevertBtn() {
   const input = document.querySelector('.GINcommitHash');
   const btn   = document.querySelector('.GINrevertBtn');
   if (!input || !btn) return;
   btn.classList.toggle('BUTTONdisabled', !input.value);
}

function GINrevertCall() {
   const input = document.querySelector('.GINcommitHash');
   const btn   = document.querySelector('.GINrevertBtn');
   if(!input || !btn || btn.classList.contains('BUTTONdisabled')) { return; }
   const hash = input.value;
   APIcall('instance/rollback', {hash: hash}, function(R) {
      if(!R.success) {
         if(!R?.data?.clean) { GINrevertEnableCommitStep(); }
         return GINrevertError(R.error);
      }
      // GINaddAndCommit({message: `Rollback to ${hash}`}); // TODO - come back to this
      SNCKsnackbar('success', `Successfully reverted to commit: ${hash}`);
      POPpopoutRemoveAll();
      GINrestart();
   })
}

function GINcommitChangesHtml() {
   return /*html*/`
   <input class="GINcommitInput FORMinput" type="text" placeholder="Enter your commit message">
   <div class="GINcommitChangesBtn BUTTONdefault BUTTONaction BUTTONstandard BUTTONdisabled">COMMIT CHANGES</div>`;
}

function GINrevertEnableCommitStep() {
   if(document.querySelector('.GINcommitChangesBtn')) { return; }
   let div = document.querySelector('.GINcommitChangesTarget');
   if(!div) { return; }
   div.innerHTML = GINcommitChangesHtml();
}

function GINrevertCommitAll() {
   const commitBtn = document.querySelector('.GINcommitChangesBtn');
   const commitMsg = document.querySelector('.GINcommitInput');
   if(!commitMsg || !commitBtn || commitBtn.classList.contains('BUTTOndisabled')) { return; }
   APIcall('instance/commit/all', {message: commitMsg.value}, function(R) {
      GINclearFields();
      if(!R.success) { return GINrevertError(R.error); }
      SNCKsnackbar('success', 'Successfully committed all changes');
   });
}

function GINtoggleCommitBtn() {
   const commitBtn = document.querySelector('.GINcommitChangesBtn');
   const commitMsg = document.querySelector('.GINcommitInput');
   if(!commitMsg || !commitBtn) { return; }
   commitBtn.classList.toggle('BUTTONdisabled', !commitMsg.value);
}

function GINrevertBack() {
   const count          = document.querySelector('.GINrevertCount').innerText;
   let [current, total] = count.split('/').map(Number);
   current              = Math.max(0, current - (GINrevertPagIncrement*2));
   GINrevertLoadCommits({pag: GINrevertPagIncrement, skip: current});
}

function GINrevertNext() {
   const count          = document.querySelector('.GINrevertCount').innerText;
   let [current, total] = count.split('/').map(Number);
   current              = Math.min(total, current);
   GINrevertLoadCommits({pag: GINrevertPagIncrement, skip: current});
}

function GINselectCommit(e) {
   const trElement     = e.target.closest('tr');
   const anchorElement = trElement ? trElement.querySelector('a') : null;
   if(trElement && !e.target.closest('a') && anchorElement) {
      const commitHash = anchorElement.textContent;
      GINrevertSetModalHtml(GINrevertSearchHtml(commitHash));
      GINupdateRevertBtn();
   }
}

function GINrevertLoadCommits(args) {
   let pag  = GINrevertPagIncrement;
   let skip = 0;
   if(typeof args === 'object' && args !== null) {
      if(args.hasOwnProperty('pag')) { pag = args.pag; }
      if(args.hasOwnProperty('skip')) { skip = args.skip; }
   }
   APIcall('git/commit/pag', {pag: pag, skip: skip}, function(R) {
      if(!R.success) { return GINrevertError(R.error); }
      GINrevertCommitTable(R.data);
   })
}

function GINrevertCommitTable(commitData) {
   const commits = commitData["commits"];
   const index   = commitData["index"];
   const total   = commitData["total"];
   const url     = commitData["url"];
   const tableBody = document.querySelector('.GINcommitTable tbody');
   tableBody.innerHTML = ''; // Clear existing rows
   document.querySelector('.GINrevertCount').innerText = `${index}/${total}`;
   if(commits.length <= 0) { return; }
   commits.forEach(commit => {
      const row = document.createElement('tr');
      const hashCell = document.createElement('td');
      const messageCell = document.createElement('td');
      const commitUrl = `${url}${commit.hash}`;
      const hashLink = document.createElement('a');
      hashLink.href = commitUrl;
      hashLink.textContent = commit.hash;
      hashLink.target = "_blank";
      hashCell.appendChild(hashLink);
      messageCell.textContent = commit.message;
      row.appendChild(hashCell);
      row.appendChild(messageCell);
      tableBody.appendChild(row);
   });
}

function GINrevertError(error) {
   let div = document.querySelector(".GINrevertError");
   if(div) { div.innerHTML = error; }
}

function GINclearFields() {
   document.querySelector('.GINcommitChangesBtn')?.remove();
   document.querySelector('.GINcommitInput')?.remove();
   GINrevertError('');
}

function GINrevertSearchHtml(commit='') {
   return /*html*/`
   <div class="GINrevertFlex">
      <div class="FORMlabel GINcommitLabel GINmarginTop">Commit:</div>
      <input type="text" class="GINcommitHash FORMinput" placeholder="Enter a valid commit hash or select from the menu" value="${commit}">
     <div class="GINcommitSearch BUTTONdefault BUTTONstandard">...</div>
     <div class="GINrevertBtn BUTTONdisabled BUTTONaction BUTTONstandard">REVERT</div>
   </div>
   <div class="GINrevertError"></div>
   <div class="GINcommitChangesTarget"></div>`;
}

function GINrevertCommitTableHtml() {
   return /*html*/`
   <div class="GINcommitTable">
         <table>
            <thead>
               <tr>
                  <th class="GINcommitTableHash">Commit Hash</th>
                  <th class="GINcommitTableMessage">Commit Message</th>
               </tr>
            </thead>
            <tbody>
            </tbody>
         </table>
      </div>
      <div class="GINrevertCount"></div>
      <div class="GINrevertPagination">
         <div class="GINrevertBack BUTTONdefault BUTTONstandard">BACK</div>
         <div class="GINrevertNext BUTTONaction BUTTONstandard">NEXT</div>
      </div>
      <div class="GINrevertError"></div>
   </div>`;
}