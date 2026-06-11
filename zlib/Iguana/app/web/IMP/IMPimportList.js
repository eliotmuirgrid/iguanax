function IMPimportListHtml(Data, MatchingData){
   let ErrorMessage = "";
   if(Object.keys(IMPfromFileData).length === 0) ErrorMessage = "Select a VMD source";
   if(Object.keys(IMPtoFileData).length === 0)   ErrorMessage = "Select a VMD destination";
   if((Object.keys(IMPtoFileData).length === 0) && (Object.keys(IMPfromFileData).length === 0)) ErrorMessage = "Select a VMD source and destination";
   if(ErrorMessage !== "") return IMPselectFileErrorMessage(ErrorMessage);
   if(Data === undefined) return "";
   let Dependencies = IMPcalculateDependencies(Data);
   let MessageCount      = IMPimportMessageList(Data, "message").length;
   let SegmentGroupCount = IMPimportMessageList(Data, "segment_group").length;
   let SegmentCount      = IMPimportMessageList(Data, "segment").length;
   let CompositeCount    = IMPimportMessageList(Data, "composite").length;
   return /*html*/`
   <div class="IMPaccordian">
      <div class="IMPmessageFolder IMPtrayOpen">
         <div class="IMPfolderNode"></div>
         <div class="IMPlistTitle">Message Definitions (${MessageCount})</div>
      </div>
      <div class="IMPmessageFolderContent IMPcontent IMPtrayOpen">
         ${IMPeditFolderListHtml(Data, "message", Dependencies)}
      </div>
      <div class="IMPsegmentgroupFolder IMPtrayOpen">
         <div class="IMPfolderNode"></div>
         <div class="IMPlistTitle">Segment Groups (${SegmentGroupCount})</div>
      </div>
      <div class="IMPsegmentGroupFolderContent IMPcontent IMPtrayOpen">
         ${IMPeditFolderListHtml(Data, "segment_group", Dependencies)}
      </div>
      <div class="IMPsegmentFolder IMPtrayOpen">
         <div class="IMPfolderNode"></div>
         <div class="IMPlistTitle">Segments (${SegmentCount})</div>
      </div>
      <div class="IMPsegmentFolderContent IMPcontent IMPtrayOpen">
         ${IMPeditFolderListHtml(Data, "segment", Dependencies)}
      </div>
      <div class="IMPcompositeFolder IMPtrayOpen">
         <div class="IMPfolderNode"></div>
         <div class="IMPlistTitle">Composites (${CompositeCount})</div>
      </div>
      <div class="IMPcompositeFolderContent IMPcontent IMPtrayOpen">
         ${IMPeditFolderListHtml(Data, "composite", Dependencies)}
      </div>
      <div class="IMPmatchingFolder IMPtrayOpen">
         <div class="IMPfolderNode"></div>
         <div class="IMPlistTitle">Matching Rules (${MatchingData.length})</div>
      </div>
      <div class="IMPmatchingFolderContent IMPcontent IMPtrayOpen">
         ${IMPeditMatchingListHtml(MatchingData)}
      </div>
   </div>`;
}

function IMPselectFileErrorMessage(ErrorMessage){ 
   return /*html*/`<div class="IMPmessage">-- ${ErrorMessage} --</div>`; 
}

function IMPeditFolderListHtml(FileData, FolderType, Dependencies){
   let NodeList = IMPimportMessageList(FileData, FolderType);
   let Html = ``;
   for(NodeName of NodeList) Html += IMPeditListNode(NodeName, FileData, Dependencies[NodeName]); 
   return Html;
}

function IMPeditMatchingListHtml(MatchingData){
   let Html = ``;
   for(Rule of MatchingData) Html += IMPeditListMatchingRule(Rule); 
   return Html;
}

function IMPeditListNode(Parameter, FileData, Dependencies){
   let removeButton = Dependencies && Object.keys(Dependencies).length ? "IMPcannotDeleteListItem" : "IMPdeleteListItem";
   return /*html*/`
   <div class="IMPlistRow">
      <div class="IMPnodeValue ${IMPiconClass(FileData[Parameter].type)}" value="${Parameter}">
      ${Parameter} ${FileData[Parameter].desc ? ` (${FileData[Parameter].desc})` : "" }</div>
      <div class="${removeButton}" title="Remove ${Parameter}">x</div>
   </div>`; 
}

function IMPimportMessageList(FileData, Type){
   let TypeList = [];
   for(Item in FileData) {
      if(!IMPimportNodeList[Item]) continue;
      if(FileData[Item].type === Type ) TypeList.push(Item);
   }
   return TypeList;
}

function IMPcalculateDependencies(Data){
   let DependencyData = {};
   for(NodeName in Data) {
      if(!IMPimportNodeList[NodeName]) continue;
      DependencyData[NodeName] = IMPparents(NodeName, Data); 
   }
   return DependencyData;
}

function IMPparents(DeletedName, Data){
   let NodeNames = {};
   for(NodeName in Data){
      if(!IMPimportNodeList[NodeName]) continue;
      if(!Data[NodeName].children) continue;
      for(let i=0; i < Data[NodeName].children.length; i++) if(Data[NodeName].children[i].type === DeletedName) NodeNames[NodeName] = true; 
   }
   return NodeNames;
}

function IMPcalculateChildren(NodeName){
   let ChildData = IMPfromFileData["nodes"][NodeName];
   if(!ChildData?.children) return;
   for(ChildNodeData of ChildData.children){ 
      if(IMPtoFileData["nodes"][ChildNodeData.type]) continue;
      IMPimportNodeList[ChildNodeData.type] = true;
      IMPcalculateChildren(ChildNodeData.type);
   }
}

function IMPassociatedRules(MessageName){
   let Rules = [];
   for(Rule of IMPfromFileData["matching"]){ if(Rule.name === MessageName) Rules.push(Rule); }
   return Rules;
}

function IMPaddMatchingRules(NewRules){
   for(Rule of NewRules){ 
      if(IMPcontainsRule(Rule, IMPtoFileData["matching"]) || IMPcontainsRule(Rule, IMPimportMatchingList)) continue;
      IMPimportMatchingList.push(Rule);
   }
}

function IMPeditListMatchingRule(Rule){
   let Classes = "";
   let Parameter = Rule.name;
   if(IMPtoFileData["matching"] && IMPcontainsRule(Rule, IMPtoFileData["matching"]) ){ Classes = "IMPdisabled"; }
   return /*html*/`
   <div class="IMPlistRow" name="${Parameter}" rule="${Rule.rule}">
      <div class="IMPmatchingValue IMPmatchingRule">[${Rule.rule}] -> [${Parameter}]</div>
      <div class="IMPdeleteMatchingItem" title="Remove ${Parameter}">x</div>
   </div>`; 
}

function IMPremoveMatchingRulesImporting(Rule){
   for(RuleIdx in IMPimportMatchingList){ 
      if(IMPimportMatchingList[RuleIdx].name === Rule.name && IMPimportMatchingList[RuleIdx].rule === Rule.rule) {
         return IMPimportMatchingList.splice(RuleIdx, 1);
      }
   }
}