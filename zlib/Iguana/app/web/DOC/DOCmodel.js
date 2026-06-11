function DOCmodelInit() {
   COL_TRC("Set up model.");
   MODELshutdownAll();
   // TODO make the API names more consistent.
   MODELpollerAdd("DOCmodel", "DOClist"      , 30,  "doc/tag",        DOClistParams);
   MODELpollerAdd("DOCmodel", "DOCdocument"  , 30,  "doc/get",        DOCdocumentParams);
   MODELpollerAdd("DOCmodel", "DOCcomponents", 100, "component/list", {});
}

function DOClistModelInit(){
   COL_TRC("Set up list model.");
   MODELshutdownAll();
   MODELpollerAdd("DOClistModel", "DOClist", 30, "doc/search",       DOCsearchParams);
   MODELpollerAdd("DOClistModel", "DOCdocument"  , 30,  "doc/get",   DOCdocumentParams);
   MODELpollerAdd("DOClistModel", "DOCcomponents", 100, "component/list", {});
}

function DOCmodelRefresh(){
   MODELforcePoll('DOCmodel', function(){
      COL_TRC("Model refresh done");
   });
}

function DOClistModelRefresh(){
   MODELforcePoll('DOClistModel', function(){
      COL_TRC("Model refresh done");
   });
}