function CARDupdateInjectHtml(){
   let Data = CARDdata();
   let InjectTarget = document.querySelector('.CARDinjectTarget');
   if(!InjectTarget) return;
   let Html = Data.html_block || "";
   if(InjectTarget.innerHTML !== Html) InjectTarget.innerHTML = Html;
   if(Html == "") InjectTarget.classList.add   ('CARDhidden');
   else           InjectTarget.classList.remove('CARDhidden');
}