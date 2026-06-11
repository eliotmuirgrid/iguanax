function HLPsetListeners2() {
   let div = document.querySelector('.HLPcontent');
   if(!div) return;
   div.removeEventListener('click', HLPclick);
   div.addEventListener('click', HLPclick);
}

function HLPclick(e) {
   if(e.target.closest('.HLPsearchClear')) return HLPclearSearch();
}

function HLPclearSearch() {
   let input = document.querySelector('.HLPsearch');
   if(!input) return;
   input.value = '';
   HLPupdateList();
}

function HLPupdateList() {
   // TODO - did not want to use dispatchEvent but the listeners are set strangely
   // and do not make it easy to call update list.
   let input = document.querySelector('.HLPsearch');
   if(!input) return;
   input.dispatchEvent(new Event('input'));
}