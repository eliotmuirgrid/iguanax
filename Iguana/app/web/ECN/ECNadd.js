function ECNaddSourceUpdate(){
   let div = document.querySelector(".ECNaddSource");
   if(!div) return;
   let Html = ECNaddSourceHtml();
   if(div.innerHTML !== Html) div.innerHTML = Html;
}

function ECNaddReceiverUpdate(){
   let div = document.querySelector(".ECNaddReceiver");
   if(!div) return;
   let Html = ECNaddReceiverHtml();
   if(div.innerHTML !== Html) div.innerHTML = Html;
}

function ECNaddSourceHtml(){
   return /*html*/`
   <div class="ECNselectionRow">
      ${ECNcomponentList("", "ECNaddSourceComp", ECNallowableSources(), "Select a source to add", true)}
      <div class="BUTTONstandard BUTTONaction ECNaddSourceBtn" title="Add component as a source">ADD</div>
   </div>`;
}

function ECNaddReceiverHtml(){
   return /*html*/`
   <div class="ECNselectionRow">
      ${ECNcomponentList("", "ECNaddReceiverComp", ECNallowableDestinations(),"Select a destination to add", true)}
      <div class="BUTTONstandard BUTTONaction ECNaddReceiverBtn" title="Add component as a destination">ADD</div>
   </div>`;
}