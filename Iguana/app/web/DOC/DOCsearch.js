function DOCsearchButtonEvent(){
   document.querySelector(".DOCsearch").addEventListener('click', function(E){DOCsearch(E);});
   document.querySelector(".DOCsearchInput").addEventListener("keydown", function(E){DOCsearchKey(E);});
}

function DOCsearchKey(E){
   if (E.key === "Enter"){
      COL_TRC("Trigger search.");
      DOCsearch();
   }
}

function DOCsearch() {
   return new Promise((resolve, reject) => {
      const Search = document.querySelector(".DOCsearchInput").value.trim();
      COL_TRC("Search!", Search);
 
      // TODO - later we can get right of this.
      APIcall("doc/index", { search: Search, component: PAGEhashParameter("component") },
      function (R) {
         try {
           DOCsearchComplete(R, Search);
           resolve({ R, Search });
         } catch (err) {
           reject(err);
         }
       }
     );
   });
 }

function DOCsearchComplete(R, Search){
   COL_TRC("Search complete!", R);
   PAGEsetHashParameter("search", Search);
}