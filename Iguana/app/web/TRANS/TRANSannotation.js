/**
 * Copyright (c) 2010-2011 iNTERFACEWARE Inc.  All rights reserved.
 */
// TODO - prefixes are wrong.

let TRANSeditor = null;
let TRANScurrentDirtyLine = null;
let TRANSmoduleState = {};
let TRANScurrentHighlightedLine = null;
let TRANSfadeTimeout = null;

let TRANSrequestFunctionalBlock = null;

function TRANSblockLeftMargin(Line) {
   let Count = 0;
   for (let i = 0; i < Line.length; ++i) {
      if (Line.charAt(i) == " ") ++Count;
       else break;
   }
   return Count;
}

function TRANSretrieveCodeBlockData() {
   const CharWidth = TRANSifware.CharWidth = TRANScharWidth();
   TRANSifware.BlockDataSet = {};
   TRANSifware.BlockDataSet.length = 0;
   let i = 0;
   let LineNumber = -1;
   let IsMultiLineComment = false;
   while ((Line = TRANSeditor.getLine(++LineNumber)) !== undefined) {
      if (!Line.match(/^\s*$/)) {
         const BlockData = {};
         BlockData.c = TRANSblockLeftMargin(Line);
         BlockData.w = CharWidth * Line.length;
         BlockData.s = LineNumber;
         BlockData.e = BlockData.s + 1;
         if (IsMultiLineComment) {
            if (Line.match(/^.*\]\].*$/)) {
               BlockData.type = "comment";
               IsMultiLineComment = false;
            }
            else  BlockData.type = "comment";
         }
         else {
            if (Line.match(/^\s*\-\-\[\[.*/)) {
               BlockData.type = "comment";
               IsMultiLineComment = true;
            }
            else if (Line.match(/^\s*\-\-.*$/) || Line.match(/^\s*\-\-\[\[.*\]\].*/)) { BlockData.type = "comment"; }
            else if (Line.match(/^\s*require\s*.*$/))                                 { BlockData.type = "requires";}
            else if (Line.match(/^function .*$/))                                     { BlockData.type = "function";}
            else if (Line.match(/^end.*$/))                                           { BlockData.type = "end";     }
            else                                                                      { BlockData.type = "line";    }
         }
         TRANSifware.BlockDataSet[i] = BlockData;
         ++(TRANSifware.BlockDataSet.length);
         ++i;
      }
   }
}

function TRANSonTabPress(){
   // for sample data, we want to remove {{IFWTOKEN}}s before fixing indentation
   // for regular files, they don't have {{IFWTOKEN}}s so no harm done
   const regexToExclude = /^end\w+/;
   const SampleDataTokensRegex = /({{IFWTOKEN}}([0-9A-F]{2}|CR|LF){{IFWTOKEN}}|\n)/g;
   const SelectedText = TRANSeditor.getSelection().replace(SampleDataTokensRegex, "");
   const LineText = TRANSeditor.getLine(TRANSeditor.getCursor().line).replace(SampleDataTokensRegex, "");
   if (SelectedText === "" && LineText === "") {
      TRANSeditor.replaceSelection("\t");
      return;
   }
   const cursorPos = TRANSeditor.getCursor(); 
   const currentLine = TRANSeditor.getLine(cursorPos.line).trim();
   if (currentLine.match(regexToExclude) && SelectedText==="") {  // Allows indentation for code that has 'end' as a prefix.
      TRANSeditor.replaceSelection("\t");
      return;
   }
   TRANSeditor.operation(function(){
      const from = TRANSeditor.getCursor('from').line;
      const to = TRANSeditor.getCursor('to').line;
      for(let i = from; i <= to; i++){         // Allows indentation for code that has 'end' as a prefix.
         const lineText = TRANSeditor.getLine(i);
         if (lineText.trim().match(regexToExclude)) {
            return;
         }
         TRANSeditor.indentLine(i);
      }
   });
}

