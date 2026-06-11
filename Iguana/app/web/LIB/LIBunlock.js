let LIBpopout;

function LIBlock(menuItem){
   let Path = menuItem.path;
   let Name = menuItem.name;
   let Repo = TRGTlibraryList()[Name].url;
   LIBUrlApiLock(Repo, Path);
}

function LIBUrlApiLock(Repo, Path) {
   APIcall("git/all_urls", {url : Repo}, function(R){
      if (R.success){
         LIBunlockExecute(Path, R?.data?.web);
      } else {
         SNCKsnackbar('error', 'Cannot lock library: ' + R?.error || "");
      }
   });
}

function LIBunlock(Path, Name, callback) {
   const Repo = TRGTlibraryList()[Name].url;
   SNCKsnackbar("info", "Unlocking library...", 1500);
   APIcall("git/writable", {repo: Repo}, function(R){
      let Writable = R?.data?.writable;
      APIcall("git/all_urls", {url : Repo}, function(AllUrlsResponse){
         if(!AllUrlsResponse?.success) return SNCKsnackbar('error', 'Cannot unlock library - could not convert git url to ssh form');
         let SshUrl = AllUrlsResponse?.data?.ssh || "";
         let WebUrl = AllUrlsResponse?.data?.web || "";
         if(Writable) LIBunlockExecute(Path, SshUrl, callback);
         else         LIBwarningDialog(WebUrl, SshUrl, Path);
      });
   });
}

function LIBwarningDialog(Repo, SshUrl, Path, callback){
   let Content = LIBwarningHtml(Repo);
   LIBpopout = POPpopoutOpen({
      title: 'Library Edit Warning', 
      content: Content, 
      width: "600px", 
      callback : TRANSsetCode
   });
   document.querySelector('.LIBunlockLib').addEventListener('click', function() { 
      LIBunlockExecute(Path, SshUrl, callback); 
      POPpopoutClose(LIBpopout);  
   });
   document.querySelector('.LIBcancel')   .addEventListener('click', function() { POPpopoutClose(LIBpopout); TRANSsetCode(); });
}

function LIBwarningHtml(Repo){
   return /*html*/`
   <div class="LIBrow">
      <div>You do not have write access to: </div>
      <a class="LIBlink" target="_blank" href="${Repo}">${Repo}</a>
   </div>
   <br>
   <br>
   <b class="LIBwarningText">Are you sure you want to unlock the library?</b>

   <div class="BUTTONgroup LIBbuttons">
      <div class='LIBcancel BUTTONcancel BUTTONstandard'>CANCEL</div>
      <div class='LIBunlockLib BUTTONaction BUTTONstandard'>CONFIRM</div>
   </div>`;
}

function LIBunlockExecute(Path, Url, callback){
   let LibMap = TRGTlibraryList();
   if(!LibMap) {return false; }
   let Args = {};
   Args.component = TRANScomponentGuid();
   Args.name = Path;
   Args.git = Url;
   Args.development = TRANSdevelopment();
   Args.push = false;
   APIcall("component/library/set_upstream", Args, function(R){
      if(!R.success) SNCKsnackbar("error", "Could not complete setting upstream of library: " + R?.error || "");
      TRANSopenDir = Path;
      MODELforcePoll('TRANSmodel');
      MODELforcePoll('TRANSstatusModel', function(){
         if(callback) callback();
      });
      MENUremoveTooltip();
      TRANSsetCode();
   });
}

function LIBcheckUnlocked() {
   const isDevView = TRANSdevelopment();
   const libName   = TRANSfile().split(FILsep())[0];
   if(!libName)   { return; }
   if(!isDevView) { return TRANSsetWritable(false); }
   let Unlocked = LIBlibraryUnlocked(libName);
   TRANSsetWritable(Unlocked);
}

function TRANSsetWritable(writable) {
   let $editor = document.querySelector('#TRANSeditorDiv');
   if(!$editor) { return; }
   !writable ? $editor.classList.add('TRANSreadOnlyClass') : $editor.classList.remove('TRANSreadOnlyClass');
   if (!TRANSeditor){
      COL_TRC("TRANSsetWritable called before we have an editor");
   } else {
      // TODO - Why does this get called so much
      COL_DBG("Writable=", writable);
      TRANSeditor.options.readOnly = !writable;
   }
}
