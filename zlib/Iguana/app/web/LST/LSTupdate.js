function LSTupdateTitle(){
   let div = document.querySelector(".LSTtitle");
   if(!div) return;
   let Title = LSTstatus();
   if(div.innerHTML != Title) div.innerHTML = Title;
}

function LSTupdateStop(){
   let div = document.querySelector(".LSTstop");
   if(!div) return;
   let done = LSTdone();
   if( done && !div.classList.contains("LSThidden")) div.classList.add("LSThidden");
   if(!done &&  div.classList.contains("LSThidden")) div.classList.remove("LSThidden");
}

function LSTupdateDismiss(){
   let div = document.querySelector(".LSTdismiss");
   if(!div) return;
   let done = LSTdone();
   if(!done && !div.classList.contains("LSThidden")) div.classList.add("LSThidden");
   if( done &&  div.classList.contains("LSThidden")) div.classList.remove("LSThidden");
}

function LSTupdateHref(){
   let div = document.querySelector(".LSTseeDetHref");
   if(!div) return;
   let comp = LSTargs.target || "";
   if(!comp) return;
   let href = `#logs?component=${comp}&tags=%23resubmit`;
   if(div.href != href) div.href = href;
}