function TRANScreateEditor(Response, Callback){
   COL_TRC("Creating editor", "IsDevelopment=", TRANSdevelopment());
   if (!Response.success) return TRANShandleEditorLaunchError(Response.error);
   TRANScreateEditorWithContent(document.querySelector('#TRANSeditorDiv'), Response.data);
   TRANSifware.CharWidth = TRANScharWidth();
   TRANSifware.BlockInfo = {};
   TRANSinit();
   if(Callback) Callback();
}

function TRANSgetLineTop(LineNumber){
   COL_TRC(LineNumber);
   let Div = document.querySelector('#TRANSeditorDiv .CodeMirror-sizer');
   if(!Div) return;
   let Offset = Div.getBoundingClientRect().top + window.scrollY;
   const LineTop = TRANSeditor.charCoords({line: LineNumber, ch: 0}).top;
   return LineTop - Offset + 1;
}

function TRANSgetLineLeft() {
   return document.querySelector('#TRANSeditorDiv .CodeMirror-gutter:last-child')?.offsetWidth + 4;
}

function TRANSeditorIsEmpty(){ return TRANSeditor.lineCount() === 0; }
function TRANScharWidth()    { return TRANSeditor.defaultCharWidth(); }
function TRANScharHeight()   { return TRANSeditor.defaultTextHeight(); }
function TRANSgetEditorScrollLeft() {
   return document.querySelector('#TRANSeditorDiv .CodeMirror-scroll')?.scrollLeft || 0;
}

function TRANSsetEditorScrollLeft(NewScrollLeft) {
   let ScrollDiv = document.querySelector('#TRANSeditorDiv .CodeMirror-scroll');
   if (ScrollDiv) ScrollDiv.scrollLeft = NewScrollLeft;
}

function TRANSeditorHeight() { return document.querySelector('#TRANSeditorDiv .CodeMirror')?.clientHeight || 0;}
// Get the width of the editor, excluding the scroll-bar on the right.
function TRANSeditorInnerWidth()   { 
   let Div = document.querySelector("#TRANSeditorDiv .CodeMirror-sizer");
   if(!Div) return;
   return Div.clientWidth;  
}
function TRANSmarkLineAsDirty(LineNumber) {
   TRANScurrentDirtyLine = LineNumber;
   TRANSeditor.setGutterMarker(LineNumber, "dirty", (function(){
      const span = document.createElement('span');
      span.className = 'dirty';
      span.style.display = 'inline-block';
      span.style.width = '100%';
      span.innerHTML = '';
      return span;
   })());
   TRANSmarkAnnotationAsStale(LineNumber);
}

function TRANShighlightLine(LineNumber) {
   const CurrentLineClasses = TRANSeditor.lineInfo(LineNumber)?.bgClass;
   const NoOriginalBackground = CurrentLineClasses == undefined || !CurrentLineClasses.includes('DIFF');
   if (LineNumber === TRANSerrorLine) return;
   if (TRANSfadeTimeout && TRANScurrentHighlightedLine !== null) {
      clearTimeout(TRANSfadeTimeout);
      TRANSeditor.removeLineClass(TRANScurrentHighlightedLine, "background", 'ANNhighline');
      TRANSeditor.removeLineClass(TRANScurrentHighlightedLine, "background", 'ANNhighlineFading');
   }
   TRANSeditor.addLineClass(LineNumber, "background", 'ANNhighline');
   TRANScurrentHighlightedLine = LineNumber;
   if (!NoOriginalBackground) return;
   document.querySelectorAll('.ANNhighline').forEach(span => span.removeAttribute('style'));
   TRANSfadeTimeout = setTimeout(function () {
      TRANSeditor.addLineClass(LineNumber, "background", 'ANNhighlineFading');
      let Fading = document.querySelectorAll('.ANNhighlineFading');
      Fading.forEach(span => span.removeAttribute('style'));
      Fading.forEach(span => span.style.backgroundColor = '#D5E9FF');
      Fading.forEach(span => {
         span.animate([
            { backgroundColor: '#D5E9FF' },
            { backgroundColor: '#FFFFFF' }
         ], {
            duration: 600, // Approximate 'slow' duration in ms
            easing: 'ease'
         }).onfinish = () => {
            TRANSeditor.removeLineClass(LineNumber, "background", 'ANNhighlineFading');
            TRANSeditor.removeLineClass(LineNumber, "background", 'ANNhighline');
         };
      });
   }, 5000);
}

