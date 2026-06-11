function HLPsave() {
   const Data = {component: TRANScomponentGuid(), path: HLPcurrentFile, content: HLPextract()};
   if(HLPisDuplicateName(Data.content.Name)) { return SNCKsnackbar('error', `${Data.content.Name} already exists`);   }
   APIcall("component/help/set", Data, function(Response) {
      if(!Response.success) { return SNCKsnackbar('error', Response["error"] + ' HELP file not saved.'); }
      SNCKsnackbar('success', 'Help file saved.', 6000);
      HLPlistAll(Data.content.Name);
      TRANSrunIfAutoRunEnabled();
   });
}

function HLPisDuplicateName(name) {
   document.querySelectorAll('.HLPlink').forEach(link => { if(name === link.innerText) { return true; } })
   return false;
}

function HLPextract() {
   const IsParameterTable = document.querySelector(".HLPisTable")?.checked;
   return {
      "Name"          : HLPfunctionName(),
      "Description"   : document.querySelector(".HLPdescriptionContent")?.querySelector('.ql-editor')?.innerHTML,
      "ParameterTable": IsParameterTable,
      "Parameters"    : IsParameterTable ? HLPextractTableParams() : HLPextractListParams(),
      "SummaryLine"   : document.querySelector(".HLPeditSummary")?.value.trim()
   };
}

function HLPfunctionName() {
   const editField    = document.querySelector('.HLPeditName');
   const coreField    = document.querySelector('.HLPfunctionName');
   const headingField = document.querySelector('.HLPheading');
   if(editField)    { return editField.value.trim(); }
   if(coreField)    { return coreField.innerText.trim(); }
   if(headingField) { return headingField.innerText.trim(); }
}

function HLPextractTableParams() {
   let Parameters       = [];
   const ParameterItems = document.querySelectorAll(".HLPtableParameterRows .HLProw");
   for(let Parameter of ParameterItems) {
      const Name        = Parameter.querySelector(".HLPparameter").value.trim();
      const Description = Parameter.querySelector('.ql-editor');
      const Required    = Parameter.querySelector(".HLPrequired").checked;
      if(!Name && !Description.innerText.trim()) { continue; }
      Parameters.push({"Name": Name, "Description": Description.innerHTML, "Required": Required});
   }
   return Parameters;
}

function HLPextractListParams() {
   let Parameters       = [];
   const ParameterItems = document.querySelectorAll(".HLPlistParameterRows .HLProw");
   for(let Parameter of ParameterItems) {
      const Description = Parameter.querySelector('.ql-editor');
      if(!Description.innerText.trim()) { continue; }
      Parameters.push({ "Description": Description.innerHTML })
   }
   return Parameters;
}

function HLPguessApiName(Path) {
   const PathArray = Path.split(/[/\\.]+/).filter(Boolean);
   if (PathArray.length < 3) { return ""; }
   return PathArray[0] + "." + PathArray[PathArray.length - 2];
}

function HLPblankFile(Path) {
   let Name = HLPguessApiName(Path);
   return `{
   "ParameterTable": false,
   "Name": "${Name}",
   "SummaryLine": "",
   "Description": "",
   "Parameters": []
}`
}