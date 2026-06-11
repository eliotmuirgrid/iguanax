function MENUopenMenu() {
   if (!TRANSopenMenu) return;
   let path = TRANSsourceDirectory + TRANSopenMenu;
   const sep = FILsep();
   if (path.endsWith(sep) && path.length > 1) path = path.slice(0, -1);
   const idxArr = MENUindexPath(PRJdirTreeRendered, path);
   TRANSopenMenu = "";
   console.log(idxArr);
   let node = MENUnodeAt(PRJdirTreeRendered, idxArr);
   console.log(node);
   let el = MENUgetElement(idxArr);
   console.log(el);
   MENUrender(node, el);
   return idxArr;
}
 
function MENUnodeAt(root, idxPath) {
   let node = Array.isArray(root) ? root[0] : root;
   for (let j = 0; j < idxPath.length; j++) {
     const i = idxPath[j];
     if (!node.contents
      || !Array.isArray(node.contents)
      || i < 0
      || i >= node.contents.length
     ) return null;
     node = node.contents[i];
   }
   return node;
}
 

function MENUgetElement(idxArr) {
   const rootUL = document.querySelector('.PRJfileTree');
   if (!rootUL) return null;
 
   // always descend into the root folder first
   const path = [0, ...idxArr];
   let currentUL = rootUL;
   let el = null;
 
   for (const idx of path) {
     const items = Array.from(currentUL.querySelectorAll(':scope > li'));
     el = items[idx];
     if (!el) return null;
     currentUL = el.querySelector(':scope > ul');
   }
   if(el.querySelector(".PRJrow")) el = el.querySelector(".PRJrow");
   return el;
 }

function MENUindexPath(root, targetPath) {
   const idxPath = [];
   let node = Array.isArray(root) ? root[0] : root;
   while (node && Array.isArray(node.contents)) {
     const i = node.contents.findIndex(ch =>
       targetPath.startsWith(ch.filepath)
     );
     if (i === -1) return null;
     idxPath.push(i);
     const child = node.contents[i];
     if (child.filepath === targetPath) return idxPath;
     node = child;
   }
   return null;
}
 