function TRANSgetSampleFileFromParams(Var){
   const SampleFile = PAGEhashParameter("sample_file");
   if (SampleFile){
      Var.sample_file = SampleFile;
   }
}

function TRANSprepareLuaPostInfo() {
   let Vars = {};
   Vars.filename          = TRANSfile();
   Vars.sample_data_index = SMPLindex();
   Vars.component         = TRANScomponentGuid();
   Vars.development       = TRANSdevelopment();
   Vars.line              = TRANSeditor.getCursor().line;
   Vars.code              = TRANSeditor.getValue();
   Vars.dirty             = TRANScurrentDirtyLine;
   TRANSgetSampleFileFromParams(Vars); // provide a way to override the sample index param for now
   COL_TRC("PostVariables =", Vars);
   return Vars;
}

function TRANSprocessAnnotation(Response, FirstRequest) {
   COL_TRC(Response);
   // FirstRequest should be set to true if this is the first annotations request being sent for the editor.
   TRANSannotationStatus('');
   Response = Response.data;
   if (Response.ErrorMessage) {
      COL_TRC("We got an error message.");
      return TRANSannotationStatus(Response.ErrorMessage);
   }
   // The following regular expression checks to see if the editor only contains
   // whitespace. This is typically the case when the user has not imported a starting project yet.
   if (!(TRANSeditor.getValue() || '').match(/^\s*$/)) {
      // Apply focus to the editor upon initialization but only if the warning
      // dialog is not displayed. This is so that users can still close the dialog by pressing Escape.
      if (FirstRequest) {
         let Active = document.activeElement;
         let Div = document.querySelector(".PRJcomponentSelect");
         let Search = document.querySelector('.SRCHsidebarSearchInput')
         if(Div !== Active && Search !== Active) TRANSeditor.focus();
      }
   }
   TRANSretrieveCodeBlockData();
   if (Response.BlockData) {
      TRANSannotationStatusOk();
      TRANSrefreshBlockInfo2(Response.BlockData);
      TRANSretrieveAnnotationBlockData(Response.BlockData);
      TRANSmarkCurrentLine();
   }
   else{
      TRANSclearAnnotations();
   }
   TRANSscrollToLineCheckCallBack();
   //BROupdateDialogs(Response.BroArgs, Response.ScriptError);
   TRANSupdateDialogs(Response.FullArgumentResponses, Response.ScriptError);
   ITMresetCopiedHtml();
};

function TRANSclearAnnotations(){ document.querySelectorAll('div.ANNblock').forEach(div => div.remove()); }

function TRANSupdateAnnotations(Options, Callback) {
   if (!Options) Options = {};
   COL_TRC("Options =", Options);
   if (!TRANSannotationsEnabled() && !Options.force) {
      COL_TRC("Annotations are not enabled.");
      TRANSannotationStatus('');
      if(Callback) Callback();
      return TRANSclearAnnotations();
   }
   COL_TRC("About to update annotations");
   TRANSclearErrors();
   let Values = TRANSprepareLuaPostInfo();  // Use common helper function to pack in Lua information
   COL_TRC("RequestFunctionBlock =", TRANSrequestFunctionalBlock);
   if (TRANSrequestFunctionalBlock) {
      Values.function_block = TRANSrequestFunctionalBlock;
      TRANSrequestFunctionalBlock = null;
      COL_TRC("Clearing request functional block");
   }
   let IsViewModeChange = Options?.view_mode_change;
   if (Options.view_mode_change) { delete Options.view_mode_change; }
   Values.full_argument_requests = TRANSgetFullArgumentRequestsList(IsViewModeChange);
   COL_TRC("Values=", Values);
   return TRANSideAnnotationRequest(Values, Options, Callback);
}

