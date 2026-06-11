// An "Item" represents an object related to annotations which can be
// viewed in a dialog or dock tab.  An 'Item', as passed through functions
// in this file, is represented as a hidden HTML <input> element.  Place
// this element wherever the display for the item should be rendered.
//
// The 'Item' has various members, accessible through jQuery's data()
// function:
//  * Id: the id of the item.  This is usually in the format:
//    (function address)-(index)-[r|a], but can also be a string
//    to uniquely identify some other type of content (for example,
//    ITM_ERROR_STACK_ID).  This will also be set as the 'value'
//    attribute of the <input> element, for easy jQuery selectors.
//  * ViewMode: [optional] view mode of the item.  Can be empty,
//    'string_pt', 'string_et', or 'string_hx'.
//  * Title: for display purposes.
//  * LabelClass: the class which indicates which icon should be used
//    in this item's label.
//  * Tooltip: the tooltip for this item.
//

const ITM_CHUNK_SIZE = 200; // this MUST match the size the server uses
const ITM_ERROR_STACK_ID = 'ERROR_STACK';
const ITM_ERROR_STACK_TITLE = 'Error Stack';
const ITM_HELP_TITLE = 'Help';
const ITM_VIEW_MODE_CHANGE_EVENT = 'itemviewmodechange'; // event that tells the popup or dock controller to update its cookie of opened items
const ITM_COOKIE_EXPIRY = 60; // ~2 months

function ITMgetContainerLineHeight(Container) {
   // Create and remove a tester div, so we can figure out the line height.
   const TesterDiv = document.createElement('div');
   Container.appendChild(TesterDiv);
   const LineHeight = TesterDiv.offsetHeight;
   Container.removeChild(TesterDiv);
   return LineHeight;
}

function ITMonScrollNearEmptyChunk(EmptyChunk, Id, ViewMode) {
   const ChunkNumber = EmptyChunk.id.split('_')[1];
   const Vars = {
      'id'               : Id,
      'view_mode'        : ViewMode,
      'chunk'            : ChunkNumber,
      'component'        : TRANScomponentGuid(),
      'development'      : TRANSdevelopment(),
      'filename'         : TRANSfile(),
      'sample_data_index': SMPLindex(),
   };
   COL_TRC("/ide/annotation/window call", Vars);
   APIcall('ide/annotation/window', Vars, function (Response) {
      if (!Response.success) { 
         EmptyChunk.classList.remove('empty');
         EmptyChunk.classList.add('error');
         EmptyChunk.textContent = Response.error;
      }
      Response = Response.data;
      if (Response.ErrorMessage) {
         EmptyChunk.classList.remove('empty');
         EmptyChunk.classList.add('error');
         EmptyChunk.textContent = Response.ErrorMessage;
      } else {
         EmptyChunk.classList.remove('empty');
         EmptyChunk.style.height = '';
         EmptyChunk.innerHTML = Response.ChunkInfo.Chunks[0].Value;
      }
   });
}

function ITMsetChunkScrollHandler(ContentDiv, Id, ViewMode, ChunkInfo) {
   ContentDiv.removeEventListener('scroll', handleScroll);
   if (ChunkInfo.Chunks.length == ChunkInfo.TotalChunks) return;
   function handleScroll() {
      const RenderRangeTop = this.scrollTop - 200;
      const RenderRangeBottom = this.scrollTop + ContentDiv.offsetHeight + 200;
   
      const EmptyChunks = ContentDiv.querySelectorAll('div.chunk.empty');
      let i = 0;
      const n = EmptyChunks.length;
      for (; i < n; i++) {
         const EmptyChunk = EmptyChunks[i];
   
         const EmptyChunkTop = EmptyChunk.getBoundingClientRect().top + window.scrollY;
         const EmptyChunkBottom = EmptyChunkTop + EmptyChunk.offsetHeight;
   
         // If chunk bottom is in range, or chunk top is in range, or range is inside chunk.
         if ((RenderRangeTop <= EmptyChunkBottom && EmptyChunkBottom <= RenderRangeBottom) ||
            (RenderRangeTop <= EmptyChunkTop && EmptyChunkTop <= RenderRangeBottom) ||
            (EmptyChunkTop <= RenderRangeTop && RenderRangeBottom <= EmptyChunkBottom)) {
            ITMonScrollNearEmptyChunk(EmptyChunk, Id, ViewMode);
         }
      }
   }
   ContentDiv.addEventListener('scroll', handleScroll);
}

