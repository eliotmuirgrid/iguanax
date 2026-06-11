let STGrulesData;

function STGupdateRules(){
   let div = document.querySelector(".STGrulesTarget");
   if(!div) return;
   let Rules = STGruleData();
   let Html = STGrulesHtml(Rules);
   if(!DEEPequal(STGrulesData, Rules)) div.innerHTML = Html;
   STGrulesData = Rules;
}

function STGpurgeRuleRowHtml(Rule) {
   const Days = Rule.days_to_keep;
   let EscapedRule = ESChtmlEscape(Rule.hash);
   return /*html*/`
   <div class="STGruleRow">
      <div class="STGruleData STGpaddingLeft"><a class="SETlink" href='#dashboard?filter=${EscapedRule}&whole_word=true'>${EscapedRule}</a></div>
      <div class="STGruleData">${Days} Day${Days == 1 ? "" : "s"}</div>
      <div class="STGruleData"></div>
   </div>`;
}

function STGrowsHtml(Rules){
   let Html = "";
   for(Rule of Rules) Html += STGpurgeRuleRowHtml(Rule);
   return Html;
}

function STGeditRowHtml(Rule){
   return /*html*/`
   <div class="STGruleRow">
      <div class="STGruleVal STGpaddingLeft">
         <input class="FORMinput STGhash" value="${ESChtmlEscape(Rule?.hash || "")}">
      </div>
      <div class="STGruleVal STGdaysRow">
         <input class="FORMinput STGdays" value="${Rule?.days_to_keep}" type="number">
         <div>Days</div>
      </div>
      <div class="SETpurgeRuleData STGpaddingRight">
         <div class="FORMbutton BUTTONcancel BUTTONwarning STGdelete">
            ${SETtrashIcon()}
         </div>
      </div>
   </div>`;
}

function STGrowEditHtml(Rules){
   let Html = ``;
   for (let Rule of Rules) Html += STGeditRowHtml(Rule);
   if(!Html) return STGeditRowHtml();
   return Html;
}

function STGeditRowsHtml(Rules){
   let Rows = STGrowEditHtml(Rules);
   return /*html*/`
   <div class="STGruleTable">
      <div class="STGruleHeading">
         <div class="STGheading STGpaddingLeft">Matching Rule</div>
         <div class="STGheading">Purge Logs Older than</div>
         <div class="STGheading"></div>
      </div>
      ${Rows}
   </div>
   ${STGaddRuleHtml()}`;
}

function STGaddRuleHtml(){
   return /*html*/`
   <div class="STGadd">
      <svg class="SETaddIcon" width="14" height="14">
         <use href="SET/SETiconAdd.svg#SETiconAdd"></use>
      </svg>
      PURGING RULE
   </div>`;
}

function STGrulesHtml(Rules){
   let View  = !STGeditPage();
   if(View && Rules.length == 0) return /*html*/ `<div class="SETemptyTable">No existing log purging rules - click EDIT to create log purging rules</div>`;
   if(!View) return STGeditRowsHtml(Rules);
   let Rows = STGrowsHtml(Rules);
   return /*html*/`
   <div class="STGruleTable">
      <div class="STGruleHeading">
         <div class="STGheading STGpaddingLeft">Matching Rule</div>
         <div class="STGheading">Purge Logs Older than</div>
         <div class="STGheading"></div>
      </div>
      ${Rows}
   </div>`;
}