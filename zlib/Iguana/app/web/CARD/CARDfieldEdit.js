const FIELD_INPUT_TYPES = {text: 'text', password: 'password', file: 'file', folder: 'folder'};
Object.freeze(FIELD_INPUT_TYPES); // js enum

function CARDeditFieldHtml(Field){
   switch(Field.type) {
      case 'File'    : return CARDeditBrowserFieldHtml(Field, FIELD_INPUT_TYPES.file);
      case 'Folder'  : return CARDeditBrowserFieldHtml(Field, FIELD_INPUT_TYPES.folder);
      case 'Password': return CARDeditFieldTextHtml(Field, FIELD_INPUT_TYPES.password);
      case 'Bool'    : return CARDeditFieldBoolHtml(Field);
      case 'List'    : return CARDeditFieldListHtml(Field);
      default        : return CARDeditFieldTextHtml(Field, FIELD_INPUT_TYPES.text);
   }
}

function CARDeditFieldListHtml(Field) {
   let options = Field.options.map(value => value.trim());
   let optionsHtml = options.map(value =>
      `<option value="${ESChtmlEscape(value)}" ${Field.true_value === value ? 'selected' : ''}>
         ${ESChtmlEscape(value)}
      </option>`
   ).join('');

   return /*html*/ `
   <div class="CARDviewFields CARDedit">
      <div class="CARDrowName FORMlabel">${ESChtmlEscape(Field.name)}:</div>
      <div class="CARDeditInline">
         <select class="FORMselectBox CARDvalueEdit CARDboolValue" name="fieldValue">
            ${optionsHtml}
         </select>
      </div>
      <div class="CARDinplaceActions BUTTONgroup">
         <div class="CARDcancelInplace BUTTONcancel BUTTONstandard"><img src="CARD/CARDiconCancel.svg"></div>
         <div class="CARDsaveInplace BUTTONaction BUTTONstandard"><img src="CARD/CARDiconCheck.svg"></div>
      </div>
   </div>
   `;
}


function CARDeditFieldTextHtml(Field, inputType) {
   const isPassword = inputType === FIELD_INPUT_TYPES.password;
   const trueValue  = isPassword ? "" : Field.true_value; // if password start blank
   const placeholder = isPassword ? "Cancel to keep current password" : `default=${Field.default}`;
   if(!isPassword) { inputType = FIELD_INPUT_TYPES.text; }
   return /*html*/ `
   <div class="CARDviewFields CARDedit">
      <div class="CARDrowName FORMlabel">${ESChtmlEscape(Field.name)}:</div>
      <div class="CARDeditInline">
         <form class="CARDEditInlineForm">
            <input class="FORMinput CARDvalueEdit" onsubmit="return false;" autocomplete="off" type="${inputType}" placeholder="${placeholder}" value="${ESChtmlEscape(trueValue)}">
         </form>
      </div>
      <div class="CARDinplaceActions BUTTONgroup">
         <div class="CARDcancelInplace BUTTONcancel BUTTONstandard"><img src="CARD/CARDiconCancel.svg"></div>
         <div class="CARDsaveInplace BUTTONaction BUTTONstandard"><img src="CARD/CARDiconCheck.svg"></div>
      </div>
   </div>
   `;
}

function CARDeditFieldBoolHtml(Field) {
   return /*html*/ `
   <div class="CARDviewFields CARDedit">
      <div class="CARDrowName FORMlabel">${ESChtmlEscape(Field.name)}:</div>
      <div class="CARDeditInline">
        <select class="FORMselectBox CARDvalueEdit CARDboolValue" name="fieldValue">
          <option value="true" ${Field.true_value === 'true' ? 'selected' : ''}>true</option>
          <option value="false" ${Field.true_value === 'false' ? 'selected' : ''}>false</option>
        </select>
      </div>
      <div class="CARDinplaceActions BUTTONgroup">
         <div class="CARDcancelInplace BUTTONcancel BUTTONstandard"><img src="CARD/CARDiconCancel.svg"></div>
         <div class="CARDsaveInplace BUTTONaction BUTTONstandard"><img src="CARD/CARDiconCheck.svg"></div>
      </div>
   </div>
   `;
}

function CARDeditBrowserFieldHtml(Field, inputType) {
   let browseClass = 'CARDfileBrowse';
   if(inputType !== FIELD_INPUT_TYPES.file) { browseClass = 'CARDfolderBrowse'; } // default to folder if not file
   return /*html*/ `
   <div class="CARDviewFields CARDedit">
      <div class="CARDrowName FORMlabel">${ESChtmlEscape(Field.name)}:</div>
      <div class="CARDeditInline FORMinputGroup">
         <input class="FORMinput CARDvalueEdit" type="text" placeholder="${Field.default ? "default=" + Field.default : ""}" value="${ESChtmlEscape(Field.true_value)}">
         <div class="${browseClass} BUTTONcancel BUTTONstandard">BROWSE...</div>
      </div>
      <div class="CARDinplaceActions BUTTONgroup">
         <div class="CARDcancelInplace BUTTONcancel BUTTONstandard"><img src="CARD/CARDiconCancel.svg"></div>
         <div class="CARDsaveInplace BUTTONaction BUTTONstandard"><img src="CARD/CARDiconCheck.svg"></div>
      </div>
   </div>
   `;
}

