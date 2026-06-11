let LOGupdateTimer;
let LOGupdateInProgress;

function LOGinitUpdateLatestModel(){
   LOGupdateTimer = setInterval(function(){
      LOGupdateLatestLogs();
   }, 2000);
}

function LOGupdateLatestLogs(){
   let Page = PAGEhash();
   if(Page !== "logs") clearInterval(LOGupdateTimer);
   if(LOGtopViewPosition() <= 50 && !LOGstartTimeArgs() && LOGreceivedData && !LOGupdateInProgress) {
      LOGrenderNewestNewLogs();
   } 
   else{
      LOGsetTopBanner();
   }
}

function LOGsetTopBanner(){
   let Div = document.querySelector(".LOGtopBanner");
   if(!Div) return;
   let html = `Newest results`;
   if(Div.innerHTML != html) Div.innerHTML = html;
}

function LOGsetWarningBanner(){
   let Div = document.querySelector(".LOGwarningBanner");
   if(!Div) return;
   Div.classList.add("LOGhidden");
}

function LOGupdateLatestMessagesBanner(Number){
   let Div = document.querySelector(".LOGtopBanner");
   if(!Div) return;
   let html = `Newest results`;
   if(Number){
      let NumString = `${Number > 100 ? "+100" : Number}`;
      html = `<div class="LOGshowNewLogs">Show ${NumString} new logs</div>`;
   }
   if(Div.innerHTML != html) Div.innerHTML = html;
}


function LOGrenderNewestNewLogs(){
   let Args = LOGnextArgs();
   Args.trace = "Render newest";
   LOGupdateInProgress = true;
   APIcall("logs/retrieve", Args, function(R){
      LOGupdateInProgress = false;
      let Data = R.data || [];
      let Number = Data.length;
      LOGupdateLatestMessagesBanner(Number);
   });
 }