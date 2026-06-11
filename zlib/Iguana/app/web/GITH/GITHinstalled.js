function GITHerrorHtml(Task){
   return /*html*/`<div class="STAwarningText">Git must be installed to ${Task}. [<a class="SETlink" href="#settings">See About</a>]</div>`;
}

function GITHupdateInstalled(){
   let GitInstalled = GITHgitInstalled();
   if(GitInstalled) return;
   let div = document.querySelector(".SETpageDescription");
   if(!div) return;
   div.innerHTML = GITHerrorHtml("configure your git credentials");
   let div2 = document.querySelector(".GITHmodal");
   if(!div2) return;
   div2.innerHTML = "";
   MODELremove("GITHmodel");
}