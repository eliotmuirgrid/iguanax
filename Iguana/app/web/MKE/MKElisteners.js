function MKEsetListeners(){
   let div = document.querySelector('.MKEmodal');
   if(!div) return; 
   div.addEventListener('click', MKEmodalClick);      
}

function MKEmodalClick(e) {
   if(e.target.closest('.MKEcancel'))   return MKEclose();
   if(e.target.closest('.MKEmakeCopy')) return MKEmakeEditable(false, this);
   if(e.target.closest('.MKEeditOrig')) return MKEmakeEditable(true, this);
}

function MKEclose(){
   POPpopoutClose(MKEpopout);
   MODELremove('MKEmodel');
}