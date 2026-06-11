function CYTOrename(componentId, oldName, newName) {
   APIcall('component/name/set', {component: componentId, value: newName}, function(Response) {
      if(!Response.success) {
         if(Response.error.search(/permission to rename/)) {
            return SNCKsnackbar("error", `${Response.error  } <a href='/#logs?tags=%23error&filter=true'>View error in Logs</a>`, 10000, false);
         }  return SNCKsnackbar("error", `Problem saving the component name: ${  Response.error}`); 
      }
      const Message = GINsaveComponentName(oldName, newName);
      GINaddAndCommit({message: Message, component: CARDcomponentId()});
      SNCKsnackbar('success', 'Component name updated.', 6000);
      MODELforcePoll('DASHmodel');
   });
}

function CYTOsetTags(componentId, componentName, newTags) {
   APIcall('component/tags/set', {component: componentId, value: newTags}, function(Response) {
      if(!Response.success) { return SNCKsnackbar('error', `Problem saving component tags: ${  Response.error}`); }
      const Message = GINsetTags(`"${componentName || componentId}" component`, newTags);
      GINaddAndCommit({message: Message, component: CARDcomponentId()});
      SNCKsnackbar('success', 'Component tags updated.', 6000);
      MODELforcePoll('DASHmodel');
   });
}

function CYTOremoveConnection(targetId, sourceId) {
   APIcall('component/source/delete', {component: targetId, remove_source: sourceId}, function(Response) {
      if(!Response.success) { return SNCKsnackbar('error', `Could not delete connection: ${  Response.error}`); }
      MODELforcePoll('DASHmodel');
   });
}

function CYTOaddConnection(targetId, sourceId) {
   APIcall('component/source/add', {component: targetId, source: sourceId}, function(Response){
      if(!Response.success) {return SNCKsnackbar('error', `Could not add connection from ${sourceId}: ${  Response.error}`);}
      MODELforcePoll('DASHmodel'); // TODO if we force poll here we wont be able to see any errors when a list of guids is used
   });
}

const CYTOpinSaveMinY = 0;

function CYTObuildPinSavePayload(nodes, savedPins = DASHpinData() || {}) {
   const pins = {...savedPins};
   if(!nodes || nodes.length === 0) { return pins; }

   nodes.forEach(node => {
      delete pins[node.id()];
      const pos = node.position();
      const px  = Math.round(Number(pos.x));
      const py  = Math.round(Number(pos.y));
      if(!Number.isFinite(px) || !Number.isFinite(py) || py < CYTOpinSaveMinY) { return; }
      pins[node.id()] = {px, py};
   });
   return pins;
}

function CYTOsavePins(nodes, {onComplete} = {}) {
   const savedPins = DASHpinData() || {};
   const pins      = CYTObuildPinSavePayload(nodes, savedPins);
   if(DEEPequal(savedPins, pins)) {
      if(onComplete) onComplete();
      return;
   }
   APIcall("pins/save", {pins}, function(Response) {
      if(!Response.success) {
         SNCKsnackbar("error", `Failed to save pins: ${  Response.error || "Unknown error"}`);
         if(onComplete) onComplete();
         return;
      }
      if(!MODELdata.DASHpinData) { MODELdata.DASHpinData = {data: pins}; }
      else                       { MODELdata.DASHpinData.data = pins; }
      MODELupdate('DASHmodel', 'CYTOsavePins');
      if(onComplete) onComplete();
   });
}

function CYTOdelete(componentId) {
   if(!componentId) { return; }
   const componentList = [componentId];
   if(!DASHhasBulkPermission("delete_component", "Delete Component", componentList)) { return; }
   if(DASHcomponentListCanBeDeleted(componentList)) {
      CYTObulkActionDialog("Confirm Deletion", "DELETION", "delete", componentList, () => { DASHdeleteComponent(componentList); });
   } else {
      SNCKsnackbar("error", "Bulk delete failed: Stop components before deleting.");
   }
}
