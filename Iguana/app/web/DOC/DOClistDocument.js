function DOCdocumentHtml(){
   return `
<div class="DOCdocumentHeader">
<div class="DOCdate"></div>
   <div class="DOCtitleSection"></div>
   <div class="DOCdocumentHeaderIcons">
      <button class="DOCbutton DOCeditButton">
         <svg
            class="DOCicon"
            viewBox="0 0 24 24"
            width="20" height="20"
            fill="none"
            stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"
            role="img" aria-hidden="true" focusable="false">
            <!-- Pencil body / edit shape -->
            <path d="M3 17.25V21h3.75L20.5 7.25a2.5 2.5 0 1 0-3.54-3.54L3 17.25z"></path>
            <!-- Facet line on the tip -->
            <path d="M14.5 5.5l3 3"></path>
         </svg>
      </button>
      <button class="DOCbutton DOCreprocessButton">
         <svg
            class="DOCicon"
            viewBox="0 0 24 24"
            width="20" height="20"
            fill="none"
            stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"
            role="img" aria-hidden="true" focusable="false">
            <!-- Reprocess loop (two chasing arrows) -->
            <path d="M6 15c0 2.2 1.8 4 4 4h2"></path>
            <polyline points="11 21 12 19 10 19"></polyline>
            <path d="M18 9c0-2.2-1.8-4-4-4h-2"></path>
            <polyline points="13 3 12 5 14 5"></polyline>

            <!-- Document with dog-ear (Feather-style file) -->
            <path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"></path>
            <polyline points="14 2 14 8 20 8"></polyline>
         </svg>
      </button>
      <button class="DOCbutton DOCdownloadButton">
         <svg
            class="DOCicon"
            viewBox="0 0 24 24"
            width="20" height="20"
            fill="none"
            stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"
            role="img" aria-hidden="true" focusable="false">
            <!-- Arrow -->
            <path d="M12 3v10"></path>
            <path d="M7 9l5 5 5-5"></path>
            <!-- Tray -->
            <path d="M5 21h14a2 2 0 0 0 2-2v-4H3v4a2 2 0 0 0 2 2z"></path>
         </svg>
      </button>
      <button class="DOCbutton DOCdeleteButton">
         <svg
            class="DOCicon"
            viewBox="0 0 24 24"
            width="20" height="20"
            fill="none"
            stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"
            role="img" aria-hidden="true" focusable="false">
            <!-- Lid -->
            <path d="M4 6h16"></path>
            <path d="M9 6V4a2 2 0 0 1 2-2h2a2 2 0 0 1 2 2v2"></path>
            <!-- Can body -->
            <path d="M6 6l1 13a2 2 0 0 0 2 2h6a2 2 0 0 0 2-2L18 6"></path>
            <!-- Inner lines -->
            <path d="M10 11v6"></path>
            <path d="M14 11v6"></path>
         </svg>
      </button>
   </div>
</div>
<div class="DOCdocumentTagBar"></div>
<div class="DOCrelations"></div>
<div class="DOCdescription"></div>
<div class="DOCcontent DOCmd"></div>
<div class="DOCtagCloud"></div>`;
}


function DOCreplaceInnerTextIfChanged(Element, Value){
   if (Element.innerText !== Value){
      Element.innerText = Value;
   }
}

function DOClistDocumentDisplay(Doc){
   if (!document.querySelector(".DOCeditOpen")){
      if (!document.querySelector(".DOCtitle")){
         document.querySelector(".DOCrightDisplay").innerHTML = DOCdocumentHtml(); 
         document.querySelector(".DOCdownloadButton") .addEventListener('click', function(E){ DOCdocumentDownload ();});
         document.querySelector(".DOCreprocessButton").addEventListener('click', function(E){ DOCreprocessDialog  ();});
         document.querySelector(".DOCdeleteButton").addEventListener('click',    function(E){ DOCdeleteDialog     ();});
         DOCdocumentMdView();
         DOCtagCloudUpdate();
         DOCtreeTagsEventRegister();
      }
      document.querySelector(".DOCdate").innerHTML = DOCconvertDate(Doc.meta.created);
      if (!document.querySelector(".DOCtitleEditCancel")){
         DOCdocumentTitleView();
      }
      if (!document.querySelector(".DOCtagEditCancel")){
         DOCdocumentTagsView();
      }
      DOCrelationsPopulate();
      if (DOCpaintableAttachment(Doc.meta.source)){
         DOCpaintAttachment(Doc.meta.source, Doc.meta.id, Doc.meta.created);
      }else{
         DOCreplaceInnerTextIfChanged(document.querySelector(".DOCcontent"),Doc.markdown); 
      }
   }     
}
   
function DOClistDocumentUpdate(){
   COL_TRC("Document update");
   if (!MODELdata.DOCdocument.success) {
      COL_TRC("No document loaded.");
      document.querySelector(".DOCrightDisplay").innerHTML = "No document loaded.";
   } else {
      COL_TRC(MODELdata.DOCdocument.data);
      DOClistDocumentDisplay(MODELdata.DOCdocument.data); 
   }
}

