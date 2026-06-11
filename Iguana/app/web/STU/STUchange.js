let STUpath;
let STUchangeCard;

function STUchange(){
   let Data = STUlogData();
   let Path = Data?.path || "";
   let LogPath = Data?.log_path || "";
   STUpath = Path;
   STUchangeCard = POPpopoutOpen({
      title    : "Change log folder",
      content  : STUchangeFolderHtml(Path, LogPath),
      width : "750px",
      esc_close: true
   });
   STUchangeListeners();
}
function STUchangeFolderHtml(Path, LogPath){
   let LogData = STUlogData();
   let Val = LogData?.dir_size || undefined;
   let Size = STUfuzzySize(Val);
   return /*html*/`
   <div class="STUmodal">  
      <div>Choose where Iguana should write future log files.</div>
      <br>
      <div class="STUbox">
         <div class="FORMlabel">Current log folder: (kept)</div>
         <br>
         <div class="STUarchivePathDialog">${LogPath} (${Size})</div>
      </div>
      <div>What happens:</div>
      <ul>
         <li>Existing logs stay in the current folder (not moved automatically).</li>
         <li>Iguana must restart to apply the change.</li>
         <li>Logs are stored inside a sub-folder: <b>logt/</b> (unencrypted) or <b>loge/</b> (encrypted).</li>
      </ul>
      <br>
      <div class="STUbox">
         <div class="FORMlabel">Future log folder:</div>
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

function STUchangeListeners(){
   let div = document.querySelector(".STUmodal");
   if(!div) return;
   div.addEventListener("click", STUchangeClick);
}

function STUchangeClick(e){
   if(e.target.closest(".STUbrowse")) STUbrowse();
   if(e.target.closest(".STUconfirm")) STUchangeConfirm();
   if(e.target.closest(".STUcancel"))  STUchangeCancel();
}

function STUchangeCancel(){
   POPpopoutClose(STUchangeCard);
}

function STUbrowse(){
   FILbrowser({
      value : document.querySelector('.STUnewPath').value,
      new_folder : false,
      dir_only : true,
      callback : function(T) { document.querySelector('.STUnewPath').value = T; }
   });
}

function STUchangeConfirm(){
   let div = document.querySelector(".STUnewPath");
   if(!div) return;
   let val = div?.value.trim();
   if(STUpath == val) return STUdoNothing();
   STUchangeExecute(val);
}

function STUdoNothing(){
   SNCKsnackbar("success", "No change.", 3000);
   STUchangeCancel();
}

function STUchangeExecute(Path){
   APIcall("log/directory/set", { log_dir : Path }, function(R){
      if(!R?.success) {
         STUchangeCancel();
         return SNCKsnackbar("error", R?.error || "An error occurred while saving.");
      }
      let Path = R?.data?.log_dir || "";
      let Message = GINsaveNewLogFolder(Path);
      GINaddAndCommit({message: Message, settings: true, callback : GINrestart});
   });
}