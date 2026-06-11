function GITutilsUpdateWorkspaces(selector, STOREvariable, data) {
   if(!selector || !data) { return; }
   selector.innerHTML    = "";
   let SelectedWorkspace = STORElocalRead(STOREvariable);
   data.forEach(workspace => {
      const newEl = document.createElement('option');
      newEl.value = workspace;
      newEl.text  = workspace;
      if(workspace === SelectedWorkspace) { newEl.selected = true; }
      selector.appendChild(newEl);
   });
}

function GITupdateWorkspaces(callback, force) {
   let ApiData = {};
   ApiData.force = force;
   APIcall('git/owners/update', ApiData, function(R) {
      if(!R.success) { return SNCKsnackbar('error', 'Failed to update owners: ' + R.error); }
      if(callback) { callback(); }
   })
}