let EDImaxDepth = 5;

function EDInodeHtml(NodeName, FileName, Dependencies){
   let FileData = EDIfileData();
   if(!FileData[FileName]["nodes"][NodeName]) return;
   let EditButton = TRANSreadOnly() ? "" : `<div class="EDIeditNodeWindow BUTTONdefault BUTTONstandard">EDIT</div>`;
   let Children = 0;
   if(FileData[FileName]["nodes"][NodeName].children) Children = FileData[FileName]["nodes"][NodeName].children.length;
   return /*html*/`
   <div class="EDIwindow">
      <div class="EDIeditBanner">
         <div class="EDInodeName ${EDIiconClass( FileData[FileName]["nodes"][NodeName].type )}">${ESChtmlEscape(NodeName)}</div>
         <div class="EDInodeDescription">${ESChtmlEscape(FileData[FileName]["nodes"][NodeName].desc)}</div>
         ${EditButton}
      </div>   
      <br>
      <div class="EDInodeWindow">
         ${EDImatchingRulesFolderHtml(NodeName, FileName)}
         ${EDIchildrenHtml(NodeName, FileName, Children)}
         <br>
         ${ FileData[FileName]["nodes"][NodeName].type !== "message" ? EDIparentFolderHtml(FileName, Dependencies) : ""}
      </div>
   </div>`;
}

function EDIparentFolderHtml(FileName, Dependencies){
   let FileData = EDIfileData();
   return /*html*/`      
   <div class="EDIrowDiv EDIclosed">
      <div class="EDIsectionTitle">
         <div class="EDIfolderNode"></div>
         <div class="FORMlabel">Parents (${Dependencies.length} reference${Dependencies.length===1?"":"s"})</div>
      </div>
      <div class="EDIparentDiv">
         ${EDIparentHtml(FileData[FileName]["nodes"], Dependencies)}
      </div>
   </div>`;
}

function EDImatchingRules(MessageName, FileData){
   let MatchingRules = [];
   for(RuleIdx in FileData["matching"]){
      if(FileData["matching"][RuleIdx].name === MessageName) MatchingRules.push(RuleIdx); 
   }
   return MatchingRules;
}

function EDImatchingRuleRowHtml(RuleData, Idx){
   return /*html*/`
   <div class="EDImatchingValue">${parseInt(Idx,10)+1}</div>
   <div class="EDImatchingValue">${ESChtmlEscape(RuleData.rule)}</div>
   <div class="EDImatchingValueEnd" value="${ESChtmlEscape(RuleData.name)}">${ESChtmlEscape(RuleData.name)}</div>`;
}

function EDImatchingRulesFolderHtml(NodeName, FileName){
   let FileData = EDIfileData();
   if(FileData[FileName]["nodes"][NodeName].type !== "message") return "";
   let MatchingRules = EDImatchingRules(NodeName, FileData[FileName]);
   return /*html*/`
   <div class="EDIrowDiv EDIclosed">
      <div class="EDIsectionTitle">
         <div class="EDIfolderNode"></div>
         <div class="FORMlabel">${MatchingRules.length===0?"⚠":""} Matching Rules (${MatchingRules.length} reference${MatchingRules.length===1?"":"s"}) </div>
      </div>
      <div class="EDIparentDiv EDImatchingRulesNodeTable">
         ${EDImatchingRulesHtml(NodeName, FileName)}
      </div>
   </div>
   <br>`;
}

function EDImatchingRulesHtml(NodeName, FileName){
   let FileData = EDIfileData();
   if (!EDImatchingRuleExists(NodeName, FileData[FileName])) return "Currently, there are no matching rules set for this message";
   let Html = /*html*/`
   <div class="EDImatchingRulesTable">
      <div class="EDImatchingHeader FORMlabel">#</div>
      <div class="EDImatchingHeader FORMlabel">RULE</div>
      <div class="EDImatchingHeader FORMlabel">NAME</div>`;
   for(RuleIdx in FileData[FileName]["matching"]){
      if(FileData[FileName]["matching"][RuleIdx].name === NodeName) Html += EDImatchingRuleRowHtml(FileData[FileName]["matching"][RuleIdx], RuleIdx);
   }
   return Html + /*html*/`</div>`;
}

function EDIchildrenHtml(NodeName, FileName, Children){
   let FileData = EDIfileData();
   if(!FileData[FileName]["nodes"][NodeName].children && FileData[FileName]["nodes"][NodeName].type === "composite") return 'This is a leaf node';
   return /*html*/`
   <div class="EDIrowDiv ${Children === 0 ? "EDIclosed" : "EDIopen"}">
      <div class="EDIsectionTitle">
         <div class="EDIfolderNode"></div>
         <div class="FORMlabel">${Children===0?"⚠":""} Children (${Children} reference${Children===1?"":"s"})</div>
      </div>
      ${EDIchildHtml(NodeName, FileName, Children)}
   </div>`;
}

