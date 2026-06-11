function TBLeditNodeHtml(NodeName, FileName){
   let FileData = TBLfileData();
   return /*html*/`
   <div class="TBLwindow">
      <div class="TBLeditBanner">
         <input class="FORMinput TBLeditName" type="text" placeholder="Name" value="${ESChtmlEscape(NodeName)}">
         <input class="FORMinput TBLeditDescription" type="text" placeholder="Description" value="${ESChtmlEscape(FileData[FileName]["tables"][NodeName].desc)}">
         <div class="TBLsaveEditGroup BUTTONgroup TBLbuttonGroup">
            <div class="TBLsaveEdit BUTTONaction BUTTONstandard">SAVE</div>
            <div class="TBLcancelEdit BUTTONcancel BUTTONstandard">CANCEL</div>
         </div>
      </div>
      <div class="TBLwindowInner">
         <div class="TBLnameMainEditHint TBLhidden"></div>
         <div class="TBLeditChildrenDiv">
            <div class="TBLcolumnRow TBLnodeHeaders">
               <div></div>
               <div></div>
               <div class="TBLnodeHeader FORMlabel">NAME</div>
               <div class="TBLnodeHeader FORMlabel">DESCRIPTION</div>
               <div class="TBLnodeHeader FORMlabel">TYPE</div>
               <div class="TBLnodeHeader TBLboolValue FORMlabel">KEY</div>
           
            </div>
         </div>
         <div class="TBLaddEditRow">
            <div class="TBLaddColumn">+ ADD COLUMN</div>
         </div>
      </div>
   </div>`;
}

function TBLeditColumnsHtml(NodeName, FileName, WindowId){
   let FileData = TBLfileData();
   let Data = FileData[FileName]["tables"][NodeName].columns;
   let anchor = `#${WindowId} .TBLeditChildrenDiv`;
   if(!document.querySelector(anchor)) anchor = `#content-_${WindowId}docked .TBLeditChildrenDiv`;
   let Args= {};
   Args.anchor = anchor;
   Args.callback = TBLrowNumbers;
   TBLeditList[WindowId] = DRGcreateDragSortList(Args);
   if(!Data || Data.length === 0) {
      TBLaddBlankRow(WindowId);
      TBLsetupTabLoop(WindowId);
      return;
   }
   for(RowData of Data){ DRGdragSortListAddItem(TBLeditList[WindowId], TBLeditChildHtml(RowData), {'dragHandleSelector': '.TBLmoveRow', 'prepend': false}); }
   TBLsetupTabLoop(WindowId);
}

function TBLaddBlankRow(WindowId){
   let Content = TBLeditChildHtml({"name" : "", "desc" : "", "type" : "", "key" : ""});
   DRGdragSortListAddItem(TBLeditList[WindowId], Content, {'dragHandleSelector': '.TBLmoveRow', 'prepend': false});
   let LastRow = Array.from( WNDcontentSelector(WindowId).querySelectorAll('.TBLcolumnRow') ).pop();
   LastRow.scrollIntoView();
}

function TBLeditChildHtml(ColumnData){
   let Name        = ColumnData.name ? ColumnData.name : "";
   let Description = ColumnData.desc ? ColumnData.desc : "";
   let Type        = ColumnData.type ? ColumnData.type : "";
   let Key         = ColumnData.key === 1;
   return /*html*/`
   <div class="TBLcolumnRow">
      <div class="TBLeditCell TBLmoveRow">⋮⋮</div>
      <div class="TBLeditCell TBLnumber"></div>
      <div class="TBLeditCell"><input class="FORMinput TBLnameEdit"        type="text" placeholder="Name"        value="${ESChtmlEscape(Name)}"></div>
      <div class="TBLeditCell"><input class="FORMinput TBLdescriptionEdit" type="text" placeholder="Description" value="${ESChtmlEscape(Description)}"></div>
      <div class="TBLeditCell">
         <select class="FORMselectBox TBLtypeEdit" value="${Type}">
            <option value="string"   ${Type === "string"   ? " selected" : ""}>String</option>
            <option value="integer"  ${Type === "integer"  ? " selected" : ""}>Integer</option>
            <option value="double"   ${Type === "double"   ? " selected" : ""}>Double</option>
            <option value="datetime" ${Type === "datetime" ? " selected" : ""}>Datetime</option>
         </select>
      </div>
      <div class="TBLeditCell ">
         <div class="FORMinput TBLboolValue"><input type="checkbox" class="TBLkeyEdit"${Key ? "checked" : ""}></div>
      </div>
      
      <div class="TBLeditCell">
         <div class="BUTTONcancel TBLdeleteRow"><img src="/TBL/images/TBLdeleteRow.svg"></div>
      </div>
   </div>
   `;
}

