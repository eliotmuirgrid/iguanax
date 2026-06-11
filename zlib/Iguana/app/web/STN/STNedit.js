function STNedit(){
   SETrender();
   document.querySelector('.STNnotifications').classList.add('selected');
   STNrender();
   STNmodelInit(false);
}

function STNconditionHtml(Type){
   let HasErrors    = Type === 'error_count'   ? 'selected' : '';
   let LogContains  = Type === 'log_contains'  ? 'selected' : '';
   let QueueCount   = Type === 'queue_count'   ? 'selected' : '';
   let InactiveTime = Type === 'inactive_time' ? 'selected' : '';
   return /*html*/`
   <div class="SETnotificationData STNruleRow">
      <select class="FORMselectBox SETnotificationsCondition">
         <option ${HasErrors}    value="error_count"  >Has Errors</option>
         <option ${InactiveTime} value="inactive_time">Inactive</option>
         <option ${LogContains}  value="log_contains" >Log Contains</option>
         <option ${QueueCount}   value="queue_count"  >Queue</option>
      </select>
   </div>`;
}

function STNpriorityHtml(Priority){
   let High         = Priority === 'high'   ? 'selected' : '';
   let Medium       = Priority === 'medium' ? 'selected' : '';
   let Low          = Priority === 'low'    ? 'selected' : '';
   return /*html*/`
   <div class="SETnotificationData">
      <select class="FORMselectBox SETprioritySelect">
         <option ${High}>High</option>
         <option ${Medium}>Medium</option>
         <option ${Low}>Low</option>
      </select>
   </div>`;
}
function STNlimitHtml(Limit){
   return /*html*/`
   <div class="STNlimitEditDiv">
      <input class="FORMinput STNlimit" type="number" min="0" max="60" value="${Limit}">
      <div class="SETconditionWrapperEdit">per hour</div>
   </div>`;
}

function STNdeleteHtml(){
   return /*html*/`
   <div class="STNruleRow STNdeleteDiv">
      <div class="FORMbutton BUTTONcancel BUTTONwarning STNdelete">
         ${SETtrashIcon()}
      </div>
   </div>`;
}

function STNlogContainsCriteriaEdit(Criteria){
   return /*html*/`
   <input class="STNcontainsVal FORMinput SETnotificationVal SETconditionWrapper STNthreshold" value="${ESChtmlEscape(Criteria)}">`;
}

function STNcriteriaEditHtml(Rule){
   let Type = Rule?.type || "";
   if(Type == "log_contains") return STNlogContainsCriteriaEdit(Rule?.threshold || "");
   return /*html*/`
   <div class="STNcriteria">
      <div class=" SETnotificationData STNgt">${STNsign(Type)}</div>
      <div class=" SETnotificationData SETnotificationVal">${STNcriteriaEditVal(Rule)}</div>
      <div class=" SETnotificationData SETnotificationValId">${STNrenderNotificationValUnit(Type)}</div>
      <div></div>
   </div>`;
}

function STNcriteriaEditVal(Rule){
   let Type = Rule?.type;
   return Type == "queue_count" || Type == "inactive_time" ? /*html*/`<input class="STNthreshold FORMinput SETconditionWrapper" value="${ESChtmlEscape(Rule?.threshold || "")}">` : "";
}

function STNrowEditHtml(Rules) {
   let Html = ``;
   for (let Rule of Rules) Html += STNeditRowHtml(Rule);
   if(!Html) return STNeditRowHtml();
   return Html;
}

function STNeditRowHtml(Rule){
   let Type = Rule?.type || "";
   let Tags = Rule?.tags || [];
   let Priority = Rule?.priority || "medium";
   let Limit = Rule?.limit || 0;
   return /*html*/`
   <div class="STNrulesRow">
      <div class="STNruleRow SETnotificationData STNpadLeft">
         <input class="FORMinput SETnotificationCriteriaEditValue" value="${ESChtmlEscape(Tags.join(" "))}">
      </div>
      ${STNconditionHtml(Type)}
      <div class="STNruleRow STNcriteriaEdit">
         ${STNcriteriaEditHtml(Rule)}
      </div>
      <div class="STNruleRow STNpriorityView">
         ${STNpriorityHtml(Priority)}
      </div>
      <div class="STNruleRow STNlimitView">
         ${STNlimitHtml(Limit)}
      </div>
      ${STNdeleteHtml()}
   </div>`;
}