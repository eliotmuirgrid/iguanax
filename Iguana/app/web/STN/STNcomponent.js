function STNupdateSelectedComponent(){
   let div = document.querySelector(".SETselectNotificationComponent");
   if(!div) return;
   let Html = STNcomponentSelectedHtml();
   if(div.innerHTML !== Html) div.innerHTML = Html;
}

function SETgetSelectedComponent() {
   let SelectedData  = SETselectedNotiData()?.data;
   let ComponentData = MODELdata?.SETcomponentNotiData?.data?.component_data;
   if (SelectedData.component_id === "DISABLED") return "DISABLED";
   for(let i in ComponentData) {
      if (ComponentData[i].guid === SelectedData.component_id) return ComponentData[i].name;
   }
}

function STNcomponentSelectedHtml(){
   if(STNeditPage()) return STNeditComponentSelectedHtml();
   return SETrenderNotificationMessageHtml();
}

function STNeditComponentSelectedHtml(){
   return /*html*/`
   <div class="SETselectNotificationComponentEdit">
      <div class="SETselectComponentLabel">When these rules are triggered, the selected component will be invoked: </div>
      <select class="SETcomponentDropdown FORMselectBox" name="Component List">${SETlistOfComponents()}</select>
   </div>
   ${!SETselectedNotiData().success ? `<div class='SETcomponentNotiDataErrorMsg'>Error receiving the list of components to select from: ${SETselectedNotiData().error}</div><div></div>` : ""}`;
}

function SETrenderNotificationMessageHtml() {
   let OperationalData = MODELdata.SEToperationalNotiData.data;
   let SelectedData    = SETselectedNotiData().data;
   let ComponentName = ESChtmlEscape(SETgetSelectedComponent());
   if (ComponentName === "DISABLED" || !OperationalData?.exists) {
      return `<span class="SETnotificationWarning">No component is selected to be called when notification rules are triggered.</span>
          <p>To enable notifications create a component to receive these calls.<p>This component would typically be programmed to redirect to email or a messaging system like Slack or Teams etc.`;
   } else {
      let CompLink = `<a href="#dashboard/details?component=${SelectedData.component_id}">${ComponentName}</a> component`;
      let TestLink = `<span class="STNtest">click here</span>`;
      let LogLink  = `<a href="#logs?component=${SelectedData.component_id}">logs</a>`;
      if (OperationalData.enabled ){
         return `Notifications are enabled, the rules are checked every sixty seconds — if triggered the ${CompLink} will be called. 
                 <p>They can be disabled by turning off the component.</p>
                 For testing ${TestLink} to check the rules immediately, then look at the ${LogLink} for the ${ComponentName} component.`;

      } else {
         return `<span class="SETnotificationWarning">The component <b> ${ComponentName} </b> is not running, so the notifications system is disabled.</span>
         <p>To enable notifications go to the ${CompLink} and turn it on.</p>`;
      }
   }
}

function SETlistOfComponents() {
   let ComponentData = MODELdata.SETcomponentNotiData.data.component_data;
   let SelectedData  = SETselectedNotiData().data;
   let ComponentOptionsHtml = "";
   ComponentOptionsHtml += SETlistHtml(SETcreateDisableOption(), SelectedData);
   for(const i in ComponentData) { ComponentOptionsHtml += SETlistHtml(ComponentData[i], SelectedData); }
   return ComponentOptionsHtml;
}

function SETlistHtml(Item, SelectedData) {
   return `<option value="${ESChtmlEscape(Item.guid)}"${Item.guid === SelectedData.component_id ? ` selected=""` : ""}> ${ESChtmlEscape(Item.name)} </option>`
}

function SETcreateDisableOption() { return {name: "DISABLED", guid: "DISABLED"} }
