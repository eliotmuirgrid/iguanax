function ECNtargetUpdate(){
   let div = document.querySelector(".ECNtargetDivInfo");
   if(!div) return;
   let Html = ECNtargetHtml();
   if(div.innerHTML !== Html) div.innerHTML = Html;
}  

function ECNcomponentList(Component, Class, List, DefaultOption, Edit){
   let Html = `<select class="FORMselectBox ${Class}">`;
   Html += `<option value="" disabled selected>-- ${DefaultOption} --</option>`;
   for (ComponentData of List) {
      let Guid = ComponentData?.guid || ComponentData;
      if(Edit && !ECNeditPermission(Guid)) continue;
      let Name = ECNname(Guid);
      let Selected = (Guid == Component) ? " selected" : "";
      Html += `<option${Selected} value="${Guid}">${Name}</option>`;
   }
   Html += "</select>";
   return Html;
}

function ECNtargetHtml(){
   let href = `href="#dashboard/details?component=${ECNcomponent()}"`;
   let list = ECNlistData();
   let Warning = !ECNeditPermission(ECNcomponent());
   let WarningHtml = Warning ? "Warning: You do not have edit connections permission for this component." : "";
   return /*html*/`
      <div class="ECNtargetInfoHeader">Component:</div>
      ${ECNcomponentList(ECNcomponent(), "ECNtargetSelect", list, "Select a component")}
      <div class="ECNwarning">${WarningHtml}</div>
      <div class="ECNcomponentDetails">
         <div>[</div>
         <a ${href} class="ECNdetails">See details</a>
         <div>]</div>
      </div>`;
}