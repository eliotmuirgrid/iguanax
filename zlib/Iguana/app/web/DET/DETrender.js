function DETdetailScreen() {
   PAGEresetPageSpecial("DETwindow");
   LOGscreenInit();
   if(!document.querySelector(".DETwindow")){
      POPpopoutOpen({
         title:'Log details', 
         content: DETdetailHtml(),
         callback : DETclose,
         className: 'DETwindow DETdetailCard',
         width: '85%', 
         height: '90%'
      });
   }
   DETsetListeners();
   DETresetScroll();
   DETmessageModelInit();
   DETmodelInit();
   DETfindExecute();
   FORMfocusDelay(document.querySelector(".DETsearchInput"));
   LOGparamsPast = PAGEhashParameters();
}

function DETclose(){
   let Args = {};
   Args.component = PAGEhashParameter("component");
   Args.from      = PAGEhashParameter("from");
   Args.to        = PAGEhashParameter("to");
   Args.time      = PAGEhashParameter("time");
   Args.id        = PAGEhashParameter("id");
   Args.exact     = PAGEhashParameter("exact");
   Args.search    = PAGEhashParameter("search");
   Args.tags      = PAGEhashParameter("tags");
   Args.filter    = PAGEhashParameter("filter");
   window.location.hash = "#logs" + PAGEformatParams(Args);
}

function DETdetailArgs(){
   let Args = {};
   Args.component = PAGEhashParameter("component");
   Args.from      = PAGEhashParameter("from");
   Args.time      = PAGEhashParameter("time");
   Args.id        = PAGEhashParameter("id");
   Args.search    = PAGEhashParameter("search");
   Args.exact     = PAGEhashParameter("exact");
   Args.tags      = PAGEhashParameter("tags");
   let Filter     = PAGEhashParameter("filter");
   Args.filter    = Filter;
   if(Filter) Args.to = PAGEhashParameter("time");
   else {
      Args.to    = PAGEhashParameter("to");
      Args.time  = PAGEhashParameter("time");
   }
   return Args;
}

function DETcomponentHtml(){
   return /*html*/`         
   <div class="DETcomponentRowFlex">
      <div class="DETcomponentRow DETcomponentContainer">
         <a class="DETcomponentName" href="${DASHformatUrl(DETcomponentId())}"></a>
      </div>
   </div>`;
}

function DETcontextViewHtml(){
   return /*html*/`<div class="DETgrey">[<a class="RELlink DETrelated"> Go to context view </a>]</div>`;
}

function DETchooseViewHtml(){
   return /*html*/`
      <select class="DETstringView FORMselectBox">
         <option value="string_pt">View as Plain Text</option>
         <option value="string_et">View as Escaped Text</option>
         <option value="string_hx">View as Hexadecimal</option>
         <option value="string_seg">View as Segment</option>
      </select>`;
}

function DETactionsHtml(){
   return /*html*/`    
   <div class="DETactionsGrid">
      ${DETcontextViewHtml()}
      ${DETchooseViewHtml()}
      <div class="DETexport DETrowBox DETdisabled"><svg class="DETtoolbarIcon" width="16" height="16"><use href="DET/DETexportIcon.svg#DETexportIcon"></use></svg>Export as Sample Data</div>
      <div class="DETreposition DETrowBox"><svg class="DETtoolbarIcon" width="16" height="16"><use href="DET/DETrepositionIcon.svg#DETrepositionIcon"></use></svg>Reposition Queue</div>
      <div class="DETresubmit   DETrowBox"><svg class="DETtoolbarIcon" width="16" height="16"><use href="DET/DETresubmitIcon.svg#DETresubmitIcon"></use></svg>Edit and Resubmit Message</div>
      <div class="DETresubmitOrig DETrowBox"><svg class="DETtoolbarIcon" width="16" height="16"><use href="DET/DETresubmitIcon.svg#DETresubmitIcon"></use></svg>Resubmit Message</div>
      <a class="DETdownloadLink DETrowBox" title="Download Log"><svg class="DETtoolbarIcon" width="16" height="16"><use href="LOG/icons/LOGiconDownload.svg#LOGiconDownload"></use></svg>Download Log</a>
      <div class="DETtags"></div>
      <div></div>
   </div>`;
}



function DETlogSizeHtml(){
   return /*html*/`  
   <br>
   <br>
   <div class="DETlogSizeGrid">
      <div class="DETtitle">SIZE OF THIS LOG:</div>
      <div class="DETtitle DETmessageSize"></div>
   </div>`;
}

function DETswitchHtml(){
   return /*html*/`
   <div class="DETswitchTarget">
      ${TOGswitchHtml("DETlight", "", false, false, false)}
      <div></div>
   </div>`;
}

function DETviewModeHtml(){
   return /*html*/`      
   <div class="DETviewRow">
      <div class="DETcomponentHeader">
         ${DETswitchHtml()}
         ${DETcomponentHtml()}
      </div>
      <a class="DETdetailNext DETarrow BUTTONstandard BUTTONdefault"><div class="DETarrowFont">▲</div><div>NEWER LOG</div></a>
      <a class="DETdetailPrev DETarrow BUTTONstandard BUTTONdefault"><div class="DETarrowFont">▼</div><div>OLDER LOG</div></a>
   </div>`;
}
function DETlocationSizeFooterHtml(){
   return /*html*/`      
   <div class="DETfooter">
      <div class="DETfooterContents">
         <div class="DETtitle">FILE:</div>
         <div class="DETlocation">...</div>
      </div>
   </div>`;
}

function DETsearchBoxHtml(){
   let startingVal = PAGEhashParameter("search") || "";
   return /*html*/`
   <div class="DETdateTimeSearchBox">
      <input type="text" class="DETsearchInput" placeholder="Search" value="${startingVal}">
      <input class="DETsearchNumber">
      <div class="DETsearchSlash">/</div> 
      <div class="DETsearchResultsCount"></div> 
      <div class="DETsearchIndexUp"><img src="/LOG/LOGiconArrowUp.svg"></div>
      <div class="DETsearchIndexDown"><img src="/LOG/LOGiconArrowDown.svg"></div>
   </div>`;
}

function DETdetailHtml() {
   return /*html*/`
   <div class="DETlayout">
      <div class="DETmodal">
         <div class="DETdetailedDiv">
            <div class="DETheaderGrid">
               ${DETactionsHtml()}
               ${DETlogSizeHtml()}
            </div>
            <div class="DETrightSide">
               ${DETviewModeHtml()}
               <div class="DETdetailContentGrid">
                  <div class="DETdateTimeSearchRow">
                     <div class="DETdateTime LOGdateRow"></div>
                     ${DETsearchBoxHtml()}
                  </div>
                  <div class="DETdetailContent">
                     <div class="LOGdetailTypeIcon"></div>
                     <div class="DETdetailContentDiv"></div>
                  </div>
                  ${DETlocationSizeFooterHtml()}
               </div>
            </div>

         </div>
      </div>
   </div>`;
}
