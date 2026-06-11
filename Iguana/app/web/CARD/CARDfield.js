let CARDrenderedFields  = null;
let CARDrenderedRunning = false;
let CARDrenderedError   = "";

function CARDupdateFields(){
   let EditDiv = document.querySelector(".CARDviewFields.CARDedit");
   if(EditDiv) return;
   let Fields  = CARDfieldData();
   let Running = CARDcomponentOn();
   let Error   = CARDfieldError();
   if(DEEPequal(Fields,    CARDrenderedFields) && 
                Running == CARDrenderedRunning && 
                Error   == CARDrenderedError) return;
   let Div = document.querySelector('.CARDfieldTarget');
   CARDfieldVisibility(Div, Fields, Error);
   CARDrenderedRunning = CARDcomponentOn();
   if(Error) return CARDfieldErrorRender(Div, Error);
   CARDfieldRender(Div, Fields);
}

function CARDfieldErrorRender(Div, Error){
   if(CARDrenderedError == Error) return;
   Div.innerHTML = CARDfieldErrorHtml(Error);
   CARDrenderedError = Error;
}

function CARDfieldRender(Div, Fields){
   CARDrenderedFields = Fields;
   Div.innerHTML = Fields.map((fieldData) => { return CARDfieldRowViewHtml(fieldData) }).join('');
   CARDfieldAttachTooltips();
}

function CARDfieldVisibility(Div, Fields, Error){
   let Empty = Fields.length === 0 && !Error;
   if(!Div.classList.contains('CARDhidden') && Empty) Div.classList.add   ('CARDhidden');
   if( Div.classList.contains('CARDhidden') &&!Empty) Div.classList.remove('CARDhidden');
}

function CARDfieldErrorHtml(ErrorMessage) {
   const isPermError = ErrorMessage.includes('permission required to');
   const labelHtml = isPermError ? '<div class="FORMlabel">Custom Fields:</div><div class="FORMlabel"></div>' :
      '<div class="FORMlabel">Custom Fields [<div class="CARDfieldErrorLabel FORMlabel">Error</div><div class="FORMlabel">]:</div>';
   return `
        <div class="CARDviewFields">
            <div class="CARDcustomFieldsErrorRow">
                ${labelHtml}
                <div></div>
            </div>
            <div class="CARDfieldError">${ErrorMessage}</div>
        </div>
    `;
}


function CARDfieldRowViewHtml(Field) {
   const isOn           = CARDcomponentOn();
   const withEdit       = isOn ? 'CARDviewFieldNoEdit' : 'CARDviewFieldWithEdit';
   const hasDescription = Field.description && Field.description.trim() !== '';
   const infoIcon       = hasDescription ? '<span class="BUTTONdefault BUTTONstandard CARDfieldInfo">ⓘ</span>' : '';
   const editButton     = '<div class="CARDeditValueInPlace CARDeditBtn"><img src="CARD/CARDiconMoreOptions.svg"></div>';
   const actions        = isOn ? infoIcon : '<div class="CARDfieldViewActions">' + infoIcon + editButton + '</div>';
   const isPassword     = Field.type === FIELD_INPUT_TYPES.password;
   const currentValue   = isPassword ? 'CARDcurrentValue CARDpasswordView' : 'CARDcurrentValue';
   return `
      <div class="CARDviewFields ${withEdit}">
         <div class="CARDrowName FORMlabel">${ESChtmlEscape(Field.name)}:</div>
         <div class="${currentValue}">${ESChtmlEscape(Field.display_value)}</div>
         ${actions}
      </div>
   `;
}

function CARDfieldAttachTooltips() {
   let InfoIcons    = document.querySelectorAll('.CARDfieldInfo');
   let AllFieldRows = document.querySelectorAll('.CARDviewFields');
   InfoIcons.forEach(Icon => {
      let FieldRow   = Icon.closest('.CARDviewFields');
      let FieldIndex = Array.from(AllFieldRows).indexOf(FieldRow);
      if(CARDrenderedFields && CARDrenderedFields[FieldIndex]) {
         let Field = CARDrenderedFields[FieldIndex];
         if(Field.description && Field.description.trim() !== '') {
            POPtooltip({
               target  : Icon,
               content : ESChtmlEscape(Field.description),
               position: POPtooltipPosition.AUTO,
               event   : POPtooltipEvent.HOVER,
               type    : POPtooltipType.INFO,
               style   : 'CARDfieldInfoTooltip',
            });
         }
      }
   });
}