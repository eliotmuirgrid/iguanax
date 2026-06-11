function LBUusageUpdate(libdata, url){
   let div = document.querySelector(".LBUdetailsDiv");
   if(!div) return;
   let Html = LBUusageHtml(libdata, url);
   if(div.innerHTML != Html) div.innerHTML = Html;
}

function LBUusageHtml(libdata, url){
   let usage = libdata?.usage?.length;
   if(!usage) return "";
   return LBUusageSection(libdata, url);
}

function LBUusageSection(libdata, key){
   let name = LBUname(libdata?.name, key);
   let Html = LBUheader(name, "usage");
   let Usage = libdata?.usage || [];
   let table = LBUusageTable(Usage);
   return Html + table + "<br>";
}