function LBUaheadUpdate(libdata, url){
   let div = document.querySelector(".LBUdetailsDiv");
   if(!div) return;
   let Html = LBUaheadHtml(libdata, url);
   if(div.innerHTML != Html) div.innerHTML = Html;
}

function LBUaheadHtml(libdata, url){
   let ahead = LBUcount(libdata?.usage, "ahead");
   if(!ahead) return "";
   return LBUaheadSection(libdata, url);
}

function LBUaheadSection(libdata, key){
   let name = LBUname(libdata?.name, key);
   let Html = LBUheader(name, "ahead");
   let Usage = libdata?.usage || [];
   let table = LBUaheadTable(Usage);
   return Html + table + "<br>";
}