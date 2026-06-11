function STNupdateRules(){
   let div = document.querySelector(".STNrulesTarget");
   if(!div) return;
   let Rules = STNnotificationRules();
   let Html = STNrulesHtml(Rules);
   if(!DEEPequal(STNruleData, Rules)) div.innerHTML = Html;
   STNruleData = Rules;
}

function STNrulesHtml(Rules){
   let View  = !STNeditPage();
   if(View && Rules.length == 0) return /*html*/ `<div class="SETemptyTable">No existing notification rules - click EDIT to create notification rules</div>`;
   if(!View) return STNeditRowsHtml(Rules);
   let Rows = SETnotificationRowView(Rules);
   return /*html*/`
   <div class="STNrules">
      <div class="STNheading STNpadLeft">Matching Rule</div>
      <div class="STNheading">Condition</div>
      <div class="STNheading"></div>
      <div class="STNheading">Priority</div>
      <div class="STNheading">Limit</div>
      <div class="STNheading"></div>
      ${Rows}
   </div>`;
}

function STNeditRowsHtml(Rules){
   let Rows = STNrowEditHtml(Rules);
   return /*html*/`
   <div class="STNrulesBox">
      <div class="STNrulesHeading">
         <div class="STNheading STNpadLeft">Matching Rule</div>
         <div class="STNheading">Condition</div>
         <div class="STNheading"></div>
         <div class="STNheading">Priority</div>
         <div class="STNheading">Limit <i>(60 max)</i></div>
         <div class="STNheading"></div>
      </div>
   ${Rows}
   </div>
   ${STNaddRuleHtml()}`;
}

function STNaddRuleHtml(){
   return /*html*/`
   <div class="STNadd">
      <svg class="SETaddIcon" width="14" height="14">
         <use href="SET/SETiconAdd.svg#SETiconAdd"></use>
      </svg>
      NOTIFICATION RULE
   </div>`;
}

function SETnotificationRowView(Rules) {
   let Html = "";
   for (let Rule of Rules){
      let Type = Rule?.type;
      let Priority = Rule?.priority || "medium";
      let PriorityView = Priority == "high" ? "High" : Priority == "low" ? "Low" : "Medium";
      let Limit = Rule?.limit || 0;
      let LimitView = Limit > 60 || Limit == 0 ? "<i>No limit</i>" : Limit + " per hour";
      Html += /*html*/
      `<div class="STNruleRow SETnotificationData STNtagMatching">${STNrenderNotificationCriteria(Rule['tags'])}</div>
      <div class="STNruleRow SETnotificationData SETnotificationCondition">${STNrenderNotificationCondition(Type)}</div>
      <div class="STNruleRow">${STNcriteria(Rule)}</div>
      <div class="STNruleRow STNpriorityView">${PriorityView}</div>
      <div class="STNruleRow STNlimitView">${LimitView}</div>
      <div class="STNruleRow SETnotificationDelete"></div>`;
   }
   return Html;
}
