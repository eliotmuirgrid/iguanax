/**
 * Copyright (c) 2010-2011 iNTERFACEWARE Inc.  All rights reserved.
 */
 const ITScoreData = {
   ITSpopoutHelp      : undefined,
   ITSisMouseMoving   : false,
   ITSwaitingForChange: false,
   SelectionKeyMap    : {40: '(', 41: ')', 44: ',', 46: '.', 58: ':', 61: '=', 91: '[', 93: ']', 123: '{', 125: '}'},
   ITSnoNeedRegex     : /^(\s*--.*|\W*)$/,
   ITSmaxHelpHeight   : 400,
   ITSmaxOptionsHeight: 175
};

function ITSgetPopup(){ 
   let Popup = document.querySelector(".ITSintellisensePopup");
   // COL_TRC("ITSgetPopup", Popup, Popup===null);
   if (!Popup){
      COL_TRC("Creating intellisense object.");
      // Needed to make annotations work, as well as keyboard arrow keys and tab while in codemirror.
      $('body:first').prepend('<div class="ITSpopupProtector"><div class="ITSintellisensePopup" style="display:none;"></div></div>');
      Popup = document.querySelector(".ITSintellisensePopup");

   }
   if (Popup.style == null) { Popup.style.display = "none"; }
   return Popup;
}

function ITSdetectMouseMove(){
   $(document).on('mousemove', '.ITSintellisensePopup', function(){
      ITScoreData.ITSisMouseMoving = true;
   });
}
ITSdetectMouseMove();

function ITSclosePopup() {
   $('.ITSpopupProtector').remove();
   //TRANSeditor.focus();//refocus
}

function ITSgetHighlightedOption() {
   let Popup = ITSgetPopup();
   return $(Popup).find('.ITSintellisenseOptionsList .ITSoptionRow.hover').get(0);
}

function ITSscrollToSelectedOption() {
   let Popup = ITSgetPopup();
   const Row = ITSgetHighlightedOption();
   if (Row) {
      const OptionsContainer = $('.ITSintellisenseOptionsContainer', Popup).get(0);
      const RowBeg = Row.offsetTop - 2;
      const RowEnd = Row.offsetTop + Row.offsetHeight + 2 + 20; // 20 accounts for a possible horizontal scrollbar in the popup.
      const ScrBeg = OptionsContainer.scrollTop;
      const ScrEnd = OptionsContainer.scrollTop + OptionsContainer.offsetHeight;
      const LimBeg = 0;
      const LimEnd = OptionsContainer.scrollHeight - OptionsContainer.offsetHeight + 20; // 20 accounts for a possible horizontal scrollbar in the popup.
      if (RowEnd > ScrEnd) OptionsContainer.scrollTop = Math.max(LimBeg, Math.min(LimEnd, RowEnd - OptionsContainer.offsetHeight)) + 4;
      if (RowBeg < ScrBeg) OptionsContainer.scrollTop = Math.max(LimBeg, Math.min(LimEnd, RowBeg));
   }
}

function ITSselectOption(Row)   { $(Row).addClass('hover'); }
function ITSdeselectOption(Row) { $(Row).removeClass('hover'); }

function ITSselectFirstOption() {
   let Popup = ITSgetPopup();
   const CurrentOption = ITSgetHighlightedOption();
   if (CurrentOption) { ITSdeselectOption(CurrentOption); }
   ITSselectOption($(Popup).find('.ITSintellisenseOptionsList .ITSoptionRow').get(0));
}

function ITSselectLastOption() {
   let Popup = ITSgetPopup();
   const CurrentOption = ITSgetHighlightedOption();
   if (CurrentOption) { ITSdeselectOption(CurrentOption); }
   const Options = $(Popup).find('.ITSintellisenseOptionsList .ITSoptionRow').get();
   ITSselectOption(Options[Options.length - 1]);
}

function ITSoptionRowOnMouseOver() {
   if (ITScoreData.ITSisMouseMoving) {
      ITSdeselectOption(ITSgetHighlightedOption());
      ITSselectOption(this);
   }
}

function ITSoptionRowOnMouseOut() {
   if (ITScoreData.ITSisMouseMoving) { ITSdeselectOption(this); }
}

function ITScalculatePopupPosition(Editor, FullLine, SearchStringLength) {
   const CursorPos = Editor.getCursor();
   FullLine = FullLine.substring(0, CursorPos.ch - SearchStringLength);
   return Editor.charCoords({ line: CursorPos.line, ch: FullLine.length });
}

