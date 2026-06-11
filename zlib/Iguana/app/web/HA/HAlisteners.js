function HAlisteners() {
   let div = document.querySelector(".HAmodal");
   if(!div) { return; }
   div.removeEventListener("click", HAclick);
   div.addEventListener("click", HAclick);
}

function HAclick(e) {
   if(e.target.closest(".HAsave")) { return HAsave(); }
   if(e.target.closest(".HAsharedDirBrowse")) { return HAopenBrowser(); }
}

function HAopenBrowser() {
   let pathInput = document.querySelector(".HAsharedDirInput");
   FILbrowser({
      value      : pathInput.value,
      new_folder : true,
      show_hidden: true,
      dir_only   : true,
      callback   : function(selectedPath) { pathInput.value = selectedPath; }
   });
}

function HAenabledSelected() {
   let select = document.querySelector(".HAenabledSelect");
   return select?.value === 'true';
}

function HAsharedDirInputed() {
   let inp = document.querySelector(".HAsharedDirInput");
   return inp?.value || "";
}

function HAtypeSelected() {
   let select = document.querySelector(".HAtypeSelect");
   return select?.value;
}

function HAsave() {
   let Args = {
      enabled: HAenabledSelected(),
      shared_working_directory: HAsharedDirInputed(),
      type: HAtypeSelected()
   };
   const sameData = HAcompareData(Args);
   APIcall("ha/set", Args, function(R) {
      if(!R.success) {
         SNCKsnackbar('error', R?.error);
      } else {
         if(!sameData) {
            SNCKsnackbar('success', 'High availability settings saved');
         }
      }
      window.location = "#settings/high_availability";
   });
}
