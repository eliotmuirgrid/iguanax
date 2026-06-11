function LSTsetListeners(){
   let div = document.querySelector(".LSTmodal");
   if(!div) return;
   div.addEventListener("click", LSTclick);
}

function LSTclick(e){
   if(e.target.closest(".LSTstop"))    return LSTstop();
   if(e.target.closest(".LSTdismiss")) return LSTdismiss();
}

function LSTstop(){
   APIcall("logs/resubmit/stop", {}, function(){
      
   });
}

function LSTdismiss(){
   let div = document.querySelector(".LOGbulkStatus");
   if(!div) return;
   div.innerHTML = "";
}