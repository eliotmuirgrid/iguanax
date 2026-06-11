function COMrender(){
   let exists = document.querySelector('.COMmodal');
   if(exists) return; 
   let div = document.querySelector('.DASHcommitTarget');
   if(!div) return;
   let html = COMskeletonHtml();
   div.innerHTML = html;
}

function COMskeletonHtml(){
   return /*html*/`
      <div class="COMmodalContainer">
         <div class="COMheader DASHsideHeader">
            <div>Commit History</div>
         </div>
         <div class="COMmodal COMlist"></div>
         <div class="COMcommitListFade"></div>
      </div>
   `;
}

function COMskeletonSplitter(){
   return /*html*/`
   <div class="COMpopoutListSplitter">
      <div class="COMpopoutList COMlist">${COMhtml()}</div>
      <div class="COMresizeControl">
         <div class="COMresizeShadow"></div>
      </div>
   </div>
   `
}

function COMskeletonPopoutHtml(){
   return /*html*/`
   <div class="COMpopoutModal">
      ${COMskeletonSplitter()}
      <div class="COMpopoutDiffContainer">
         <div class="COMpopoutDiff"></div>
      </div>
   </div>`;
}

function COMcommitDateHtml(DateHtml, showCommitId){
   let Words = showCommitId ? "Commits from" : "";
   return /*html*/`
   <div class="COMcommitDateContainer">
      <div class="COMcommitDate"> ${Words} ${DateHtml}</div>
   </div>`
}
