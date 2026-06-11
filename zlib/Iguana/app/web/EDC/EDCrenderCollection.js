let EDCdata = {};
let EDCgit;

function EDCcollectionRenderHtml(Data, AddToCollection){
   if(ADCcard) POPpopoutClose(ADCcard);
   EDCdata = Data;
   let Modal = document.querySelector('.EDCmodal');
   Modal.innerHTML = EDCcollectionHtml2(Data);
   EDCrenderTemplates(Data);
   if(AddToCollection) EDCaddTemplate(AddToCollection, true);
   if(!document.querySelector('.EDCreferenceRow')) EDCaddTemplate({});
   EDChandleFocus();
   EDCnameTyped = false;
   let EDCnameDiv = document.querySelector(".EDCnameEdit")
   FORMenterListener(EDCnameDiv, EDCsave);
   if (EDCnameDiv?.value) EDCnameTyped = true;
   EDCguessName();
}

function EDChandleFocus(){
   let Name = document.querySelector('.EDCnameEdit');
   if(!Name) return;
   let NameValue = Name.value.trim();
   if(NameValue === "") {
      Name.setAttribute('tabindex','0');
      return Name.focus();
   }
   let RepoNames = document.querySelectorAll('.EDCreferenceName');
   if(!RepoNames) return;
   let NameInput = RepoNames[RepoNames.length - 1];
   if(!NameInput) return;
   NameInput.setAttribute('tabindex','0');
   NameInput.focus();
}

function EDCrenderTemplates(Data){
   for(let i=0; i < Object.keys(Data.collection).length; i++){
      let git = Object.keys(Data.collection)[i];
      EDCaddTemplate(Data.collection[git]);
   }
}

function EDCaddTemplate(Data, highlight=false){
   let List = document.querySelector(`.EDCeditChildrenDiv`);
   let NewDiv = document.createElement('div');
   NewDiv.innerHTML = EDCtemplate2Html(Data, highlight);
   List.appendChild(NewDiv);
   NewDiv.scrollIntoView({ block: 'end', behavior: 'smooth' });
}

function EDCcollectionHtml2(Data){
   return /*html*/`
   <div class="EDCwindow">
      <div class="EDCeditBanner">
         <input class="FORMinput EDCnameEdit"        type="text" placeholder="Collection name"        value="${ESChtmlEscape(Data.name)}">
         <input class="FORMinput EDCdescriptionEdit" type="text" placeholder="Collection description" value="${ESChtmlEscape(Data.description)}">
         <div class="EDCsaveEditGroup BUTTONgroup">
            <div class="EDCsave BUTTONaction BUTTONstandard">SAVE</div>
            <div class="EDCcancel BUTTONcancel BUTTONstandard">CANCEL</div>
         </div>
      </div>
      <div class="EDCwindowError"></div>
      <div class="EDCwindowInner">
         <div class="EDCeditChildrenDiv">
            <div class="EDCchildRow EDCchildHeaders">
               <div class="EDCmatchingHeader FORMlabel" >TYPE</div>
               <div class="EDCmatchingHeader FORMlabel">NAME</div>
               <div class="EDCmatchingHeader FORMlabel">DESCRIPTION</div>
               <div class="EDCmatchingHeader FORMlabel">GIT URL</div>
               <div class="EDCmatchingHeader FORMlabel">BRANCH (Components only)</div>
            </div>
         </div>
         <div class="EDCaddEditRow ">
            <div class="EDIaddChild EDCadd">+ ADD REFERENCE</div>
         </div>
      </div>
   </div>`;
}

function EDCtemplate2Html(RowData, highlight){
   let Name = RowData.name ? RowData.name : "";
   let Description = RowData.description ? RowData.description : "";
   let Git = RowData.git ? RowData.git : "";
   let Library = RowData.type == "library";
   let BranchRaw = RowData.branch ? String(RowData.branch) : "";
   // Use placeholder for "—" so it never becomes actual saved data
   let BranchValue = Library ? "" : BranchRaw;
   let BranchPlaceholder = Library ? "—" : "Branch";
   let BranchTitle = Library
      ? "Branch is only supported for Components. Libraries use the repo default branch."
      : "";
   let BranchDisabledAttr = Library ? "disabled" : "";
   let BranchClass = Library ? "EDCbranchNA" : "";
   return /*html*/`
   <div class="EDCchildRow EDCreferenceRow ${highlight ? "EDCadded" : ""}">
      <div class="EDCeditCell">
         <select class="FORMselectBox EDCreferenceType">
            <option value="component" ${Library ? "" : "selected"}>Component</option>
            <option value="library"   ${Library ? "selected" : ""}>Library</option>
         </select>
      </div>
      <div class="EDCeditCell">
         <input class="FORMinput EDCreferenceName" type="text" placeholder="Name" value="${ESChtmlEscape(Name)}">
      </div>
      <div class="EDCeditCell">
         <input class="FORMinput EDCreferenceDescription" type="text" placeholder="Description" value="${ESChtmlEscape(Description)}">
      </div>
      <div class="EDCeditCell">
         <input class="FORMinput EDCreferenceGit" type="text" placeholder="Git url" value="${ESChtmlEscape(Git)}">
      </div>
      <div class="EDCeditCell">
         <input class="FORMinput EDCreferenceBranch ${BranchClass}"
            type="text"
            placeholder="${ESChtmlEscape(BranchPlaceholder)}"
            value="${ESChtmlEscape(BranchValue)}"
            ${BranchDisabledAttr}
            ${BranchTitle ? `title="${ESChtmlEscape(BranchTitle)}"` : ""}>
      </div>
      <div class="EDCeditCell">
         <div class="BUTTONcancel EDCremove"><img src="/EDC/EDCdeleteRow.svg"></div>
      </div>
   </div>
   `;
}
