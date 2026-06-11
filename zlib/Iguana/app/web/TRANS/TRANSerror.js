// Copyright (c) 2010-2020 iNTERFACEWARE Inc.  All rights reserved.
// This may not actually find the 100% correct stack frame, as there
// could be recursive calls.  In practice, though, it'll be good enough.
let TRANSstackTraceError = null;
let TRANSerrorLine = undefined;

function TRANSfindErrorStackFrame(ScriptError) {
   COL_TRC(ScriptError);
   const CallStack = ScriptError.CallStack;
   if (!CallStack) { 
      COL_TRC("There was no callstack so no error frame.");
      return null 
   };
   for (let i = CallStack.length - 1; i >= 0; i--) {
      const StackFrame = CallStack[i];
      if (StackFrame.f == ScriptError.FileName && StackFrame.l == ScriptError.LineNumber) { 
         COL_TRC("Returning StackFrame = ", StackFrame);
         return StackFrame; 
      }
   }
   COL_TRC("Unable to find stack frame.");
   return null;
}

function TRANSscrollToError(ScriptError) {
   // Precondition: the current file is the one in which the error should be displayed.
   const LineNumber = ScriptError.LineNumber - 1; 
   COL_TRC(ScriptError, LineNumber);
   const StackFrame = TRANSfindErrorStackFrame(ScriptError);
   if (StackFrame) {
      COL_TRC("We have a stack frame so jumping there"); 
      TRANSjumpToStackFrame(StackFrame); 
   } else{ 
      COL_TRC("No stack frame, so we are jumping to the line ", LineNumber);
      TRANSscrollToLine(LineNumber); 
   }
   TRANSshowErrorBox(LineNumber, ScriptError);
}

function TRANSerrorOnNavClick(Event, ErrorInCurrentFile, ScriptError){
   COL_TRC(Event, ErrorInCurrentFile, ScriptError);
   Event.preventDefault();
   if (ErrorInCurrentFile) {
      COL_TRC('Error is in the current file.  Going to scroll there.');
      TRANSscrollToError(ScriptError);
   } else {
      COL_TRC("Error in other file - try switching.");
      TRANSscrollToLineSetCallback(function() { TRANSscrollToError(ScriptError);});
      TRANSfileChange(ScriptError.FileName);
   }
}

function TRANSisErrorInCurrentFile(ScriptError){
   // On windows and posix, ScriptError.FileName is POSIX style, but TRANSfile depends on the platform IguanaX is installed on
   // so normalize to posix form before checking
   return (TRANSgetPosixName(TRANSfile()) === ScriptError.FileName &&
           ScriptError.FileName !== ''); 
}

function TRANSdisplayScriptError(ScriptError) {
   if (!ScriptError){
      return;
   }
   if (!ScriptError.Description.length) {
      COL_TRC("Weird - no script error description");
      ScriptError.Description = ' ';  // TODO weird - seems wrong.
   }

   TRANSstackTraceError = ScriptError;
   const ErrorInCurrentFile = TRANSisErrorInCurrentFile(ScriptError);
   TRANSifware.CharWidth = TRANScharWidth();
   COL_TRC("Script error:", ScriptError);
   if (!ScriptError.LineNumber) { ScriptError.LineNumber = 1; } // TODO - in an ideal world, we will always have a line number.
   // CodeMirror's line numbers are 0-based, while Lua's are 1-based.
   const StatusBar = document.querySelector(".TRANSannotationStatus");
   if(!StatusBar) return;
   let Html = '<a href="" class="el" tabindex="-1">' + TRANSerrorText(ScriptError) + '</a>';
   if (ScriptError.CallStack){ Html += '<a class="TRANSshowStackTrace">[Show Stack Trace]</a>'; }
   StatusBar.innerHTML = Html;
   StatusBar.querySelector('a.el').addEventListener('click', function (Event) {
      TRANSerrorOnNavClick(Event, ErrorInCurrentFile, ScriptError);
   });
}

function TRANSnavigateToScriptError(data){
   let ScriptError = data.ScriptError;
   if(!ScriptError) { return; }
   const CmLineNumber = ScriptError.LineNumber - 1;
   const ErrorInCurrentFile = TRANSisErrorInCurrentFile(ScriptError);
   if (!ErrorInCurrentFile) {
      COL_TRC("We won't navigate there because we are not in the current file.");
      return; 
   }
   // #20333 - Switch to correct iteration of function.
   // const StackFrame = TRANSfindErrorStackFrame(ScriptError);
   // TODO - disabled this for IX-3565 - we don't actually have the error index as part of StackFrame.
   // if (StackFrame) { TRANSchangeFunctionIteration(StackFrame.s, StackFrame.c, StackFrame.t); }
   TRANSeditor.addLineClass(CmLineNumber, "background", 'error-highlight-row');
   TRANSerrorLine = CmLineNumber;
   TRANSshowErrorBox(CmLineNumber, ScriptError);
   TRANSautocompleteEnd(data);
}

