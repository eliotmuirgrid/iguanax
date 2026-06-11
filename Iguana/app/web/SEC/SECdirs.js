let SECdirsData;
   
function SECupdateDirs(Data){
   let div = document.querySelector(".SANDdirsTarget");
   if(!div) return;
   let Dirs;
   if(Data) {
      Dirs         = Data;
      SECdirsData = undefined;
   } else {
      Dirs = SECdata();
   }
   let Html = SECdirsHtml(Dirs);
   if(!DEEPequal(SECdirsData, Dirs)) div.innerHTML = Html;
   SECdirsData = Dirs;
}

function SECviewRowHtml(Path){
   return /*html*/`<span class="SECrow">${ESChtmlEscape(Path)}</span>`;
}

function SECrowsHtml(Dirs){
   let Html = "";
   for(const dir of Dirs) Html += SECviewRowHtml(dir);
   return Html;
}

function SECcheckboxDiv(Use, Edit, Classname){
   if(!Edit) return /*html*/`<div>${Use ? 'Restricted <div class="SECsecure">(more secure)</div>' : 'No Restrictions <div class="SECsecure">(less secure)</div>'}</div>`;
   return /*html*/`<select class="FORMselectBox ${Classname}">
      <option value="1" ${Use ? 'selected' : ''}>Restricted (more secure)</option>
      <option value="0" ${Use ? '' : 'selected'}>No Restrictions</option>
   </select>`;
}

function SECuseAccessHtml(DirAccessOn, OsExOn, Edit){
   return /*html*/`
   <div class="SEConDiv">
      <div class="FORMlabel">OS Command Execution:</div>
      ${SECcheckboxDiv(OsExOn, Edit, "SECosExecute")}
   </div>
   <br>
   <div class="SEConDiv">
      <div class="FORMlabel">Directory Access:</div>
      ${ SECcheckboxDiv(DirAccessOn, Edit, "SANDuse")}
   </div>`;
}

function SECtableHtml(Dirs, Use){
   let Rows = SECrowsHtml(Dirs);
   let Class = Use ? "" : " SECdisabled";
   let Title = Use ? "" : `title="You do not have access restrictions enabled."`;
   return /*html*/`
   <br>
   <div class="SECdirsTable SECviewTable${Class}" ${Title}>
      <div class="SECdirHeading">
         <div class="SECheading SANDpaddingLeft">Directories Allowed to be Accessed:</div>
      </div>
      ${Rows}
   </div>`;
}

function SECdirsHtml(Data) {
   let Dirs              = Data?.dirs || [];
   let EnableDirs        = Data?.enable_directories;
   let RestrictOsExecution = Data?.restrict_execution;
   let View              = !SECeditPage();
   if(!View) { return SECeditRowsHtml(Dirs, EnableDirs, RestrictOsExecution); }
   let UseAccess = SECuseAccessHtml(EnableDirs, RestrictOsExecution, false);
   let Table     = SECtableHtml(Dirs, EnableDirs);
   if(!Dirs.length) { Table = SETemptyList('directories are set', 0); }
   if(!EnableDirs && !Dirs.length) { Table = ""; }
   return /*html*/`
   ${UseAccess}
   ${Table}`;
}