// Eliot - following a convention of naming all the annotation calls TRANSide to make them easy to trace with trace=TRANSide
function TRANSideAnnotationRequest(Values, Options, Callback){
   TRANSannotationRequestButtons(true);
   TRANSannotationStatusBusy();
   TRANSsetDialogsToWaiting();
   TRANSabort('TRANSannotationRequest');
   COL_TRC("/ide/annotation call", Values);
   let Request = APIcall("ide/annotation", Values, function(R){
      TRANSannotationRequestButtons(false);
      if (!R.success) {
         if(Callback) Callback();
         return TRANSannotationStatus(R.error);
      }
      TRANSabortRequestDone('TRANSannotationRequest');
      TRANSprocessAnnotation(R, Options.FirstRequest);
      TRANSdisplayScriptError(R.data.ScriptError);
      TRANSnavigateToScriptError(R.data);
      WNDremoveOuterPopouts();
      if(Callback) Callback();
   });
   TRANSabortRegister('TRANSannotationRequest', Request);
}

function TRANSannotationRequestButtons(AnnotationsInProgress){
   TRANSplayButton(!AnnotationsInProgress);
   TRANSstopButton(AnnotationsInProgress);
}

function TRANSsaveCurrentModuleCursorPosition() {
   if(!TRANSeditor) return;
   let ModuleState = TRANSmoduleState[TRANSrenderedFileName];
   if (!ModuleState) {
      ModuleState = {};
      TRANSmoduleState[TRANSrenderedFileName] = ModuleState;
   }
   ModuleState.CursorPosition = TRANSeditor.getCursor();
}

function TRANScurrentModuleState() {
   let ModuleState = TRANSmoduleState[TRANSrenderedFileName];
   if (!ModuleState) {
      ModuleState = {};
      ModuleState.CursorPosition = {line:0, ch:0};
      TRANSmoduleState[TRANSrenderedFileName] = ModuleState;
   }
   return ModuleState;
}

function TRANSjumpToErrorIfExists() {
   const LineJump = STOREreadFileJump();
   if(LineJump) {
      TRANSeditor.setCursor({line: LineJump, ch: 0});
      return true;
   }
   return false;
}

function TRANSsetCode(){
   if(!TRANSeditor) return;
   if( SRCHsearchOpen()) return;
   let Active = document.activeElement;
   let Div = document.querySelector(".PRJcomponentSelect");
   let Search = document.querySelector('.SRCHsidebarSearchInput')
   if(Div === Active || Search === Active) return;
   const ModuleState = TRANScurrentModuleState();
   if (ModuleState.History) TRANSeditor.setHistory(ModuleState.History);
   else                     TRANSeditor.clearHistory();
   let CursorData = ModuleState.CursorPosition || {line:0, ch:0};
   TRANSeditor.focus();
   if(TRANSjumpToErrorIfExists()) {
      return;
   }
   if(DEEPequal(TRANSeditor.getCursor(), CursorData)) return;
   if(DEEPequal({line:0, ch:0}, CursorData)) return;
   TRANSeditor.setCursor(CursorData);
}
// TODO - maybe do a save and annotate around this
function TRANScleanDirtyLine() {
   TRANSeditor.clearGutter("dirty");
   TRANScurrentDirtyLine = null;
}

// 'Change' is an object passed through by codemirror with the following properties:
// 'from' - an object in the format {ch, line}
// 'to' - an object in the format {ch, line}
// 'text' - an array of lines of text inserted in the change.  If no text was inserted, the array contains one entry, "".
function TRANSshouldMarkCursorLineAsDirty(Change, CursorLineNumber) {
   // If the current line is whitespace then we won't consider the line dirty.
   // If the change was a multi-line insertion, we won't consider the line dirty.
   if ( TRANSeditor.getLine(CursorLineNumber).match(/^\s*$/) == null && Change.text.length == 1) {
      if (Change.to.line == CursorLineNumber && Change.from.line == CursorLineNumber) {
         return true;// If the change is localized to one line, line is dirty.
      } else if (Change.text[0] != "" && CursorLineNumber >= Change.from.line && CursorLineNumber <= Change.to.line) {
         return true; // If some characters were inserted, even though the change spans multiple lines.
      }
   }
   return false;
}