function ITSpopupResize(Editor, LineContent, SearchString, MaxPopupHeight) {
   const PopupPosition = ITScalculatePopupPosition(Editor, LineContent, SearchString.length);
   let Popup = ITSgetPopup();
   Popup.style.left = PopupPosition.left   + 'px';
   Popup.style.top  = PopupPosition.bottom + 'px';
   Popup.style.height = '';
   Popup.style.width  = '';
   if ($(Popup).outerHeight() > MaxPopupHeight) {
      Popup.style.height = MaxPopupHeight + 'px';
      Popup.style.width = ($(Popup).innerWidth() + 18) + 'px'; // make up space lost from scrollbar
   }
   // Limit the width of the popup so that it doesn't run off the window
   const WindowWidth = WINgetWindowWidth();
   const DesiredMaxWidth = WindowWidth - PopupPosition.left - 50;// the 50 just gives us a bit of padding
   const CurrentWidth = $(Popup).outerWidth();
   const MinWidth = 80;
   if (CurrentWidth > DesiredMaxWidth) { Popup.style.width = DesiredMaxWidth + 'px'; }
   else if (CurrentWidth < MinWidth)   { Popup.style.width = MinWidth        + 'px'; } //min size
   let PopupOuterHeight = $(Popup).outerHeight();
   $('.ITSintellisenseHelpTitle', Popup).each(function () {
      //help mode, set the optioncontainer and helpdata to 100% height, and fetch its new height
      let CalcHeight = $('.ITSintellisenseOptionsContainer:visible,.ITSintellisenseHelpData:visible').height('100%').outerHeight();
      if (CalcHeight > (PopupOuterHeight - $(this).outerHeight())) {
         //now adjust height to take into account the title
         CalcHeight -= $(this).outerHeight();
         $('.ITSintellisenseOptionsContainer:visible,.ITSintellisenseHelpData:visible').height(CalcHeight)
      }
      PopupOuterHeight = $(Popup).outerHeight();
   })
   // If the bottom of the popup goes off the window, put it above the current line instead.
   if (PopupPosition.bottom + PopupOuterHeight > WINgetWindowHeight()) {
      Popup.style.top = (PopupPosition.top - PopupOuterHeight) + 'px';
   }
}

function ITSdoHelpCall(Editor, HelpFile){
   COL_TRC(HelpFile);
   HLPlistAll(HLPnameResolve(HelpFile));
   Editor.focus();
   MODELforcePoll('TRANSmodel');
}

function ITSattachHelpTitle(Editor, BaseElement, HelpFile) {
   COL_TRC(Editor, BaseElement, HelpFile);
   $(BaseElement).prepend(/*html*/`
      <div class="ITSintellisenseHelpTitle">
         HELP
         <div class="ITSintellisenseHelpButtons">
            <img class="ITSintellisenseHelpButtons-expand" title="Click to view help" src="/TRANS/images/maximize_button_yellow.gif">
            <img class="ITSintellisenseHelpButtons-close" title="Close" src="/TRANS/images/close_button_yellow.gif">
         </div>
      </div>`);
   // $('.ITSintellisenseHelpTitle', BaseElement).click(ITScoreData.ITSpopoutHelp);
   $('.ITSintellisenseHelpTitle', BaseElement).click(function() { ITSdoHelpCall(Editor, HelpFile)});
   $('.ITSintellisenseHelpButtons-close', BaseElement).click(ITSclosePopup);
}

// Negative Count means move up, positive Count means move down.
function ITSmoveSelection(Count) {
   const Increment = (Count < 0) ? 1 : -1;
   let CurrentOption = ITSgetHighlightedOption();
   if (!CurrentOption) {
      ITSselectFirstOption();
      CurrentOption = ITSgetHighlightedOption();
      Count += Increment;
   }
   while (Count != 0 && CurrentOption) {
      const NextOption = (Count < 0) ? $(CurrentOption).prev().get(0) : $(CurrentOption).next().get(0);
      if (NextOption) {
         ITSdeselectOption(CurrentOption);
         ITSselectOption(NextOption);
      }
      CurrentOption = NextOption;
      Count += Increment;
   }
   ITSscrollToSelectedOption();
}

