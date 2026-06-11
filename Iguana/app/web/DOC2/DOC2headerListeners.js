function DOC2headerListeners(){
   //handling for document2 would need to be added to TGITgit.js logic for this to work
   document.querySelector(".DOC2diff").addEventListener   ("click",  DOC2diffClick);
   document.querySelector(".DOC2reset").addEventListener  ("click",  DOC2resetDialog);
   document.querySelector(".DOC2upload").addEventListener ('click',  DOC2uploadDialog);
   document.querySelector(".DOC2import").addEventListener ('click',  DOC2importDialog);
   document.querySelector(".DOC2seeTags").addEventListener('click',  DOC2seeTagsClick);
   document.querySelector(".DOC2search").addEventListener ("click",  DOC2searchClick);
}

function DOC2seeTagsClick(){
   let SeeTags = PAGEhashParameter("seetags");
   SeeTags = !SeeTags;
   COL_TRC(SeeTags);
   PAGEsetHashParameter("seetags", SeeTags);
}

function DOC2searchClick(){
   const Search = document.querySelector(".DOC2searchInput").value.trim();
   let Args = {
      component : PAGEhashParameter("component"),
      seetags   : PAGEhashParameter("seetags"),
      search    : Search,
      tag       : PAGEhashParameter("tag"),
      id        : PAGEhashParameter("id"),
      created   : PAGEhashParameter("created")
   };
   let hash =  `#documents2${PAGEformatParams(Args)}`;
   let oldhash = window.location.hash;
   if(oldhash != hash) window.location.hash = hash;
}
