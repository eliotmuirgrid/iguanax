/**
 * Copyright (c) 2010-2014 iNTERFACEWARE Inc.  All rights reserved.
 */

let TRANSifware = {};

function TRANSrequestCompileResult(R){
   COL_TRC(R);
   if (!R.success) { 
      COL_TRC("Failed:", R.error);
   }
}

function TRANSideRequestCompile(Callback){
   COL_TRC("ComponentId =", TRANScomponentGuid());
   APIcall("ide/compile/set", { component: TRANScomponentGuid(), development: TRANSdevelopment() }, function(R){
      TRANSrequestCompileResult(R);
      if(Callback) Callback();
   });
}

function TRANSrefreshCodeMirror(){ $(".CodeMirror").each(function (i, el) { el.CodeMirror.refresh(); }); }

// FilePath - Relative path (Ex. main.lua, test/main.lua)
// Returns true if locked, false otherwise
function TRANSisReadOnly(FilePath) {
   if (!FilePath || typeof(FilePath) !== "string") return false;
   const LibName = FilePath.split(FILsep())[0];
   if (TRANSreadOnly()) return true;
   return !LIBlibraryUnlocked(LibName);
}

function TRANSreadOnly(){
   return !TRANSdevelopment();
}

function TRANScurrentFileType() {
   return SMPLfileType(TRANSrenderedFileName);
}

function TRANSloadCode(Code, FileName){
   let IsSampleData = SMPLisSampleData(FileName);
   let Content = Code;
   if(IsSampleData) {
      Content = LFDloadDecoded(Code);
      LFDsetSelectedLineFeed(Content);
   }
   TRANSeditor.setValue(Content);
   if (FileName) SMPLsetIndexWithFileName(FileName);
}

function TRANScreateEditorWithContent(Element, ScriptInfo) {
   COL_TRC("Create editor with content.");
   document.querySelectorAll('.CodeMirror').forEach(editor => editor.remove());
   TRANSeditor = new CodeMirror(Element, {
      mode       : TRANScurrentFileType(),
      value      : ScriptInfo.Code,
      theme      : 'ifware',
      gutters    : ['CodeMirror-linenumbers', 'dirty'],
      extraKeys  : {'Tab': TRANSonTabPress, 'Shift-Tab': TRANSonTabPress},
      lineNumbers: true,
      indentUnit : 3,
      tabSize    : 3,
      fixedGutter: true,
      specialChars: LFDspecialChars(),
      specialCharPlaceholder: LFDspecialCharPlaceholder,
      readOnly   : !TRANSdevelopment()   /* Mode = true means read only, false means edit allowed */
   });
   TRANSloadCode(ScriptInfo.Code, TRANSrenderedFileName);
   if (!TRANSeditor.getHistory || !TRANSeditor.setHistory) { alert('Warning: editor history patch (#17864) is unavailable.  Has CodeMirror been upgraded?'); }
   TRANSifware.focusLastFocussedElement = function(){};
}

function TRANSresetGlobals(InitializePage){
   TRANSifware = {};
   if(InitializePage) TRANSmoduleState = {};
   TRANScurrentDirtyLine = null;
   TRANSfileResetGlobal();
}