function ITMrenderChunks(ContentDiv, Id, ViewMode, ChunkInfo, ScrollTop) {
   let LineHeight = ITMgetContainerLineHeight(ContentDiv);
   if (ScrollTop) ContentDiv.scrollTop = 0;
   // Create all the empty chunk divs.
   let ChunksH = '';
   const ChunkHeight = LineHeight * ITM_CHUNK_SIZE;
   for (let i = 0, n = ChunkInfo.TotalChunks; i < n; i++) {
      ChunksH += '<div class="chunk empty" id="chk_' + i + '" style="height:' + ChunkHeight + 'px;"></div>';
   }
   ContentDiv.innerHTML = ChunksH;

   // Next populate the chunks with actual content.
   for (let i = 0, n = ChunkInfo.Chunks.length; i < n; i++) {
      const Chunk = ChunkInfo.Chunks[i];
      const ChunkDiv = ContentDiv.querySelector('#chk_' + Chunk.ChunkNumber);
      ChunkDiv.classList.remove('empty');
      ChunkDiv.style.height = '';
      ChunkDiv.innerHTML = Chunk.Value;
   }

   // Set the height of the last chunk, which will likely be different from the others.
   const Chunks = ContentDiv.querySelectorAll('div.chunk');
   const LastChunk = Chunks[Chunks.length - 1];
   LastChunk.style.height = LineHeight * ChunkInfo.LinesInLastChunk + 'px';

   ITMsetChunkScrollHandler(ContentDiv, Id, ViewMode, ChunkInfo);
}

function ITMonViewModeChange(ViewModeElement) {
   ViewModeElement.addClass('waiting');
   console.log("About to post");
   let Options = {}
   Options.view_mode_change = true;
   TRANSupdateAnnotations(Options);
   ViewModeElement.removeClass('waiting');
}

function ITMsliderOptionsHtml(){
   return /*html*/`      
   <div class="string_slider">
      <a href="" class="slide_arrow" title="Show/hide view options"></a>
      <div class="content"></div>
   </div>
   <div class="view_options">
      <div class="view_options_title">View Mode:</div>
      <a href="" class="string_pt">Plain Text</a>
      <a href="" class="string_et">Escaped Text</a>
      <a href="" class="string_hx">Hexadecimal</a>
   </div>
   ${ITMrenderBannerContainer("...")}`;
}

function ITMrenderBannerContainer(Message){
   return /*html*/`
   <div class="ITMbannerContainer">
      <div class="ITMcopyFlexDiv">
         <div class="ITMcopyContentIcon"></div>
         <div class="ITMcopyContent">Copy Content</div>
      </div>   
      <div class="ITMbanner"></div>
      <a href="" class="fc fcout ITMbanner2">
         ${Message}
         <div></div>
      </a>
   </div>`
}

