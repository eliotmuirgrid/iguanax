function STApathHtml(){
   let data = STAsystemPathData();
   let html = `<div class="STApathModal">${data.join("<br>")}</div>`;
   return html;
}

function STApathDisplay(){
   let div = document.querySelector(".STApathModal");
   if(div) return;
   let target = document.querySelector(".STAsystemPath");
   if(!target) return;
   target.innerHTML = STApathHtml();
}