function TRANSunmarkSaveAndRequestAnnotations(){
   COL_TRC("Unmarking dirty line and saving and requesting annotations");
   clearTimeout(TRANSupdateAnnotationsTimer);
   TRANScleanDirtyLine();
   TRANSfileCurrentSave(function(){
      MODELforcePoll("TRANSstatusModel");
      TRANSupdateAnnotations({FirstRequest : true}); // TODO talk to Tyler about FirstRequest
   });
}

function TRANSdefaultOnChange(Change) {
   COL_TRC(Change);
   if (TRANScurrentDirtyLine) TRANScleanDirtyLine();
   let div = document.querySelector('.error-box');
   if (div && !div.classList.contains('moved')) TRANSclearErrorBox();
   const CursorLine = TRANSeditor.getCursor().line;
   if (TRANSshouldMarkCursorLineAsDirty(Change, CursorLine) ) { // have this return on switch file
      TRANSmarkLineAsDirty(CursorLine);
      return;
   }
   TRANSretrieveCodeBlockData();
   TRANSunmarkSaveAndRequestAnnotations();
}

function TRANSdefaultOnCursorMove() {
   COL_TRC("On cursor move.");
   const CursorLineNumber = TRANSeditor.getCursor().line;
   if (SMPLeditingSampleData()) LFDrenderLineFeeds(TRANSeditor);
   if (TRANScurrentDirtyLine !== null && CursorLineNumber !== TRANScurrentDirtyLine) {
      TRANSunmarkSaveAndRequestAnnotations();
      return;
   }
   // Only mark the line if the editor is in focus.
   if ($('#TRANSeditorDiv > .CodeMirror-focused').size()) {
      TRANSmarkCurrentLine(CursorLineNumber);
   }
}

function TRANSmarkCurrentLine(CursorLineNumber) {
   if (!CursorLineNumber) {
      TRANSifware.CurrentLine = undefined;
      CursorLineNumber = TRANSeditor.getCursor().line;
   }
   if (TRANSifware.CurrentLine !== CursorLineNumber) {
      const AnnotationLines = $('#TRANSeditorDiv .CodeMirror-sizer').find('div.ANNblock pre');  // seems indirect?
      AnnotationLines.filter('.ANNhighlight').removeClass('ANNhighlight');
      AnnotationLines.filter('.ANNline_' + (CursorLineNumber + 1) + ':last').addClass('ANNhighlight');
      TRANSifware.CurrentLine = CursorLineNumber;
   }
}

function TRANSclearAnnotationBlockData() {
   const CleanedBlockDataSet = {};
   const NumberOfBlocks = TRANSifware.BlockDataSet.length;
   let j = 0;
   for (let i = 0; i < NumberOfBlocks; ++i) {
      const BlockData = TRANSifware.BlockDataSet[i];
      if ("annotation" != TRANSifware.BlockDataSet[i].type) {
         CleanedBlockDataSet[j] = BlockData;
         ++j;
      }
   }
   TRANSifware.BlockDataSet = CleanedBlockDataSet;
   TRANSifware.BlockDataSet.length = j;
}

