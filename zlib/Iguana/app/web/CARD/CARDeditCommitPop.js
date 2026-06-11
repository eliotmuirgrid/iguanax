let CARDcommitEditPop = null;

let CARDdevelopmentRow = {
   hash: "DEVELOPMENT",
   message: "This will run what is currently in the translator.",
   date: ""
};

function CARDrenderCommitEditPop(){
   let RelevantCommits = [...CARDcommitsRelevant()];
   let FullCommits = [...CARDcommits()];
   if(CARDwritable()) FullCommits.unshift(CARDdevelopmentRow);
   if(CARDgitPlain()) RelevantCommits.unshift(CARDdevelopmentRow);
   let Html = CARDeditCommitHelpHtml(RelevantCommits, FullCommits);
   CARDcommitEditPop = POPpopoutOpen({
      title : "Select a Commit",
      content : Html,
		className: 'CARDcommitHelpPopWindow',
      width : "800px",
      height : "90%"
   });
   document.querySelector('.CARDcommitHelpPopWindow .POPpopoutCard-content').classList.add('CARDcommitHelpPop');
   let div = document.querySelector(".CARDcommitHelpModal");
   if(!div) return;
   div.addEventListener("click", CARDeditCommitHelpClick);
}

function CARDeditCommitHelpSimpleHtml(FullCommits){
   let RowsHtml = CARDcommitHelpRowsHtml(FullCommits);
   return /*html*/`<div>
      <div>FULL COMMITS:</div>
      ${RowsHtml}
   </div>`;
}

function CARDconvertedDate(DateString) {
   if (!DateString) return "";
   const now = Date.now();
   const commitDate = new Date(DateString);
   const timeDiffMs = now - commitDate.getTime();
   const timeDiffMinutes = Math.floor(timeDiffMs / 60000);
   const timeDiffHours = Math.floor(timeDiffMinutes / 60);
   const timeDiffDays = Math.floor(timeDiffHours / 24);
   if (timeDiffDays > 6) {
      return commitDate.toLocaleDateString();
   } else if (timeDiffHours >= 24) {
      return `${timeDiffDays} day${timeDiffDays > 1 ? "s" : ""} ago`;
   } else if (timeDiffMinutes >= 60) {
      return `${timeDiffHours} hour${timeDiffHours > 1 ? "s" : ""} ago`;
   } else {
      return `${timeDiffMinutes} minute${timeDiffMinutes > 1 ? "s" : ""} ago`;
   }
}

function CARDformatTime(DateString){
   if (!DateString) return "";
   let formattedTime = "";
   if(DateString) {
      let [dayOfWeek, month, day, time, year] = DateString.split(" ");
      formattedTime = time || "";
   }
   return  formattedTime;
}

function CARDformatDate(DateString){
   if (!DateString) return "";
   let formattedDate = "";
   if(DateString) {
      let [dayOfWeek, month, day, time, year] = DateString.split(" ");
      formattedDate = `${dayOfWeek || ""} ${month || ""} ${day || ""} ${year || ""}`.trim();
   }
   return formattedDate;
}

function CARDcommitHelpRowsHtml(Commits) {
   let Html = "<div class='CARDcommitHelpTable'>";
   Html += "<div class='CARDcommitHelpHeader'>";
   Html += "<div class='CARDcommitHelpHeading'>Hash</div>";
   Html += "<div class='CARDcommitHelpHeading'>Message</div>";
   Html += "<div class='CARDcommitHelpHeading'>Date</div>";
   Html += "</div>";
   for (let Commit of Commits) {
      let   DateData      = Commit?.date;
      const ConvertedDate = DateData ? CARDconvertedDate(DateData) : "";
      const CommitTime    = CARDformatTime(DateData);
      const CommitDate    = CARDformatDate(DateData);
      Html += /*html*/`<div class='CARDcommitHelpRow'>
         <div class='CARDcommitHelpHash'>${Commit?.hash}</div>
         <div>${Commit?.message}</div>
         <div title="${CommitDate} at ${CommitTime}">${ConvertedDate}</div>
      </div>`;
   }
   Html += "</div>";
   return Html;
}

function CARDrelevantCommitsHtml(RelevantCommits){
   if(RelevantCommits.length === 0) return "";
   return /*html*/`<div class='CARDhelpHeader'>RELEVANT COMMITS:</div>
      ${CARDcommitHelpRowsHtml(RelevantCommits)}`;
}

function CARDeditCommitHelpHtml(RelevantCommits, FullCommits){
   let RelevantRowsHtml = CARDrelevantCommitsHtml(RelevantCommits);
   let FullRowsHtml = CARDcommitHelpRowsHtml(FullCommits);
   let div = document.querySelector(".CARDcommitInputValue");
   let Value = div ? div.value : "";
   let FullCommitsClass = !CARDgitPlain() ? " CARDcommitHelpFullCommits" : "";
   return /*html*/`
   <div class="CARDcommitHelpModalContent${FullCommitsClass}">
      <div class="CARDcommitHelpModal ">
         <div class='CARDhelpHeader'>FULL COMMITS:</div>
         ${FullRowsHtml}
         ${RelevantRowsHtml}
         <div class="FORMlabel CARDcommitHelpLabel">Commit Selected:</div>
         <input type='text' class='FORMinput CARDcommitHelpValue' value="${Value}">
         <div class='BUTTONgroup CARDhelpFooter'>
            <button class='BUTTONstandard BUTTONdefault CARDcancelAction'>CANCEL</button>
            <button class='BUTTONaction BUTTONstandard CARDokAction'>CONFIRM</button>
         </div>
      </div>
   </div>
   `;
}

function CARDeditCommitHelpClick(Event){
   let Row = Event.target.closest(".CARDcommitHelpRow");
   if(Row) {
      let Hash = Row.querySelector(".CARDcommitHelpHash").innerText;
      document.querySelector(".CARDcommitHelpValue").value = Hash;
      return;
   }
   if(Event.target.closest(".CARDokAction")){
      let Hash = document.querySelector(".CARDcommitHelpValue").value;
      CARDsetCommit(Hash);
      POPpopoutClose(CARDcommitEditPop);
      return;
   }
   if(Event.target.closest(".CARDcancelAction")){
      POPpopoutClose(CARDcommitEditPop);
      return;
   }
}

function CARDsetCommit(Hash){
   let div = document.querySelector(".CARDcommitInputValue");
   if(div) div.value = Hash;
}