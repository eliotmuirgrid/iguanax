function HLPjumpToFunction(functionName) {
   if(!HLPisWindowOpen()) { return HLPlistAll(functionName);}
   const left     = document.querySelector('.HLPlistBody');
   const items    = HLPgetAllFunctions();
   const linkItem = items.find(item => item.innerText === functionName);
   if (!left || !linkItem) {
      COL_TRC("No help pane or no target function found. Early return without scrolling.");
      return;
   }
   linkItem.click();
   left.scrollTo({ top: linkItem.offsetTop, behavior: 'auto' });
}

function HLPgetAllFunctions() {
   const functionsList = document.querySelector('.HLPlistBody');
   return [...functionsList.querySelectorAll('.HLPlink')];
}

function HLPgetSelected() {
   const selected = document.querySelector('.HLPlink.HLPstrong');
   const heading  = document.querySelector('.HLPheading');
   if(!heading) { return '';}
   if(selected) { return selected.innerText; }
   return heading ? heading.innerText : '';
}

function HLPhandleClick(targetElement, addToHistory = true) {
   const Filepath = targetElement.getAttribute('data-location');
   HLPloadFunction(Filepath);
   HLPselect(targetElement);
   if(addToHistory) { HLPnavTo(targetElement); }
   HLPupdateNavButtons();
   HLPsearchFocus();
}

function HLPselect(targetElement) {
   document.querySelectorAll('.HLPstrong').forEach(item => item.classList.remove('HLPstrong'));
   targetElement.classList.add('HLPstrong');
   HLPtoggleDeprecated();
}

function HLPreloadWindow() {
   const selected = document.querySelector('.HLPstrong')?.innerText;
   if(document.querySelector('.HLPcontent')) { HLPlistAll(selected); }
}

function HLPcontentHtml(){ return /*html*/`
   <div class="HLPcontent">
      <div class="HLPindexContainer"> 
         <div class="HLPindexPane"> 
            <div class="HLPsearchContainer">
            <div class="HLPnavButtons">
               <a class="HLPnavButtonDisabled HLPprev">< Go Back</a>
               <a class="HLPnavButtonDisabled HLPnext">Go Forward ></a>
            </div>
               <div class="HLPsearchWrapper">
                  <input class="HLPsearch FORMinput" placeholder="Search">
                  <div class="HLPsearchClear BUTTONcancel">
                     <img src="/LOG/LOGclearSearch.svg" alt="Clear">
                  </div>
               </div>
               <div class="BUTTONdefault BUTTONstandard HLPdepToggle"><input type="checkbox" ${HLPshowDeprecated ? 'checked' : ''}>Show Deprecated</div>
            </div>
            <div class="HLPlistBody"></div>
         </div>
         <div class="HLPresizeControl">
            <div class="HLPresizeShadow"></div>
         </div>
      </div>
      <div class="HLPrightPane HLPhidden"> 
         <div class="HLPcontainer"> <h3> No help file found. </h3> </div> 
      </div>
   </div>
   `
}