function TRANSerrorText(Err) {
   COL_TRC(Err.LineNumber, Err.FileName);
   if (Err.FileName != '') { return 'Error in ' + Err.FileName + ' on line ' + Err.LineNumber; } 
   else                    { return 'Error ' + ' on line ' + Err.LineNumber; }
}

function TRANStruncateErrorMessage(Original, BoxLeft) {
   const CharWidth = TRANSifware.CharWidth = TRANScharWidth();
   // Yes, magic numbers are bad, but this beats creating a test box, finding the difference between the width of the box and the width of the characters, all to find a number that will remain constant.
   const BoxWidthConstant = 70;
   const AvailableSpace = TRANSeditorInnerWidth() + TRANSgetEditorScrollLeft() - BoxLeft;
   const MaxCharactersToFit = (AvailableSpace - BoxWidthConstant) / CharWidth;
   let Result = '';
   let MoreLines = true;
   for (let i = 0; i < 3 && MoreLines; i++) { // we display the first 3 lines of the error
      const LineEnd = Original.search(/[\r\n]/);
      let LineLength = LineEnd;
      if (LineLength == -1) {
         LineLength = Original.length;
         MoreLines = false;
      }
      const TruncateTo = Math.max(48, MaxCharactersToFit); // we truncate to at least 48 characters (more, if there's space)
      if (LineLength > TruncateTo) { Result += ESChtmlEscape(Original.substring(0, TruncateTo)) + '&hellip;'; } 
      else                         { Result += ESChtmlEscape(Original.substring(0, LineLength)); }
      if (MoreLines) {
         if (LineLength > 0) { Result += '\n'; }
         else                { i--; }
         const NextLineStart = (Original.substring(LineEnd, LineEnd + 2) == '\r\n' ? LineEnd + 2 : LineEnd + 1);
         Original = Original.substring(NextLineStart);
      }
   }
   if (MoreLines) { Result += '&hellip;'; }
   return Result;
}

function TRANSerrorBoxHtml(ScriptError){
   return /*html*/`
   <div class="error-box collapsed">
      <div class="error-toolbar">
         <span>ERROR (<a href="" title="Dismiss this error. Note that you need not explicitly dismiss each error; you may simply continue typing.">Dismiss</a>)</span>
         <div class="error-buttons">
            ${ScriptError.CallStack ? /*html*/`<img class="error-stack" src="/TRANS/images/call_stack_button.gif" title="Show Stack Trace" />` : ''}
            <img class="error-expand" src="/TRANS/images/maximize_button_red.gif" title="Expand" />
            <img class="error-collapse" src="/TRANS/images/minimize_button_red.gif" title="Collapse" />
            <img class="error-close" src="/TRANS/images/close_button_red.gif" title="Close" />
         </div>
      </div>
      <div class="error-message"></div>
   </div>`;
}
function TRANSshowErrorBox(LineNumber, ScriptError) {
   COL_TRC("Show error box at ", LineNumber, ScriptError);
   const CharWidth = TRANSifware.CharWidth = TRANScharWidth();
   if (document.querySelector('div.error-box')) return; // there's already an error box
   const LineTop    = TRANSgetLineTop(LineNumber);
   const BoxTop     = LineTop - 4;
   const LineLeft   = TRANSgetLineLeft();
   const LineLength = TRANSeditor.getLine(LineNumber)?.length ? TRANSeditor.getLine(LineNumber).length : 0;
   const BoxLeft    = (CharWidth * (LineLength + 2) + LineLeft);
   const ErrorBox   = document.createElement('div');
   ErrorBox.innerHTML = TRANSerrorBoxHtml(ScriptError);
   ErrorBox.style.top = BoxTop + 'px';
   ErrorBox.style.left = BoxLeft + 'px';
   ErrorBox.style.position = 'absolute';
   ErrorBox.style.width = '-webkit-fill-available';
   TRANSsetTruncatedMessageAndTooltip(ErrorBox, ScriptError, BoxLeft);
   TRANSsetEventHandlers(ErrorBox, ScriptError);
   document.querySelector('#TRANSeditorDiv .CodeMirror-sizer').appendChild(ErrorBox);
   TRANSmakeErrorBoxDraggable(ErrorBox, ScriptError);
}

function TRANSsetTruncatedMessageAndTooltip(Box, ScriptError, BoxLeft) {
   const TruncatedMessage = TRANStruncateErrorMessage(ScriptError.Description, BoxLeft);
   const CollapsedTooltip = 'Expand/move this Message';
   Box.title = CollapsedTooltip;
   Box.querySelector('div.error-message').innerHTML = TruncatedMessage;
}

