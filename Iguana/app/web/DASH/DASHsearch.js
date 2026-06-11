let DASHsearchTimeout;

function DASHclearSearchText() {
   if(typeof CYTOmarkSearchViewportRestoreSkip === 'function') { CYTOmarkSearchViewportRestoreSkip(); }
   DASHsaveArgs();
   DASHpreviousFilter.filter = "";
   DASHpreviousFilter.whole_word = "";
   window.location.hash = `dashboard${PAGEformatParams(DASHpreviousFilter)}`;
}

function DASHsearchText() {
   return document.querySelector('.EDITfilter') && document.querySelector('.EDITfilter').value;
}

function DASHsearchTextChanged() {
   clearTimeout(DASHsearchTimeout);
   DASHsearchTimeout = setTimeout(function(){ 
      if(typeof CYTOmarkSearchViewportRestoreSkip === 'function') { CYTOmarkSearchViewportRestoreSkip(); }
      PAGEsetHashParameter("filter", DASHsearchText());
   }, 50);
}
