function LOGscrollbarHtml(){
   return /*html*/`
   <div class="LOGscrollbar">
      <div class="LOGnavButton LOGloadNewest" title="Newest Log">⏏</div>
      <div class="LOGnavButton LOGloadNewer" >▲</div>
      <div class="LOGscrollbarBox">
         <div class="LOGscrollbarHoverLine LOGhidden"></div>
         <div class="LOGscrollbarLine"></div>
         <div class="LOGscrollbarLineSelected LOGhidden"></div>
         <div class="LOGscrollbarLineTop" title="Start Time"></div>
         <div class="LOGscrollbarLineBottom" title="End Time"></div>
         <div class="LOGunknownTop"></div>
         <div class="LOGknownArea LOGscrollbarFilterTop"></div>
         <div class="LOGunknownBottom LOGscrollbarFilterBottom"></div>
      </div>
      <div class="LOGnavButton LOGloadOlder">▼</div>
      <div class="LOGnavButton LOGloadOldest" title="Oldest Log">⏏</div>
   </div>`;
}

function LOGtagAutoComplete(){
   return /*html*/`
   <datalist id="LOGtagsIn">
      <option value="#message">
      <option value="#error">
   </datalist>`;
}

function LOGsearchHtml(className, Tags = false){
   let SearchBox = /*html*/`<input class="LOGsearchInput" spellcheck="false" value="${ PAGEhashParameter("search") || ""}" placeholder="Search log content, e.g. error AND NOT timeout">`;
   let ClearClass = !Tags ? "LOGclearSearch": "LOGclearTags";
   if(Tags) SearchBox = /*html*/`<input class="LOGtagsInput" spellcheck="false" list="LOGtagsIn" value="${ PAGEhashParameter("tags") || ""}" placeholder="Search log tags, e.g. #error OR #git">${LOGtagAutoComplete()}`;
   let Val = Tags ? PAGEhashParameter("tags") || "" : PAGEhashParameter("search") || "";
   let Hid = Val ? "" : " LOGhidden";
   return /*html*/`
   <div class="${className}">
      <div class="LOGsearchInput2">
         <div class="LOGsearchContainer">
            ${SearchBox}
         </div>
      </div>
      <div class="BUTTONcancel ${ClearClass}${Hid}"><img src="/LOG/LOGclearSearch.svg"></div>
   </div>`;
}

function LOGcheckBoxRowHtml(){
   return /*html*/`
   <div class="LOGsearchFooterRowTop">
      <div class="LOGfilterCheckbox"><input type="checkbox" class="LOGfilterAllCheckbox" ${PAGEhashParameter("filter") === "true" ? 'checked' : ''}>Matches Only</div>
      <div class="LOGfilterCheckbox"><input type="checkbox" class="LOGexactCheckbox" ${PAGEhashParameter("exact") === "true" ? 'checked' : ''}>Match Whole Word</div>
   </div>`;
}

function LOGsearchBoxHtml(Time, EndTime){
   return /*html*/`
   <div class="LOGsearchContainerDiv">
      <div class="LOGclearAllRow">
         <div class="LOGtitle">Filters</div>
         <div class="LOGclearButton">[ <div class="LOGclearSearchFilters">Reset All</div> ]</div>
         <div></div>
      </div>   
      ${LOGsearchHtml("LOGsearch")}
      ${LOGsearchHtml("LOGtagsBox", true)}
      ${LOGcheckBoxRowHtml()}
   </div>
   ${LOGdateTimeHtml(Time, EndTime)}
   ${LOGsearchRowFeedbackHtml()}`;
}

function LOGsearchNavFeedbackHtml(){
   return /*html*/`
   <div class="LOGsearchNavRow LOGhidden">
      <div class="LOGsearchCount">
         <input type="number" class="LOGsearchIndexInput" min="1" step="1">
         <span class="LOGsearchTotal"></span>
      </div>
      <div class="LOGsearchIndexUp"><img src="/LOG/LOGiconArrowUp.svg"></div>
      <div class="LOGsearchIndexDown"><img src="/LOG/LOGiconArrowDown.svg"></div>
   </div>`;
}

function LOGtoFromHtml(Time, EndTime){
   return /*html*/`      
   <div class="LOGendTimeTitleDiv">
      <div class="LOGtitle LOGendTimeTitle">Time Filter</div>
   </div>
   ${LOGclearDateRowHtml(Time,    "LOGstartTime", "LOGclearStart", "start")}
   ${LOGclearDateRowHtml(EndTime, "LOGendTime",   "LOGclearEnd",   "end")}
   `;
}

