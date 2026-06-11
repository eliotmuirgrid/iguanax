function EDIchildrenOptions(NodeName, FileName, SelectedName){
   let FileData = EDIfileData();
   let NodeType = FileData[FileName]["nodes"][NodeName].type;
   let ValidChilds = EDIdetermineValidChildren(NodeName, FileName);
   let SegmentGroups = EDIchildListByType(ValidChilds, FileName, "segment_group");
   let Segments      = EDIchildListByType(ValidChilds, FileName, "segment");
   let Composites    = EDIchildListByType(ValidChilds, FileName, "composite");
   let Html = /*html*/`<select class="FORMselectBox EDInameEdit" value="${ESChtmlEscape(SelectedName)}">`;
   if(NodeType === "message" || NodeType === "segment_group"){
      Html += EDIsegmentGroupHeader(SelectedName) + EDIchildOptionsHtml(SegmentGroups, SelectedName);
      Html += EDIsegmentHeader     (SelectedName) + EDIchildOptionsHtml(Segments,      SelectedName);
   }
   if(NodeType === "segment" || NodeType === "composite"){
      Html += EDIcompositeHeader(SelectedName) + EDIchildOptionsHtml(Composites, SelectedName);
   }
   Html += `</select>`;
   return Html;
}

function EDIdetermineValidChildren(NodeName, FileName){
   let FileData = EDIfileData();
   let NodeType = FileData[FileName]["nodes"][NodeName].type;
   let Dependencies = EDIcalculateDependencies(FileData[FileName]["nodes"]);
   let NodeData = FileData[FileName]["nodes"];
   let NodeMap = EDIcreateNodeMap(NodeData);
   EDIfilterOutType(NodeMap, "message", NodeData);
   if(NodeType === "message")       EDIfilterOutType(NodeMap, "composite",     NodeData);
   if(NodeType === "segment_group") EDIfilterOutType(NodeMap, "composite",     NodeData);
   if(NodeType === "segment")       EDIfilterOutType(NodeMap, "segment_group", NodeData);
   if(NodeType === "composite")     EDIfilterOutType(NodeMap, "segment_group", NodeData);
   if(NodeType === "segment")       EDIfilterOutType(NodeMap, "segment",       NodeData);
   if(NodeType === "composite")     EDIfilterOutType(NodeMap, "segment",       NodeData);
   EDIremoveParents(NodeName, Dependencies, NodeMap);
   delete NodeMap[NodeName];
   return NodeMap;
}

function EDIcreateNodeMap(FileData){
   let NodeMap = {};
   for(NodeName in FileData){ NodeMap[NodeName] = true; }
   return NodeMap;
}

function EDIfilterOutType(NodeMap, Type, FileData){
   for(NodeName in NodeMap){ if(FileData[NodeName].type === Type) delete NodeMap[NodeName]; }
}

function EDIremoveParents(NodeName, Dependencies, NodeMap){
   for(Parent in Dependencies[NodeName]){ 
      delete NodeMap[Parent];
      EDIremoveParents(Parent, Dependencies, NodeMap);
   }
}

function EDIchildListByType(NodeMap, FileName, Type){
   let FileData = EDIfileData();
   let ChildNodes = [];
   for(NodeName in NodeMap){ if(FileData[FileName]["nodes"][NodeName].type === Type) ChildNodes.push(NodeName); }
   return ChildNodes;
}

function EDIchildOptionsHtml(NodeArray, SelectedName){
   let Html;
   for(NodeName of NodeArray) { Html += `<option value="${ESChtmlEscape(NodeName)}" ${NodeName === SelectedName ? " selected" : ""}>${NodeName}</option>`; }
   return Html;
}

function EDIsegmentGroupHeader(SelectedName){ return /*html*/`
   <option value="" ${!SelectedName ? " selected " : ""} disabled>-- Select Segment Group --</option>
   <option value="{}[newsegmentgroup321">+ New Segment Group +</option>`;
}

function EDIsegmentHeader(SelectedName){ return /*html*/`
   <option value="" ${!SelectedName ? " selected " : ""} disabled>-- Select Segment --</option>
   <option value="{}[newsegment321">+ New Segment +</option>`;
}

function EDIcompositeHeader(SelectedName){ return /*html*/`
   <option value="" ${!SelectedName ? " selected " : ""} disabled>-- Select Composite --</option>
   <option value="{}[newcomposite321">+ New Composite +</option>`;
}