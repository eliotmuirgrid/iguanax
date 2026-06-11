// Copyright (c) 2010-2014 iNTERFACEWARE Inc.  
// Updated to store annotation state per component GUID.

// Reads the per-component map, creating it if needed.
function TRANSgetAnnotationsMap() {
   let MapJson = STORElocalRead("TRANSannotationsMap");
   if (!MapJson) {
      MapJson = "{}";
      STORElocalCreate("TRANSannotationsMap", MapJson);
   }
   return JSON.parse(MapJson);
}

// Writes the map back to storage.
function TRANSsaveAnnotationsMap(map) {
   STORElocalCreate("TRANSannotationsMap", JSON.stringify(map));
}

// Store state for the current component
function TRANSsetAnnotationsEnabled(TurnOn) {
   const guid = TRANScomponentGuid();
   if (!guid) return;

   let map = TRANSgetAnnotationsMap();
   map[guid] = TurnOn ? "true" : "false";
   TRANSsaveAnnotationsMap(map);
}

function TRANSdisableAnnotationsForAllComponents(){
   let map = TRANSgetAnnotationsMap();
   if (!map) map = {};
   const List = PRJcomponentListData() || [];
   for (let i = 0; i < List.length; i++){
      const c = List[i];
      const guid = c?.guid || "";
      if (!guid) continue;
      map[guid] = "false";
   }
   TRANSsaveAnnotationsMap(map);
}


// Read state for the current component
function TRANSannotationsEnabled() {
   const guid = TRANScomponentGuid();
   if (!guid) return false;

   let map = TRANSgetAnnotationsMap();

   // Default to true if missing
   if (!(guid in map)) {
      map[guid] = "true";
      TRANSsaveAnnotationsMap(map);
   }

   let State = map[guid];
   let Result = State !== "false";

   if (!TRANSeditor) {
      COL_TRC("This got called before TRANSeditor was initialized.");
      return false;
   }
   return Result;
}

function TRANSupdateAutoButton() {
   let AutoOn = TRANSannotationsEnabled();
   let Button = document.querySelector(".TRANSautoRunButton");
   if (!Button) return;
   if (AutoOn) Button.classList.add("on");
   else        Button.classList.remove("on");
}

// -------------------------
// Tooltip wiring
// -------------------------
function TRNSautoStartUpdate(){
   const div = TRNSautoStartTarget();
   const tooltip = TRNSautoStartPopover();
   if(!div || tooltip) return;

   POPtooltip({
      target: div,
      position: "bottom",
      showDelay: 1200,   // wait 400ms before showing
      yOffset: -12,
      content: `<div class="TRNSautoStartTarget">Loading...</div>`,
      style: "TRNSautoStartModal",
      callback: function(){
         TRNSautoStartRenderHtml();
         const Toggle = document.querySelector(".TRNSautoStartToggle");
         if(!Toggle) return;
         Toggle.addEventListener("click", function(){
            TRNSdisableGlobally();
         });
      }
   });
}

function TRNSdisableGlobally(){
   TRANSdisableAnnotationsForAllComponents();
   TRANSupdateAutoButton();
   document.querySelector(".TRNSautoStartTarget").innerHTML = "Disabled globally.";
}

function TRNSautoStartTarget(){
   // Put this class on whatever icon/button opens the tooltip
   return document.querySelector(".TRANSautoRunButton");
}

function TRNSautoStartPopover(){
   return document.querySelector(".TRNSautoStartModal");
}

function TRNSautoStartRenderHtml(){
   const div = document.querySelector(".TRNSautoStartTarget");
   if(!div) return;
   const html = TRNSautoStartHtml();
   if(div.innerHTML !== html) div.innerHTML = html;
}

function TRNSautoStartHtml(){
   return /*html*/`
      <div class="TRNSautoStartGrid">
         <div class="TRNSautoStartSub">Disable globally if you want:</div>
         <div class="BUTTONdefault BUTTONstandard TRNSautoStartToggle">
            Disable globally
         </div>
   </div>
   `;
}
