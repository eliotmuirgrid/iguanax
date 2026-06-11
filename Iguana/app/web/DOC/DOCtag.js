function DOCaddressOf(node, className) {
   const path = [];
   let n = node;
   while (n?.classList?.contains(className)) {
     const p = n.parentElement;
     const sibs = [...p.children].filter(el => el.classList.contains(className));
     path.unshift(sibs.indexOf(n));
     n = p.closest('.' + className);
   }
   return path;
}
 
function DOCtagAddress(node) {
   return DOCaddressOf(node, 'DOCtag');
}
 
function DOCtagName(Address){
   let Tag = '';
   let TagList = MODELdata.DOClist.data.tags;
   for (let i=0; i < Address.length; i++){
      Tag += TagList[Address[i]].name;
      if (i + 1 != Address.length){
         Tag+= "_";
         TagList = TagList[Address[i]].children.tags;
      }
   }
   COL_TRC(Tag);
   return Tag;
}

function DOCtagClick(E){
   COL_TRC(E.target);
   COL_TRC(DOCtagAddress(E.target));
   const NewTag = DOCtagName(DOCtagAddress(E.target));
   COL_TRC("NewTag");
   PAGEsetHashParameter("tag", NewTag); 
   PAGEsetHashParameter("id"); 
   PAGEsetHashParameter("created");
}