// Returns true if a character, or backspace, has been pressed.  False otherwise.
function ITScheckForCharacterTyped(Event, KeyCode) {
   if (Event.type == 'keypress') {
      if ((Event.charCode != 0 && KeyCode != 13 /* Enter */) || KeyCode == 8 /* backspace */) { return true; }
      // The condition below is for Chrome, where the above condition doesn't work for some reason.
   } else if (Event.type == 'keydown' && KeyCode == 8 /* backspace */) { return true; }
   return false;
}

function ITShandleKeyEvent(KeyCode, ShiftKey, Event) {
   let Popup = ITSgetPopup();
   switch (KeyCode) {
      case 32:  // Space
      case 13:  // Enter
         const Option = ITSgetHighlightedOption();
         if(Option) {
            Option.onclick();
            Event.preventDefault();
         }
         return true;
      case 38:  // Up-arrow
         ITSmoveSelection(-1);
         return true;
      case 9:   // Tab
         Event.preventDefault();
         if (ShiftKey) ITSmoveSelection(-1);
         else ITSmoveSelection(1);
         return true;
      case 40:  // Down-arrow
         ITSmoveSelection(1);
         return true;
      case 33:  // Page Up
         ITSmoveSelection(-7);
         return true;
      case 34:  // Page Down
         ITSmoveSelection(7);
         return true;
      case 36:  // Home
         ITSselectFirstOption();
         ITSscrollToSelectedOption();
         return true;
      case 35:  // End
         ITSselectLastOption();
         ITSscrollToSelectedOption();
         return true;
      case 27:  // Esc
         ITSclosePopup();
         TRANSeditor.focus();
         return true;
      case 112: //F1
         if (ITScoreData.ITSpopoutHelp && $('.ITSintellisenseHelpButtons', Popup).is(':visible')) ITScoreData.ITSpopoutHelp();
         return true;
   }
   return false; // event not handled
}

function ITSonEditorKeyDown(Event) {
   let Popup = ITSgetPopup();
   if (!TRANSannotationsEnabled()) { return; } // tells CodeMirror to treat the event normally.
   const KeyCode = (window.event ? event.keyCode : Event.keyCode);
   const CurrentOption = ITSgetHighlightedOption();
   const IsKeyPress = (Event.type == 'keypress');
   // Handle punctuation key events (select the highlighted item and place punctuation character afterwards).
   // In IE we need to use KeyCode, in others we can use Event.charCode.
   let SelectionKey = undefined;
   if (IsKeyPress) SelectionKey = ITScoreData.SelectionKeyMap[Event.charCode === undefined ? KeyCode : Event.charCode];
   if (SelectionKey && CurrentOption) {
      Event.preventDefault();
      CurrentOption.onclick(null, SelectionKey);
      Event.codemirrorIgnore = true; // tells CodeMirror we've handled the event
      return;
   }
   // End handling of punctuation keys.
   if (IsKeyPress) ITScoreData.ITSwaitingForChange = false;
   if (ITScheckForCharacterTyped(Event, KeyCode)) {
      ITScoreData.ITSwaitingForChange = true;
      return; // tells CodeMirror to treat the event normally.
   }
   // If Popup is not showing, or if it is showing but the user hit enter/space with no currently selected item.
   if (Popup.style.display !== 'none') {
      // tells CodeMirror to treat the event normally.
      ITScoreData.ITSisMouseMoving = false;
      // Stop events as necessary for keys used by the popup.
      switch (KeyCode) {
         case 13:  // Enter
         case 9:  // Tab
         case 32:  // Space
         case 38:  // Up-arrow
         case 40:  // Down-arrow
         case 33:  // Page Up
         case 34:  // Page Down
         case 36:  // Home
         case 35:  // End
         case 27:  // Esc
         case 112: // F1
            if (Event.type != 'keydown') {
               // stop, but ignore, keypress and keyup events for these keys.
               //Event.codemirrorIgnore = true;
               return;
            }
      }
      if (ITShandleKeyEvent(KeyCode, Event.shiftKey, Event)) { 
         Event.codemirrorIgnore = true; 
      } // we've handled the event, and CodeMirror should ignore it.
   } else {
      // without the below logic we will not get annotations if we press enter in the middle of a line of code
      // if(KeyCode === 13) { Event.codemirrorIgnore = true; }
   }
}

