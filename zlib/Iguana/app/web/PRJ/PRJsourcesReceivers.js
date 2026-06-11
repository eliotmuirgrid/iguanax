function LOGcontrolSourcesReceivers(){
   let Hide = TRANScomponentHide();
   TRANScontrolElement(".PRJsources-receivers", Hide);
}

function PRJsourcesReceiversHtml() {
   let Hide = TRANScomponentHide();
   let AddReceiverHtml = TRANSviewingTranslator() ? /*html*/`
   <a class="PRJ-table-add" href="#dashboard/new?source=${TRANScomponentGuid()}">+ Add +</a>
   ` : "";
   return /*html*/`
   <div class="PRJsources-receivers${Hide}">
      <div class="PRJ-table-container">
         <div class="PRJ-table-header">
            <div class="PRJ-table-title">Sources</div>
         </div>
         <div class="PRJ-table-scroller PRJsourcesTable"></div>
      </div>
      <div class="PRJbreak"></div>
      <div class="PRJ-table-container">
         <div class="PRJ-table-header">
            <div class="PRJ-table-title">Receivers</div>
            ${AddReceiverHtml}
         </div>
         <div class="PRJ-table-scroller PRJreceiversTable"></div>
      </div>
   </div>`;
}

function PRJpreprocessArray(array, componentData) {
   return array
     .map(key => ({
       name: componentData[key].name,
       guid: componentData[key].guid
     }))
     .sort((a, b) => (a.name > b.name ? 1 : -1));
 }
 
 function PRJgetSourcesAndReceivers(currentComponent, componentData) {
   return {
     sources: PRJpreprocessArray(currentComponent?.sources || [], componentData),
     receivers: PRJpreprocessArray(currentComponent?.receivers || [], componentData)
   };
 }
 
 function PRJpopulateSourcesReceivers() {
   const page = PAGEhash();
   let componentData, currentComponent, dev, urlFormatter;
 
   switch (page) {
     case 'translator':
       componentData = TRANScomponentMapData();
       currentComponent = componentData[TRANScomponentGuid()];
       dev = TRANSdevelopment();
       urlFormatter = TRANSformatUrl;
       break;
 
     case 'logs':
       componentData = LOGcomponentMapData();
       currentComponent = componentData[LOGcomponentId()];
       urlFormatter = LOGformatUrl;
       break;
 
     default:
       return;
   }
 
   const { sources, receivers } = PRJgetSourcesAndReceivers(currentComponent, componentData);
 
   const limit = 5;
   const slicedSources = sources.slice(0, limit);
   const slicedReceivers = receivers.slice(0, limit);
 
   PRJpopulateTable('.PRJsourcesTable', slicedSources, dev, urlFormatter);
   PRJpopulateTable('.PRJreceiversTable', slicedReceivers, dev, urlFormatter);
 }
 

function PRJsourcesReceiversBubbleHtml(items, dev, urlFormatter){
   let hrefs = {};
   let names = [];
   for(let item of items){
      let name = item?.name || "Iguana Service";
      if(name) {
         hrefs[name] = urlFormatter(item?.guid || "", dev);
         names.push(name);
      }
   }
   return HASHTAGrenderTags(names, "PRJcomps", "", hrefs);
}

function PRJpopulateTable(containerClass, items, dev, urlFormatter) {
   let html = PRJsourcesReceiversBubbleHtml(items, dev, urlFormatter);
   if(items.length == 0) html = `<div class="PRJnone">None</div>`;
   const container = document.querySelector(containerClass);
   if(container.innerHTML != html) container.innerHTML = html;
}


// function PRJsourcesReceiversTargetClicked(){
//    POPtooltip({
//       target: document.querySelector(".PRJsourcesReceiversTarget"),
//       content: PRJsourcesReceiversHtml(),
//       event: POPtooltipEvent.CLICK,
//       position: POPtooltipPosition.BOTTOM,
//       style: "PRJsourcesReceiversTargetTooltip",
//       yOffset: -10,
//       keepExisting: true,
//       callback: function(){
//          PRJpopulateSourcesReceivers();
//       }
//    });
// }