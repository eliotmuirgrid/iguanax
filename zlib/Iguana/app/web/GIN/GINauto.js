let GINeditAutoCard;

function GINupdateAuto(){
   let div = document.querySelector(".GINauto");
   if(!div) return;
   div.innerHTML = GINautoHtml();
}

function GINautoValueDisplay(){
   let I = GINautoValue();
   if(I == 0) return "Disabled";
   let min = I == 1 ? "minute" : "minutes";
   return `${I} ${min}`;
}

function GINautoHtml(){
   return /*html*/`
   <div class="FORMlabel GINmarginTop">Auto Push Every:</div>
   <div class="GINautoValue GINmarginTop">${GINautoValueDisplay()}</div>
   <div class="GINeditAuto BUTTONgroup"></div>`;   
}

function GINeditAuto(){
   GINeditAutoCard = POPpopoutOpen({
      title: "Edit Auto Push Interval", 
      content: GINeditAutoHtml(),
      width: "500px"
   });
   GINeditAutoListeners();
   let input = document.querySelector(".GINeditAutoInput");
   FORMfocus(input);
   FORMenterListener(input, GINeditAutoConfirm);
}

function GINeditAutoListeners(){
   let div = document.querySelector(".GINeditAutoModal");
   div.addEventListener("click", function(e){
      if(e.target.closest(".GINeditAutoConfirm")) GINeditAutoConfirm();
      if(e.target.closest(".GINeditAutoCancel"))  GINeditAutoClose();
   });
}

function GINeditAutoConfirm(){
   let div = document.querySelector(".GINeditAutoInput");   
   if(!div) return;
   let value = div.value;
   APIcall("instance/interval/set", {interval: value}, function(r){
      if(r.error) return SNCKsnackbar('error', r.error);
      MODELforcePoll("GINmodel");
      let Message = GINupdateAutoInterval(value);
      GINaddAndCommit({message: Message, settings: true});
      GINeditAutoClose();
   });
}  

function GINeditAutoClose(){
   POPpopoutClose(GINeditAutoCard);
}  

function GINeditAutoHtml(){
   return /*html*/`
   <div class="GINeditAutoModal">
      <div class="GINeditAutoDescription">Iguana pushes changes to the repository at set intervals. Set to 0 to disable.</div>
      <br>
      <br>
      <div class="GINautoGrid">
         <div class="FORMlabel GINmarginTop">Auto Push Every:</div>
         <div class="GINeditAutoInputContainer">
            <input type="number" class="GINeditAutoInput FORMinput" value="${GINautoValue()}">
            <div class="GINeditAutoUnit">minutes</div>
         </div>
      </div>
      <div class="GINeditAutoError"></div>
      <br>
      <br>  
      <div class="GINeditAutoButtons BUTTONgroup">
         <div class="GINeditAutoCancel BUTTONdefault BUTTONstandard">CANCEL</div>
         <div class="GINeditAutoConfirm BUTTONaction BUTTONstandard">CONFIRM</div>
      </div>
   </div>`;
}