function ITMrenderViewOptions(Container, ViewMode, Item, Response) {
   let Div = Container[0];
   if(!Div) return;
   if(Div.querySelector('.string_slider')) return;
   Div.innerHTML = ITMsliderOptionsHtml(Response);
   Div.addEventListener('click', function(Event){
      Event.preventDefault();
      const StringSlider = Event.target.closest('.string_slider');
      const SliderArrow = Event.target.closest('.slide_arrow');
      if(!StringSlider || !SliderArrow) return;
      StringSlider.classList.toggle('expanded');
      const NewLeft = StringSlider.classList.contains('expanded') ? '0px' : '110px';
      StringSlider.style.transition = 'left 0.2s';
      StringSlider.style.left = NewLeft;
      TRANSsetCode();
   });
   const ModeLinks = Container.find('div.view_options a').click(function(Event) {
      Event.preventDefault();
      const jThis = $(this);
      jThis.siblings().andSelf().removeClass('selected').removeClass('waiting');
      const NewViewMode = this.className; // grab this when we know there is no "selected" or "waiting" in the class.
      jThis.addClass('selected');
      ITMhandleCopyHideShow(Event, NewViewMode);
      if(Item.data('ViewMode') != NewViewMode) {
         Item.data('ViewMode', NewViewMode);
         ITMonViewModeChange(jThis);
      }
      TRANSsetCode();
   });
   ModeLinks.filter('.' + ViewMode).addClass('selected');
}

function ITMremoveViewOptions(Container) {
   const slider = Container.querySelector('.string_slider');
   if (slider) {
      const ContentDiv = Container.querySelector('div.content');
      ContentDiv.remove();
      Container.querySelectorAll('div').forEach(div => div.remove());
      Container.appendChild(ContentDiv);
   }
}

// YUCK - we can use closest and querySelector to make this more reliable.
function ITMonShowNoDataError(Item) {
   let content = Item.closest(".content");
   if (!content) return;
   let Message = content.querySelector(".ITMnoDataErrorMessage");
   if (!Message) return;
   if (Message.classList.contains("ITMhide")) {
      Message.classList.remove("ITMhide");
   } else {
      Message.classList.add("ITMhide");
   }
}

function ITMnoDataErrorClicked(Event) {
   Event.preventDefault();
   const Item = Event.target.closest('.ITMnoDataErrorToggle');
   ITMonShowNoDataError(Item);
}

function ITMnoDataErrorToDisplay(ErrorFromServer, Element) {
   let ErrorToShow = '';
   if ("Unknown error." == ErrorFromServer) {
      ErrorToShow = "No data. Tracepoint not reached.";
   }
   else {
      if (!Element.id) {
         Element.id = Element.data('Id');
      }
      ErrorToShow = `No data. The script was stopped or an error occurred. <br><a class='ITMnoDataErrorToggle'>Details</a>`;
      ErrorToShow += `<div class='ITMnoDataErrorMessage ITMhide'>${ErrorFromServer.replace(/^.*@@<><>.+<><>@@/, "")}</div>`;
   }
   return ErrorToShow;
}

function ITMsetContentItem(Container, Item) {
   // Item should already be detached from the Container at this point.
   Container.append(Item);
}

// If a content div does not exist, will append one to Container and
// return that.
function ITMgetContentDiv(Container) {
   let ContentDiv = Container.find('div.content:first');
   if (!ContentDiv.size()) {
      ContentDiv = $('<div class="content"></div>').appendTo(Container);
   }
   return ContentDiv;
}

//////////////////////////////////////////////////////////////////////////
// Begin public API                                                     //
//////////////////////////////////////////////////////////////////////////

// Sets and returns the display class and title for an Item.
// If ProtocolType or NodeType is undefined, Default will be used.
function ITMsetLabelInfo(Item, Title, ProtocolType, NodeType, Default) {
   let LabelClass;
   if (undefined === ProtocolType || undefined === NodeType) {
      LabelClass = Default;
   } else {
      LabelClass = 'nt_' + ProtocolType + '_' + NodeType;
   }
   Item.data('Title', Title);
   Item.data('LabelClass', LabelClass);
   return LabelClass;
}

