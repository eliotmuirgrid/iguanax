function STNlisteners(){
   let div = document.querySelector(".SETmainContent");
   if(!div) return;
   div.addEventListener("click",  STNclick);
   div.addEventListener("change", SETnotificationActionsOnChange);
}

function STNclick(e){
   if (e.target.closest(".STNtest"))   STNtest();
   if (e.target.closest(".STNadd"))    STNaddRule();
   if (e.target.closest(".STNsave"))   STNsave();
   if (e.target.closest(".STNdelete")) STNdelete(e);
   if (e.target.closest(".STNcancel")) STNcancelEdit();
}

function STNaddRule() {
   let div = document.querySelector(".STNrulesBox");
   if(!div) return;
   SETappendTo(div, STNeditRowHtml());
   SETfocusLast(".SETnotificationCriteriaEditValue");
}

function STNdelete(e){
   let row = e.target.closest(".STNrulesRow");
   if(row) row.remove();
}

function STNtest() {
   let OperationalData = MODELdata.SEToperationalNotiData.data;
   if (!OperationalData.enabled) {
      return;
   }
   APIcall('notifications/test', null, function(R) {
      if (R.success) SNCKsnackbar('success', R.data, 20000);
      else           SNCKsnackbar('error',   R.error);
   });
}

function STNsave() {
   APIcall('notifications/set', SETprepareNotifications(), function(R) {
      if(R.error) { return SNCKsnackbar('error', R.error); }
      window.location = "#settings/notifications";
      let Message = GINsaveNotifications();
      GINaddAndCommit({message: Message, settings: true});
      MODELforcePoll('STNmodel');
      MODELforcePoll('CORmodel');
      MODELforcePoll('CORmodelSlow');
      MODELforcePoll('SETmodel');
   });
}

function STNcancelEdit(){
   STNeditMode  = false;
   MODELforcePoll('STNmodel');
   MODELforcePoll('CORmodel');
   MODELforcePoll('CORmodelSlow');
   MODELforcePoll('SETmodel');
}

function SETnotificationActionsOnChange(e){
   let row = e.target.closest(".STNrulesRow");
   if(e.target.closest('.SETnotificationsCondition')) STNupdateRow(row);
}

function STNupdateRow(Row){
   if(!Row) return;
   let Condition = Row.querySelector(".SETnotificationsCondition")?.value;
   let Data = {};
   Data.type = Condition;
   let Html = STNcriteriaEditHtml(Data);
   let Div = Row.querySelector(".STNcriteriaEdit");
   Div.innerHTML = Html;
}

function SETprepareNotifications() {
   let InvokedComponent = document.querySelector('.SETcomponentDropdown')?.value || "";
   let SaveData = {rules: [], component_id: InvokedComponent, is_on: MODELdata.SETselectedNotiData.data.is_on};
   SaveData = SETnotiGetRules(SaveData);
   console.log(SaveData);
   return SaveData;
}

function SETnotiGetRules(SaveData) {
   const rows = document.querySelectorAll('.STNrulesRow');
   rows.forEach(function(row) {
      const Data      = {tags: '', type: '', threshold: '', priority: '', limit: ''};
      const tagInput  = row.querySelector('.SETnotificationCriteriaEditValue');
      const TagString = tagInput ? tagInput.value.trim() : "";
      const Tags      = TagString.split(/\s+/);
      Data.tags       = Tags;
      if(!Tags) { return; }
      const Condition = row.querySelector('.SETnotificationsCondition');
      Data.type       = Condition.value;
      Data.priority   = row.querySelector('.SETprioritySelect').value.toLowerCase();
      Data.limit      = row.querySelector('.STNlimit').value;
      if(Data.type !== 'error_count') Data.threshold = row.querySelector('.STNthreshold')?.value?.trim() || "";
      else                            Data.threshold = 1;
      SaveData.rules.push(Data);
   });
   return SaveData;
}
