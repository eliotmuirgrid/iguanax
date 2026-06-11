/**
 * Copyright (c) 2010-2011 iNTERFACEWARE Inc.  All rights reserved.
 */

// This stack keeps track of all popups opened through the popup dialogs
// controller, with the frontmost popup being at the top of the stack and the
// backmost at the bottom. Only popup ids are stored in the stack.
// Note that this data structure is not a "true" stack because it supports the
// removal of elements from the middle.
var m_PopupStack = new Array();

let TRANSdialogsController;

var m_EditorHeight = 0;
var m_EditorWidth = 0;
var m_EditorTop = 0;
var m_EditorLeft = 0;

const DEFAULT_TITLE_BAR_HTML = '<div class=\'title ntv waiting\'>Refreshing&hellip;</div>';

function TRANSgetPopupWithId(Id) {
   COL_TRC(Id);
   return $("div.ANNpopup:has(input.link_id[value=" + Id + "])");
}

function TRANSinitDialog(Popup, Item) {
   Popup.append(Item);
   const ItemId = Item.data('Id');
   const PopupHeight = $('div.CodeMirror-scroll').height() / 2;
   const PopupWidth = $('div.CodeMirror-scroll').width() / 2;
   const PopupTop = m_EditorTop + (m_EditorHeight - PopupHeight) / 2;
   const PopupLeft = m_EditorLeft + (m_EditorWidth - PopupWidth) / 2;
   const DialogOptions = {
      focus: function(event, ui) {
         // Opening a jQuery dialog automatically applies focus to it, so we don't
         // need to explicitly call pushToPopupStack after opening a popup.
         TRANSpushToPopupStack(ItemId);
      },
      close: function(event, ui) {
         $(this).dialog('destroy').remove();
         TRANSremoveFromPopupStack(ItemId);
         TRANSifware.focusLastFocussedElement();
      },

      create: function(event, ui) {
         $(this).parent().find('span.ui-dialog-title').empty().append(DEFAULT_TITLE_BAR_HTML);
      },
      autoOpen   : false,
      width      : PopupWidth,
      height     : PopupHeight,
      position   : [PopupLeft, PopupTop],
      dialogClass: 'ANNouterPopup',
      minWidth   : 230,
      minHeight  : 500,
   };
   Popup.dialog(DialogOptions);
   Popup.dialog("open");
}

// Done in this roundabout way to break up the long period
// of browser processing (key/mouse events can be processed
// by the browser in between the timeout events).
function TRANSenqueueFullArgumentRender(Response, Item) {
   setTimeout(function () {
      Item.resetScrollPos = true;
      TRANSsetContent(Response, Item);
   }, 0);
}

function TRANSupdateDialogs(FullArgumentResponses, ScriptError, isRequestCancelled){
   const PopupItems = ITMgetItems($('div.ANNpopup')); 
   COL_TRC("PopupItems=", PopupItems);
   PopupItems.each(function () {
      const Item = $(this);
      const ItemId = Item.data('Id');
      if (Item.data('ContentIsStatic')) {
            // do nothing
      } else if (isRequestCancelled) {
         TRANSsetContent({ErrorMessage:'Script did not finish executing because it was cancelled by the user.'}, Item);
      } else {
         if (FullArgumentResponses && FullArgumentResponses[ItemId]){
            TRANSenqueueFullArgumentRender(FullArgumentResponses[ItemId], Item);
         } else {
            TRANSsetContent({ErrorMessage:'Unknown error.'}, Item);
         }
      }
   });
   // Eliot making this an explicit call make the code simpler than the ID check stuff.
   if (ITMgetErrorStackContainer()) {
      ITMpopulateErrorStackTrace(ScriptError);
   }
   WNDremoveOuterPopouts();
}

function TRANSsetAnnotationToDefaultForAutoScriptExecutionDisabled(Item) {
   const obj = {
      ChunkInfo : {
         Chunks : [
            {
               ChunkNumber: 0,
               Value      : 'To display this object <a href="#" class="execScriptButton executeNowText">run the script now</a>.',
            },
         ],
         TotalChunks     : 1,
         LinesInLastChunk: 7,
      }, title  : '&nbsp;',
      file_name : '',
      funct_name: '',
      call      : 1,
      line      : -1,
      // commenting out pt = 0 and nt = 4 will result in the view modes not being shown
//         pt:0,
//         nt:4
   };
   TRANSsetContent(obj, Item);
   const t = document.querySelector('.execScriptButton');
   t.removeEventListener('click', TRANSsetRunButtonClickHandler);
   t.addEventListener   ('click', TRANSsetRunButtonClickHandler);
}

function TRANSsetRunButtonClickHandler(Event) {
   Event.preventDefault();
   TRANSrunButtonClicked();
}

