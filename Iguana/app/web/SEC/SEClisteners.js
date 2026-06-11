function SEClisteners(){
   let div = document.querySelector(".SANDmodal");
   if(!div) return;
   div.removeEventListener("click", SECclickListeners);
   div.addEventListener   ("click", SECclickListeners);
}

function SECclickListeners(e){
   if(e.target.closest(".SANDdelete")) return SECdeleteRow(e.target);
   if(e.target.closest(".SANDsave"))   return SECsave();
   if(e.target.closest(".SECadd"))    return SECaddRow();
   if(e.target.closest(".SANDbrowse")) return SECbrowse(e.target);
}

function SECdeleteRow(target){
   target.closest(".SECrow").remove();
}

function SECaddRow(){
   let Row = SECrowEditHtml("");
   let Container = document.querySelector(".SECdirsTable");
   Container.insertAdjacentHTML("beforeend", Row);
}  

function SECgetDirs(){
   let Dirs = [];
   for(let input of document.querySelectorAll('.SANDeditValue')){
      let value = input.value.trim();
      if(value === "") continue;
      Dirs.push(value);
   }
   return Dirs;
}

function SECgetEnabled(){
   return document.querySelector('.SANDuse').value === "1";
}

function SECosExEnabled(){
   return document.querySelector('.SECosExecute').value === "1";
}

function SECsave() {
   const Dirs           = SECgetDirs();
   const DirsEnabled    = SECgetEnabled();
   const OsExRestricted = SECosExEnabled();
   let Args             = {dirs: Dirs, enable_directories: DirsEnabled, restrict_execution: OsExRestricted};
   APIcall('security/set', Args, R => {
      if(!R.success) { SNCKsnackbar('error', R.error); }
      else {
         let Message = GINsaveDirectoryAccess(Dirs, DirsEnabled, OsExRestricted);
         GINaddAndCommit({message: Message, settings: true});
      }
      window.location = "#settings/security";
   });
}

function SECbrowse(target){
   let input = target.closest(".SECrow").querySelector(".SANDeditValue");
   FILbrowser({
      value      : input.value,
      new_folder : false,
      show_hidden: false,
      dir_only   : true,
      callback   : function(T) { input.value = T; }
   });
}