function TRANSsetEventHandlers(Box, ScriptError) {
   const ErrorStackButton = Box.querySelector('img.error-stack');
   if (ErrorStackButton) {
      ErrorStackButton.addEventListener('click',     function()      { TRANSerrorStackDialog(ScriptError); })
      ErrorStackButton.addEventListener('mousedown', function(Event) { Event.stopPropagation(); });
   }

   const ErrorCollapseButton = Box.querySelector('img.error-collapse');
   if (ErrorCollapseButton) {
      ErrorCollapseButton.addEventListener('click', function() { TRANScollapseErrorBox(Box, ScriptError); });
   }

   const ErrorCloseButton = Box.querySelector('img.error-close, a');
   if (ErrorCloseButton) {
      ErrorCloseButton.addEventListener('click', function(Event) {
         Event.preventDefault();
         TRANSsetCode();
         Box.style.opacity = '1';
         Box.style.transition = 'opacity 200ms';
         Box.style.opacity = '0';
         setTimeout(() => { TRANSclearErrorBox(); }, 200);
      });
      ErrorCloseButton.addEventListener('mousedown', function(Event) { Event.stopPropagation(); });
   }

   const ErrorMessage = Box.querySelector('div.error-message');
   if (ErrorMessage) {
         ErrorMessage.addEventListener('mousedown', function(Event) {
         if(Box.classList.contains('expanded')) { Event.stopPropagation(); }
      });
   }
}

function TRANScollapseErrorBox(Box, ScriptError) {
   let div = Box.querySelector('.error-box');
   const TruncatedMessage = TRANStruncateErrorMessage(ScriptError.Description, Box.getBoundingClientRect().left);
   const CollapsedTooltip = 'Expand/move this Message';
   div.classList.remove('expanded');
   div.classList.add('collapsed');
   Box.title = CollapsedTooltip;
   Box.querySelector('.error-message').innerHTML = TruncatedMessage;
}

function TRANSmakeErrorBoxDraggable(Box, ScriptError) {
   let isDragging = false;
   let startX, startY;
   let boxX, boxY;
   Box.addEventListener('mousedown', function(e) {
      if(Box.querySelector(".expanded") && !e.target.closest(".error-toolbar")) return;
      isDragging = true;
      startX = e.clientX;
      startY = e.clientY;
      boxX = Box.offsetLeft;
      boxY = Box.offsetTop;
      if(e.target.closest(".error-close")) return TRANSclearErrorBox();
      if(Box.querySelector('.collapsed')) TRANSexpandErrorBox(Box, ScriptError);
   });
   document.addEventListener('mousemove', function(e) {
      if (!isDragging) return;
      const deltaX = e.clientX - startX;
      const deltaY = e.clientY - startY;
      Box.style.left = boxX + deltaX + 'px';
      Box.style.top = boxY + deltaY + 'px';
   });
   document.addEventListener('mouseup', function() {
      if (!isDragging) return;
      isDragging = false;
      Box.classList.add('moved');
      const DraggedError = {};
      DraggedError.Description = ScriptError.Description;
      const LinesContainer = document.querySelector('#TRANSeditorDiv .CodeMirror-lines');
      const LinesPadding = LinesContainer.querySelector('div').getBoundingClientRect().top - LinesContainer.getBoundingClientRect().top;
      DraggedError.LineNumber = Math.ceil((Box.getBoundingClientRect().top - LinesPadding) / TRANScharHeight() + 1);
   });
}

function TRANSexpandErrorBox(Box, ScriptError) {
   let div = Box.querySelector('.collapsed');
   div.classList.remove('collapsed');
   div.classList.add('expanded');
   Box.title = '';
   let message = Box.querySelector('.error-message');
   message.style.display = 'none';
   message.textContent = ScriptError.Description;
   message.style.display = 'block';
}

// TODO - perhaps by using an orgthonal class for these elements this code could be simplified
function TRANSclearErrors() {
   if (!TRANSeditor){
      COL_ERR("TRANSclearErrors called before TRANSeditor is created");
      return;
   }
   COL_TRC(TRANSeditor, TRANSifware);
   // in some rare cases an error line is left behind (#18541).
   if (TRANSerrorLine !== undefined) {
      const LineCount = TRANSeditor.lineCount();
      for (let i = 0; i < LineCount; i++) {
         TRANSeditor.removeLineClass(i, "background",  null);
      }
   }
   TRANSerrorLine = undefined;
   document.querySelectorAll(".error-close,.error-box").forEach(element => {
      element.remove();
   });
}

function TRANSclearErrorBox() {
   document.querySelectorAll('.error-box').forEach(element => {
      element.remove();
   });
}

