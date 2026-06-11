function STGlisteners(){
   let div = document.querySelector(".STGmodal");
   if(!div) return;
   div.addEventListener("click", STGclick);
}

function STGclick(e){
   if(e.target.closest(".STGtest"))   return STGtestPurgeRules();
   if(e.target.closest(".STGadd"))    return STGaddPurgeRule();
   if(e.target.closest(".STGdelete")) return STGdelete(e);
   if(e.target.closest(".STGsave"))   return STGsave();
}

function STGdelete(e){
   let row = e.target.closest(".STGruleRow");
   if(row) row.remove();
}

function STGaddPurgeRule(){
   let div = document.querySelector(".STGruleTable");
   if(!div) return;
   SETappendTo(div, STGeditRowHtml());
   SETfocusLast(".STGhash");
}

function STGpreparePurgeRules() {
   const SETpurgingEditRows = document.querySelectorAll('.STGruleRow');
   const Data = [];
   SETpurgingEditRows.forEach(function(row) {
      const Hash = row.querySelector('.STGhash').value.trim();
      let Days = row.querySelector('.STGdays').value.trim();
      Data.push({ hash: Hash, days_to_keep: Days });
   });
   return Data;
}

function STGsave() {
   APIcall('log/purge_rules/set', STGpreparePurgeRules(), function(R) {
      if(R.success) {
         let Message = GINsavePurgeRules();
         GINaddAndCommit({message: Message, settings: true});
      }
      window.location = "#settings/purging";
   });
}