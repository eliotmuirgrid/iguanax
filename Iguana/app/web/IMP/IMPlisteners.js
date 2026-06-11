function IMPsetAllListeners() {
   const Modal = document.querySelector('.IMPmodal');
   if(!Modal) { return; }
   Modal.addEventListener('click', IMPlisteners);
   Modal.addEventListener('keydown', IMPkeyDownListeners);
   Modal.addEventListener('change', IMPchangeListeners)
}

function IMPlisteners(e){
   if(e.target.closest('.IMPfromLibrary div')){ IMPsetFromLibrary(e.target); }
   if(e.target.closest('.IMPfromFile div'))   { IMPsetFromFile(); }
   if(e.target.closest('.IMPtoFile div'))     { IMPsetToFile();   }
   if(e.target.closest('.IMPmatchingValue')){
      let MatchingName = e.target.closest(".IMPmatchingValue").getAttribute('name');
      let MatchingRule = e.target.closest(".IMPmatchingValue").getAttribute('rule');
      let Rule = {"name" : MatchingName, "rule" : MatchingRule};
      if(IMPcontainsRule(Rule, IMPtoFileData["matching"]) || IMPcontainsRule(Rule, IMPimportMatchingList)) return;
      IMPimportMatchingList.push(Rule);
      MODELforcePoll('IMPmodel');
   }
   if(e.target.closest('.IMPdeleteMatchingItem')){
      let MatchingName = e.target.closest(".IMPlistRow").getAttribute('name');
      let MatchingRule = e.target.closest(".IMPlistRow").getAttribute('rule');
      let Rule = {"name" : MatchingName, "rule" : MatchingRule};
      IMPremoveMatchingRulesImporting(Rule);
      MODELforcePoll('IMPmodel');
   }
   if(e.target.closest('.IMPnodeValue')){
      let NodeName = e.target.closest(".IMPnodeValue").getAttribute('value');
      if(!IMPtoFileData["nodes"]) return;
      if(IMPtoFileData["nodes"][NodeName]) return;
      IMPimportNodeList[NodeName] = true;
      IMPcalculateChildren(NodeName);
      if(IMPfromFileData["nodes"][NodeName].type === "message") {
         let Rules = IMPassociatedRules(NodeName);
         IMPaddMatchingRules(Rules);
      }
      MODELforcePoll('IMPmodel');
   }
   if(e.target.closest('.IMPimportCancel')) POPpopoutClose(VMDimportWindow);
   if(e.target.closest('.IMPimportButton')){ IMPimportSave(); }
   if(e.target.closest('.IMPdeleteListItem')){
      let NodeName = e.target.closest('.IMPlistRow').querySelector(".IMPnodeValue").getAttribute('value').trim();
      delete IMPimportNodeList[NodeName];
      MODELforcePoll('IMPmodel');
   }
   if(e.target.closest(".IMPmessageFolder"))  { 
      IMPtoggleTray(e.target.closest(".IMPmessageFolder")); 
      IMPtoggleTray(e.target.closest('.IMPaccordian').querySelector('.IMPmessageFolderContent')); 
      e.target.closest('.IMPaccordian').querySelector('.IMPmessageFolderContent').scrollIntoView();
   }
   if(e.target.closest(".IMPsegmentgroupFolder"))  { 
      IMPtoggleTray(e.target.closest(".IMPsegmentgroupFolder")); 
      IMPtoggleTray(e.target.closest('.IMPaccordian').querySelector('.IMPsegmentGroupFolderContent')); 
      e.target.closest('.IMPaccordian').querySelector('.IMPsegmentGroupFolderContent').scrollIntoView();
   }
   if(e.target.closest(".IMPsegmentFolder"))  { 
      IMPtoggleTray(e.target.closest(".IMPsegmentFolder")); 
      IMPtoggleTray(e.target.closest('.IMPaccordian').querySelector('.IMPsegmentFolderContent')); 
      e.target.closest('.IMPaccordian').querySelector('.IMPsegmentFolderContent').scrollIntoView();
   }
   if(e.target.closest(".IMPcompositeFolder")){ 
      IMPtoggleTray(e.target.closest(".IMPcompositeFolder")); 
      IMPtoggleTray(e.target.closest('.IMPaccordian').querySelector('.IMPcompositeFolderContent')); 
      e.target.closest('.IMPaccordian').querySelector('.IMPcompositeFolderContent').scrollIntoView();
   }
   if(e.target.closest(".IMPmatchingFolder")){ 
      IMPtoggleTray(e.target.closest(".IMPmatchingFolder")); 
      IMPtoggleTray(e.target.closest('.IMPaccordian').querySelector('.IMPmatchingFolderContent')); 
      e.target.closest('.IMPaccordian').querySelector('.IMPmatchingFolderContent').scrollIntoView();
   }
}

function IMPtoggleTray(target){
   if(target.classList.contains("IMPtrayOpen")){
      target.classList.add("IMPtrayClosed");
      target.classList.remove("IMPtrayOpen");
   }
   else if(target.classList.contains("IMPtrayClosed")){
      target.classList.add("IMPtrayOpen");
      target.classList.remove("IMPtrayClosed");
   }
}

function IMPchangeListeners(e){
   if(e.target.closest('.IMPlibrarySelect')){ 
      IMPfromFile = "";
      MODELforcePoll('IMPmodel'); 
   }
}

function IMPkeyDownListeners(e){
   if(e.target.closest('.IMPfromFileSearch')){ MODELforcePoll('IMPmodel'); }
   if(e.target.closest('.IMPtoFileSearch'))  { MODELforcePoll('IMPmodel'); }
}

function IMPcleanPath(Path){ return Path.trim().endsWith(".vmd") ? Path.replace(TRANSprojectRoot(), "") : ""; }

function IMPsetFromFile(){
   FILbrowser({
      new_folder   : true,
      show_hidden  : true,
      dir_only     : false,
      value        : TRANSprojectRoot(),
      file_types   : ["vmd"],
      callback     : function(Path) { 
         IMPfromFile = IMPcleanPath(Path);
         IMPfromLibrary = "";
         MODELforcePoll('IMPmodel');
      }
   });
}

function IMPsetToFile(){
   FILbrowser({
      new_folder   : true,
      show_hidden  : true,
      dir_only     : false,
      file_types   : ["vmd"],
      value        : TRANSprojectRoot(),
      callback     : function(Path) { 
         IMPtoFile = IMPcleanPath(Path);
         MODELforcePoll('IMPmodel');
      }
   });
}