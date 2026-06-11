function DOClistParams(){
   return {
      tag       : PAGEhashParameter("tag"), 
      component : PAGEhashParameter("component")
   };
}

function DOCupdate(){
   if (!MODELdata.DOClist.success){
      PAGEsetHashParameter("tag");
      COL_ERR("No such tag.");
      return;
   }
   if (!document.querySelector(".DOClist")){
      // kind of gross - Eliot TODO separate concerns.
      return;
   }

   document.querySelector(".DOClist").innerHTML = DOCpaintMenuTree(MODELdata.DOClist.data);
}

function DOCmainScreen(ResetPage = false){
    DOCmainScreenImp(ResetPage);
}

function DOClayout() {
   let SeeList = PAGEhashParameter("seetags");
   return /*html*/ `
   <div class="DOCtoolbar">
      <div class="BUTTONaction BUTTONstandard DOCseeTags">${SeeList?"SEE TREE":"SEE LIST"}</div>
      <div class="BUTTONaction BUTTONstandard DOCdiff">DIFF</div>
      <div class="BUTTONaction BUTTONstandard DOCreset">RESET</div>
      <div class="BUTTONaction BUTTONstandard DOCupload">UPLOAD</div>
      <div class="BUTTONaction BUTTONstandard DOCimport">IMPORT</div>
      <input class="DOCsearchInput" type="text" value="${PAGEhashParameter("search")}">
      <div class="BUTTONaction BUTTONstandard DOCsearch">SEARCH</div>
      <div class="DOCerror"></div>
   </div>
   <div class="DOCmain"></div>`;
}

function DOCbodyHtml(){
   return `<div class="DOCbody">
         <div class="DOCtree">
            <div class="DOClist"></div>
            <div class="DOCresizeControl">
					<div class="DASHresizeShadow"></div>
				</div>
         </div> 
         <div class="DOCrightDisplay"></div>
      </div>`;
}

function DOCtreeEventRegister(){
   document.querySelector(".DOClist").addEventListener('click', function(E){
      DOClistClick(E);
   });   
   
   DOCrightDisplayEvent();
}

function DOCmainEventsRegister(){
   COL_TRC("Registered events");
   document.querySelector(".DOCseeTags").addEventListener('click', function(E){
      DOCseeTagsClick(E);
   });
   document.querySelector(".DOCdiff").addEventListener('click', function(E){
      DOCdiffClick();
   });
   SPLTsetupSplitter('.DOCbody', '.DOCresizeControl', '.DOCtree', 'DOCratio', DOCminListWidth, undefined, undefined)
   DOCresetClick();
   DOCuploadClick();
   DOCimportButtonEvent();
   DOCsearchButtonEvent();

}

function DOClistClick(E){
   const Target = E.target;
   if (Target.closest('.DOCdocumentTitle')) { DOCdocumentClick(E); return; }
   if (Target.closest('.DOCtag'))           { DOCtagClick     (E); return; }  
}

function DOCmainScreenImp(ResetPage){
   if(ResetPage) PAGEresetPage();
   DOCmodelInit();
   let body = document.body;
   body.className = "DOCapp";
   body.innerHTML = DOClayout();
   if (PAGEhashParameter("seetags") !== "true"){
      document.querySelector(".DOCmain").innerHTML = DOCbodyHtml();
      DOCtreeEventRegister();
      MODELviewAdd('DOCmodel','DOCupdate');
      MODELviewAdd('DOCmodel','DOCdocumentUpdate');
      DOCmodelRefresh();
      DOCmainEventsRegister();
      MODELstart('DOCmodel');
   } else {
      DOClistView();
   }

  
}

function DOCseeTagsClick(){
   let SeeTags = PAGEhashParameter("seetags");
   SeeTags = !SeeTags;
   COL_TRC(SeeTags);
   PAGEsetHashParameter("seetags", SeeTags);
}

// Change later to support having multiple tags open at once
// Use similar mechanism as the rest of Iguana to remember the latest component