// Sets the tooltip for the Item, and returns the tooltip as well.
function ITMsetTooltip(Response, Item){
   if (Response.title === '&nbsp;')
      return 'This information cannot be displayed until the script is re-run.';

   const FunctionName = Response.funct_name;
   if (undefined != FunctionName) {
      let Tooltip = '';

      const ContentTypeInfo = (Item.data('Id')).replace(/^(.+_)+/, '');
      const ContentType = ContentTypeInfo.split('-');

      if ("a" == ContentType[2]) {
         Tooltip += "Argument " + parseInt(ContentType[1]);
      } else if ("r" == ContentType[2]) {
         Tooltip += "Return " + parseInt(ContentType[1]);
      }

      if ("main" == FunctionName) {
         Tooltip += " of " + FunctionName;
      } else {
         Tooltip += " of call " + Response.call + " of " + FunctionName;
         Tooltip += " at " + Response.file_name + ":" + Response.line;
      }

      Item.data('Tooltip', Tooltip);
      return Tooltip;
   } else {
      return '';
   }
}

function ITMrenderBanner(Response, Item, Popup) {
   let Content         = '';
   let Banner1         = '.ITMbanner';
   let Banner2         = '.fc div';   
   let FunctionName    = Response.funct_name;
   let Call            = Response.call;
   let File            = Response.file_name;
   let LineNumber      = Response.line;
   let ContentTypeInfo = (Item.data('Id')).replace(/^(.+_)+/, '');
   let Id              = ContentTypeInfo.split('-');
   let WindowDiv       = Popup[0].closest(".ANNpopup");
   let Div             = WindowDiv.querySelector(Banner1);
   let Link            = WindowDiv.querySelector('.ITMbanner2');
   let Div2            = WindowDiv.querySelector(Banner2);
   if (!WindowDiv || !Div || !Div2 || !File) return;
   if ("a" == Id[2])  Content = "Argument " + parseInt(Id[1]);
   else if ("r" == Id[2])  Content = "Return " + Id[1];
   if ("main" == FunctionName) {
      Content += " of " + FunctionName;
   } else {
      Content += " of call " + Call + " of " + FunctionName + " at ";
   }
   Div.innerHTML =  Content;
   Link.childNodes[0].nodeValue = File + ":" + LineNumber;
   Div2.innerHTML = File + ":" + LineNumber + ":" + Call + ":" + LineNumber + ":" + "AKLJDLKSAJDLKJSDKLSD123213";
}

function ITMrenderErrorView(Response, Item) {
   const Container = Item.parent();
   Item.detach();
   ITMgetContentDiv(Container).html(ITMnoDataErrorToDisplay(Response.ErrorMessage, Item));
   ITMsetContentItem(Container, Item);
}

function ITMrenderStaticView(Item, StaticContent) {
   const Container = Item.parent();
   Item.detach();
   const ContentDiv = ITMgetContentDiv(Container);
   ContentDiv.empty().append(StaticContent);
   ITMsetContentItem(Container, Item);
   Item.data('ContentIsStatic', true);
}

function ITMrenderTreeviewView(Response, Item) {
   const Container = Item.parent();
   ITMremoveViewOptions(Container[0]);
   Item.detach();
   const ContentDiv = ITMgetContentDiv(Container);
   ContentDiv.html('<ul class="ifwareTreeview"></ul>');
   NTRinitTreeview(Response.Root, ContentDiv.find('ul.ifwareTreeview').get(0),Item, true);
   ITMsetContentItem(Container, Item);
}

function ITMrenderGenericView(Response, Item, Popup) {
   const Container = Item.parent();
   Item.detach();
   const IsLuaString = (Response.pt === 0 && Response.nt === 4);
   let ContentDiv;
   const ViewMode = Item.data('ViewMode') || 'string_pt';
   if (!IsLuaString) {
      // Displaying anything but a string - no view modes.
      ITMremoveViewOptions(Container[0]);
      ContentDiv = ITMgetContentDiv(Container);
   } else {
      // Displaying a string - need view options.
      ITMrenderViewOptions(Container, ViewMode, Item, Response);      
      ContentDiv = Container.find('div.content');
   }
   ITMrenderChunks(ContentDiv[0], Item.data('Id'), ViewMode, Response.ChunkInfo, Item?.resetScrollPos);
   ITMsetContentItem(Container, Item);
}

