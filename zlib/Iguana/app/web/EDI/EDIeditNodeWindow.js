function EDIeditNodeHtml(NodeName, FileName){
   EDIdetermineValidChildren(NodeName, FileName);
   let FileData = EDIfileData();
   return /*html*/`
   <div class="EDIwindow">
      <div class="EDIeditBanner">
         <input class="FORMinput EDInameMainEdit EDInodeName" type="text" placeholder="Name" value="${ESChtmlEscape(NodeName)}">
         <input class="FORMinput EDIdescriptionMainEdit EDInodeDescription" type="text" placeholder="Description" value="${ESChtmlEscape(FileData[FileName]["nodes"][NodeName].desc)}">
         <div class="EDIsaveEditGroup BUTTONgroup EDIbuttonGroup">
            <div class="EDIsaveEdit BUTTONaction BUTTONstandard">SAVE</div>
            <div class="EDIcancelEdit BUTTONcancel BUTTONstandard">CANCEL</div>
         </div>
      </div>
      <div class="EDIwindowInner">
         <div class="EDInameMainEditHint EDIhidden"></div>
         <div class="EDIeditChildrenDiv ${FileData[FileName]["nodes"][NodeName].type === "composite" ? "EDIcomposite" : ""}">
            <div class="EDIchildRow EDIchildHeaders"><div>
               </div>${EDIeditChildHeadersHtml(FileData[FileName]["nodes"][NodeName].type)}</div>
            </div>
            <div class="EDIaddEditRow">
               <div class="EDIaddChild">+ ADD CHILD</div>
            </div>
         </div>
      </div>
   </div>`;
}

function EDIeditChildrenHtml(NodeName, FileName,WindowId){
   let FileData = EDIfileData();
   let Data = FileData[FileName]["nodes"][NodeName].children;
   let anchor = `#${WindowId} .EDIeditChildrenDiv`;
   if(!document.querySelector(anchor)) anchor = `#content-_${WindowId}docked .EDIeditChildrenDiv`;
   let Args = {};
   Args.anchor = anchor;
   Args.callback = EDIrowNumbers;
   EDIeditList[WindowId] = DRGcreateDragSortList(Args);
   if(!Data || Data.length === 0) {
      EDIaddBlankRow(FileName, NodeName, WindowId);
      EDIsetupTabLoop(FileName, NodeName, WindowId);
      return;
   }
   for(RowData of Data){ DRGdragSortListAddItem(EDIeditList[WindowId], EDIeditChildHtml(RowData, FileName, NodeName), {'dragHandleSelector': '.EDImoveRow','prepend': false}); }
   EDIsetupTabLoop(FileName, NodeName, WindowId);
}

function EDIaddBlankRow(FileName, NodeName, WindowId){
   let Content = EDIeditChildHtml({"type" : "", "desc" : "", "req" : "", "repeats" : ""}, FileName, NodeName);
   DRGdragSortListAddItem(EDIeditList[WindowId], Content, {'dragHandleSelector': '.EDImoveRow','prepend': false});
   let LastRow = Array.from( WNDcontentSelector(WindowId).querySelectorAll('.EDIchildRow') ).pop();
   LastRow.scrollIntoView();
}

function EDIeditChildHtml(RowData, FileName, NodeName){
   let FileData = EDIfileData();
   let NodeType = FileData[FileName]["nodes"][NodeName].type;
   let Required =  RowData?.req ?  "checked" : "";
   let Repeats = RowData?.repeats ? "checked" : "";
   return /*html*/`
   <div class="EDIchildRow">
      <div class="EDIeditCell EDImoveRow">⋮⋮</div>
      <div class="EDIeditCell EDInumber"></div>
      <div class="EDIeditCell">
         ${EDIchildrenOptions(NodeName, FileName, RowData.type)}
      </div>
      <div class="EDIeditCell">
         <input class="FORMinput EDIdescriptionEdit" type="text" placeholder="Description" value="${RowData.desc}">
      </div>
      <div class="EDIeditCell ">
         <div class="FORMinput EDIboolValue"><input type="checkbox" class="EDIrequiredEdit"${Required}></div>
      </div>
      ${NodeType !== "composite" ? /*html*/`
      <div class="EDIeditCell">
         <div class="FORMinput EDIboolValue"><input type="checkbox" class="EDImaxEdit"${Repeats}></div>
      </div>` : ""}
      <div class="EDIeditCell">
         <div class="BUTTONcancel EDIdeleteRow"><img src="/EDI/images/EDIdeleteRow.svg"></div>
      </div>
   </div>
   `;
}

function EDIextractData(FileName, NodeName, WindowId){
   let FileData = EDIfileData();
   let Content = WNDcontentSelector(WindowId);
   let Data = {};
   Data.type = FileData[FileName]["nodes"][NodeName].type;
   Data.desc = Content.querySelector(`.EDIdescriptionMainEdit`).value;
   let Children = EDIextractChildren(Content);
   if(Children.length) Data.children = Children;
   return Data;
}