function TRANSretrieveAnnotationBlockData(BlockDataSet) {
   COL_TRC(BlockDataSet);
   TRANSclearAnnotationBlockData();
   const NumberOfAnnotations = BlockDataSet.length;
   const NumberOfCodeLines = TRANSifware.BlockDataSet.length;
   let i = NumberOfCodeLines, j = 0;
   for (; j < NumberOfAnnotations; ++i, ++j) {
      const BlockData = BlockDataSet[j];
      const CopiedBlockData = {};
      //CopiedBlockData.t = BlockData.t;
      CopiedBlockData.c = BlockData.c;
      CopiedBlockData.w = BlockData.w;
      CopiedBlockData.s = BlockData.scope.s;
      CopiedBlockData.e = BlockData.scope.e;
      CopiedBlockData.type = "annotation";
      TRANSifware.BlockDataSet[i] = CopiedBlockData;
      ++(TRANSifware.BlockDataSet.length);
   }
   // Unfortunately I have to keep two copies since some functions expect you to pass only the annotation blocks while others want all block types.
   TRANSifware.AnnotationBlockDataSet = BlockDataSet;
}

function TRANSrefreshBlockInfo2(BlockUpdate) {
   const CharWidth = TRANSifware.CharWidth = TRANScharWidth();
   const NewBlockTable = {};
   let i;
   let Blocks = TRANSifware.BlockInfo;
   // the substring conditional is to prevent annotations from rendering on non-lua pages
   // going from main.lua to config.json would render main.lua annotations if check doesnt exist
   let SelectedFile = TRANSselectedFile();
   COL_TRC("SelectedFile =", SelectedFile);
   if (TRANSeditorIsEmpty() || !BlockUpdate || SelectedFile.substring(SelectedFile.length - 4) !== '.lua') {
      TRANSclearAnnotations();
      return;
   }
   for (i = 0; i < BlockUpdate.length; i++) {
      NewBlockTable[BlockUpdate[i].scope.s] = i;
   }
   COL_TRC('Set up new block table');
   for (i in Blocks) {
      if (typeof NewBlockTable[i] == "undefined") {
         delete Blocks[i];
         $("#TRANSeditorDiv .CodeMirror #n" + i).remove();
      }
   }
   COL_TRC('Removed blocks that no longer exist.');
   const LineLeft = TRANSgetLineLeft();
   for (i in NewBlockTable) {
      if (typeof Blocks[i] == "undefined") {
         Blocks[i] = {"count":BlockUpdate[NewBlockTable[i]].scope.l};
      } else {
         $("#TRANSeditorDiv .CodeMirror #n" + i).remove(); // removing for now since our creation logic is dumb
         Blocks[i].count = BlockUpdate[NewBlockTable[i]].scope.l; // call count
      }
      TRANScreateBlock(CharWidth, LineLeft, BlockUpdate[NewBlockTable[i]]);
   }
   // #26714 In previous CodeMirror, this happened automatically, but here is an issue
   // now where it's not displaying the scrollbars for annotations on page load.
   if (TRANSeditor) TRANSeditor.refresh();
}

function TRANStreeviewOnClick(Event, Element) {
   Event.preventDefault();
   TRANSshowPopupOrDockTab(Element.id);
}

function TRANSonFunctionJumpToLine(Line){
   COL_TRC(Line);
   TRANSscrollToLine(Line);
   TRANSsetEditorScrollLeft(0); // scroll to the leftmost of the page since otherwise the scroll down might be too far right to see the function definition.
   TRANShighlightLine(Line);
}

function TRANSonFunctionCallClick(Event, Element) {
   Event.preventDefault();
   // TODO - horrible design putting application state into DOM - it's brittle
   // when it comes to file names with : characters in the middle of them.
   const FcInfo = $(Element).find('div').text().split(':');
   COL_TRC($(Element).find('div').text())
   COL_TRC("File Jump Parameters", FcInfo);
   const FileLabel = FcInfo[0];
   const FunctionStartLine = parseInt(FcInfo[1]);
   const FunctionCallNumber = parseInt(FcInfo[2]);
   const GoToLine = parseInt(FcInfo[3]) - 1; //codemirror line versus the line where the Lua function starts 
   const CalleeAddress = FcInfo.length > 4 ? FcInfo[4] : undefined;
   COL_TRC("CalleeAddress =", CalleeAddress)
   if (CalleeAddress){
      TRANScalleeJump(FileLabel, FunctionStartLine, GoToLine, CalleeAddress);
   } else {
      COL_TRC("Caller jump");
      TRANScallerJump(FileLabel, FunctionStartLine, FunctionCallNumber, GoToLine);
   }
}

