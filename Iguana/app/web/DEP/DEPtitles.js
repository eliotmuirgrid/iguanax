function DEPtitles(){
   let divs = document.querySelectorAll(".cm-DEPdeprecated");
   let Title = "Deprecated function - click for more info";
   for(div of divs) {
      if(div.title != Title) div.title = Title;
   }
}