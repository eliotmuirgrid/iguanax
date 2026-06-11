function DETupdateName(){
   let Div = document.querySelector(".DETcomponentName");
   if(!Div) return;
   let ComponentData = DETcomponentData();
   if(!ComponentData) ComponentData = {name : "Iguana Service"};
   let Name = ComponentData.name;
   if(Div.innerHTML != Name ) Div.innerHTML = Name;
}

function DETupdateTags(){
   let Div = document.querySelector(".DETtags");
   if(!Div) return;
   let Data = DETmessageData();
   let Tags = Data?.tags || [];
   let tagsHtml = LOGrowTagsHtml(Tags)
   if(Div.innerHTML != tagsHtml ) Div.innerHTML = tagsHtml;
}