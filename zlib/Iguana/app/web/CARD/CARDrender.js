let CARDcard;

// TODO - GROSS how we have the CARDnotificationTarget notification target in all cards where we need it or not IX-1195
// Not worth having a ticket tracking this specific issue but if someone is maintaining this please
// get rid of it.

function CARDskeletonHtml() {
   COL_TRC("Getting CARD HTML");
   return /*html*/`
   <div class="CARDdetailedCard" xmlns="http://www.w3.org/1999/html">
      <div class="CARDnotificationTarget CARDhidden">${CARDnotificationViewHtml()}</div>
      <div class='CARDdescriptionTarget'>${CARDdescriptionHtml()}</div>
		<div class='CARDtableContainer'>
         ${CARDtagInitHtml()}
         <div class='CARDstatusRow'>
            <div class="FORMlabel">Last Log:</div>
            <div class="CARDlastActivityValue">Loading...</div>
            <div class="FORMlabel">Error Count:</div>
            <div class="CARDerrorCountValue">${CARDerrorHtml()}</div>
            <div class="FORMlabel">Queue Position:</div>
            <div class="CARDqueueCountValue">
               <div class="CARDqueuePosition">0</div>
               <div class="CARDstatusClearQueue">[<div class="CARDclearFont">Clear</div>]</div> or 
               <div class="CARDstatusRepositionQueue">[<div class="CARDclearFont">Reposition</div>]</div>
            </div>
         </div>
         <div class='CARDfieldTarget'></div>
         <div class="CARDdetailRow">
            <div class="CARDdetailsDiv ${CARDimplentationFolder()}">
               ${CARDsourceHtml()}
            </div>
         </div>
		</div>
      <div class="CARDinjectTarget">${CARDdata().html_block}</div>
   </div>`;
}

function CARDimplentationFolder(){
   return "";  // was a template thing
}

function CARDtitleBarHtml(){
   return /*html*/`
   <div class='CARDswitchAndTitle'>
      <div class='CARDswitchTarget'>${CARDswitchHtml()}</div>
      <div class='CARDnameTarget'>${CARDnameViewHtml()}</div>
   </div>`;
}
function CARDcomponentExists(){
   let Error = CARDerror();
   if(!Error) return;
   let ComponentId = CARDcomponentId();
   SNCKsnackbar('error',  `Cannot open card for component ${ComponentId} due to: ${Error}`);
   DASHcloseModal();
}

function CARDrender() {
   let Card = document.querySelector('.CARDdetailedCard');
   if(Card) { 
      COL_TRC("No card element found - returning");
      return; 
   }  // KIND OF GROSS HACK to deal late creation of popout card.  I don't like my solution.
   COL_TRC("Opening card");
   CARDcard = POPpopoutOpen({className: `CARDdetailedCardId`, content: CARDskeletonHtml(), callback: DASHcloseModal});
   document.querySelector('.POPpopoutCard-title').innerHTML = CARDtitleBarHtml();
   // CARDfieldRender();
   CARDlisteners();
}

function CARDclearQueueHtml(ComponentGuids) {
   return /*html*/`
   <div class="DASHbulkEditSubmitFeedback"></div>
   ${DASHcomponentTable('CLEARING QUEUES', ComponentGuids)}
   <br>
   <div class="CARDbulkDialogConfirm">Would you like to clear the queue for this component?</div>
   <div class='BUTTONgroup DASHbuttonGroup'>
      <div class='CARDcancelTags BUTTONstandard BUTTONcancel'>CANCEL</div>
      <div class='CARDsaveTags BUTTONstandard BUTTONaction'>SUBMIT</div>
   </div>`;
}

function CARDclearErrorsHtml(ComponentGuids) {
   return /*html*/`
   <div class="DASHbulkEditSubmitFeedback"></div>
   ${DASHcomponentTable('CLEARING ERRORS', ComponentGuids)}
   <br>
   <div class="CARDbulkDialogConfirm">Would you like to clear the error for this component?</div>
   <div class='BUTTONgroup DASHbuttonGroup'>
      <div class='CARDcancelTags BUTTONstandard BUTTONcancel'>CANCEL</div>
      <div class='CARDsaveTags BUTTONstandard BUTTONaction'>SUBMIT</div>
   </div>`;
}

function CARDgenerateComponentHtml() {
   let Tags = CARDtagData();
   let Html = `<div class="DASHbulkListName">${CARDdata().name}</div>  <div>${HASHTAGrenderTags(Tags)}</div>`;
   
   let ComponentTable = '<div class="DASHcomponentList">';
   ComponentTable += '<div>' + Html + '</div>';
   ComponentTable += '</div>';
   
   return ComponentTable;
}

function CARDredirectToLogs(Type) {
   let Perm = CARDpermissionData();
   if(!Perm?.view_logs){
      return;
   }
   let Hash = `#logs?component=${CARDcomponentId()}`;
   if (Type) { Hash += `&type=${Type}`; }
   window.location.hash = Hash;
}
