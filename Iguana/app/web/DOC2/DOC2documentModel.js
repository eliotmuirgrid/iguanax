function DOCdocumentModelInit(){
   MODELpollerAdd("DOCmodel", "DOCdocument"  , 30,  "doc/get",   DOC2documentParams);
   MODELpollerAdd("DOCmodel", "DOCcomponents", 100, "component/list", {});
   //Should we make a new api to get the flat tags?
   MODELpollerAdd("DOCmodel", "DOClist",   300,  "doc/search",DOC2searchParams);
  
   DOC2documentModelRefresh();
   MODELstart("DOCmodel");
   MODELupdate("DOCmodel");
   MODELviewAdd('DOCmodel','DOC2documentUpdate');
}

function DOC2documentModelRefresh(){
   MODELforcePoll('DOCmodel', function(){
      COL_TRC("DOCmodel refresh done");
      MODELupdate("DOCmodel");
   });
}