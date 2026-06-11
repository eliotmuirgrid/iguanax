// Done my best to try and get file save/loading consistent.
// I've tried to put some of the relevant routines int TRNfile and tried
// to name them with TRNfile prefixes for convenient prefixes for tracing.
// Eliot

let TRANSlastContentSaved = {};
let TRANSfileReloadAttempts = 0;

function TRANSfileLoad(Callback){
   let FileName = TRANSfile();
   APIcall('file/load', { 'component': TRANScomponentGuid(), 'file': FileName, development: TRANSdevelopment() }, function(Response) {
      if(!Response.success) {
         if(TRANSfileReloadAttempts < 2 )TRANSfileChange("main.lua");
         TRANSfileReloadAttempts++;
         return SNCKsnackbar('error', `Could not load file "${FileName}": ${Response.error}`);
         // reset the source dir values etc
      }
      TRANSfileReloadAttempts = 0;
      TRANSrenderedFileName = FileName;
      TRANSlastContentSaved[FileName] = Response?.data?.Code;
      TRANSfileValid = Response?.data?.FileValid;
      TRANSclearAnnotations();
      if(Callback) Callback(Response);  
   });
}

function TRANSfileResetGlobal(){
   COL_TRC("Resetting file state.");
   TRANSlastContentSaved = {};
}

function TRANSfileSave(FileName, Code, Callback){
   COL_TRC("FileName +", FileName, "Code =", Code);
   if(!FileName || !TRANSfileValid || !TRANSdevelopment()) {
      if(Callback) Callback();
      return;
   }
   if (FileName.indexOf(SMPLrootFolder()) == 0) Code = LFDsaveEncoded(Code);
   let Data = {};
   Data.component   = TRANScomponentGuid();
   Data.development = TRANSdevelopment();
   Data.content     = Code;
   Data.file        = FileName;
   if (Code === TRANSlastContentSaved[FileName]){
      COL_TRC("We already saved this - not again - TODO - why does Code Mirror do this?");
      if (Callback){
         COL_TRC("Still going to call back even though there was no change.");
         Callback();
      }
      return;
   }
   TRANSlastContentSaved[FileName] = Code;
   APIcall('file/save', Data, function(R) {
      if(!R.success) return SNCKsnackbar('error', `Could not save file "${FileName}": ${R.error}`);
      TRANScleanDirtyLine();
      if(Callback) { Callback(); }
   });
}

// TODO - rename?
function TRANShandleSaveCombo(Event) {
   if ((window.navigator.platform.match("Mac") ? Event.metaKey : Event.ctrlKey) && Event.keyCode == 83) {
      Event.preventDefault();
      TRANSfileCurrentSave(function(){
         MODELforcePoll("TRANSstatusModel");
         TRANSupdateAnnotations(); // TODO talk to Tyler about FirstRequest
      });
   }
}

function TRANSfileCurrentSave(Callback){
   COL_TRC("Saving file.");
   let NewContent = TRANSeditor.getValue();
   TRANSfileSave(TRANSrenderedFileName, NewContent, Callback);
}

function TRANSfile(){ 
   let Module = PAGEhashParameter("edit");
   if (!Module){
      Module = 'main.lua';
   }
   return Module; 
}

function TRANScompareToRenderedFile(FilePath){
   let Current = TRANSrenderedFileName; // TRANSrenderedFileName has os style path separators, but FilePath is always posix style
   Current = Current.replace(FILsep(), "/")
   return FilePath == Current
}

function TRANSgetPosixName(Name){
   let ReplaceValue = "/";
   let PosixFileName = Name.replaceAll(FILsep(), ReplaceValue);
   return PosixFileName;
}

function TRANSfileChange(Value){
   TRANSsaveCurrentModuleCursorPosition();
   let ReplaceValue = FILsep() == "\\" ? "/" : "\\";
   let OsFileName = Value.replace(ReplaceValue, FILsep());
   if(!SMPLisSampleData(Value)) return PAGEsetHashParameter("edit", OsFileName); 
   let Index = SMPLgetSampleIndex(Value);
   let Args = PAGEhashParameters();
   if(Index) Args.sample = Index;
   Args.edit = OsFileName;
   let Url = "#translator" + PAGEformatParams(Args);
   window.location.hash = Url;
}

function TRANSfileChangeWithoutSaving(Value){ 
   TRANSrenderedFileName = "";
   TRANSfileChange(Value);
}

function TRANSfileCurrentReload(){
   TRANSsaveCurrentModuleCursorPosition();
   TRANSideRequestCompile();
   TRANStranslatorScreen();
   HLPreloadWindow();
   TRANSrenderedFileName = "";
}

function TRANSfileReloadGitModules(){
   if(TRANSfile() === ".gitmodules") TRANSfileCurrentReload();
}

function TRANSswitchBetweenLinks(ComponentGuid="Custom_7zhUunvobnliIJ") {
   const urls = [
       'http://localhost:7654/?#translator?component=' + ComponentGuid + '&development=true&edit=Samples%2FSample_001&sample=1',
       'http://localhost:7654/?#translator?component=' + ComponentGuid + '&development=true&edit=Samples%2FSample_002&sample=2',
       'http://localhost:7654/?#translator?component=' + ComponentGuid + '&development=true&edit=Samples%2FSample_003&sample=3',
       'http://localhost:7654/?#translator?component=' + ComponentGuid + '&development=true&edit=Samples%2FSample_004&sample=4'
   ];

   let index = 0;

   function switchLink() {
       // Switch to the next URL
       window.location.href = urls[index];

       // Update the index to loop through the URLs
       index = (index + 1) % urls.length;

       // Call this function again after a short delay (e.g., 500 milliseconds)
       setTimeout(switchLink, 50); // Adjust the delay time as needed
   }

   // Start switching between the links
   switchLink();
}

// Start the test by calling the function
