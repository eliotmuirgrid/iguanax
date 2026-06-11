function EDIlistHtml(FileData, FileName){
   let MessageCount        = EDImessageList(FileData["nodes"], "message")      .length;
   let SegmentGroupCount   = EDImessageList(FileData["nodes"], "segment_group").length;
   let SegmentCount        = EDImessageList(FileData["nodes"], "segment")      .length;
   let CompositeCount      = EDImessageList(FileData["nodes"], "composite")    .length;
   let EditButton          = TRANSreadOnly() ? "" : `<div class="EDIeditListWindow BUTTONdefault BUTTONstandard">EDIT</div>`;
   return /*html*/`
   <div class="EDIwindow">
      <div class="EDIeditBanner">
         <div class="EDInodeName">Node List</div>
         <div class="EDImatchingFile">(${FileName})</div>
         ${EditButton}
      </div>   
      <div class="EDIaccordian">
         <div class="EDImessageFolder EDItrayOpen">
         <div class=EDIheaderTitle>
               <div class="EDIfolderNode"></div>
               <div class="EDItitle">Message Definitions (${MessageCount})</div>  
               <div class="EDImatchOrderDiv">[ <div class="EDImatchingOrderLink">Matching Rules (${FileData["matching"] ? FileData["matching"].length : ""})</div>  ]</div>
            </div>
         </div>
         <div class="EDImessageFolderContent EDIcontent EDItrayOpen">
            ${EDImessageListHtml(FileData)}
         </div>
         <div class="EDIsegmentgroupFolder EDItrayOpen">
            <div class="EDIfolderNode"></div>
            <div class="EDIlistTitle">Segment Groups (${SegmentGroupCount})</div>
         </div>
         <div class="EDIsegmentGroupFolderContent EDIcontent EDItrayOpen">
            ${EDIfolderListHtml(FileData["nodes"], "segment_group")}
         </div>
         <div class="EDIsegmentFolder EDItrayOpen">
            <div class="EDIfolderNode"></div>
            <div class="EDIlistTitle">Segments (${SegmentCount})</div>
         </div>
         <div class="EDIsegmentFolderContent EDIcontent EDItrayOpen">
            ${EDIfolderListHtml(FileData["nodes"], "segment")}
         </div>
         <div class="EDIcompositeFolder EDItrayOpen">
            <div class="EDIfolderNode"></div>
            <div class="EDIlistTitle">Composites (${CompositeCount})</div>
         </div>
         <div class="EDIcompositeFolderContent EDIcontent EDItrayOpen">
            ${EDIfolderListHtml(FileData["nodes"], "composite")}
         </div>
      </div>
   </div>`;
}

function EDImessageListHtml(FileData){
   let NodeList = EDImessageList(FileData["nodes"], "message");
   return /*html*/`${EDImessageListNodeHtml(NodeList, FileData)}`;
}

function EDImessageListNodeHtml(NodeList, FileData){
   let Html = ``;
   for(NodeName of NodeList) Html += EDImessageNode(NodeName, FileData); 
   return Html;
}

function EDImessageNode(NodeName, FileData){  
   return /*html*/`
   <div class="EDIlistRow">
      <div class="EDIlistMessage EDInodeValue ${EDIiconClass(FileData["nodes"][NodeName].type)}" value="${ESChtmlEscape(NodeName)}">
         ${NodeName} ${FileData["nodes"][NodeName].desc ? ` (${FileData["nodes"][NodeName].desc})` : "" }
      </div>
      ${!EDImatchingRuleExists(NodeName, FileData) ? /*html*/`<div class="EDImessageWarning" title="No matching rule"><img src="/TREE/images/EDIwarning.svg" alt="⚠"></div>` : ""}
   </div>
   `;
}

function EDImatchingRuleExists(MessageName, FileData){
   for(MatchingRule of FileData["matching"]){ if(MatchingRule.name === MessageName) return true; }
   return false;
}

function EDIfolderListHtml(FileData, FolderType){
   let NodeList = EDImessageList(FileData, FolderType);
   return /*html*/`${EDIlistNodeHtml(NodeList, FileData)}`;
}

function EDIlistNodeHtml(NodeList, FileData){
   let Html = ``;
   for(NodeName of NodeList) Html += EDIlistNode(NodeName, FileData); 
   return Html;
}

function EDIlistNode(Parameter, FileData){  
   let Padding = "";
   if(Parameter.length === 1) Padding= "&nbsp;&nbsp;";
   if(Parameter.length === 2) Padding= "&nbsp;";
   return /*html*/`
   <div class="EDInodeValue ${EDIiconClass(FileData[Parameter].type)}" value="${ESChtmlEscape(Parameter)}">
   ${ESChtmlEscape(Parameter)}${Padding} ${FileData[Parameter].desc ? ` (${ESChtmlEscape(FileData[Parameter].desc)})` : "" }</div>`; 
}

function EDImessageList(FileData, Type){
   let TypeList = [];
   for(Item in FileData) if(FileData[Item].type === Type) TypeList.push(Item);
   return TypeList;
}

