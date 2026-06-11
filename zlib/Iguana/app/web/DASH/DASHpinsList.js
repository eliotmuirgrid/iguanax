function DASHmatchingTagComponents(Tag){
   let Components = [];
   let Data = DASHlistData();
   if(Tag == 'All') return Data.map(comp => comp.guid);
   for(let comp of Data){
      if(comp.tags.includes(Tag)) Components.push(comp.guid);
   }
   return Components;
}

function DASHtagSorted(PinPos, TagComps, Result){
   for(posy in PinPos){
      for(posx in PinPos[posy]){
         let Component = PinPos[posy][posx];
         if(TagComps.includes(Component) && !Result.includes(Component)) {
            Result.push(Component);
         }
      }
   }
}

function DASHcalcPinsPositions(){
   let Pins = DASHpinData();
   let Map = {};
   for(let guid in Pins){
      let Pos = Pins[guid];
      let Py = Pos.py;
      let Px = Pos.px;
      if(!Map[Py]) Map[Py] = {};
      Map[Py][Px] = guid;
   }
   return Map;
}

function DASHlistDataMap(){
   let Data = DASHlistData();
   let Map = {};
   for(let comp of Data) Map[comp.guid] = comp;
   return Map;
}

function DASHsortListData(){
   if(PAGEhashParameters().sort) return;
   let PinnedComponents = [];
   let PinPos = DASHcalcPinsPositions();
   let Tags = DASHtagData();
   for(let tag of Tags){
      let TagComps = DASHmatchingTagComponents(tag);
      DASHtagSorted(PinPos, TagComps, PinnedComponents);
   }
   let AllComps = DASHmatchingTagComponents('All');
   DASHtagSorted(PinPos, AllComps, PinnedComponents);
   let Data = DASHlistData();
   let Map = DASHlistDataMap();
   let PinnedSet = new Set(PinnedComponents);
   let NewData = [];
   for(let comp of Data){
      if(PinnedSet.has(comp.guid)) continue;
      NewData.push(Map[comp.guid]);
   }
   for(let comp of PinnedComponents) NewData.push(Map[comp]);
   MODELdata.DASHcomponentListData.data.component_data = NewData;
}
