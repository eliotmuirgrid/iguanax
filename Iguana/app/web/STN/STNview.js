function STNview() {
   SETrender();
	document.querySelector('.STNnotifications').classList.add('selected');
   STNrender();
   STNmodelInit(true);
}

function STNrenderNotificationCriteria(Tags) {
   if(!Tags) { return ''; }
   const TagString = ESChtmlEscape(Tags.join(' '));
   return `<a href="#dashboard?filter=${TagString}&whole_word=true">${TagString}</a>`;
}

function STNrenderNotificationCondition(Type){
   let Out = `<div class="SETconditionWrapper SETconditionVal">`;
   switch(Type){
      case "queue_count":     Out += "Queue";      break;
      case "error_count":     Out += "Has Errors"; break;
      case "inactive_time":   Out += "Inactive";   break;
      case "log_contains":    Out += "Log Contains";   break;
   }
   Out += "</div>";
   return Out;
}

function STNsign(Type){
   if(Type == "queue_count" || Type == "inactive_time") return "&gt;";
   return "";
}

function STNrenderNotificationVal(Rule){
   switch(Rule?.type){
      case "queue_count":      
      case "log_contains":
      case "inactive_time":   return `<div class="SETconditionWrapper">${ESChtmlEscape(Rule?.threshold || "")}</div>`;
      default:                return `<div class="SETconditionWrapper" style="display:none;"></div>`;
   }
}

function STNrenderNotificationValUnit(Type){
   switch(Type){
      case "queue_count":     return `<div class="SETconditionWrapper SETvalId">messages</div>`;
      case "inactive_time":   return `<div class="SETconditionWrapper SETvalId">minutes</div>`;
      default:                return `<div class="SETconditionWrapper SETvalId" style="display:none;"></div>`;
   }
}

function STNlogContainsCriteria(Criteria){
   return /*html*/`
   <div class="STNcontainsRow">
      <div class="STNcontainsVal SETnotificationVal SETconditionWrapper">${ESChtmlEscape(Criteria)}</div>
      <div></div>
   </div>`;
}

function STNcriteria(Rule){
   let Type = Rule?.type;
   if(Type == "log_contains") return STNlogContainsCriteria(Rule?.threshold || "");
   return /*html*/`
   <div class="STNcriteria">
      <div class=" SETnotificationData STNgt">${STNsign(Type)}</div>
      <div class=" SETnotificationData SETnotificationVal">${STNrenderNotificationVal(Rule)}</div>
      <div class=" SETnotificationData SETnotificationValId">${STNrenderNotificationValUnit(Type)}</div>
      <div></div>
   </div>`;
}

