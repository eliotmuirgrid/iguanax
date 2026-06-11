function DEThighlights(){
   let cont = document.querySelector(".DETmessageDiv");
   if(!cont) return undefined;
   let divs = cont.querySelectorAll(".LOGhighlight");
   return divs || [];
}

function DETgetSelectedIndex(){
   let cont = document.querySelector(".DETmessageDiv");
   if(!cont) return;
   let divs = cont.querySelectorAll(".LOGhighlight");
   for(let i = 0; i < divs.length; i++){
      let div = divs[i];
      if(!div) continue;
      if(div.classList.contains("DETselected")) return i;
   }
   return undefined;
}

function DETcount(){ 
   let divs = DEThighlights();
   return divs?.length ? divs.length : undefined; 
}
function DETdone() { return DETfindData()?.done || false; }

function DETdownIdxClicked(){
   let sel   = DETgetSelectedIndex();
   if(sel === undefined) return;
   let newC  = sel + 1;
   let count = DETcount();
   let done  = DETdone();
   if(newC < count) return DETsetIdx(newC);
   if(!done) return;
   DETsetIdx(0);
}

function DETupIdxClicked(){
   let sel   = DETgetSelectedIndex();
   if(sel === undefined) return;
   let newC  = sel - 1;
   let count = DETcount();
   let done  = DETdone();
   if(newC >= 0) return DETsetIdx(newC);
   if(!done) return;
   DETsetIdx(count - 1);
}

function DETsearchNumberInput(){
   let input = document.querySelector(".DETsearchNumber");
   let val = input?.value || "";
   if(val == "") return;
   let count = DETcount();
   let det = DETvalidInput(val, 1, count);
   if(det === false) return;
   DETsetIdx(det - 1);
}

function DETvalidInput(val, min = 1, max = 1000) {
   const n = Number(val);
   if (!Number.isFinite(n)) return false;
   const i = Math.trunc(n);
   if (i < min) return String(min);
   if (i > max) return String(max);
   return String(i);
}

function DETsearchInputEnter(){
   DETdownIdxClicked();
}