function EDIchildHtml(NodeName, FileName, Children){
   let depth = 0;
   let FileData = EDIfileData();
   let ParentType = FileData[FileName]["nodes"][NodeName].type;
   if(Children === 0){
      return /*html*/`
      <div class="EDIparentDiv">
         Currently, there are no children set
      </div>`;
   }
   return /*html*/`
   <div class="EDIchildrenDiv ${FileData[FileName]["nodes"][NodeName].type === "composite" ? "EDIcomposite" : ""}">
      ${EDIchildHeadersHtml(ParentType)}
      ${EDIchildRowsHtml(NodeName, FileName, ParentType, depth)}
   </div>`;

}

function EDIparentHtml(FileData, Dependencies){
   if(!Dependencies.length ) return 'This node has no parents';
   return /*html*/`
   ${EDIparentRowsHtml(FileData, Dependencies)}`;
}

function EDIchildRowsHtml(NodeName, FileName, ParentType, depth, Index){
   let FileData = EDIfileData();
   let ChildData = FileData[FileName]["nodes"][NodeName];
   let Html = '';
   let NodeType = FileData[FileName]["nodes"][NodeName].type
   if(Index!== 0) Index = (NodeType === "message" || NodeType === "segment_group") ? 0 : 1;
   if(!ChildData.children) return Html;
   for(let ChildNodeData of ChildData.children){
      Html += EDIchildRowHtml(ChildNodeData, FileName, ParentType, depth, Index); 
      if(Index) Index++;
   }
   return Html;
}

function EDIparentRowsHtml(FileData, Dependencies){
   let Html = /*html*/`
   <div class="EDIparentTableDiv">
      <div class="EDImatchingHeader FORMlabel">TYPE</div>
      <div class="EDImatchingHeader FORMlabel">DESCRIPTION</div>`;
   for(NodeName of Dependencies){ Html += EDIparentRowHtml(FileData[NodeName], NodeName); }
   return Html + /*html*/`</div><br>`;
}

function EDIparentRowHtml(NodeData, NodeName){
   let TypeIcon = EDItypeIcon(NodeData.type);
   return /*html*/ `
   <div class="EDImatchingValue EDInodeValue ${TypeIcon} EDIparentType" value="${ESChtmlEscape(NodeName)}">${NodeName}</div>
   <div class="EDImatchingValueEnd">${NodeData.desc}</div>`;
}

function EDIchildRowHtml(ChildNodeData, FileName, ParentType, Depth, Index){
   let NodeName = ChildNodeData.type;
   let FileData = EDIfileData();
   let NodeType = FileData[FileName]["nodes"][NodeName].type;
   let IndexStr = Index !== 0 ? Index : "";
   let LeafNode = !FileData[FileName]["nodes"][NodeName].children || FileData[FileName]["nodes"][NodeName].children.length === 0 ;
   let DepthArrow = "&nbsp;|".repeat(Depth) + IndexStr;
   return /*html*/ `
   <div class="EDInodeRowDiv EDIclosed">
      <div class="EDInameNode EDImatchingValue">
         <div>${DepthArrow}</div>
         <div class="EDIfolderIcon ${LeafNode ? "EDIleafNode" : "EDIfolderNode"}"></div>
         <div class="EDInodeType ${EDItypeIcon(NodeType)} ${ LeafNode ? "EDInodeValue" : ""}" value="${ESChtmlEscape(NodeName)}">${ESChtmlEscape(NodeName)}</div>
         ${ LeafNode ? "" : EDIopenOpenOutArrow(NodeName)}
      </div>
      <div class="EDIchildDescription EDImatchingValue">${ESChtmlEscape(ChildNodeData.desc)}</div>
      <div class="EDImatchingValue${ParentType !== "composite" ? "" : "End"} EDIboolValue"><input type="checkbox" disabled ${ChildNodeData.req ? "checked" : ""}></div>
      ${ParentType !== "composite" ? `<div class="EDIboolValue EDImatchingValueEnd"><input type="checkbox" disabled ${ChildNodeData.repeats ? "checked" : ""}></div>` : ""}
</div>`;
}
// ${  LeafNode ? "" : EDIchildRowsHtml(NodeName,FileName,ParentType, Depth+1, Index) }

function EDIopenOpenOutArrow(NodeName){
   return /*html*/`
   <div class="EDInodeValue EDIopenNode" value="${ESChtmlEscape(NodeName)}">			
      <svg width="16px" height="16px"><use href="EDI/images/EDIopenNewNodeWindow.svg#EDIopenNewNodeWindow" class="EDIopenNode"></use></svg>
   </div>`;
}

function EDItypeIcon(Type){
   if(Type === "segment_group") return `EDIsegmentDefinitions`;
   if(Type) return `EDI${Type}Def`;
   return "";
}

function EDIchildHeadersHtml(NodeType){
   return /*html*/`
   <div class="EDImatchingHeader FORMlabel">TYPE</div>
   <div class="EDImatchingHeader FORMlabel">DESCRIPTION</div>
   <div class="EDImatchingHeader EDIboolValue FORMlabel" title="Required">REQ</div>
   ${NodeType !== "composite" ? "<div class='EDImatchingHeader EDIboolValue FORMlabel' title='Repeats'>REP</div>" : ""}`;
}