// It would also be easy to set the tooltip here, if we wanted one.
function TRANSupdateDialogTitle(Popup, Item, NewClass) {
   const TitleElement = Popup.dialog('widget').find('div.title.ntv:first');
   TitleElement[0].className = 'title ntv ' + NewClass;
   Title = $(TitleElement[0]);
   if ("waiting" == NewClass) return Title.html("Refreshing&hellip;");
   let titleString = Item.data('Title') || ' ';
   titleString = titleString.replace(/\s/g, "&nbsp;")
   Title.html(titleString);
   TRANSupdateWindowTitle(Popup, titleString);
   TRANSupdateTabTitle   (Popup, titleString);
}

function TRANSsetContentError(Response, Item, Popup) {
   const Title = 'Error';
   const TitleClass = ITMsetLabelInfo(Item, Title, undefined, undefined, 'error');
   TRANSupdateDialogTitle(Popup, Item, TitleClass);
   ITMrenderErrorView(Response, Item);
}

function TRANSsetContentHelp(Item, Popup, HelpContent) {
   const TitleClass = ITMsetLabelInfo(Item, ITM_HELP_TITLE, undefined, undefined, 'help');
   TRANSupdateDialogTitle(Popup, Item, TitleClass);
   ITMrenderStaticView(Item, HelpContent);
   ITMstoreMostRecentHelp(HelpContent);
}

function TRANSsetContentTreeview(Response, Item, Popup) {
   ITMsetTooltip(Response, Item);
   ITMrenderBanner(Response, Item, Popup);
   const TitleClass = ITMsetLabelInfo(Item, Response.Root.title, Response.Root.pt, Response.Root.nt, 'error');
   TRANSupdateDialogTitle(Popup, Item, TitleClass);
   ITMrenderTreeviewView(Response, Item);
}

function TRANSsetContentGeneric(Response, Item, Popup) {
   ITMsetTooltip(Response, Item);
   const TitleClass = ITMsetLabelInfo(Item, Response.title, Response.pt, Response.nt, 'error');
   TRANSupdateDialogTitle(Popup, Item, TitleClass);
   ITMrenderGenericView(Response, Item, Popup);
   ITMrenderBanner(Response, Item, Popup);
   let Id = TRANSpopupToId(Popup);
   WNDhandleResize(Id);
}

function TRANSsetContent(Response, Item) {
   COL_TRC("Id=",Item.data('Id'));
   COL_TRC("Response=", Response);
   const Popup = TRANSgetPopupWithId(Item.data('Id'));
   if (!Popup[0]) {
      COL_TRC("No popup found");
      return;
   }
   if (Response.ErrorMessage){
      COL_TRC("Setting content to error");
      TRANSsetContentError   (Response, Item, Popup);    
   } else if (Response.Root){
      COL_TRC("Setting content to treeview");
      TRANSsetContentTreeview(Response, Item, Popup);
   } else if (Response.ChunkInfo) {
      COL_TRC("Setting content to generic");
      TRANSsetContentGeneric (Response, Item, Popup);
   }
   WNDremoveOuterPopouts();
}

function TRANSsetDialogsToWaiting() {
   $("div.ANNpopup").each(function () {
      const Item = ITMgetItems($(this));
      if (!Item.data('ContentIsStatic')) { TRANSupdateDialogTitle($(this), Item, 'waiting'); }
   });
}

function TRANSrenderArgumentHtml(PopupNew) {
   if (!PopupNew) return;
   let RowHtml = ITMrenderBannerContainer("");
   PopupNew.insertAdjacentHTML('beforeend', RowHtml);
}

function TRANScreateDialog(Item, DialogTitle) {
   const ItemId = Item.data('Id');
   let Popup = TRANSgetPopupWithId(ItemId);
   let PopupNew = Popup[0];
   if(PopupNew) return Popup;
   Popup = $("<div class='ANNpopup'></div>").appendTo($(".TRANSwindow"));
   PopupNew = Popup[0];
   // TODO - relies on 'widgets' currently so it is hard ...
   // let TransWindow = document.querySelector(".TRANSwindow");
   // let Div = document.createElement('div');
   // Div.classList.add('ANNpopup');
   // TransWindow.appendChild(Div);
   TRANSinitDialog(Popup, Item);
   TRANSrenderArgumentHtml(PopupNew);
   PopupNew.style.height = '100%';
   let OuterPopups = document.querySelectorAll('.ANNouterPopup');
   OuterPopups.forEach(function(OuterPopup) {
      //console.log(OuterPopup);
      OuterPopup.style.display = 'none';
   });
   let AnnotationWindowObject = {
      ID        : WNDannotationToWindowId(ItemId),
      Title     : ESCstripHtml(DialogTitle),
      Content   : "<div style='height: 100%' id='CONT-"+ ItemId +"'></div>",
      PositionX : "",
      PositionY : "",
      Height    : "400px",
      Width     : "500px",
      iconClass : "",
   }
   WNDcreateWindowWithOptions(AnnotationWindowObject);
   let ContDiv = document.querySelector("#WND"+ItemId).querySelector("#CONT-"+ItemId);
   ContDiv.appendChild(PopupNew);
   if(WNDdocksOpen().length){ WNDaddToDock(WNDannotationToWindowId(ItemId), WNDpickDock()); };
   return Popup;
}