function ITSonOptionClick(OptionText, OriginalExpression, LineNumber, CursorPos, HasMoreOptions, SelectionKey, TextExistsAlready) {
   ITSclosePopup();
   TRANSeditor.focus();
   const OptionLength = OptionText.length;
   const OptionLastChar = OptionText[OptionLength - 1];
   let LooksLikeFunction = (OptionLength && (OptionLastChar === '(' || OptionLastChar === '{'));
   const StripEqualFromParam = (OptionLength && SelectionKey === '=' && OptionLastChar === '=');

   const CharAtCursor = TRANSeditor.getLine(LineNumber)[CursorPos];
   const HasOpen      = CharAtCursor === '(' || CharAtCursor === '{';
   //Adjust OptionText to not include the bracket (or =) if SelectionKey is defined
   if (HasOpen ||!HasMoreOptions && ((LooksLikeFunction && SelectionKey) || StripEqualFromParam)) {
      OptionText = OptionText.substring(0, OptionLength - 1);
      LooksLikeFunction = false; //set to false, because the action of adding this selection key will trigger an autocomplete anyway
   }
   if (SelectionKey) OptionText += SelectionKey;
   if (!TextExistsAlready) {
      const OriginalLineContent = TRANSeditor.getLine(LineNumber);
      const startCh = Math.max(0, CursorPos - (OriginalExpression?.length || 0));
      let endPos = CursorPos;
      // Find next character, whitespace or comma
      while (endPos < OriginalLineContent.length && !/[A-Za-z0-9_\s,:]/.test(OriginalLineContent[endPos])) { endPos++; }
      // If our completion ends with '=', and the next char in the doc is already '=', consume it to avoid '=='
      if (OptionText.endsWith('=') && OriginalLineContent[endPos] === '=') { endPos++; }
      const startPos  = { line: LineNumber, ch: startCh };
      const endPosObj = { line: LineNumber, ch: endPos };
      TRANSeditor.replaceRange(OptionText, startPos, endPosObj);
      TRANSeditor.setCursor({ line: LineNumber, ch: startCh + OptionText.length }); // Move cursor to just after inserted text
   } else { // If text already exists, just put cursor after expected insertion point
      TRANSeditor.setCursor({ line: LineNumber, ch: CursorPos + (OptionText.length - (OriginalExpression?.length || 0)) });
   }
   TRANSeditor.setCursor({ line: LineNumber, ch: CursorPos + (OptionText.length - OriginalExpression.length) });
   TRANSeditor.focus();
   //trigger further autocomplete if we think we can
   if (HasMoreOptions && !SelectionKey) { 
      ITSrequestIntellisense({ AdditionalRequestExpression: OptionText });
   }
   else if (LooksLikeFunction || SelectionKey) { ITSrequestIntellisense({ TextExistsAlready: TextExistsAlready }); }
   if(OriginalExpression === "\nend") { TRANSautocompleteIndent(LineNumber, CursorPos); }
}

function ITSgenerateOption(Option, OriginalExpression, LineNumber, CursorPos, TextExistsAlready) {
   const OptionRow = document.createElement('div');
   OptionRow.className = 'ITSoptionRow';
   OptionRow.onmouseover = ITSoptionRowOnMouseOver;
   OptionRow.onmouseout = ITSoptionRowOnMouseOut;
   OptionRow.onclick = function (Event, SelectionKey) {
      if (Option.ft.endsWith('(=')) return ITSclosePopup();
      ITSonOptionClick(Option.ft, OriginalExpression, LineNumber, CursorPos, !Option.s, SelectionKey, TextExistsAlready);
   }
   const OptionCol = document.createElement('div');
   OptionCol.className = 'ITSoptionData nt_' + Option.pt + '_' + Option.nt;
   if (Option.r) { OptionCol.className += ' r'; }
   OptionCol.innerHTML = Option.dt;
   OptionRow.appendChild(OptionCol);
   return OptionRow;
}

function ITSonChange() {
   if (!TRANSfile().endsWith(".lua")){
      COL_TRC("Not a lua file - so no autocompletion.");
      return;
   }
   if (ITScoreData.ITSwaitingForChange) {
      ITScoreData.ITSwaitingForChange = false;
      const CursorPos = TRANSeditor.getCursor();
      const LineToCursor = TRANSeditor.getLine(CursorPos.line).substring(0, CursorPos.ch);
      if (!ITScoreData.ITSnoNeedRegex.test(LineToCursor)) { ITSrequestIntellisense(); }
   }
}

