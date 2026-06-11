
function STUpermissionUpdate(){
   STUupdateButton(STUclearArchivePerm(), "STUclearArchive");
   STUupdateButton(STUchangeFolderPerm(), "STUchange");
   STUupdateButton(STUchangeFolderPerm(), "STUchangeBackup");
}

function STUupdateButton(Perm, Class){
   let Args = {};
   Args.enabled = Perm;
   Args.target = document.querySelector(`.${Class}`);
   Args.disabledTitle = "You do not have permission to edit this.";
   BUTTONcontrol(Args);
}

function STUclearArchivePerm(){ return STUpermissionData()?.clear_archive; }
function STUchangeFolderPerm(){ return STUpermissionData()?.change_log_folder; }
