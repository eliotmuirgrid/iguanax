let FLDcustomList;

function FLDedit(){
   WNDwindow(FLDwindowId, FLDcustomizeHtml());
   FLDloadFieldDefinition(FLDrenderEdit);
}

function FLDrenderEdit(Data){
   FLDcustomList  = DRGcreateDragSortList({'anchor': '.FLDdragList'});
   if(Data === null || Data.length === 0) return FLDaddBlankCustomField();
   for(Field of Data){ DRGdragSortListAddItem(FLDcustomList, FLDeditFieldRowHtml(Field), {'dragHandleSelector': '.FLDmoveRow', 'prepend': false }); }
   FLDfocusAtTheEnd(document.querySelector('.FLDnameEdit'));
   let div = document.querySelector(".FLDmodal");
   FORMenterListener(div, FLDsave);
}

function FLDfocusAtTheEnd(el){
   el.focus();
   el.selectionStart = el.selectionEnd = el.value.length;
}

function FLDcustomizeHtml(){
   return /*html*/`
   <div class="FLDmodal">
      <div class="FLDsaveConfigButton BUTTONgroup">
         <div class="FLDsaveConfig BUTTONaction BUTTONstandard">   SAVE   </div>
         <div class="FLDcancelConfig BUTTONcancel BUTTONstandard"> CANCEL </div>
      </div>
      <div class="FLDeditFieldHeader">
         <div></div>
         <div class="FORMlabel">FIELD NAME</div>
         <div class="FORMlabel">FIELD TYPE</div>
         <div class="FORMlabel">DEFAULT VALUE</div>
         <div class="FORMlabel">DESCRIPTION</div>
         <div></div>
      </div>
      <div class="FLDdragList CARDedit"></div>
         <div class="FLDaddFieldRow">
            <div class="FLDaddField">+ ADD FIELD</div>
         </div>
         <div class="FLDsaveFeedback"></div>
      </div>
   </div>
   `;
}

function FLDaddBlankCustomField(){
   let Content = FLDeditFieldRowHtml({"name": "", "type": "string", "default": "", "description": ""});
   DRGdragSortListAddItem(FLDcustomList, Content, {'dragHandleSelector': '.FLDmoveRow', 'prepend': false});
}

function FLDrerenderCustomizeRow(target){
   let CustomRow = target.closest('.DRGdragSortListItem');
   DRGremoveDragHandle(CustomRow, '.FLDmoveRow');
   let name = CustomRow.querySelector('.FLDnameEdit').value.trim();
   let type = CustomRow.querySelector('.FLDtypeSelect').value;
   let description     = CustomRow.querySelector('.FLDdescriptionEdit')?.value || "";
   CustomRow.innerHTML = FLDeditFieldRowHtml({"name": name, "type": type, "default": "", "description": description});
   DRGattachDragHandle(CustomRow, '.FLDmoveRow');
}

function FLDeditFieldRowHtml(Field){
   if(Field.type.toLowerCase() === 'string')   return FLDcustomizeStringFieldHtml  (Field);
   if(Field.type.toLowerCase() === 'number')   return FLDcustomizeNumberFieldHtml  (Field);
   if(Field.type.toLowerCase() === 'bool')     return FLDcustomizeBoolFieldHtml    (Field);
   if(Field.type.toLowerCase() === 'port')     return FLDcustomizePortFieldHtml    (Field);
   if(Field.type.toLowerCase() === 'password') return FLDcustomizePasswordFieldHtml(Field);
   if(Field.type.toLowerCase() === 'file')     return FLDcustomizeFileFieldHtml    (Field);
   if(Field.type.toLowerCase() === 'folder')   return FLDcustomizeFolderFieldHtml  (Field);
   if(Field.type.toLowerCase() === "list")     return FLDcustomizeListFieldHtml    (Field);
}

function FLDsave(){
   let Fields = FLDextractFieldData();
   if (FLDnameDuplicated(Fields)) {
      return document.querySelector('.FLDsaveFeedback').innerHTML = "Cannot save custom fields that have duplicate names.";
   }
   let Data = {};
   Data.name        = TRANScomponentData().name;
   Data.component   = TRANScomponentGuid();
   Data.fields      = Fields;
   Data.development = TRANSdevelopment();
   console.log(Data);
   APIcall('component/field/grammar/set', Data, function(Response) {
      if (!Response.success) return SNCKsnackbar('error', Response.error);
      FLDview();
      FLDloadFieldDefinition(TRANSrunIfAutoRunEnabled);
      MODELforcePoll("TRANSgitModel");
   });
}

function FLDnameDuplicated(FieldData){
   let FieldNames = [];
   for(field of FieldData) if(field.name !== "") FieldNames.push(field.name);
   return FLDcheckIfDuplicateExists(FieldNames);
}

function FLDcheckIfDuplicateExists(arr) { return new Set(arr).size !== arr.length }

function FLDextractFieldData(){
   let FieldData = [];
   for(let CustomRow of document.querySelectorAll('.FLDfieldTableRow')){
      let RowData = {};
      let name = CustomRow.querySelector('.FLDnameEdit').value;
      if(name === '') continue;
      RowData.name        = name.trim();
      RowData.type        = CustomRow.querySelector('.FLDtypeSelect').value;
      RowData.default     = CustomRow.querySelector('.FLDvalueEdit').value;
      RowData.options     = CustomRow.querySelector('.FLDvoptionsEdit')?.value || "";
      RowData.description = CustomRow.querySelector('.FLDdescriptionEdit')?.value || "";
      FieldData.push(RowData);
   }
   return FieldData;
}

function FLDfileBrowseCustomize(target) {
   let CustomFieldRow = target.closest('.FLDfieldTableRow');
   FILbrowser({
      new_folder : true,
      show_hidden: true,
      dir_only   : false,
      value      : CustomFieldRow.querySelector('.FLDvalueEdit').value,
      callback   : function(Path) { CustomFieldRow.querySelector('.FLDvalueEdit').value = Path; }
   });
}

function FLDfolderBrowseCustomize(target) {
   let CustomFieldRow = target.closest('.FLDfieldTableRow');
   FILbrowser({
      new_folder : true,
      show_hidden: true,
      dir_only   : true,
      value      : CustomFieldRow.querySelector('.FLDvalueEdit').value,
      callback   : function(Path) { CustomFieldRow.querySelector('.FLDvalueEdit').value = Path; }
   });
}