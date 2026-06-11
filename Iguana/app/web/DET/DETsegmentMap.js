function DETgetSegmentSettingsMap() {
   let MapJson = STORElocalRead("DETsegmentSettingsMap");
   if(!MapJson) {
      MapJson = "{}";
      STORElocalCreate("DETsegmentSettingsMap", MapJson);
   }
   return JSON.parse(MapJson);
}

function DETsaveSegmentSettingsMap(Map) {
   STORElocalCreate("DETsegmentSettingsMap", JSON.stringify(Map));
}

function DETsetSegmentSettings(UseDefault, VmdPath) {
   const guid = DETcomponentId();
   if(!guid) return;

   let Map = DETgetSegmentSettingsMap();
   Map[guid] = {
      use_default: UseDefault ? "true" : "false",
      vmd: VmdPath || ""
   };
   DETsaveSegmentSettingsMap(Map);
}
