function IMPupdateToFileName(){
   let ToFileName = document.querySelector('.IMPtoFilePath');
   if(!ToFileName) return;
   let ToName = IMPtoFile;
   if(ToName === "") ToName = "Select a VMD destination  →";
   if(ToFileName.innerHTML !== ToName) ToFileName.innerHTML = ToName;
}

function IMPtoFileHtml(NodeData, MatchingData){
   if(IMPtoFile === undefined || IMPtoFile.trim() === "") return IMPselectFileMessage();
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
         ${IMPmatchingListHtml(MatchingData)}
      </div>
   </div>`;
}

function IMPselectFileMessage(){ return /*html*/`<div class="IMPmessage">-- Select a VMD file --</div>`; }

function IMPmatchingListHtml(MatchingData){
   let Html = ``;
   for(Rule of MatchingData) Html += IMPlistMatchingRule(Rule); 
   return Html;
}

function IMPlistMatchingRule(Rule){
   let Classes = "";
   let Parameter = Rule.name;
   if(IMPtoFileData["matching"] && IMPcontainsRule(Rule, IMPtoFileData["matching"]) ){ Classes = "IMPdisabled"; }
   return /*html*/`<div class="IMPmatchingValue ${Classes} IMPmatchingRule" 
   name="${Parameter}" rule="${Rule.rule}">[${Rule.rule}] -> [${Parameter}]</div>`; 
}

function IMPcontainsRule(Rule, FileData){
   for(Item of FileData){ if(Item.name === Rule.name && Item.rule === Rule.rule) return true; }
   return false;
}