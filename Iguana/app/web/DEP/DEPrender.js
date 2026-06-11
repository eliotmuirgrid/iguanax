let DEPcurrentDepOverlay = null;
let DEPoverlayData = {};

function DEPreset(){
   TRANSeditor.removeOverlay(DEPcurrentDepOverlay);
   DEPcurrentDepOverlay = null;
   DEPoverlayData = {};
}

function DEPescapeRegex(str) {
   return str.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
 }

function DEPcreateOverlay(Data) {
   const keys = Object.keys(Data);
   if (!keys.length) {
      return {
         token: function(stream) {
         stream.next(); 
         return null;
         }
      };
   }

   const pattern = new RegExp("\\b(" + keys.map(DEPescapeRegex).join("|") + ")\\b");
      return {
         token: function(stream /*, state */) {
            if (stream.match(pattern)) {
               return "DEPdeprecated";
            }
            stream.next();
            return null;
         }
      };
}

function DEPupdate() {
   if (!TRANSrenderedFileName.endsWith(".lua")) {
      if (DEPcurrentDepOverlay) DEPreset()
      return;
   }
   
   let Data = MODELdata?.DEPdata?.data || {};
   const depOverlay = DEPcreateOverlay(Data);
   if(DEEPequal(Data, DEPoverlayData)) return;
   if (DEPcurrentDepOverlay) TRANSeditor.removeOverlay(DEPcurrentDepOverlay);
   DEPcurrentDepOverlay = depOverlay;
   TRANSeditor.addOverlay(depOverlay);
   DEPoverlayData = Data;
   DEPtooltips();
}
