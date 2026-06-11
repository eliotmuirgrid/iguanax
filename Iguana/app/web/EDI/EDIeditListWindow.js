function EDIeditListHtml(Data){
   let Dependencies = EDIcalculateDependencies(Data);
   let MessageCount      = EDImessageList(Data, "message").length;
   let SegmentGroupCount = EDImessageList(Data, "segment_group").length;
   let SegmentCount   = EDImessageList(Data, "segment").length;
   let CompositeCount = EDImessageList(Data, "composite").length;
   return /*html*/`
   <div class="EDIwindow">
      <div class="EDIeditBanner">
         <div class="EDImatchingRulesDiv">Node List</div>
         <div class="EDImatchingFile">(${FileName})</div>
         <div class="BUTTONgroup">
            <div class="EDIimport BUTTONaction BUTTONstandard">IMPORT</div>
            <div class="EDIdoneEditList BUTTONdefault BUTTONstandard">DONE</div>
         </div>
      </div>
      <div class="EDIaccordian">
         <div class="EDImessageFolder EDItrayOpen">
            <div class="EDIfolderNode"></div>
            <div class="EDIlistTitle">Message Definitions (${MessageCount})</div>
         </div>
         <div class="EDImessageFolderContent EDIcontent EDItrayOpen">
            ${EDIeditFolderListHtml(Data, "message", Dependencies)}
         </div>
         <div class="EDIsegmentgroupFolder EDItrayOpen">
            <div class="EDIfolderNode"></div>
            <div class="EDIlistTitle">Segment Groups (${SegmentGroupCount})</div>
         </div>
         <div class="EDIsegmentGroupFolderContent EDIcontent EDItrayOpen">
            ${EDIeditFolderListHtml(Data, "segment_group", Dependencies)}
         </div>
         <div class="EDIsegmentFolder EDItrayOpen">
            <div class="EDIfolderNode"></div>
            <div class="EDIlistTitle">Segments (${SegmentCount})</div>
         </div>
         <div class="EDIsegmentFolderContent EDIcontent EDItrayOpen">
            ${EDIeditFolderListHtml(Data, "segment", Dependencies)}
         </div>
         <div class="EDIcompositeFolder EDItrayOpen">
            <div class="EDIfolderNode"></div>
            <div class="EDIlistTitle">Composites (${CompositeCount})</div>
         </div>
         <div class="EDIcompositeFolderContent EDIcontent EDItrayOpen">
            ${EDIeditFolderListHtml(Data, "composite", Dependencies)}
         </div>
      </div>
   </div>`;
}

function EDIeditFolderListHtml(FileData, FolderType, Dependencies){
   let NodeList = EDImessageList(FileData, FolderType);
   return /*html*/`
   <div class="EDIlistRow"><div class="EDIaddListItem EDIadd${FolderType}"> + ADD ${FolderType.toUpperCase().replace("_", " ")}</div></div>
   ${EDIeditListNodeHtml(NodeList, FileData, Dependencies)}`;
}

function EDIaddNew(Name, Type, FileName){
   let FileData = EDIfileData();
   let NodeData = {};
   NodeData.desc = "";
   NodeData.type = Type;
   FileData[FileName]["nodes"][Name] = NodeData;
   let ApiData = {};
   ApiData.path = FileName;
   ApiData.component   = TRANScomponentGuid();
   ApiData.content     = JSON.stringify(FileData[FileName]);
   ApiData.development = TRANSdevelopment();
   APIcall('component/edi/set', ApiData, function(R){ 
      EDIopenNodeWindow(Name, FileName);
      MODELforcePoll('EDImodel'); 
   });
}

function EDIeditListNodeHtml(NodeList, FileData, Dependencies){
   let Html = ``;
   for(NodeName of NodeList) Html += EDIeditListNode(NodeName, FileData, Dependencies[NodeName]); 
   return Html;
}

function EDIeditListNode(Parameter, FileData, Dependencies){
   let removeButton = Dependencies && Object.keys(Dependencies).length ? "EDIcannotDeleteListItem" : "EDIdeleteListItem";
   return /*html*/`
   <div class="EDIlistRow">
      <div class="EDInodeValue ${EDIiconClass(FileData[Parameter].type)}" value="${ESChtmlEscape(Parameter)}">
      ${Parameter} ${FileData[Parameter].desc ? ` (${FileData[Parameter].desc})` : "" }</div>
      <div class="${removeButton}" title="Remove ${Parameter}">x</div>
   </div>`; 
}

function EDIremoveListItem(Name, FileName){
   console.log("remove: " + Name);
   let FileData = EDIfileData();
   delete FileData[FileName]["nodes"][Name];
   EDIremoveMatchingRules(Name, FileName);
   let ApiData = {};
   ApiData.path = FileName;
   ApiData.component   = TRANScomponentGuid();
   ApiData.content     = JSON.stringify(FileData[FileName]);
   ApiData.development = TRANSdevelopment();
   APIcall('component/edi/set', ApiData, function(R){ MODELforcePoll('EDImodel'); });
}

function EDIremoveMatchingRules(Name, FileName){
   let FileData = EDIfileData();
   for(index in FileData[FileName]["matching"]){
      if(FileData[FileName]["matching"][index].name === Name) {
         FileData[FileName]["matching"].splice(index, 1);
      }
   }
}