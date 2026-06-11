function EDIeditMatchingHtml(){
   return /*html*/`
   <div class="EDIwindow">
      <div class="EDIeditBanner">
         <div class="EDImatchingRulesDiv">Matching Rules</div>
         <div class="EDImatchingFile">(${FileName})</div>
         <div class="BUTTONgroup EDIbuttonGroup">
            <div class="EDIsaveMatching BUTTONaction BUTTONstandard">SAVE</div>
            <div class="EDIcancelMatching BUTTONcancel BUTTONstandard">CANCEL</div>
         </div>
      </div>
      <div class="EDIwindowInner">
         <div class="EDIeditMatchingDiv">
            <div class="EDImatchingHeaders">
               <div class="EDImatchingHeader"></div>
               <div class="EDImatchingHeader FORMlabel">RULE</div>
               <div class="EDImatchingHeader FORMlabel">NAME</div>
               <div class="EDImatchingHeader"> </div>
            </div>
         </div>
         <div class="EDIaddMatchingRow">
            <div class="EDIaddRowMatching">+ ADD RULE</div>
         </div>
         <div class="EDImatchingHint">Rules are tried in the order shown. The first matching rule wildcard (glob) to match the MSH.9 field
         determines which message grammar is used to parse the HL7 message.  <p>
         Typical rules would be like: ADT^A04 for patient registration, then ADT* to match all ADT messages and say * for catching unexpected messages.</div>
      </div>
   </div>`;
}



function EDIeditMatchingRowsHtml(FileName, WindowId){
   let FileData = EDIfileData();
   let Data = FileData[FileName]["matching"];
   let anchor = `#${WindowId} .EDIeditMatchingDiv`;
   if(!document.querySelector(anchor)) anchor = `#content-_${WindowId}docked .EDIeditMatchingDiv`;
   let Args= {};
   Args.anchor = anchor;
   // Args.callback = EDIrowNumbers;
   EDIeditList[WindowId] = DRGcreateDragSortList(Args);
   if(!Data || Data.length === 0) return EDIaddBlankMatchingRow(FileName, WindowId);
   for(RowData of Data){ 
      DRGdragSortListAddItem(EDIeditList[WindowId], EDIeditMatchingRowHtml(RowData, FileName), {'dragHandleSelector': '.EDImoveRow', 'prepend': false}); 
   }
}
function EDIaddBlankMatchingRow(FileName, WindowId){
   let Content = EDIeditMatchingRowHtml({"rule" : "", "name" : ""}, FileName);
   DRGdragSortListAddItem(EDIeditList[WindowId], Content, {'dragHandleSelector': '.EDImoveRow', 'prepend': false});
   let LastRow = Array.from( WNDcontentSelector(WindowId).querySelectorAll('.EDImatchingRow') ).pop();
   LastRow.scrollIntoView(true);
}

function EDIeditMatchingRowHtml(RuleData, FileName){
   let FileData = EDIfileData();
   let NodeData = FileData[FileName]["nodes"];
   let Messages = EDImessageList(NodeData, "message");
   let SelectedName = RuleData.name;
   if(Messages.indexOf(SelectedName) === -1) SelectedName = "";
   return /*html*/`
   <div class="EDImatchingRow">
      <div class="EDIeditCell EDImoveRow">⋮⋮</div>
      <div class="EDIeditCell">
         <input class="FORMinput EDImatchingRuleValue" type="text" placeholder="Matching Rule" value="${RuleData.rule}">
      </div>
      <div class="EDIeditCell">
         ${EDImatchingOptions(Messages, SelectedName)}
      </div>
      <div class="EDIeditCell">
         <div class="BUTTONcancel EDIdeleteRow"><img src="/EDI/images/EDIdeleteRow.svg"></div>
      </div>
   </div>
   `;
}

function EDImatchingOptions(Messages, SelectedName){
   let Html = /*html*/`<select class="FORMselectBox EDImatchingNameValue" value="${ESChtmlEscape(SelectedName)}">
   <option value="" ${!SelectedName ? " selected " : ""} disabled>-- Select Message --</option>
   <option value="{}[newmessage321">+ New Message +</option>`;
   for(NodeName of Messages) { Html += `<option value="${ESChtmlEscape(NodeName)}" ${NodeName === SelectedName ? " selected" : ""}>${NodeName}</option>`; }
   Html += `</select>`;
   return Html;
}

function EDIextractMatchingData(WindowId){
   let MatchingData = WNDcontentSelector(WindowId).querySelectorAll('.EDImatchingRow');
   let Data = [];
   for(CustomRow of MatchingData){
      let RowData = {};
      let rule = CustomRow.querySelector('.EDImatchingRuleValue');
      if(!rule) continue;
      let RuleValue = rule.value.trim();
      let NameValue = CustomRow.querySelector('.EDImatchingNameValue').value.trim();
      if(RuleValue === "" || NameValue === "") continue;
      RowData.rule = RuleValue;
      RowData.name = NameValue;
      Data.push(RowData);
   }
   return Data;
}

function EDIsaveMatchingData(FileName, WindowId){
   let FileData = EDIfileData();
   FileData[FileName]["matching"] = EDIextractMatchingData(WindowId);
   let ApiData = {};
   ApiData.path = FileName;
   ApiData.component   = TRANScomponentGuid();
   ApiData.content     = JSON.stringify(FileData[FileName]);
   ApiData.development = TRANSdevelopment();
   APIcall('component/edi/set', ApiData, function(R){ MODELforcePoll('EDImodel'); });
   return true;
}
