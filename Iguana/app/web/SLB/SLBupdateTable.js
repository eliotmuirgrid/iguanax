function SLBupdateTable(Data){
   let div = document.querySelector(".LBUcontents");
   if(!div) return;
   let Html = LBUtableHtml(SLBdetData(), "", true);
   if(Html != div.innerHTML) div.innerHTML = Html;
}