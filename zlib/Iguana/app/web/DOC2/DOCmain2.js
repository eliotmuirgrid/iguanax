function DOCmain2ToolBarHtml(){
   return `
   <div class="DOC2toolbar2">
      <div class="BUTTONaction BUTTONstandard DOC2seeTags">SEE TREE</div>
      <div class="BUTTONaction BUTTONstandard DOC2diff">DIFF</div>
      <div class="BUTTONaction BUTTONstandard DOC2reset">RESET</div>
      <div class="BUTTONaction BUTTONstandard DOC2upload">UPLOAD</div>
      <div class="BUTTONaction BUTTONstandard DOC2import">IMPORT</div>
      <input class="DOC2searchInput" type="text" value="${PAGEhashParameter("search")}">
      <div class="BUTTONaction BUTTONstandard DOC2search">SEARCH</div>
   </div>
   `;}

function DOCmain2BulkActionsHtml(){
   return`
   <div class="DOC2bulkActions">
      <div class="BUTTONaction BUTTONstandard DOC2bulkEditTag DOC2bulkButton DOC2buttonDisabled"> EDIT TAGS </div>
      <div class="BUTTONaction BUTTONstandard DOC2bulkDelete DOC2bulkButton DOC2buttonDisabled"> DELETE DOCS </div>
   </div>`;} 

function DOCscrollbarHtml(){
   return `
   <div class="DOC2scrollbar">
      <div class="DOC2navButton DOC2loadNewest" title="Newest DOC2">⏏</div>
      <div class="DOC2navButton DOC2loadNewer" >▲</div>
         <div class="DOC2scrollbarBox">
            <div class="DOC2scrollbarHoverLine DOC2hidden"></div>
            <div class="DOC2scrollbarLine"></div>
            <div class="DOC2scrollbarLineSelected DOC2hidden"></div>
            <div class="DOC2scrollbarLineTop" title="Start Time"></div>
            <div class="DOC2scrollbarLineBottom" title="End Time"></div>
            <div class="DOC2unknownTop"></div>
            <div class="DOC2knownArea DOC2scrollbarFilterTop"></div>
            <div class="DOC2unknownBottom DOC2scrollbarFilterBottom"></div>
         </div>
      <div class="DOC2navButton DOC2loadOlder">▼</div>
      <div class="DOC2navButton DOC2loadOldest" title="Oldest DOC2">⏏</div>
   </div>`;}

function DOC2documentTitleHtml(){
   return `<div class="DOC2title"></div>  <img class="DOC2editTitle" src="DOC/DOCellipsis.svg">`;
}

function DOC2tagBarHtml(){
   return ` <div class="DOC2tagBarTags"></div> <img class="DOC2editTags" src="DOC/DOCellipsis.svg">`;
}

function DOCmain2documentHeaderHtml(){
   return `
   <div class="DOC2documentHeader">
   <div class="DOC2date"></div>
   <div class="DOC2titleSection">${DOC2documentTitleHtml()}</div>
      <div class="DOC2documentHeaderIcons">
         <button class="DOC2button DOC2linkButton">     <img class="DOC2icon" src="DOC/DOCiconLink.svg">     </button>
         <button class="DOC2button DOC2editButton">     <img class="DOC2icon" src="DOC/DOCiconEdit.svg">     </button>
         <button class="DOC2button DOC2reprocessButton"><img class="DOC2icon" src="DOC/DOCiconReprocess.svg"></button>
         <button class="DOC2button DOC2downloadButton"> <img class="DOC2icon" src="DOC/DOCiconDownload.svg"> </button>
         <button class="DOC2button DOC2deleteButton">   <img class="DOC2icon" src="DOC/DOCiconTrash.svg">    </button>
      </div>
   </div>
   <div class="DOC2documentTagBar">${DOC2tagBarHtml()}</div>
   <div class="DOC2relations"></div>`;
}

function DOCmain2ListHeaderHtml(){
   return `
   <div class="DOC2listRowHeader">
      <div class="DOC2listRowHeaderCheckBox DOC2listRowElm">
         <input type="checkbox" class="DOC2selectListRow">
      </div>
      <div class="DOC2listRowTitle DOC2listRowElm">
         Title
      </div>
      <div class="DOC2listRowHeaderDate DOC2listRowElm">
         Date
      </div>
    </div>`;
}

function DOCmain2Html(){
   return `
   <body class="DOC2application">
      ${DOCmain2ToolBarHtml()}
         <div class="DOC2workspace DOCmain">
            <div class="DOC2tree DOC2listTree" style="width: 418px;">
               ${DOCmain2ListHeaderHtml()}
               <div class="DOC2list">
               </div> 
               <div class="DOC2resizeControl">
					   <div class="DASHresizeShadow"></div>
				   </div>
               ${DOCmain2BulkActionsHtml()}
               ${DOCscrollbarHtml()}
            </div>
            <div class="DOC2rightDisplay DOC2listRightDisplay">
               ${DOCmain2documentHeaderHtml()}
               <div class="DOC2description"></div>
               <div class="DOC2content DOC2md"></div>
               <div class="DOC2tagCloud"></div>
            </div>
         </div>
   </body>`;}

function DOCmain2Screen(){
   DOC2resetGlobals();
   document.body.innerHTML = DOCmain2Html();
   document.body.classList.add("DOC2application");
   DOC2listInit();
   DOCscrollListeners();
   DOC2listListners();
   SPLTsetupSplitter('.DOC2workspace', '.DOC2resizeControl', '.DOC2tree', 'DOC2ratio', DOC2minListWidth, undefined, undefined)
   DOCscrollbarModelInit();
   DOC2headerListeners();
   DOC2documentListeners();
}

function DOC2resetGlobals(){
   DOC2FilteredCount = null;
   DOCshowListData   = [];
   DOCcheckboxActive = [];
   DOCscrollbarLineSelectedPosition = null;
   DOCscrollbarTopIndex = null;
   DOCscrollbarBottomIndex = null;
   DOCrenderDown = false;
   DOCrenderUp   = false;
   DOCdownIndex = null;
   DOCupIndex = null;
   DOCscrollTimerDown = null;
   DOCscrollTimerDate = null;
   DOCflagResetTimer = null;
   DOCscrollTimerUp = null;
   DOCstartOfDocs = false;
   DOCendOfDocs = false;
   DOClimitScrollSpeed = true;
   DOCinProgress = false;
}

function DOC2docTable(Action, ActiveCheckbox) {
   let Count = ActiveCheckbox.length;
   let DocTable = DOC2generateDocHtml(ActiveCheckbox);
   return /*html*/`
   <div class="DOC2components">
      <div class="DOC2bulkDialogHeader">CONFIRM ${Action} OF ${Count > 1 ? `DOCUMENTS: (${Count} TOTAL)` : `DOCUMENT:`}</div>
      ${DocTable}
   </div>`;
}

function DOC2generateDocHtml(ActiveCheckbox) {
   let DocNames = [];
   for (let i = 0; i < ActiveCheckbox.length; i++) {
      let Html = `<div class="DOC2bulkListName">${ActiveCheckbox[i].title}</div>`;
      DocNames.push(Html);
   }
   let DocTable = '<div class="DOC2componentList">';
   for (let i = 0; i < DocNames.length; i++) {
      DocTable += '<div>' + DocNames[i] + '</div>';
   }
   DocTable += '</div>';
   return DocTable;
}