let DOCminListWidth = 410;

function DOClistView(){
   DOClistModelInit();
   COL_TRC("Set up tag cloud.");
   document.querySelector(".DOCmain").innerHTML = DOCbodyHtml();
   document.querySelector(".DOCrightDisplay").classList.add("DOClistRightDisplay");
   document.querySelector(".DOCtree").classList.add("DOClistTree");
   DOCbulkActions();
   MODELviewAdd('DOClistModel','DOClistUpdate');
   MODELviewAdd('DOClistModel','DOClistDocumentUpdate');
   DOClistModelRefresh();
   MODELstart("DOClistModel");
   DOClistViewListeners();
}

function DOCsearchParams(){
   let D = {};
   D.component = PAGEhashParameter("component");
   D.search    = PAGEhashParameter("search") || "";
   return D
}

function DOClistViewListeners(){
   document.querySelector(".DOClist").addEventListener('click', function(E){
      DOClistRowClick(E);
   });  
   document.querySelector(".DOCseeTags").addEventListener('click', function(E){
      DOCseeTagsClick(E);
   });
   SPLTsetupSplitter('.DOCbody', '.DOCresizeControl', '.DOCtree', 'DOCratio', DOCminListWidth, undefined, undefined)
   DOCrightDisplayEvent();
   DOCsearchButtonEvent();
}

function DOClistRowClick(E){
   const Target = E.target;
   let DocTitle = Target.closest(".DOClistRow");
   COL_TRC("Display document", DocTitle);
   let i  = DOCdocumentListIndex(DocTitle);
   let DocList = MODELdata.DOClist.data.list
   PAGEsetHashParameter("id",      DocList[i].id) 
   PAGEsetHashParameter("created", DocList[i].created);
}

function DOCdocumentListIndex(DocTitle){
   const parent = DocTitle.closest('.DOClist'); 
   const siblings = parent.querySelectorAll('.DOClistRow'); 
   const i = Array.prototype.indexOf.call(siblings, DocTitle);
   return i-1;
}