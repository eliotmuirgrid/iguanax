/*
This PAGE hash library is *DELIBERATELY* very simple.  If you come across requirements which
the library does not meet, please have an architectural conversation with Eliot before modifying the
library.  This is very important thank you!
*/

let PAGEtable = {}

function PAGEstart(PageTable){
   COL_TRC(PageTable);
   PAGEtable = PageTable;
   window.onhashchange = PAGEroute;
}

function PAGEserialize(Info){
   let Result = '#' + Info.page + '?';
   for (const K in Info.vars) {
      Result += encodeURIComponent(K) + "=" + encodeURIComponent(Info.vars[K]) + '&';
   }
   Result = Result.substring(0, Result.length - 1);
   COL_TRC(Result);
   return Result;
}

function PAGEsetHashParameter(Name, Value){
   COL_TRC(Name, "=", Value);
   let Info = PAGEinfo();
   if (!Value){
      COL_TRC("Removing the parameter.");
      delete Info.vars[Name];
   } else {
      Info.vars[Name] = Value;
   }
   location.hash = PAGEserialize(Info);
}

function PAGEhash(){
   return PAGEinfo().page;
}

function PAGEhashParameters(){
   return PAGEinfo().vars;
}

function PAGEhashParameter(Name){
   return PAGEhashParameters()[Name] || '';
}

function PAGEroute(){
   PAGEchangePage(PAGEhash());
}

function PAGEchangePage(Hash){
   COL_TRC("Changing to page",  "'" + Hash + "'");
   if (PAGEtable[Hash]){
      PAGEtable[Hash]();
   } else {
      COL_TRC("No such page at " + Hash);
      PAGEtable['error']();
   }  
}

function PAGEinfo(){
   return PAGEparseHash(location.hash);
}

// '#' symbol is optional. Query params (key-value pairs after the '?') are also optional
function PAGEparseHash(AnchorString) {
   COL_TRC(AnchorString);
   if (AnchorString[0] === '#') {
      AnchorString = AnchorString.slice(1);
   }
   let Result = {}
   const HashParts = AnchorString.split('?');
   Result.vars = PAGEargs(HashParts[1]);
   Result.page = HashParts[0];
   COL_TRC(Result);
   return Result;
}

function PAGEargs(Hash) {
   const Results = {};
   if (!Hash) {
      COL_TRC("No variables") 
      return Results; 
   }
   const Pairs = Hash.split('&');
   for (let i = 0; i < Pairs.length; i++) {
      const A = Pairs[i].split('=');
      Results[decodeURIComponent(A[0])] = decodeURIComponent(A[1]);
   }
   return Results;
}

function PAGEresetPageSpecial(PopoutClass){
   MODELshutdownAll();
   PAGEclearAllPopoutsExcept(PopoutClass);
   PAGEtimers.forEach(Timer => clearInterval(Timer));
   CACHErefreshPoller();
}

function PAGEresetPage() {
   // remove MODEL views
   MODELshutdownAll();
   PAGEremovePopouts();
   PAGEtimers.forEach(Timer => clearInterval(Timer));  // TODO can we remove this soon.
   CACHErefreshPoller();
}

function PAGEremovePopouts(){
   COL_TRC("Remove popouts");
   // This cleans out POPoutCards mainly and is actually a decent clean up function
   document.querySelectorAll('body > *:not(.CORmain):not(.CORheader)').forEach(function(el) { el.remove();});
}

// TODO - look to eliminate this
const PAGEtimers = [];
function PAGEaddTimer(TimerInt) {
   PAGEtimers.push(TimerInt);
}

function PAGEparamsToString(Obj){
   if(!Obj) return "";
   let P = [];
   let Keys = Object.keys(Obj);
   for(let i = 0; i< Keys.length; i++){
      let K = encodeURIComponent(Keys[i]);
      let V = encodeURIComponent(Obj[K]);
      if(V) P.push(`${K}=${V}`);
   }
   if(!P.length) return "";
   return P.join("&");
}

function PAGEformatParams(Obj){
   let UrlString = PAGEparamsToString(Obj);
   if(!UrlString) return "";
   return "?" + UrlString;
}

function PAGEclearAllPopoutsExcept(PopoutClass){
   let bodyElement = document.body;
   var childNodes = bodyElement.childNodes;
   for (var i = 0; i < childNodes.length; i++) {
     var node = childNodes[i];
     // Check if the node is an element node (nodeType 1)
     if (node.nodeType === 1) {
       // Check if the node has a class of 'POPpopoutCard'
       if (node.classList.contains('POPpopoutCard')) {
         // Check if it contains a child node with class 'DETwindow'
         var detWindowNode = node.querySelector(`.${PopoutClass}`);
         if (!detWindowNode) {
           // Remove the 'POPpopoutCard' element if it doesn't contain the 'PopoutClass'
           bodyElement.removeChild(node);
         }
       } else {
         // Remove any other child nodes that do not have the class 'CORmain' or 'CORheader'
         if (
           !node.matches('.CORmain') &&
           !node.matches('.CORheader')
         ) {
           bodyElement.removeChild(node);
         }
       }
     }
   }
}