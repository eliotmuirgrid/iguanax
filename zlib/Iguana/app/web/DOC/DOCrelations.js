function DOCtransformRelations(Relations){
   let RTags = [];
   for (let i=0; i < Relations.length; i++){
      if (Relations[i].title){
         RTags.push(Relations[i].title);
      }else{
         RTags.push(Relations[i].id);
      }
   }
   let R = HASHTAGrenderTags(RTags, "DOCrelationDiv");
   return R;
}

function DOCrelationsPopulate(){
   let Relations = MODELdata.DOCdocument.data.meta.relations
   if (Relations.length > 0){
      document.querySelector(".DOCrelations").innerHTML = DOCtransformRelations(MODELdata.DOCdocument.data.meta.relations);
   }
}
