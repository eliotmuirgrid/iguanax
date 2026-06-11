let STUcritical = 95;
let STUwarning = 85;

function STUfreeUpdate(){
   let data = STUlogData();
   let Used = data.disk_used;
   let Size = data.disk_size;
   const Free = STGfuzzySize(Size - Used);
   const FreePercent = Math.floor( (Size - Used) / Size * 100 );
   let html = `${Free} free of ${STGfuzzySize(Size)} (${FreePercent}% free)`;
   let div = document.querySelector('.STUfree');
   if(!div) return;
   if(div.innerHTML != html) div.innerHTML = html;
}

function STUstatusHtml(Percent){
   let Label = "OK";
   if(Percent > STUwarning)  Label = "WARN";
   if(Percent > STUcritical) Label = "CRITICAL";
   return /*html*/`
   <div class="STUstatusGrid STUicon${Label}">
      <div class=""></div>
      <div class="STUlabel${Label}">${Label}</div>
   </div>`;
}

function STUstatusUpdate(){
   let Percent = STUpercent();
   let html = STUstatusHtml(Percent);
   let div = document.querySelector('.STUstatus');
   if(!div) return;
   if(div.innerHTML != html) div.innerHTML = html;
}

function STUpercent(){
   let data = STUlogData();
   let Used = data.disk_used;
   let Size = data.disk_size;
   return Math.floor( Used / Size * 100 );
}

function STUbarUpdate(){
   let Percent = STUpercent();
   let div = document.querySelector(".STUprogressUsed");
   if(!div) return;
   let Width = Percent + "%";
   if(div.style.width != Width) div.style.width = Width;
}

function STUbarStatusUpdate(){
   let Percent = STUpercent();
   if(Percent > STUcritical) return STUbarCritical();
   if(Percent > STUwarning)  return STUbarWarning();
   STUbarOk();
}

function STUbarCritical(){
   let div = document.querySelector(".STUprogressUsed");
   if(!div) return;
   if( div.classList.contains("STUbarWarn"))     div.classList.remove("STUbarWarn");
   if(!div.classList.contains("STUbarCritical")) div.classList.add("STUbarCritical");
}

function STUbarWarning(){
   let div = document.querySelector(".STUprogressUsed");
   if(!div) return;
   if( div.classList.contains("STUbarCritical")) div.classList.remove("STUbarCritical");
   if(!div.classList.contains("STUbarWarn"))     div.classList.add("STUbarWarn");
}

function STUbarOk(){
   let div = document.querySelector(".STUprogressUsed");
   if(!div) return;
   if( div.classList.contains("STUbarCritical")) div.classList.remove("STUbarCritical");
   if( div.classList.contains("STUbarWarn"))     div.classList.remove("STUbarWarn");
}