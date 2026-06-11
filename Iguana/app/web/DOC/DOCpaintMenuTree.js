function DOCpaintMenuTree(Node) {
   let Tags = Node.tags || [];
   let Docs = Node.docs || [];
   COL_TRC(Tags);
 
   let H = '';

   let CurrentId = PAGEhashParameter("id");
 
   // Display docs if any
   if (Docs.length > 0) {
      for (let i = 0; i < Docs.length; i++) {
         H += "<div class='DOCdocument'>";
         H += "<div class='DOCdocumentTitle"
         if (CurrentId === Docs[i].id){
            H+= " DOChighlight";
         }
         H += "'>" + Docs[i].title + "</div>";
         //H += DOCgetMeta(Docs[i]);
         H += "</div>";
      
      }
   }
 
   // Display tags and recurse if needed
   for (let i = 0; i < Tags.length; i++) {
     H += "<div class='DOCtag'>" + Tags[i].name;
     if (Tags[i].children !== undefined) {
       H += DOCpaintMenuTree(Tags[i].children);
     }
     H += "</div>";
   }
   
   return H;
 }