let DETfirstResultFound;
let DETfindTimeout;

function DETfindArgs(){
   let Args  = DETcomponentArgs();
   Args.time = PAGEhashParameter("time");
   Args.id   = PAGEhashParameter("id");
   Args.search = DETfindValue();
   Args.view_mode = DETstringViewArg();
   return Args;
}

function DETfindValue(){
   let div = document.querySelector(".DETsearchInput");
   if(!div) return;
   let val = div.value;
   return val.trim();
}

function DETfindInput(){
   clearTimeout(DETfindTimeout);
   DETfindTimeout = setTimeout(DETfindExecute, 200);
}

function DETfindExecute(){
   DETresetSearch();
   DETselectTopAndBottom();
   let search = DETfindValue();
   if(!search || DETsegmentSelected()) {
      DETupdateFindStop(true);
      return;
   }
   APIcall("log/detail/search/start", DETfindArgs(), function(R){
      DETfindModelInit();
   });
}

function DETfindModelInit(){
   DETresultsRendered = 0;
   DETfirstResultFound = false;
   MODELpollerAdd('DETfindModel', 'DETfindResults', 0.5, 'log/detail/search/poll', DETfindArgs);
   MODELviewAdd  ('DETfindModel', 'DETupdateCount');
   MODELviewAdd  ('DETfindModel', 'DETupdateResults');
   MODELviewAdd  ('DETfindModel', 'DETupdateFindFirst');
   MODELviewAdd  ('DETfindModel', 'DETupdateFindStop');
   MODELforcePoll('DETfindModel');
   MODELstart('DETfindModel');
}

function DETupdateFindStop(force = false){
   let done = DETfindData()?.done || false;
   if(done || force) MODELremove("DETfindModel");
}

function DETupdateFindFirst(){
   if(DETfirstResultFound) return;
   let top = document.querySelector(".DETtopVisible");
   let viewIdx = DETgetIdxOfRow(top);
   let data = DETfindData()?.search || [];
   if(!data) return;
   for(let i = 0; i < data.length; i++){
      let dataIdx = data[i]?.ChunkNumber || 0;
      if(dataIdx < viewIdx) continue;
      DETselectSearchIdx(i, data.length);
      return;
   }
   if(DETdone()){
      let num = DETcount() - 1;
      if(!DETvalidInput(num)) return;
      DETsetIdx(DETcount() - 1);
      DETfirstResultFound = true;
   }
}

function DETselectSearchIdx(i, length){
   let div = document.querySelector(".DETmessageDiv");
   if(!div) return;
   let divs = div.querySelectorAll(".LOGhighlight");
   let idx = DETresultsRendered - length + i;
   let focus = divs[idx];
   if(!focus) return;
   DETsetIdx(idx);
   DETfirstResultFound = true;
}

function DETsetIdx(idx){
   if(idx === "") DETsetSearchIdx(idx)
   else           DETsetSearchIdx(idx + 1);
   DETclearSelected();
   DETsetSelected(idx);
}

function DETsetSearchIdx(idx){
   let input = document.querySelector(".DETsearchNumber");
   input.value = idx;
}

function DETclearSelected(){
   let div = document.querySelector(".DETmessageDiv");
   if(!div) return;
   let divs = div.querySelectorAll(".DETselected");
   for(div of divs){
      div.classList.remove("DETselected");
   }
}

function DETsetSelected(idx){
   if(idx === "") return;
   let div = document.querySelector(".DETmessageDiv");
   if(!div) return;
   let divs = div.querySelectorAll(".LOGhighlight");
   let selected = divs[idx];
   if(!selected) return;
   selected.classList.add("DETselected");
   selected.scrollIntoView({ block: 'center', behavior: 'smooth' });
}

function DETgetIdxOfRow(el){
   if(!el) return;
   const Row = el.closest('.DETmessageRow');
   if(!Row) { return; }
   const parentElement = Row.parentNode;
   const rows = Array.from(parentElement.querySelectorAll('.DETmessageRow'));
   const Index = rows.indexOf(Row);
   return Index;
}

function DETupdateResults(){
   let Data = DETfindData()?.search || [];
   for(let i = 0; i < Data.length; i++) {
      DETrenderRow(Data[i]);
      DETresultsRendered = DETresultsRendered + 1;
   }
}

function DETrenderRow(Data){
   DETupdateRow(Data?.ChunkNumber, Data?.Value, true);
}

function DETfindData(){
   return MODELdata?.DETfindResults?.data || {};
}

function DETresetSearch(){
   DETresetSearchDivs();
   DETupdateCount("--");
   DETsetIdx("");
}

function DETresetSearchDivs(){
   let cont = document.querySelector(".DETmessageDiv");
   if(!cont) return;
   let divs = cont.querySelectorAll(".LOGhighlight");
   for(div of divs){
      if(!div) continue;
      let par = div.closest(".DETmessageRow");
      if(!par) continue;
      par.innerHTML = "";
      par.classList.add("DETmessageLoading");
   }
}

function DETcountData(){
   let count = DETfindData()?.count;
   if(!count || count == "") return "--";
   let ans = "";
   let done = DETfindData()?.done || false;
   if(!done) ans = ".. ";
   return ans + count;
}

function DETupdateCount(input){
   let html = input ? input : DETcountData();
   let div = document.querySelector(".DETsearchResultsCount");
   if(!div) return;
   if(div.innerHTML != html) div.innerHTML = html;
}