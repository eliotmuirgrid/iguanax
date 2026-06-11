function IMPupdateFromFileName(){
   let FromFileName = document.querySelector('.IMPfromFilePath');
   if(!FromFileName) return;
   let FromName = IMPfromFile;
   if(FromName === "" && IMPfromLibrary != "") FromName = IMPfromLibrary + " library";
   if(FromName === "") FromName = "Select a VMD source  →";
   if(FromFileName.innerHTML !== FromName) FromFileName.innerHTML = FromName;
}

function IMPfromFileHtml(NodeData, MatchingData){
   if(Object.keys(IMPfromFileData).length === 0) return IMPselectFileMessage();
   let MessageCount      = IMPmessageList(NodeData, "message"      ).length;
   let SegmentGroupCount = IMPmessageList(NodeData, "segment_group").length;
   let SegmentCount      = IMPmessageList(NodeData, "segment",     ).length;
   let CompositeCount    = IMPmessageList(NodeData, "composite",   ).length;
   return /*html*/`
   <div class="IMPaccordian">
      <div class="IMPmessageFolder IMPtrayOpen">
            <div class="IMPfolderNode"></div>
            <div class="IMPlistTitle">Message Definitions (${MessageCount})</div>  
      </div>
      <div class="IMPmessageFolderContent IMPcontent IMPtrayOpen">
         ${IMPfolderListHtml(NodeData, "message")}
      </div>
      <div class="IMPsegmentgroupFolder IMPtrayOpen">
         <div class="IMPfolderNode"></div>
         <div class="IMPlistTitle">Segment Groups (${SegmentGroupCount})</div>
      </div>
      <div class="IMPsegmentGroupFolderContent IMPcontent IMPtrayOpen">
         ${IMPfolderListHtml(NodeData, "segment_group")}
      </div>
      <div class="IMPsegmentFolder IMPtrayOpen">
         <div class="IMPfolderNode"></div>
         <div class="IMPlistTitle">Segments (${SegmentCount})</div>
      </div>
      <div class="IMPsegmentFolderContent IMPcontent IMPtrayOpen">
         ${IMPfolderListHtml(NodeData, "segment")}
      </div>
      <div class="IMPcompositeFolder IMPtrayOpen">
         <div class="IMPfolderNode"></div>
         <div class="IMPlistTitle">Composites (${CompositeCount})</div>
      </div>
      <div class="IMPcompositeFolderContent IMPcontent IMPtrayOpen">
         ${IMPfolderListHtml(NodeData, "composite")}
      </div>
      <div class="IMPmatchingFolder IMPtrayOpen">
         <div class="IMPfolderNode"></div>
         <div class="IMPlistTitle">Matching Rules (${MatchingData.length})</div>
      </div>
      <div class="IMPmatchingFolderContent IMPcontent IMPtrayOpen">
         ${IMPmatchingListHtml(MatchingData, NodeData)}
      </div>
   </div>`;
}

function IMPmessageList(NodeData, Type){
   let TypeList = [];
   for(Item in NodeData) { if(NodeData[Item].type === Type ) TypeList.push(Item); }
   return TypeList;
}

function IMPfolderListHtml(NodeData, FolderType){
   let NodeList = IMPmessageList(NodeData, FolderType);
   return /*html*/`${IMPlistNodeHtml(NodeList, NodeData)}`;
}

function IMPlistNodeHtml(NodeList, NodeData){
   let Html = ``;
   for(NodeName of NodeList) Html += IMPlistNode(NodeName, NodeData); 
   return Html;
}

function IMPlistNode(Parameter, NodeData){  
   let Classes = "";
   let Padding = "";
   if(Parameter.length === 1) Padding= "&nbsp;&nbsp;";
   if(Parameter.length === 2) Padding= "&nbsp;";
   if(IMPtoFileData["nodes"] && IMPtoFileData["nodes"][Parameter]) Classes = "IMPdisabled";
   return /*html*/`
   <div class="IMPnodeValue ${Classes} ${IMPiconClass(NodeData[Parameter].type)}" value="${Parameter}" title="${Parameter + " " + "(" + ESChtmlEscape(NodeData[Parameter].desc) + ")"}">
   ${ESChtmlEscape(Parameter)}${Padding} ${NodeData[Parameter].desc ? ` (${ESChtmlEscape(NodeData[Parameter].desc)})` : "" }</div>`; 
}

function IMPiconClass(Type){
   if(Type === "segment_group") return `IMPsegmentDefinitions`;
   if(Type) return `IMP${Type}Def`;
   return "";
}