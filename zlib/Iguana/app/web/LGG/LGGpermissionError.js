function LGGnoPermissionError(){
   return /*html*/`
   <div class="LGGgeneratorUsage">You do not have permission to use the log generator.</div>
   <br>
   <br>
   <a class="BUTTONstandard BUTTONaction" href="#dashboard">GO TO DASHBOARD</a>`;
}

function LGGpermissionError(){
   let Perm = LGGpermissionData();
   if(Perm) return;
   let div = document.querySelector(".LGGgeneratorContentInner");
   if(!div) return;
   let Html = LGGnoPermissionError();
   if(div.innerHTML != Html) div.innerHTML = Html;
   MODELremove("LGGgeneratorModel");
}