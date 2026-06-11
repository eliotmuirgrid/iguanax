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
<div class="DOCcontent DOCmd"></div>`;
}

function DOCtransformTags(Tags){
   //COL_TRC(Tags);
   let HTags = [];
   for (let i=0; i < Tags.length; i++){
      HTags.push("#" + Tags[i]);
   }
   let R = HASHTAGrenderTags(HTags, "DOCtagDiv");
   //COL_TRC(R);
   return R;
}

function DOCreplaceInnerTextIfChanged(Element, Value){
   if (Element.innerText !== Value){
      Element.innerText = Value;
   }
}

function DOCdocumentDisplay(Doc){
   if (!document.querySelector(".DOCeditOpen")){
      if (!document.querySelector(".DOCtitle")){
         document.querySelector(".DOCrightDisplay").innerHTML = DOCdocumentHtml(); 
         document.querySelector(".DOCdownloadButton") .addEventListener('click', function(E){ DOCdocumentDownload ();});
         document.querySelector(".DOCreprocessButton").addEventListener('click', function(E){ DOCreprocessDialog  ();});
         document.querySelector(".DOCdeleteButton").addEventListener('click',    function(E){ DOCdeleteDialog     ();});
         DOCdocumentMdView();
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
   

function DOCconvertDate(Created) {
  let TimeStamp = parseInt(Created, 16);
  const FullDate = new Date(TimeStamp * 1000);

  const days = ["Sun", "Mon", "Tues", "Wed", "Thurs", "Fri", "Sat"];
  const months = [
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
  ];

  function getOrdinal(n) {
    if (n > 3 && n < 21) return n + "th"; // 11th–13th special case
    switch (n % 10) {
      case 1: return n + "st";
      case 2: return n + "nd";
      case 3: return n + "rd";
      default: return n + "th";
    }
  }

  const dayName = days[FullDate.getDay()];
  const day = getOrdinal(FullDate.getDate());
  const month = months[FullDate.getMonth()];
  const year = FullDate.getFullYear();

  return `${dayName} ${day} ${month} ${year}`;
}


function DOCdocumentParams(){
   return {
      "component" : PAGEhashParameter("component"),
      "id"        : PAGEhashParameter("id"), 
      "created"   : PAGEhashParameter("created")
   };
}

function DOCdocumentUpdate(){
   COL_TRC("Document update");
   if (!MODELdata.DOCdocument.success) {
      COL_TRC("No document loaded.");
      document.querySelector(".DOCrightDisplay").innerHTML = "No document loaded.";
   } else {
      COL_TRC(MODELdata.DOCdocument.data);
      DOCdocumentDisplay(MODELdata.DOCdocument.data); 
   }
}

function DOCdocumentDownload(){
   let DOCdocument = MODELdata.DOCdocument.data.meta;
   const Data = { created : DOCdocument.created, id: DOCdocument.id, component: PAGEhashParameter("component") };
   let Url = "doc/download" + PAGEformatParams(Data);
   window.open(Url, "_blank");
}

function DOCdocumentTagGet(i, Doc,TagIndex){
   COL_TRC(i, Doc, TagIndex);
   if (i < TagIndex.length){
      return DOCdocumentTagGet(i+1, Doc.tags[TagIndex[i]].children, TagIndex);
   }
   return Doc;
}

function DOCdocumentGet(TagIndex, DocIndex){
   let Tag = DOCdocumentTagGet(0, MODELdata.DOClist.data, TagIndex);
   COL_TRC(Tag);
   return Tag.docs[DocIndex];
}

function DOCdocumentClick(E){
   let DocTitle = E.target.closest(".DOCdocumentTitle");
   COL_TRC("Display document", DocTitle);
   const Container = DocTitle.closest('.DOCdocument');
   let i = DOCdocumentIndex(DocTitle);
   COL_TRC(i);
   let TagIndex = DOCtagAddress(DocTitle.closest(".DOCtag"));
   COL_TRC(TagIndex)
   let DOCdocument = DOCdocumentGet(TagIndex, i);
   COL_TRC(DOCdocument);
   PAGEsetHashParameter("id",      DOCdocument.id) 
   PAGEsetHashParameter("created", DOCdocument.created);
}

function DOCdocumentIndex(DocTitle){
   const parent = DocTitle.closest('.DOCtag');                    // find the nearest ancestor with class DOCtag
   const siblings = parent.querySelectorAll('.DOCdocumentTitle'); // all the relevant children
   // Convert NodeList to array and find the index
   const i = Array.prototype.indexOf.call(siblings, DocTitle);
   return i;
}