function TRANSonFunctionCallEnter(Element) {
   let Image;
   const jElement = $(Element);
   if (jElement.hasClass('fcin')) Image = '<img src="/TRANS/images/step_in.png" />';
   else                           Image = '<img src="/TRANS/images/step_out.png" />';
   const ElementPosition = jElement.position();
   $(Image).appendTo(jElement)
      .css('top', ElementPosition.top + 'px')
      .css('left', (ElementPosition.left - 18) + 'px')
      .css('border', 'none')
      .show();
}

function TRANSonFunctionCallLeave(Element) { $(Element).find('img').stop(true, true).remove(); }

function TRANSannotationBlock(FunctionStartLine){
   let Ann = $('#n' + FunctionStartLine).closest('div.ANNblock');
   return Ann;
}

function TRANSupdateAnnBlock(BlockData) {
   COL_TRC(BlockData);
   COL_TRC("FunctionalStartLine=", BlockData.scope.s);
   let Ann = TRANSannotationBlock(BlockData.scope.s);
   if (Response.ErrorMessage) {
      Ann.find('span.content').html(BlockData.ErrorMessage);
   } else {
      TRANSpopulateBlock(BlockData, Ann);
      // Replace block in AnnotationBlockDataSet
      const Blocks = TRANSifware.AnnotationBlockDataSet;
      for (let i = 0; i < Blocks.length; i++) {
         if (Blocks[i].scope.s == BlockData.scope.s) {
            Blocks[i] = BlockData;
            break;
         }
      }
   }
}

function TRANScreateBlock(CharWidth, LineLeft, BlockData) {
   const Ann = $('<div></div>');
   Ann.attr('id', 'n' + BlockData.scope.s);
   Ann.addClass('ANNblock');
   Ann.css('position', 'absolute');
   let LineTop = TRANSgetLineTop(BlockData.scope.s - 1);
   Ann.css('top', LineTop + 'px');
   const Left = (CharWidth * (BlockData.c + 2) + LineLeft);
   Ann.css('left', Left + "px");
   Ann.find('a').click(function (Event) {
      Event.preventDefault();
      TRANSclearAnnotations();
   });
   $('#TRANSeditorDiv .CodeMirror-sizer').append(Ann);
   TRANSpopulateBlock(BlockData, Ann);
   if (Ann.innerWidth() < 102) Ann.css('width', '102px');
   // Prevent #19083 - Don't let mousedown events outside of the annotation box.
   Ann.mousedown(function (Event) { Event.stopPropagation(); });
}

function TRANSpopulateBlock(BlockData, Ann) {
   if (BlockData.scope.l > 1) {
      Ann.css('width', '');
      const Html = '<span class="content">' + BlockData.scope.d + '</span>' + TRANScontrolBlock2(BlockData.scope);
      Ann.html(Html);
      TRANSsetAllButtonVisibility(Ann.find('div.annav'), BlockData.scope);
      TRANSattachNavClick(Ann.find('div.annav'), BlockData.scope, BlockData.scope);
   } else {
      Ann.children().remove();
      Ann.append(BlockData.scope.d);
   }
   // Initialize zIndex for the top level
   const InitialDepth = 2;
   const MaxLoopBlockWidth = TRANScreateLoopBlocks(Ann, BlockData.scope, TRANScharHeight(), BlockData.scope, InitialDepth);
   const MinWidth = MaxLoopBlockWidth + 30; // 30 is for some padding
   if (Ann.innerWidth() < MinWidth) { Ann.width(MinWidth); }
   Ann.find("span.pv").click(function ()                 { TRANSshowPopupOrDockTab(this.id);      });
   Ann.find("a.ANNnodeTreeValue").click(function (Event) { TRANStreeviewOnClick(Event, this); });
   Ann.find("a.fc, a.fr").mouseenter(function ()         { TRANSonFunctionCallEnter(this);    })
                         .mouseleave(function ()         { TRANSonFunctionCallLeave(this); })
                         .click(function (Event)         { TRANSonFunctionCallClick(Event, this); });
   BlockData.w = Ann.outerWidth();
}

