function GINcommitMessage(Action, Components){
   let Message = "";
   if(Components.length <= 1){
      Message = `${Action} the component: "${Components[0].name || Components[0]}"`;
   }
   else if(Components.length <= 4){
      Message = `${Action} the components: `;
      for (let i = 0; i < Components.length; i++) {
         if (i === Components.length - 2) {
            Message += `"${Components[i].name || Components[i]}" and `;
         } else if (i === Components.length - 1) {
            Message += `"${Components[i].name || Components[i]}"`;
         } else {
            Message += `"${Components[i].name || Components[i]}", `;
         }
      }
   }
   else{
      Message = `${Action}: ${Components.length} components`;
   }
   return Message + ".";
}

function GINsetTags(Which, Filters){ 
   if(Filters.length == 0 || (Filters.length == 1 && Filters[0] == "")) return `Cleared ${Which} tags.`;
   if(Filters.length == 1) return `Set the ${Which} tags to: "${Filters[0]}".`;
   let LastFilter = Filters.pop();
   let Message = `Set the ${Which} tags to: "${Filters.join(`", "`)}" and "${LastFilter}".`;
   return Message;
}