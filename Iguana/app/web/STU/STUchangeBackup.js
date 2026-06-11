let STUbackupPath;
let STUbackupCard;

function STUchangeBackup(){
   let Data = STUbackupData();
   let Path = Data?.path || "";
   let LogPath = Data?.log_path || "";
   STUbackupPath = Path;
   STUbackupCard = POPpopoutOpen({
      title    : "Change backup log folder",
      content  : STUbackupFolderHtml(Path, LogPath),
      width : "750px",
      esc_close: true
   });
   STUbackupListeners();
}

function STUbackupFolderHtml(Path, LogPath){
   let LogData = STUbackupData();
   let Val = LogData?.dir_size || undefined;
   let Size = STUfuzzySize(Val);
   let PathHtml = Path ? `${LogPath} (${Size})`: STUnotSetHtml();
   return /*html*/`
   <div class="STUmodal">  
      <div>Choose a destination to duplicate each new log to for redundancy.</div>
      <br>
      <div class="STUbox">
         <div class="FORMlabel">Current backup folder: (kept)</div>
         <br>
         <div class="STUarchivePathDialog">${PathHtml}</div>
      </div>
      <div>What happens:</div>
      <ul>
         <li>Iguana writes into a sub-folder: <b>logt/</b> (unencrypted) or <b>loge/</b> (encrypted).</li>
         <li>Logs are duplicated here in real-time</li>
         <li>The folder should be on a remote host.</li>
         <li>The folder must be writable.</li>
      </ul>
      <br>
      <div class="STUbox">
         <div class="FORMlabel">Backup log folder:</div>
         <br>
         ${STUinputHtml(Path)}
      </div>
      <div>A logt/ or loge/ sub-folder will be created automatically.</div>
      <br>
      <div class="BUTTONgroup STUbuttons">
         <div class="BUTTONstandard BUTTONdefault STUcancel">Cancel</div>
         <div class="BUTTONstandard BUTTONaction STUconfirm">Change & Restart</div>
      </div>
   </div>
   `;
}

function STUinputHtml(Path){
   return /*html*/`
   <div class="FORMinputGroup">
      <input class="FORMinput STUnewPath" type="text" value="${Path}">
      <div class="STUbrowse BUTTONstandard BUTTONcancel">BROWSE...</div>
   </div>
   `;
}

function STUbackupListeners(){
   let div = document.querySelector(".STUmodal");
   if(!div) return;
   div.addEventListener("click", STUbackupClick);
}

function STUbackupClick(e){
   if(e.target.closest(".STUbrowse"))  STUbrowse();
   if(e.target.closest(".STUconfirm")) STUbackupConfirm();
   if(e.target.closest(".STUcancel"))  STUbackupCancel();
}

function STUbackupCancel(){
   POPpopoutClose(STUbackupCard);
}

function STUbrowse(){
   FILbrowser({
      value : document.querySelector('.STUnewPath').value,
      new_folder : false,
      dir_only : true,
      callback : function(T) { document.querySelector('.STUnewPath').value = T; }
   });
}

function STUbackupConfirm(){
   let div = document.querySelector(".STUnewPath");
   if(!div) return;
   let val = div?.value.trim();
   if(STUbackupPath == val) return STUdoNothingBackup();
   STUbackupExecute(val);
}

function STUdoNothingBackup(){
   SNCKsnackbar("success", "No change.", 3000);
   STUbackupCancel();
}

function STUbackupExecute(Path){
   APIcall("log/directory/set", { alt_dir : Path }, function(R){
      if(!R?.success) {
         STUbackupCancel();
         return SNCKsnackbar("error", R?.error || "An error occurred while saving.");
      }
      let Path = R?.data?.alt_dir || "";
      let Message = GINsaveNewBackupLogFolder(Path);
      GINaddAndCommit({message: Message, settings: true, callback : GINrestart});
   });
}