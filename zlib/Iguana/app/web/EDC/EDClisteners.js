let EDCupdateUrlTimer; 
let EDCnameTyped;

function EDClisteners(){
   let card = document.querySelector('.EDCmodal');
   card.addEventListener('click', EDCclickListeners);
   card.addEventListener('change', EDCchangeListeners);
   card.addEventListener('keyup', EDCkeyUpListeners);
}

function EDCkeyUpListeners(e){
   if(e.target.closest('.EDCnameEdit')) EDCnameTyped = true;
}
function EDCclickListeners(e){
   if(e.target.closest('.EDCcancel'))  EDCclose();
   if(e.target.closest('.EDCadd'))     EDCaddTemplate({});
   if(e.target.closest('.EDCsave'))    EDCsave();
   if(e.target.closest('.EDCremove'))  EDCremove(e.target);
}

function EDCchangeListeners(e){
   let typeSelect = e.target.closest('.EDCreferenceType');
   if(!typeSelect) return;
   let row = typeSelect.closest('.EDCreferenceRow');
   if(!row) return;
   let branchDom = row.querySelector('.EDCreferenceBranch');
   if(!branchDom) return;
   let isLibrary = typeSelect.value === "library";
   if(isLibrary){
      branchDom.disabled = true;
      branchDom.classList.add('EDCbranchNA');
      branchDom.value = "";              // ensure placeholder shows
      branchDom.placeholder = "—";
      branchDom.title = "Branch is only supported for Components. Libraries use the repo default branch.";
   }else{
      branchDom.disabled = false;
      branchDom.classList.remove('EDCbranchNA');
      branchDom.placeholder = "Branch";
      branchDom.title = "";
   }
}

function EDCcollectionNames(){
   const Data = MODELdata?.SETcollection?.data || [];
   let NamesData = [];
   for (let i = 0; i < Data.length; i++) {
     NamesData.push(Data[i].name);
   }
   return NamesData;
 }

function EDCguessName(){
   if(EDCnameTyped) return;
   NRLparseGitInput(EDCgit,"");
   clearTimeout(EDCupdateUrlTimer);
   EDCupdateUrlTimer = setTimeout(function() { NRLpopulateNameGuess(EDCgit,"",".EDCnameEdit"); }, 100);
}

function EDCremove(target){
   let ComponentCard = target.closest('.EDCchildRow');
   if(ComponentCard) ComponentCard.remove();
}

function EDCnameEmpty(){
   let div = document.querySelector('.EDCnameEdit');
   let Name = div.value?.trim() || "";
   return Name == "";
}

function EDClinkHtml(Data){
   let Links = Data?.links || {};
   let SshUrl = Links?.ssh || "";
   let WebUrl = Links?.web || "";
   return /*html*/`<a class="SETlink" href="${WebUrl}" target="_blank">${SshUrl}</a>`;
}

function EDCrefreshTemplates(){
   SETrefreshTemplates(function(){
      if(PAGEhash() === "dashboard") MODELforcePoll('CARDmodel');
   });
}

function EDCsave(){
   if (!EDCvalidateNamesInReferenceData()) { return; }
   let CollectionData = EDCcollectionData();
   console.warn(CollectionData);
   let card = document.querySelector('.EDCmodal');
   if(!card) return;
   if(EDCnameEmpty()) return SNCKsnackbar('error', "Cannot save without providing a collection name.");
   card.innerHTML = EDCwaitWhilePushingHtml();
   let ApiData = {};
   ApiData.content = CollectionData;
   ApiData.git = EDCgit;
   APIcall('git/collection/save', ApiData, function(R){
      EDCrefreshTemplates();
      let Data = R?.data || {};
      let Message = R.success ? "Your changes have been saved and pushed successfully to: <br><br>" + EDClinkHtml(Data) : R.error;
      let card = document.querySelector('.EDCmodal');
      if(!card) return;
      card.innerHTML = EDCfailedToGetCollectionHtml(Message);
   });
}

function EDCcollectionData(){
   let CollectionData = {};
   let Name        = document.querySelector('.EDCnameEdit');
   let Description = document.querySelector('.EDCdescriptionEdit');
   if(!Name || !Description) return;
   CollectionData.name        = Name.value.trim();
   CollectionData.description = Description.value.trim();
   CollectionData.collection  = EDCgetReferenceData();
   return CollectionData;
}

function EDCvalidateLibraryName(Name, ErrorDOM) {
   // Updated regex: no max length, requires at least one character
   let Valid = /^[A-Z0-9]+$/.test(Name);
   if (!Valid) {
      ErrorDOM.innerHTML = `Invalid library name '${ESChtmlEscape(Name)}'. Only uppercase letters and digits are allowed.`;
   }
   return Valid;
}

function EDCvalidateComponentName(Name, ErrorDOM) {
   Name = Name.trim();
   if (FILnameContainsReservedCharacters(Name)) {
      ErrorDOM.innerHTML = `Invalid component name '${ESChtmlEscape(Name)}'. Reserved characters [${FILreservedCharacters()}] are not allowed.`;
      return false;
   }
   if (FILnameMatchesReservedNames(Name)) {;
      ErrorDOM.innerHTML = `Invalid component name '${ESChtmlEscape(Name)}'. Reserved names are ${FILreservedNames()}.`;
      return false;
   }
   if (FILnameEndsWithDot(Name)) {
      ErrorDOM.innerHTML = `Invalid component name '${ESChtmlEscape(Name)}'. Component name cannot end with a period.`;
      return false;
   }
   return true;
}

function EDCvalidateComponentLibaryName(Type, Name) {
   let ErrorDOM = document.querySelector('.EDCwindowError');
   ErrorDOM.innerHTML = "";
   switch (Type) {
      case "library":   return EDCvalidateLibraryName(Name, ErrorDOM);
      case "component": return EDCvalidateComponentName(Name, ErrorDOM);
      default:          return false;
   }
}

function EDCvalidateNamesInReferenceData(){
   let Rows = document.querySelectorAll('.EDCreferenceRow');
   for (let CustomRow of Rows) {
      let Type = CustomRow.querySelector('.EDCreferenceType').value;
      let NameDom = CustomRow.querySelector('.EDCreferenceName');
      let Name = NameDom.value.trim();
      if (Name == "") { continue; }
      if (!EDCvalidateComponentLibaryName(Type, Name)) {
         NameDom.focus();
         NameDom.select();
         return false;
      }
   }
   return true;
}

function EDCgetReferenceData(){
   let Data = {};
   let Rows = document.querySelectorAll('.EDCreferenceRow');

   for(let CustomRow of Rows){
      let ComponentData = {};
      let Name = CustomRow.querySelector('.EDCreferenceName').value.trim();
      let Git  = CustomRow.querySelector('.EDCreferenceGit').value.trim();
      if(Name == "" || Git == "") continue;

      let Type = CustomRow.querySelector('.EDCreferenceType').value;

      ComponentData.name        = Name;
      ComponentData.type        = Type;
      ComponentData.description = CustomRow.querySelector('.EDCreferenceDescription').value.trim();
      ComponentData.git         = Git;

      // Only components support branches
      let Branch = CustomRow.querySelector('.EDCreferenceBranch').value.trim() || "";
      if(Type === "library") Branch = "";
      ComponentData.branch = Branch;

      let Key = Git;
      if(Type !== "library" && Branch !== "") Key += "#" + Branch;

      Data[Key] = ComponentData;
   }

   return Data;
}
