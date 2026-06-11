function ROLheaderClicked(target){
   let div = target.closest(".ROLcard");
   if(div.classList.contains("ROLcollapsed")){
      div.classList.remove("ROLcollapsed");
   }
   else{
      div.classList.add("ROLcollapsed");
   }
}