function EDIextractChildren(Content){
   let Parameters = []
   const ChildItems = Content.querySelectorAll(`.EDIchildRow`)
   for (let Parameter of ChildItems) {
      const Name = Parameter.querySelector(".EDInameEdit"); 
      if(!Name || !Name.value.length) continue
      Parameters.push({
         "type"    : Name.value.trim(),
         "desc"    : Parameter.querySelector(".EDIdescriptionEdit").value.trim(),
         "req"     : Parameter.querySelector(".EDIrequiredEdit").checked,
         "repeats" : Parameter.querySelector(".EDImaxEdit") ? Parameter.querySelector(".EDImaxEdit").checked  : ""
      })
   }
   return Parameters;
}

function EDIextractName(WindowId){
   let Content = WNDcontentSelector(WindowId);
   let Name = Content.querySelector('.EDInameMainEdit').value.trim();
   return Name;
}

function EDIsaveData(FileName, NodeName, WindowId){
   let FileData = EDIfileData();
   let Data = EDIextractData(FileName, NodeName, WindowId);
   let Name = EDIextractName(WindowId);
   if(Name !== NodeName && FileData[FileName]["nodes"][Name]) return EDIdisplayError(WindowId, "Another node with this name already exists");
   delete FileData[FileName]["nodes"][NodeName];
   FileData[FileName]["nodes"][Name] = Data;
   if(NodeName !== Name) EDIreplaceChildReferences   (NodeName, Name, FileName);
   if(NodeName !== Name) EDIreplaceMatchingReferences(NodeName, Name, FileName);
   let ApiData = {};
   ApiData.path = FileName;
   ApiData.component   = TRANScomponentGuid();
   ApiData.content     = JSON.stringify(FileData[FileName]);
   ApiData.development = TRANSdevelopment();
   APIcall('component/edi/set', ApiData, function(R){ 
      EDInodeWindows[WindowId].name = Name;
      EDInodeWindows[WindowId].oldName = NodeName;
      MODELforcePoll('EDImodel'); 
   });
   return true;
}

function EDIdisplayError(WindowId, ErrorMessage){
   let NameBox = WNDcontentSelector(WindowId).querySelector('.EDInameMainEdit');
   NameBox.classList.add('FORMinputError');
   let NameHintBox = WNDcontentSelector(WindowId).querySelector('.EDInameMainEditHint');
   NameHintBox.innerText = ErrorMessage;
   NameHintBox.classList.remove('EDIhidden');
   return false;
}

function EDIreplaceChildReferences(OldName, NewName, FileName){
   let FileData = EDIfileData();
   for(NodeName in FileData[FileName]["nodes"]){
      if(!FileData[FileName]["nodes"][NodeName].children) continue;
      for(Child of FileData   [FileName]["nodes"][NodeName].children){ 
         if(Child.type === OldName) Child.type = NewName; 
      }
   }
}

function EDIreplaceMatchingReferences(OldName, NewName, FileName){
   let FileData = EDIfileData();
   for(MatchRule of FileData[FileName]["matching"]){
      if(MatchRule.name === OldName) MatchRule.name = NewName;
   }
}

function EDIsetupTabLoop(FileName, NodeName, WindowId){
   const EditWindow = WNDcontentSelector(WindowId).querySelector('.EDIwindow');
   const FirstField = WNDcontentSelector(WindowId).querySelector('.EDInameMainEdit');
   EditWindow.addEventListener('keydown', function(e){
      let LastRepeatBox = WNDcontentSelector(WindowId).querySelectorAll('.EDIboolValue'); // We need to find this dynamically since rows can be added/removed/re-ordered
      LastRepeatBox = LastRepeatBox[LastRepeatBox.length - 1];
      let isTabPressed = (e.key === 'Tab' || e.keyCode === 9);
      if (!isTabPressed) { return; }
      if ( e.shiftKey ) { // handle shift-tab
         if (document.activeElement === FirstField) {
            LastRepeatBox.focus();
            e.preventDefault();
         }
      } else { // tab
         if (document.activeElement.parentElement === LastRepeatBox) {
            EDIaddBlankRow(FileName, NodeName, WindowId);
            e.preventDefault();
         }
      }
   });
}


function EDIrowNumbers(ListId){
   let ListDiv = document.querySelector(`#${ListId}`)
   const rows = ListDiv.querySelectorAll('.EDIchildRow .EDInumber');
   const list = [];
   for(let i = 0; i < rows.length; i++){
      let row = rows[i];
      let idx = i + 1;
      if(row.innerHTML != idx){row.innerHTML = idx;}
   }
}

function EDIeditChildHeadersHtml(NodeType){
   return /*html*/`
   <div class="EDImatchingHeader FORMlabel"></div>
   <div class="EDImatchingHeader FORMlabel">TYPE</div>
   <div class="EDImatchingHeader FORMlabel">DESCRIPTION</div>
   <div class="EDImatchingHeader EDIboolValue FORMlabel" title="Required">REQ</div>
   ${NodeType !== "composite" ? "<div class='EDImatchingHeader EDIboolValue FORMlabel' title='Repeats'>REP</div>" : ""}`;
}