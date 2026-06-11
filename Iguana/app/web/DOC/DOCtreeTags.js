// TODO - I have a lot of copy pasted code here :-(  hall of Eliot shame...



function DOCtreeTagsClick(E){
   const Target = E.target;
   if (Target.closest('.DOCtagDiv')){ return DOCtagListDivClick(Target); }  
}

function DOCtagListData(){
   return MODELdata.DOClist.data.flattags;
}

function DOCtagListDivClick(Target){
   let Idx = DOCaddressOf(Target, "DOCtagDiv");
   COL_TRC(Idx);
   let Data = DOCtagListData() || [];
   let Tag = Data[Idx];
   COL_TRC(Tag);
   DOCtagAddTag("DOCsearchInput", Tag);
   DOCsearch();
}