// Functions to support the closing of annotation/help popups through a
// keyboard shortcut.

// Adds the given id to the popup stack and ensures that it's at the top.
function TRANSpushToPopupStack(PopupId) {
   // Check to see if the popup is at the top of the stack first, in which
   // case we will do nothing.
   if (m_PopupStack.length > 0 && m_PopupStack[m_PopupStack.length - 1] == PopupId) return;
   // Remove any duplicate entries from the stack before adding the popup.
   TRANSremoveFromPopupStack(PopupId);
   m_PopupStack.push(PopupId);
}

// Removes the first reference to the given popup id from the popup stack.
// It is assumed here that the stack will not contain more than one reference
// to the same popup id. Note that this function only removes the popup's
// id from the popup stack, it does not close it.
function TRANSremoveFromPopupStack(PopupId) {
   const Index = m_PopupStack.indexOf(PopupId);
   if (Index != -1) { m_PopupStack.splice(Index, 1); }
}

function TRANSsetEditorDimensions() {
   const EditorDiv = document.querySelector('.CodeMirror-scroll');
   if (EditorDiv) {
      m_EditorHeight = EditorDiv.clientHeight;
      m_EditorWidth = EditorDiv.clientWidth;
      m_EditorLeft = EditorDiv.offsetLeft;
      m_EditorTop = 83;
   }
}

function TRANSdialog(ItemId) {
   const Item = ITMcreateNewItem(ItemId);
   TRANScreateDialog(Item);
}

function TRANSerrorStackDialog(ScriptError) {
   let Id = WNDannotationToWindowId(ITM_ERROR_STACK_ID);
   if(WNDfocusOnWindow(Id)) return;
   let Item;
   Item = ITMcreateNewItem(ITM_ERROR_STACK_ID);
   TRANScreateDialog(Item, "Stack Trace");
   ITMpopulateErrorStackTrace(ScriptError, Item);
}

function TRANSsetHelpContent(ItemId, HelpContents) {
   const Popup = TRANSgetPopupWithId(ItemId);
   const Item = ITMgetItems(Popup);
   TRANSsetContentHelp(Item, Popup, HelpContents);
}

function TRANSdialogExists(ItemId) {
   return TRANSgetPopupWithId(ItemId).length > 0;
}

function TRANSgetFullArgumentRequestsList(IsViewModeChange) {
   const PopupItems = ITMgetItems($('div.ANNpopup'));
   return ITMitemsToRequests(PopupItems, IsViewModeChange);
}

function TRANSinit() {
   TRANSsetEditorDimensions();
   ANNopenPreviousAnnotations();
   $(window).resize(function () {
      TRANSsetEditorDimensions();
   })
}

function TRANSshowPopupOrDockTab(ItemId, HelpContent) {
   const IsDialog = true;
   let WindowId = WNDannotationToWindowId(ItemId);
   if (TRANSdialogExists(ItemId)) WNDfocusOnWindow(WindowId);
   else {
      TRANSdialog(ItemId);
      if (!HelpContent) {
         if (!TRANSannotationsEnabled()) {
            let $item = null;
            ITMgetItems($('body')).each(function (index, elem) {
                  const $this = $(this);
                  if ($this.data('Id') === ItemId) {
                     $item = $this;
                     return false;
                  }
               });
            if ($item !== null) {
               if (IsDialog) {
                  TRANSsetAnnotationToDefaultForAutoScriptExecutionDisabled($item);
               }
            }
         } else {
            TRANSupdateAnnotations();
         }
      }
   }
   if (HelpContent) {
      if (IsDialog) {
         TRANSsetHelpContent(ItemId, HelpContent);
      }
      TRANSupdateAnnotations();
   }
   // TRANSifware.focusLastFocussedElement(); // refocus
   TRANSsetCode();
}

function TRANSpopupToId(Popup){
   let WindowDiv = Popup[0].closest(".WNDwindow");
   if(!WindowDiv) return;
   return WindowDiv.id;
}

function TRANSupdateWindowTitle(Popup, Title){
   let WindowDiv = Popup[0].closest(".WNDwindow");
   if(!WindowDiv) return;
   let TitleDiv = WindowDiv.querySelector('.WNDtitle');
   if(TitleDiv.innerHTML != Title) TitleDiv.innerHTML = Title;
}

function TRANSupdateTabTitle(Popup, Title){
   let DockContentListItem = Popup[0].closest(".WNDdockContentListItem");
   if(!DockContentListItem) return;
   let DocDockContentListItemId = DockContentListItem.id;
   if(!DocDockContentListItemId) return;
   let TabId = WNDtabId(DocDockContentListItemId);
   let TabDiv = document.querySelector(`#${TabId}`);
   if(!TabDiv) return;
   let TabTitleDiv = TabDiv.querySelector(`.WNDtitle`);
   if(TabTitleDiv.innerHTML != Title) TabTitleDiv.innerHTML = Title;
}
