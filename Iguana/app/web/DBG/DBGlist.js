let DBGdata = [];
let DBGcard = null;
let DBGlastUpload = '';

function DBGdeleteAllHtml(){
   let Delete = STAcrashPermission();
   let Classes =  Delete ? "DBGdeleteAll BUTTONaction" : "BUTTONdisabled" ;
   let title   = !Delete ? `title="You do not have permission to do this."` : "";
   return /*html*/`<div class="${Classes} BUTTONstandard" ${title}>DELETE ALL</div>`;
}

function DBGtemplate() {
   if(DBGdata.length === 0) { return /*html*/`<p>There are no crash files present.</p>`; }
   return /*html*/`
   <div class="DBGinfo">
      <p>A crash file is produced when Iguana has an unexpected exception which brings down the server. Click on a crash file to view it:</p>
   </div>
   <div class="DBGlist">${DBGlistHtml()}</div>
   <br>
   <br>
   <div class="BUTTONcancel BUTTONstandard DBGcancel">CLOSE</div> 
   ${DBGdeleteAllHtml()}
   `;
}

function DBGdeleteAllFiles() {
   POPpopoutClose(DBGcard);
   APIcall("debug/crash/file/delete/all", {}, function(R) {
      console.log("Deleted files:", R);
      let Message = GINdeleteAllFiles();
      GINaddAndCommit({message: Message, reports: true});
   });
}

function DBGaddDeleteAllListener() {
   let Target = document.querySelector(".DBGdeleteAll");
   if(Target) { Target.addEventListener('click', DBGdeleteAllFiles); }
}

function DBGcloseList() { POPpopoutClose(DBGcard); }

function DBGaddCancelListener() {
   let Target = document.querySelector(".DBGcancel");
   if(Target) { Target.addEventListener('click', DBGcloseList); }
}

function DBGlistEntry(File) {
   if (File == DBGlastUpload) {
      return `<div><a class="DBGshowFile" href="${File}"><span>Latest Uploaded File</span>${DBGconvertFormat(File)}</a></div>`;
   } else {
      return `<div><a class="DBGshowFile" href="${File}">${DBGconvertFormat(File)}</a></div>`;
   }
}

function DBGlistHtml() {
   let H = '';
   for(let i = DBGdata.length - 1; i >= 0; i--) { H += DBGlistEntry(DBGdata[i]); }
   return H;
}

function DBGlist() {
   DBGcard = POPpopoutOpen({title: "Crash Files", maxHeight: "90%", content: DBGtemplate()});
   DBGlisteners();
   DBGaddDeleteAllListener();
   DBGaddCancelListener();
}


function DBGlistFetch() {
   APIcall("debug/crash/file/list", {}, function(R) {
      DBGdata = R.data.crash_list;
      DBGlastUpload = R.data.last_upload;
      DBGlist();
   });
}

function DBGlisteners() {
   let Target = document.querySelector('.DBGlist');
   if(!Target) { return; }
   Target.removeEventListener('click', DBGclickListeners);
   Target.addEventListener('click', DBGclickListeners);
}

function DBGshowFile(Target){
   let FileName = Target.attributes.href.value
   DBGfetchFile(FileName);
}

function DBGclickListeners(e){
   e.preventDefault();
   const ShowFileElement = e.target.closest('.DBGshowFile');
   if (ShowFileElement) DBGshowFile(ShowFileElement);
}

function DBGdeleteHtml(){
   let Delete = STAcrashPermission();
   let Classes =  Delete ? "DBGdeleteFile BUTTONaction" : "BUTTONdisabled" ;
   let title   = !Delete ? `title="You do not have permission to do this."` : "";
   return /*html*/`<div class="${Classes} BUTTONstandard" ${title}>DELETE</div>`;
}

function DBGformatFile(Content) {
   return /*html*/`
      <div class="DBGcontent">${ESChtmlEscape(Content)}</div>
      ${DBGdeleteHtml()}
      <br><br>
      <div class="DBGerrorText"></div>`;
}

function DBGfetchFile(FileName){
   APIcall("debug/crash/file/get", {file : FileName}, function(R){
      let Card = POPpopoutOpen({className: `DBGcrashCard`, width : (window.innerWidth-100) + 'px', title : ESChtmlEscape(FileName), content : DBGformatFile(R.data.content)});
      let div = document.querySelector('.DBGdeleteFile');
      if(!div) return;
      div.addEventListener('click', function() { DBGdeleteFile(Card, FileName); });
   });
}

function DBGdeleteFile(Card, FileName) {
   APIcall('debug/crash/file/delete', {file: FileName}, function(R) {
      if(!R.success) { return DBGdeleteError(R.error); }
      let Message = GINdeleteFile(FileName);
      GINaddAndCommit({message: Message, reports: true});
      const IndexToRemove = DBGdata.indexOf(FileName);
      if(IndexToRemove !== -1) { DBGdata.splice(IndexToRemove, 1); }
      let List = document.querySelector('.DBGlist');
      if(List) { List.innerHTML = DBGlistHtml(); }
      if(Card) { POPpopoutClose(Card); }
   });
}

function DBGdeleteError(Error) {
   let ErrorDiv = document.querySelector('.DBGerrorText');
   if(!ErrorDiv) { return; }
   ErrorDiv.style.display = 'block';
   ErrorDiv.innerText = Error;
}

function DBGconvertFormat(inputString) {
   const Match = inputString.match(/^(\d{4})(\d{2})(\d{2})-(\d{2})(\d{2})(\d{2})\.crash$/);
   if(!Match) { return inputString; } // couldnt match so just return input as default case
   const [, year, month, day, hour, minute, second] = Match;
   const date = new Date(`${year}-${month}-${day}T${hour}:${minute}:${second}Z`);
   const options = {
      weekday: 'long',
      month: 'long',
      day: 'numeric',
      year: 'numeric',
      hour: '2-digit',
      minute: '2-digit',
      second: '2-digit',
      hour12: true,
      timeZone: 'UTC'
   };
   return date.toLocaleString(navigator.language, options);
}
