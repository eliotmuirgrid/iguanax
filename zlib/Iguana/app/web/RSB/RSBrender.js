let RSBcard;

function RSBrender(){
   let Args = {};
   Args.content = RSBresubmitHtml();
   Args.title = "Edit and Resubmit";
   Args.width = "75%";
   Args.height = "82%";
   Args.minHeight = "460px";
   Args.className = "RSBpopout";
   RSBcard = POPpopoutOpen(Args);
   RSBmodelInit();
   RSBsetListeners();
   RSBeditorInit();
}

function RSBtopHtml(){
   return /*html*/`     
   <div class="RSBcomponent">
      <div class="FORMlabel RSBconfirmText">Destination component</div>
      <select class="RSBselect FORMselectBox"></select>
   </div>
`;
}

function RSBmiddleHtml(){
   return /*html*/`
   <div class="RSBeditorDiv">
      <div class="RSBrow">
         <div class="FORMlabel RSBconfirmText">Message content</div>
         <div class="RSBbuttons">${LFDselectHtml()}</div>
      </div>
      <div class='RSBeditor LFDsampleData'>
         <textarea class='RSBmessage FORMinput' placeholder='Message'></textarea>
      </div>
   </div>`;
}

function RSBresubmitHtml(){
   return /*html*/`
   <div class="RSBcontent">
      <div class="RSBmodal">
         <div class="RSBright">
            ${RSBtopHtml()}
         </div>
         <div class="RSBleft">
            ${RSBmiddleHtml()}
         </div>
         <div class="BUTTONgroup BLKbuttons">
            <div class="BUTTONstandard BUTTONcancel RSBclose">Cancel</div>
            <div class="BUTTONstandard BUTTONaction RSBresubmit">Resubmit</div>
         </div>
      </div>
   </div>
   `;
}