function CARDfieldListeners(T){
   if( (T.targetArea('CARDeditValueInPlace') || T.targetArea('CARDcurrentValue') ) && !CARDcomponentOn()) CARDeditInPlaceClick(T.target);
   if(T.targetArea('CARDcancelInplace')) CARDdisableEditedFieldRow();
   if(T.targetArea('CARDsaveInplace'))   CARDfieldSave(T.target, function(){
      CARDdisableOpenEdit();
      MODELforcePoll("CARDmodel");
   } );
   if(T.targetArea('CARDfileBrowse'))    CARDfileBrowse(T.target);
   if(T.targetArea('CARDfolderBrowse'))  CARDfolderBrowse(T.target);
}

function CARDeditInPlaceClick(target){
   let FieldRow = target.closest('.CARDviewFields');
   let FieldRowIdx = CARDfieldRowIndex(FieldRow);
   let EditedRow = document.querySelector('.CARDeditInline');
   if(EditedRow === null) return CARDfieldEdit(FieldRowIdx);
   let PasswordType = EditedRow.querySelector("input")?.type === "password";
   if(PasswordType){
      SNCKsnackbar("Warning", "Password field not updated - must use save button to update it.", 10000);
      return CARDfieldEdit(FieldRowIdx);
   }
   CARDfieldSave(EditedRow, function(){
      CARDfieldEdit(FieldRowIdx);
   });
}

function CARDdisableEditedFieldRow(){
   let EditedRow = document.querySelector('.CARDeditInline');
   if(EditedRow === null) return;
   EditedRow = EditedRow.closest('.CARDviewFields');
   CARDfieldCancel(EditedRow);
}

function CARDfieldCancel(EditedRow){
   let EditedRowIdx = CARDfieldRowIndex(EditedRow);
   let Data = CARDfieldData()[EditedRowIdx];
   document.querySelectorAll('.CARDviewFields')[EditedRowIdx].outerHTML = CARDfieldRowViewHtml(Data);
   CARDfieldAttachTooltips();
}

function CARDfieldEdit(FieldRowIdx){
   CARDdisableOpenEdit();
   let Data = CARDfieldData()[FieldRowIdx];
   document.querySelectorAll('.CARDviewFields')[FieldRowIdx].outerHTML = CARDeditFieldHtml(Data);
   FORMfocus(document.querySelector('.CARDvalueEdit'));
   const form = document.querySelector('.CARDEditInlineForm');
   if(form) form.addEventListener('submit', (e) => { e.preventDefault();});
}

function CARDfieldRowIndex(targetRow){
   return targetRow ? Array.from(targetRow.parentElement.children).indexOf(targetRow) : -1;
}

function CARDfieldSave(target, callback){
   let FieldRow = target.closest('.CARDviewFields');
   let FieldRowIdx = CARDfieldRowIndex(FieldRow);
   let FieldData = CARDfieldData()[FieldRowIdx];
   if(!FieldData) return;
   let Key = FieldData?.name;
   const Data = {
      component: CARDcomponentId(),
      name:      Key,
      value:     FieldRow.querySelector('.CARDvalueEdit').value,
   };
   APIcall('component/fields/set', Data, function (Response) {
      if (!Response.success) return SNCKsnackbar('error', "Could not set field value: " + Response.error);
      MODELforcePoll("CARDmodel", function(){
         let Message = GINchangeField(Key, CARDdata().name);
         if(callback) { GINaddAndCommit({message: Message, component: CARDcomponentId(), callback: callback}); }
         else         { GINaddAndCommit({message: Message, component: CARDcomponentId()}); }
      });
   });
}

function CARDfileBrowse(target){
   let FieldRow = target.closest('.CARDviewFields');
   FILbrowser({
      new_folder : true,
      show_hidden: true,
      dir_only   : false,
      value      : FieldRow.querySelector('.CARDvalueEdit').value,
      callback   : function(Path) { FieldRow.querySelector('.CARDvalueEdit').value = Path; }
   });
}

function CARDfolderBrowse(target) {
   let FieldRow = target.closest('.CARDviewFields');
   FILbrowser({
      new_folder : true,
      show_hidden: true,
      dir_only   : true,
      value      : FieldRow.querySelector('.CARDvalueEdit').value,
      callback   : function(Path) { FieldRow.querySelector('.CARDvalueEdit').value = Path; }
   });
}