function LOGbulkHtml(){
   return /*html*/`
   <div class="LOGbulkDiv">
      <div class="LOGbulkTitleDiv">
         <div class="LOGtitle LOGbulkTitle">Bulk Action</div>
         <div class="LOGclearButton LOGclearDateDiv">[ <div class="LOGclearBulk">Reset</div> ]</div>
         <div class="LOGclearButton LOGcancelDateDiv">[ <div class="LOGcancelBulk">Cancel</div> ]</div>
      </div>
      <div class="LOGbulkRow LOGbulkRowStart">
         <div class="LOGstartDot"></div>
         <input class="FORMinput LOGbulkStart" type="text" value="" placeholder="From log"/>
      </div>
      <div class="LOGbulkRow LOGbulkRowEnd">
         <div class="LOGendDot"></div>
         <input class="FORMinput LOGbulkEnd" type="text" value="" placeholder="To log"/>
      </div>
      <div class="LOGbulkButtonDiv">
         <select disabled class="FORMselectBox LOGbulkAction">
            <option>Resubmit</option>
         </select>
         <div class="BUTTONstandard BUTTONdefault LOGbulkConfirm">APPLY</div>
      </div>
   </div>`;
}

function LOGselectingEndMode(){
   let div = document.querySelector(".LOGcontainer");
   if(!div) return;
   return div.classList.contains("LOGselectingEnd");
}

function LOGselectingStartMode(){
   let div = document.querySelector(".LOGcontainer");
   if(!div) return;
   return div.classList.contains("LOGselectingStart");
}

function LOGselectingMode(){
   return LOGselectingStartMode() || LOGselectingEndMode();
}

function LOGbulkEndSelect(){
   LOGclearBulk();
   let div = document.querySelector(".LOGcontainer");
   if(!div) return;
   let Class = "LOGselectingEnd";
   let Contains = div.classList.contains(Class);
   if(Contains) return LOGclearBulk();
   div.classList.add(Class);
   div = document.querySelector(".LOGbulkEnd");
   FORMfocus(div);
}

function LOGbulkStartSelect(){
   LOGclearBulk();
   let div = document.querySelector(".LOGcontainer");
   if(!div) return;
   let Class = "LOGselectingStart";
   let Contains = div.classList.contains(Class);
   if(Contains) return LOGclearBulk();
   div.classList.add(Class);
   div = document.querySelector(".LOGbulkStart");
   FORMfocus(div);
}

function LOGresetBulk(){
   // LOGupdateInputFields();
   LOGclearBulk();
   LOGbulkSetStart("");
   LOGbulkSetEnd("");
   LOGbulkSetOption("Resubmit");
}
function LOGcancelBulkClicked(){
   LOGclearBulk();
}

function LOGbulkSetOption(OptionValue){
   let div = document.querySelector(".LOGbulkAction");
   if(!div) return;
   div.value = OptionValue;
}

function LOGclearBulk(){
   let div = document.querySelector(".LOGcontainer");
   if(!div) return;
   let Class = "LOGselectingStart";
   div.classList.remove(Class);
   Class = "LOGselectingEnd";
   div.classList.remove(Class);
}

function LOGselectButton(target){
   const Item = LOGtargetToItemData(target);
   let Args  = {};
   Args.time = Item.time;
   Args.id   = Item.id;
   let Out = PAGEparamsToString(Args);
   if(LOGselectingStartMode()) LOGbulkSetStart(Out);
   if(LOGselectingEndMode())   LOGbulkSetEnd(Out);
   LOGclearBulk();
}

function LOGbulkSetStart(StartValue){
   let div = document.querySelector(".LOGbulkStart");
   if(!div) return;
   div.value = StartValue;
}

function LOGbulkSetEnd(EndValue){
   let div = document.querySelector(".LOGbulkEnd");
   if(!div) return;
   div.value = EndValue;
}