function TBLsaveNodeData(WindowId, NodeName, FileName){
   let Data = TBLextractData(WindowId);
   let Name = TBLextractName(WindowId);
   let FileData = TBLfileData();
   if(Name !== NodeName && FileData[FileName]["tables"][Name]) return TBLdisplayError(WindowId, "Another table with this name already exists");
   delete FileData[FileName]["tables"][NodeName]; // TODO - double check this works
   FileData[FileName]["tables"][Name] = Data;
   let ApiData = {};
   ApiData.path = FileName;
   ApiData.component   = TRANScomponentGuid();
   ApiData.content     = JSON.stringify(FileData[FileName]);
   ApiData.development = TRANSdevelopment();
   MODELremove('TBLmodel'); // Before saving, stop the model. We'll re-create it in the callback. This is to avoid a load being called while a save is in action.
   APIcall('component/db/set', ApiData, function(R){ 
      if(!R.success) { 
         let DisplayError = R.error
         if (-1 != DisplayError.indexOf("duplicate column")){
            DisplayError = "Failed to save changes as column names are not unique. Please make sure each column has a unique name";
         }
         SNCKsnackbar("error", DisplayError); 
      }
      TBLnodeWindows[WindowId].name = Name;
      TBLnodeWindows[WindowId].oldName = NodeName;
      TBLmodelInit(); // Re-initialize the model
   });
   return true;
}

function TBLextractData(WindowId){
   let Content = WNDcontentSelector(WindowId);
   let Data = {};
   Data.desc = Content.querySelector(`.TBLeditDescription`).value;
   let Columns = TBLextractColumns(Content);
   if(Columns.length) Data.columns = Columns;
   return Data;
}

function TBLextractColumns(Content){
   let Parameters = []
   const ChildItems = Content.querySelectorAll(`.TBLcolumnRow`)
   for (let Parameter of ChildItems) {
      const Name = Parameter.querySelector(".TBLnameEdit"); 
      if(!Name || !Name.value.length) continue
      let ColumnRow = {};
      ColumnRow.name = Name.value.trim();
      ColumnRow.desc = Parameter.querySelector(".TBLdescriptionEdit").value.trim();
      ColumnRow.type = Parameter.querySelector(".TBLtypeEdit").value.trim();
      if(Parameter.querySelector(".TBLkeyEdit").checked) ColumnRow.key = 1;
      Parameters.push(ColumnRow);
   }
   return Parameters;
}

function TBLextractName(WindowId){
   let Content = WNDcontentSelector(WindowId);
   let Name = Content.querySelector('.TBLeditName').value.trim();
   return Name;
}

function TBLdisplayError(WindowId, ErrorMessage){
   let NameBox = WNDcontentSelector(WindowId).querySelector('.TBLeditName');
   NameBox.classList.add('FORMinputError');
   let NameHintBox = WNDcontentSelector(WindowId).querySelector('.TBLnameMainEditHint');
   NameHintBox.innerText = ErrorMessage;
   NameHintBox.classList.remove('TBLhidden');
   return false;
}

function TBLsetupTabLoop(WindowId){
   const EditWindow = WNDcontentSelector(WindowId).querySelector('.TBLwindow');
   const FirstField = WNDcontentSelector(WindowId).querySelector('.TBLeditName');
   EditWindow.addEventListener('keydown', function(e){
      let LastKeyBox = WNDcontentSelector(WindowId).querySelectorAll('.TBLboolValue'); // We need to find this dynamically since rows can be added/removed/re-ordered
      LastKeyBox = LastKeyBox[LastKeyBox.length - 1];
      let isTabPressed = (e.key === 'Tab' || e.keyCode === 9);
      if (!isTabPressed) { return; }
      if ( e.shiftKey ) { // handle shift-tab
         if (document.activeElement === FirstField) {
            LastKeyBox.focus();
            e.preventDefault();
         }
      } else { // tab
         if (document.activeElement.parentElement === LastKeyBox) {
            TBLaddBlankRow(WindowId);
            e.preventDefault();
         }
      }
   });
}

function TBLrowNumbers(ListId){
   let ListDiv = document.querySelector(`#${ListId}`)
   const rows = ListDiv.querySelectorAll('.TBLcolumnRow .TBLnumber');
   const list = [];
   for(let i = 0; i < rows.length; i++){
      let row = rows[i];
      let idx = i + 1;
      if(row.innerHTML != idx){row.innerHTML = idx;}
   }
}