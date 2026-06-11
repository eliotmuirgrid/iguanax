let CMTshowAllOptionId = 'CMTshowAllOptionId';

function CMTshowAllOption(){
   let Option = {};
   Option.name = '-- All Commits on Instance --';
   Option.guid = CMTshowAllOptionId;
   return Option;
}

function CMTupdateComponents(){
   let Components = [CMTshowAllOption(), ...CMTcomponentListData()];
   let ComponentSelect = document.querySelector('.CMTcomponentSelectBox');
   ComponentSelect && (ComponentSelect.innerHTML = CMToptionHtml(Components));
}

function CMToptionHtml(Components){
   let Html = "";
   for(let Component of Components){
      Html += `<option value="${Component.guid}" ${Component.guid === CMTcomponentSelected ? "selected" : ""}>${Component.name}</option>`;
   }
   return Html;
}