function LOGclearDateRowHtml(Time, ClassOne, ClassTwo, Word){
   let Val = LOGepochToTimeSelect(Time);
   let Hid = Val ? "" : " LOGhidden";
   return /*html*/`
   <div class="LOGdateWrapper">
      <input class="FORMinput ${ClassOne} LOGinputDate" 
            type="datetime-local" 
            title="Select ${Word} time." 
            value="${Val}">
      <div class="LOGclearDate ${ClassTwo} BUTTONcancel BUTTONdefault${Hid}">
         <img src="/LOG/LOGclearSearch.svg" alt="Clear">
      </div>
   </div>`;
}

function LOGdateTimeHtml(Time, EndTime){
   return /*html*/`
   <div class="LOGdateTimeDiv">
   ${LOGtoFromHtml(Time, EndTime)}
   </div>
   `;
}

function LOGsearchRowFeedbackHtml(){
   return /*html*/`
   <div class="LOGsearchSection">
   <div class="LOGsearchFooterRow">
      <div class="LOGsearchStatus"></div>
      ${LOGsearchNavFeedbackHtml()}
   </div>
   <div class="BUTTONstandard BUTTONdefault LOGsearchButton">SEARCH</div>
   </div>`;
}

function LOGtypeHtml(){
   return /*html*/`
   <div class="LOGtypeDiv">
      <div class="LOGtypeRow">
         <div class="LOGfilterCheckbox"><input type="checkbox" class="LOGshowOnlyMessages" ${PAGEhashParameter("type") === "message" ? 'checked' : ''}>Only Messages</div>
         <div class="LOGfilterCheckbox"><input type="checkbox" class="LOGshowOnlyErrors"   ${PAGEhashParameter("type") === "error"   ? 'checked' : ''}>Only Errors</div>
      </div>
   </div>`;
}

function LOGstorageHtml(){
   return /*html*/`
   <div class="LOGstorageDiv">
      <div class="LOGstorageRow">
         <div class="LOGtitle">Component Logs:</div>
         <div class="LOGclearButton"> [ <a class="LOGclearLogs" >Delete</a> ]</div>
         <div class="LOGtitle LOGvalue LOGcomponentSizeValue">...</div>
      </div>
      <br>
      <div class="LOGstorageRow">
         <div class="LOGtitle">Total Logs Size:</div>
         <div class="LOGclearButton LOGclearButtonStats"> [ <a class="LOGmanageStorage" href="#settings/storage">Stats</a> ]</div>
         <div class="LOGtitle LOGvalue LOGtotalSizeValue">...</div>
      </div>
   </div>`;
}

function LOGsideBarHtml(Time, EndTime){
   return /*html*/`         
   <div class="LOGlistView">
      <div class="LOGsideHeader">
         <div class="LOGsideHeaderTitle">Logs</div>
      </div>
      <div class="LOGcomponentDiv">
         <div class="LOGcomponentRowFlex">
            <div class="LOGcomponentRow">
               <select class="LOGcomponentSelect FORMselectBox"></select>
               <a class="LOGcomponentLink BUTTONdefault BUTTONstandard"></a>
               <div class="LOGswitchTarget">
                  ${TOGswitchHtml("LOGlight", "", false, false, false)}
               </div>
            </div>
         </div>
      </div>
      <div class="LOGfilterCriteriaDiv">
         ${LOGsearchBoxHtml(Time, EndTime)}
         ${LOGbulkHtml()}
         <div class="LOGbulkStatus"></div>
      </div>
      ${LOGstorageHtml()}
      ${PRJsourcesReceiversHtml()}
   </div>`;
}

function LOGlogViewHtml(){
   return /*html*/`         
   <div class="LOGlayout">
      ${LOGscrollbarHtml()}
      <div class="LOGwrapper">
         <div class="LOGdateRow LOGcurrentDate"></div>
         <div class="LOGbody">
            <div class="LOGdateRow LOGtopBanner">Newest results</div>
            <div class="LOGlistDiv"></div>
            <div class="LOGdateRow LOGbottomBanner"></div>
         </div>
         <div class="LOGdateRow LOGwarningBanner LOGhidden">...</div>
         <div class="LOGdateRow LOGtoolBarBottom">
            <div class="LOGfooterContents ">
               <div class="LOGtitleFooter">LOCATION:</div>
               <div class="LOGvalue LOGlocation">...</div>
            </div>
         </div>
      </div>
   </div>`;
}

function LOGscreenHtml(){
   let Time = LOGstartTimeArgs();
   if(!Time) Time = "";
   let EndTime = LOGendTimeArgs();
   if(!EndTime) EndTime = "";
   return /*html*/`
   <div class="LOGmodal">
      <div class="LOGcontainer">
         ${LOGsideBarHtml(Time, EndTime)}
         ${LOGlogViewHtml()}
      </div>
   </div>
   `;
}
