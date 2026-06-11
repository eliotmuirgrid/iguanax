function LSTrenderSkeleton(){
   let div = document.querySelector(".LSTmodal");
   if(div) return;
   div = document.querySelector(".LOGbulkStatus");
   if(!div) return;
   div.innerHTML = LSTskeletonHtml();
   LSTsetListeners();
}

function LSTbarHtml(){
   return /*html*/`
   <div class="LSTbar">
      <div class="LSTincomplete"></div>
      <div class="LSTcomplete"></div>
   </div>
   `;
}

function LSTskeletonHtml(){
   return /*html*/`
   <div class="LSTmodal">
      <div class="LSTgrid">
         <div class="LSTtitle"></div>
         <div class="LSTseeDetails">[
            <a class="LSTseeDetHref">See details</a>
         ]
         </div>
         <div class="LSTstopDiv">[
            <div class="LSTstop">Stop</div>
            <div class="LSTdismiss">Dismiss</div>
         ]</div>
      </div>
      ${LSTbarHtml()}
      <div class="LSTdate"></div>
   </div>`;
}