function ITSintellisenseOptionsHandler(Vars, Response) {
   let Popup = ITSgetPopup();
   COL_TRC("Response:", Response);
   // CodeMirror's line numbers are 0-based, while Lua's are 1-based.
   Response.line_number--;
   const Options = Response['options'];
   const CursorPos = TRANSeditor.getCursor();
   const LineNumber = CursorPos.line;
   const LineContent = TRANSeditor.getLine(LineNumber);
   if (LineNumber == Response.line_number && (CursorPos == Response.cursor_position || Vars.cursor_position == Response.cursor_position)) {
      const OptionsTable = document.createElement('div');
      $(OptionsTable).addClass('ITSintellisenseOptionsList')
      for (let i = 0; i < Options.length; i++) {
         OptionsTable.appendChild(ITSgenerateOption(Options[i], Response['full_expression'], LineNumber, CursorPos.ch, Vars.TextExistsAlready));
      }
      if(Response.help_file && Options.length === 0){
         const BlankRow = document.createElement('div')
         BlankRow.classList.add('ITSnoParametersOption')
         BlankRow.innerHTML = 'No parameters.'
         OptionsTable.appendChild(BlankRow)
      }
      if (Response['options_truncated']) {
         const WarningRow = document.createElement('div');
         const WarningCol = document.createElement('div');
         WarningCol.className = 'ITSoptionData ITSintellisenseOptionsTruncatedWarning';
         WarningCol.innerHTML = 'Not all results could be displayed.';
         WarningRow.className = 'ITSoptionRow';
         WarningRow.appendChild(WarningCol);
         OptionsTable.appendChild(WarningRow);
      }
      Popup.cursorPos = CursorPos.ch;
      Popup.lineNo = LineNumber;
      $(Popup).empty()
      $(OptionsTable).addClass('ITSintellisenseOptions')
      const OptionsContainer = $('<div class="ITSintellisenseOptionsContainer"></div>');
      OptionsContainer.append(OptionsTable);
      $(Popup).append(OptionsContainer);
      if (Response.help_file) { ITSattachHelpTitle(TRANSeditor, Popup, Response["help_file"]); }
      if (Options.length > 0 || Response.help_file) { Popup.style.display = ''; }
      ITSpopupResize(TRANSeditor, LineContent, Response['search_string'], ITScoreData.ITSmaxOptionsHeight)
   }
}

function ITSideIntellisenseResponse(Response, Options, CursorPos, Vars){
   COL_TRC(Response)
   TRANSabortRequestDone('ITSintellisenseRequest');
   if (!Response.success){
      // we could show this in the GUI?
      COL_ERR(Response.error);
      return;
   }

   Response = Response.data;
   // Intellisense on an already-typed string
   Vars.TextExistsAlready = Options.TextExistsAlready || CursorPos.ch < Options.end
   if (Vars.TextExistsAlready) {
      Response.full_expression = Response.full_expression.substring(0, Response.full_expression.length + CursorPos.ch - Options.end);
   }
   ITSintellisenseOptionsHandler(Vars, Response);
}

function ITSrequestIntellisense(Options) {
   if (!TRANSannotationsEnabled()) { return; }  // early quit in order to disable auto complete functionality
   Options = Options || {};
   const CursorPos = TRANSeditor.getCursor();
   ITSclosePopup();
   TRANSeditor.focus();
   let Vars = TRANSprepareLuaPostInfo();
   Vars.code = TRANSeditor.getValue();  // Important to send code with intellisense requests I think?

   Vars.line_number = CursorPos.line + 1; // CodeMirror's line numbers are 0-based, while Lua's are 1-based.
   Vars.cursor_position = Options.end || CursorPos.ch; // CodeMirror's line numbers are 0-based, while Lua's are 1-based.
   if (Options.AdditionalRequestExpression) Vars.addition_request_expression = Options.AdditionalRequestExpression;
   // using a common function to pack in Lua function parameter information
   Vars.username = 'admin';
   TRANSabort('ITSintellisenseRequest');
   let Request = APIcall('ide/intellisense', Vars, function(Response) { ITSideIntellisenseResponse(Response, Options, CursorPos, Vars); });
   TRANSabortRegister('ITSintellisenseRequest', Request);
}

function ITScursorActivity() {
   let Popup = ITSgetPopup();
   const CursorPos = TRANSeditor.getCursor();
   if (Popup.style.display == '' && (Popup.cursorPos != CursorPos.ch || Popup.lineNo != CursorPos.line)) {
      ITSclosePopup();
      TRANSeditor.focus();
   }
}
