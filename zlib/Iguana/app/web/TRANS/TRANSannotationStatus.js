function TRANSannotationStatus(Message){
   let E = document.querySelector('.TRANSannotationStatus');
   if(!E) return;
   if(E.innerHTML !== Message) E.innerHTML = Message;
}

function TRANSannotationStatusBusy(){
   TRANSannotationStatus('<img class="TRANSspinner" src="/TRANS/images/spinner.gif"/>');
}

function TRANSannotationStatusOk(){
   TRANSannotationStatus('<div class="TRANSgotData">Got data</div>');
}