// Get an array of chunk numbers, indicating which chunks are visible.
function ITMgetVisibleChunks(Container) {
   const ContentDiv = Container.find('div.content');
   if (ContentDiv.children('div.chunk:first').size() == 0) return []; // no chunks

   const LineHeight = ITMgetContainerLineHeight(ContentDiv[0]);

   // In pixels
   const RenderRangeTop = Math.max(0, ContentDiv.scrollTop() - 200);
   const RenderRangeBottom = ContentDiv.scrollTop() + ContentDiv.outerHeight() + 200;

   // In lines
   const RenderRangeTopLine = Math.floor(RenderRangeTop / LineHeight);
   const RenderRangeBottomLine = Math.floor(RenderRangeBottom / LineHeight);

   // In chunks
   const RenderRangeTopChunk = Math.floor(RenderRangeTopLine / ITM_CHUNK_SIZE);
   const RenderRangeBottomChunk = Math.floor(RenderRangeBottomLine / ITM_CHUNK_SIZE);

   if (RenderRangeTopChunk != RenderRangeBottomChunk) {
      // We assume that the window isn't big enough that the user can view more than 2 chunks simultaneously.
      return [RenderRangeTopChunk, RenderRangeBottomChunk];
   } else {
      return [RenderRangeTopChunk];
   }
}

// Accepts a jQuery set representing one or more container(s).
function ITMgetItems(Container) {
   return Container.find('input[type="hidden"]');
}

function ITMitemToRequest(Item, IsViewModeChange) {
   let ViewModeChange = IsViewModeChange ? IsViewModeChange : false;
   const ItemId = Item.data('Id');
   const ItemRequest = {'id': ItemId};
   const ViewMode = Item.data('ViewMode');
   if (ViewMode) ItemRequest.view_mode = ViewMode;
   const Container = Item.parent();
   if (Container.find('div.chunk:first').size() != 0) {
      const VisibleChunks = ITMgetVisibleChunks(Container);
      if (VisibleChunks) {
         if (ViewModeChange) { // if view mode has changed, we scroll to the top so get the first 2 chunks
            ItemRequest.chunk       = 0; 
            ItemRequest.extra_chunk = 1; 
            return ItemRequest; 
         }
         if (VisibleChunks.length > 0) ItemRequest.chunk = VisibleChunks[0];
         if (VisibleChunks.length > 1) ItemRequest.extra_chunk = VisibleChunks[1];
      }
   } else {
      ItemRequest.expansion_tree = NTRgetExpansionTreeFromLocalStorage(ItemId);
   }
   return ItemRequest;
}

// Creates "full argument requests" for each Item in the set.
function ITMitemsToRequests(ItemSet, IsViewModeChange) {
   COL_TRC(ItemSet);
   let List = [];
   for (let i = 0; i < ItemSet.length; i++) {
      const Item = $(ItemSet[i]);
      if (!Item.data('ContentIsStatic')) {
         List.push(ITMitemToRequest(Item, IsViewModeChange));
      }
   }
   COL_TRC(List);
   return List;
}

function ITMcreateNewItem(ItemId) {
   const Item = $('<input class="link_id" type="hidden" />');
   Item.val(ItemId);
   Item.data('Id', ItemId);
   return Item;
}

function ITMtriggerScrollEvent(Container) {
   ITMgetContentDiv(Container).trigger('scroll');
}

function ITMstoreMostRecentHelp (HelpContent){
   // Fun little trick to get html() to include the HTML of the root object included.
   const AsHtml = HelpContent.clone().wrap('<p>').parent().html();
   STORElocalCreate(TRANSsourceDirectory + '_MostRecentHelpHtml', AsHtml, ITM_COOKIE_EXPIRY);
}