function TRANSbuildIterationTree(Scope) {
   COL_TRC("Scope =",Scope);
   const iteration_tree = {'i': Scope.p, 'c': []};
   const CountOfChild = Scope.c.length;
   for (let i = 0; i < CountOfChild; i++) {
      COL_TRC("i =", i);
      iteration_tree.c[i] = TRANSbuildIterationTree(Scope.c[i]);
   }
   return iteration_tree;
}

function TRANSupdateResult(R){
   COL_TRC(R);
   if (!R.success) { 
      return TRANSannotationStatus(R.error);
   }
   TRANSannotationStatusOk();
   TRANSabortRequestDone('TRANSannotationUpdateRequest'); 
   TRANSupdateAnnBlock(R.data.BlockData);
   TRANSdisplayScriptError(R.data.ScriptError);
}

// Eliot - following a convention of naming all the annotation calls TRANSide to make them easy to trace with trace=TRANSide
function TRANSideOnIterationChange(Vars){
   COL_TRC("/ide/annotation/update call", Vars);
   TRANSabort('TRANSannotationUpdateRequest');
   TRANSannotationStatusBusy();
   TRANSannotationRequestButtons(true);
   let Request = APIcall('ide/annotation/update', Vars, function (R){ 
      TRANSannotationRequestButtons(false);
      TRANSupdateResult(R);
   });
   TRANSabortRegister('TRANSannotationUpdateRequest', Request);
}

function TRANSonScopeIterationChange(TargetCallee, Scope, FunctionScope) {
   let Vars = TRANSprepareLuaPostInfo();  // Use common helper function to pack in Lua information
   COL_TRC("We have a scope iteration change");
   COL_TRC("Scope =", Scope);
   COL_TRC("TargetCallee =", TargetCallee, "FunctionScope =", FunctionScope);
   Vars.filename = TRANSrenderedFileName;
   Vars.start_line = parseInt(FunctionScope.s); // TODO why is this not a number already.
   if (TargetCallee) {
      COL_TRC("We have a target callee.");
     //Vars.target_call_and_callee = {"p" : FunctionScope.p, "target_callee" : TargetCallee};
     Vars.target_callee = TargetCallee;
   } else {
      COL_TRC("We have an iteration tree");
      Vars.iteration_tree = TRANSbuildIterationTree(FunctionScope);
      COL_TRC("iteration tree=", Vars.iteration_tree);
   }
   TRANSideOnIterationChange(Vars);
}

// Mark the "current" annotation as stale (making it transparent). Also moves the block if the current line is longer than the longest known line.
function TRANSmarkAnnotationAsStale(LineNumber) {
   const CharWidth = TRANSifware.CharWidth = TRANScharWidth();
   const LuaLineNumber = LineNumber + 1; // 1-based, rather than CodeMirror's 0-based line numbers.
   if(TRANSifware.AnnotationBlockDataSet) {
      for (let i = 0; i < TRANSifware.AnnotationBlockDataSet.length; i++) {
         const CurrentBlock = TRANSifware.AnnotationBlockDataSet[i];
         if (LuaLineNumber >= CurrentBlock.scope.s && LuaLineNumber <= CurrentBlock.scope.e) {
            const LineContent = TRANSeditor.getLine(LineNumber);
            const LineLength = Math.max(CurrentBlock.c, LineContent.length);
            const BlockLeft = (CharWidth * (LineLength + 2) + TRANSgetLineLeft());
            $('#n' + CurrentBlock.scope.s).css('left', BlockLeft + 'px').fadeTo(125, 0.6);
         } else if (CurrentBlock.scope.s > LuaLineNumber) {
            return; // Annotation blocks are sorted by starting line, so no more blocks will be interfering.
         }
      }
   }
}
