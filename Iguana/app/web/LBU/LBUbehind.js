let LBUbehindData;

function LBUbehindUpdate(libdata, url){
   let div = document.querySelector(".LBUdetailsDiv");
   if(!div) return;
   let Html = LBUbehindHtml(libdata, url);
   if(div.innerHTML != Html) div.innerHTML = Html;
   // behind per library
}

function LBUbehindHtml(libdata, url){
   let behind = LBUcount(libdata?.usage, "behind");
   if(!behind) return "";
   return LBUbehindSection(libdata, url);
}

function LBUbehindSection(libdata, key){
   let name = LBUname(libdata?.name, key);
   let Html = LBUheader(name, "behind");
   let Usage = libdata?.usage || [];
   let table = LBUbehindTable(Usage);
   return Html + table + "<br>";
}