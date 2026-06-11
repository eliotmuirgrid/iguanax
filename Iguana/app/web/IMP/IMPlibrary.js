let IMPlibraryPopout; 

function IMPsetFromLibrary(target){
   const Content = target.innerText === 'X12' ? IMPx12libraryHtml() : IMPhl7libraryHtml()
   IMPlibraryPopout = POPpopoutOpen({
      title:'Library Select',
      content: Content, 
      width: '360px'
   }
   );
   IMPlibraryListeners();
}

function IMPlibraryListeners(e){
   let LibraryInput = document.querySelector('.IMPlibraryInput');
   if(!LibraryInput) return;
   LibraryInput.addEventListener("click",  IMPlibraryClick);
}

function IMPlibraryClick(e){
   if(e.target.closest('.IMPlibraryCancel'))   POPpopoutClose(IMPlibraryPopout);
   if(e.target.closest('.IMPlibraryConfirm'))  IMPlibrarySelected();
}

function IMPlibrarySelected(){
   let LibrarySelected = document.querySelector('.IMPlibrarySelect');
   if(!LibrarySelected) return;
   IMPfromFileData = {};
   IMPfromFile = "";
   IMPfromLibrary = LibrarySelected.value;
   MODELforcePoll('IMPmodel');
   POPpopoutClose(IMPlibraryPopout);
}

function IMPhl7libraryHtml(){
   return /*html*/`
   <div class="IMPlibraryInput">
      <div class="IMPlibrarySkeleton">
         <div class="FORMlabel">HL7 Library:</div>
         <select class="FORMselectBox IMPlibrarySelect">
            <option value="HL7 2.7" selected>2.7</option>
            <option value="HL7 2.6"  >2.6  </option>
            <option value="HL7 2.5.1">2.5.1</option>
            <option value="HL7 2.5"  >2.5  </option>
            <option value="HL7 2.4"  >2.4  </option>
            <option value="HL7 2.3.1">2.3.1</option>
            <option value="HL7 2.3"  >2.3  </option>
            <option value="HL7 2.2"  >2.2  </option>
            <option value="HL7 2.1"  >2.1  </option>
         </select>
      </div>
      <br>
      <div class="BUTTONgroup DBMqueryButton">
         <div class="IMPlibraryCancel BUTTONdefault BUTTONstandard">CANCEL</div>
         <div class="IMPlibraryConfirm BUTTONstandard BUTTONaction">SELECT</div>
      </div>
   </div>`;
}

function IMPx12libraryHtml(){
   return /*html*/`
   <div class="IMPlibraryInput">
      <div class="IMPlibrarySkeleton">
         <div class="FORMlabel">X12 Library:</div>
         <select class="FORMselectBox IMPlibrarySelect">
            <option value="X12 5010" selected>HIPAA 5010</option>
            <option value="X12 4010">HIPAA 4010</option>
         </select>
      </div>
      <br>
      <div class="BUTTONgroup DBMqueryButton">
         <div class="IMPlibraryCancel BUTTONdefault BUTTONstandard">CANCEL</div>
         <div class="IMPlibraryConfirm BUTTONstandard BUTTONaction">SELECT</div>
      </div>
   </div>`;
}