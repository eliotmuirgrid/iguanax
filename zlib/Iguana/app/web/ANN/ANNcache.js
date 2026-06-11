// cache open and docked windows so we can reload on opening of translator
function ANNcachedObjectName(){ return TRANSprojectRoot() + "-annotations"; }

function ANNcachedObject(){ 
   let ObjectString = STORElocalRead(ANNcachedObjectName());
   let Object = {};
   try   { Object = JSON.parse(ObjectString); } 
   catch { ANNsaveToCache({});            }
   if(!Object) Object = {};
   return Object;
}

function ANNsaveToCache(Object){ STORElocalCreate(ANNcachedObjectName(), JSON.stringify(Object)); }

function ANNaddToLocalStorageOnOpen(ItemId, DockId){
   ItemId = ItemId.substr(3); // need to remove "WND" from the id
   if(!ANNcachableWindow(ItemId)) return;
   let CurrentOpenAnnotations = ANNcachedObject();
   let Value = DockId ? DockId : "";
   CurrentOpenAnnotations[ItemId] = Value;
   ANNsaveToCache(CurrentOpenAnnotations);
}

function ANNcachableWindow(ItemId){
   if(ItemId.length != 36) return false;  // seems that the ID's generated are also always 36 in length
   return ItemId.toLowerCase() != ItemId; // only the IDs with capitals from the annotations seem dynamic and updatable
}

function ANNclearFromLocalStorageOnClose(ItemId){
   ItemId = ItemId.substr(3); // need to remove "WND" from the id
   let CurrentOpenAnnotations = ANNcachedObject();
   delete CurrentOpenAnnotations[ItemId];
   ANNsaveToCache(CurrentOpenAnnotations);
}

function ANNopenPreviousAnnotations(){
   let AnnotationsToOpenObject = ANNcachedObject();
   ANNrestoreWindows(AnnotationsToOpenObject);
   ANNpositionWindowsInDocks(AnnotationsToOpenObject);
}

function ANNrestoreWindows(AnnotationsToOpenObject){
   for (let i of Object.keys(AnnotationsToOpenObject)) TRANSdialog(i); 
}

function ANNpositionWindowsInDocks(AnnotationsToOpenObject){
   for (let i of Object.keys(AnnotationsToOpenObject)) {
      if(ANNwindowAlreadyDocked(i)) continue;
      let DockId = AnnotationsToOpenObject[i];
      if( DockId !== "") WNDaddToDock(WNDannotationToWindowId(i), DockId);
   }
}

function ANNwindowAlreadyDocked(Id){
   return WNDisTab(Id);
}