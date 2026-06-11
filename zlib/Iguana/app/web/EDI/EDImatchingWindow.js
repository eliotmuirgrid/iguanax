function EDImatchingHtml(Data, FileName){
   let EditButton = TRANSreadOnly() ? "" : `<div class="EDIeditMatchingWindow BUTTONdefault BUTTONstandard">EDIT</div>`;
   return /*html*/`
   <div class="EDIwindow">
      <div class="EDIeditBanner">
         <div class="EDImatchingRulesDiv">Matching Rules</div>
         <div class="EDImatchingFile">(${FileName})</div>
         ${EditButton}
      </div>   
      <div class="EDImatchingRulesContainer">${EDImatchingRuleViewHtml(Data)}
      <div class="EDImatchingHint">Wild card matching rules identify the message type.</div>
      </div>
   
   </div>
   `;
}

function EDImatchingRuleViewHtml(Data){
   if(!Data?.length) return "Currently there are no matching rules set."
   return /*html*/`
   <div class="EDImatchingRulesTable">
      <div class="EDImatchingHeader FORMlabel">#</div>
      <div class="EDImatchingHeader FORMlabel">RULE</div>
      <div class="EDImatchingHeader FORMlabel">NAME</div>
      ${EDImatchingRuleRowsHtml(Data)}
   </div>`;
}

function EDImatchingRuleRowsHtml(Data){
   let Html = '';
   for(RuleIdx in Data){ 
      let Rules = Data[RuleIdx];
      Html += /*html*/`
      <div class="EDImatchingValue">${parseInt(RuleIdx,10)+1}</div>
      <div class="EDImatchingValue">${Rules.rule}</div>
      <div class="EDInodeValue EDImatchingValueEnd" value="${ESChtmlEscape(Rules.name)}">${Rules.name}</div>`;
   }
   return Html;
}