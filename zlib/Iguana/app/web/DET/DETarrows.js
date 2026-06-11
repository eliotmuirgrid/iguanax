function DETupdateButton(Div, disabled){
   if(!Div) return;
   if( Div.classList.contains('DETdisabled') && !disabled) Div.classList.remove("DETdisabled");
   if(!Div.classList.contains('DETdisabled') &&  disabled) Div.classList.add   ("DETdisabled");
}

function DETupdateNext(){
   let Div = document.querySelector('.DETdetailNext');
   let Latest = DETisLatest();
   DETupdateButton(Div, Latest); 
}

function DETupdatePrevious(){
   let Div = document.querySelector('.DETdetailPrev');
   let Oldest = DETisOldest();
   DETupdateButton(Div, Oldest); 
}

function DETisLatest(){ return DETnextData().length == 0; }
function DETisOldest(){ return DETprevData().length == 0; }

function DETupdateHref(A, Condition, Data){
   if(!A) return;
   let H = "";
   if(Condition){
      let Item = Data[0];
      let Args = {};
      Args.component = PAGEhashParameter("component");
      Args.time      = Item.time;
      Args.id        = Item.id;
      Args.search    = PAGEhashParameter("search");
      Args.tags      = PAGEhashParameter("tags");
      Args.exact     = PAGEhashParameter("exact");
      Args.filter    = PAGEhashParameter("filter");
      H = "#logs/details" + PAGEformatParams(Args);
   }
   if( H && A.href != H) A.href = H;
   if(!H && A.href != H) A.removeAttribute("href");
}

function DETupdateNextHref(){
   let A = document.querySelector('.DETdetailNext');
   let Data = DETnextData();
   let Condition = !DETisLatest();
   DETupdateHref(A, Condition, Data);
}

function DETupdatePreviousHref(){
   let A = document.querySelector('.DETdetailPrev');
   let Data = DETprevData();
   let Condition = !DETisOldest();
   DETupdateHref(A, Condition, Data);
}
