function REPQaddSources(SourcesList){
   let SourceSelect = document.querySelector('.REPQselectSource');
   // Create a Set of existing option values for quick lookup
   let ExistingValues = new Set();
   for (let ExistingOption of SourceSelect.options) {
      ExistingValues.add(ExistingOption.value);
   }
   // Add only new sources that don't already exist
   for (let Source of SourcesList) {
      if (!ExistingValues.has(Source.guid)) {
         let Option = document.createElement("option");
         Option.value = Source.guid;
         Option.text = Source.name;
         SourceSelect.appendChild(Option);
      }
   }
}

function REPQupdateSources(){
   COL_TRC("Updating the list of upstream components");
   let SourcesList = REPQsources();
   let SourceSelect = document.querySelector('.REPQselectSource');
   if(!SourceSelect || !SourcesList ) return;
   REPQaddSources(SourcesList);
}