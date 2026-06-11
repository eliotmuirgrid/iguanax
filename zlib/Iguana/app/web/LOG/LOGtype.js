function LOGfilterResults(){
   let Div = document.querySelector('.LOGfilterAllCheckbox');
   if(!Div) return;
   return Div.checked;
}

function LOGwholewordResults(){
   let Div = document.querySelector('.LOGexactCheckbox');
   if(!Div) return;
   return Div.checked;
}

function LOGupdateInputFields(){
   let Search = PAGEhashParameter("search");
   let Tags   = PAGEhashParameter("tags");
   let searchInp = document.querySelector('.LOGsearchInput');
   let tagInp    = document.querySelector('.LOGtagsInput');
   if(searchInp) searchInp.value = Search;
   if(tagInp)    tagInp.value    = Tags;
}

function LOGupdateFilterCheckbox(){
   let Filter = PAGEhashParameter("filter");
   let Div = document.querySelector('.LOGfilterAllCheckbox');
   if(!Div) return;
   if( Filter && !Div.checked) Div.checked = true;
   if(!Filter &&  Div.checked) Div.checked = false;
}

function LOGupdateWholeWordCheckbox(){
   let WholeWord = PAGEhashParameter("exact");
   let Div = document.querySelector('.LOGexactCheckbox');
   if(!Div) return;
   if( WholeWord && !Div.checked) Div.checked = true;
   if(!WholeWord &&  Div.checked) Div.checked = false;
}