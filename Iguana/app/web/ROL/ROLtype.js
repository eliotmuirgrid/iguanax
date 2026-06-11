function ROLcheckType(target){
   let Type = target.querySelector(".ROLroleAppliesTo select")?.value;
   if(Type == "user"){
      target.closest(".ROLcard").classList.remove("ROLcomponentRole");
   }
   if(Type == "component"){
      target.closest(".ROLcard").classList.add("ROLcomponentRole");
   }
}

function ROLcomponentType(){
   let Type = document.querySelector(".ROLroleAppliesTo select")?.value;
   return Type == "component";
}