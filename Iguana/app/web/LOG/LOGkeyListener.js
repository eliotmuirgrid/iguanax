// These are set so that we can scroll logs no matter where the focus is 
document.addEventListener('keydown', LOGkeydownHandler);
document.addEventListener('keyup',   LOGkeyupHandler);

function LOGlogPageLoaded(){
   let Container = document.querySelector('.LOGcontainer');
   if(!Container) return false;
   let Page = PAGEhash();
   return Page == "logs";
}

function LOGkeydownHandler(e){
   if(!LOGlogPageLoaded()) return;
   if (e.key === 'ArrowDown' && !e.repeat) {
      e.preventDefault();
      return LOGstartScrollingOlder(); 
   }
   if (e.key === 'ArrowUp' && !e.repeat)  {
      e.preventDefault();
      return LOGstartScrollingNewer();
   }
   if(e.key === 'PageUp' && !e.repeat){
      e.preventDefault();
      return LOGstartPageScrollingNewer();
   }
   if(e.key === 'PageDown' && !e.repeat){
      e.preventDefault();
      return LOGstartPageScrollingOlder();
   }
}

function LOGkeyupHandler(){
   let Container = document.querySelector('.LOGcontainer');
   if(!Container) return;
   clearInterval(LOGscrollNewerInterval);
   clearInterval(LOGscrollOlderInterval);
   clearInterval(LOGscrollPageNewerInterval);
   clearInterval(LOGscrollPageOlderInterval);
}