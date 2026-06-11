function DOC2transformRelations(Relations){
   let RTags = [];
   for (let i=0; i < Relations.length; i++){
      if (Relations[i].title){
         RTags.push(Relations[i].title);
      }else{
         RTags.push(Relations[i].id);
      }
   }
   let R = HASHTAGrenderTags(RTags, "DOC2relationDiv");
   return R;
}

function DOC2relationsPopulate(Doc){
   let Relations = Doc.meta.relations;
   if (Relations.length > 0){
      document.querySelector(".DOC2relations").innerHTML = DOC2transformRelations(Relations);
      return;
   }
   document.querySelector(".DOC2relations").innerHTML = "";
}
