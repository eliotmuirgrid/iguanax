let STGtestPurgeCard = ""

function STGtestPurgeRow(Row){
   let UsingAltDir = STGaltEnabled();
   return /*html*/`
   <div class="STGtestPurgeTableValue"><div>${Row.name}</div></div>
   <div class="STGtestPurgeTableValue"><div>${Row.rule}</div></div>
   <div class="STGtestPurgeTableValue"><div>${Row.size}</div></div>
   <div class="STGtestPurgeTableValue"><div>${Row.freed}</div></div>
   <div class="STGtestPurgeTableValue"><div>${UsingAltDir ? Row.alt_size : ""}</div></div>
   <div class="STGtestPurgeTableValue"><div>${UsingAltDir ? Row.alt_freed : ""}</div></div>
   `;
}



function STGtestAddPurgeRows(Dataset){
   let Out = "";
   for (i of Dataset) {
      Out += STGtestPurgeRow(i);
   }
   if(!Dataset?.length){
      Out = /*html*/`<div class="STGtestPurgeTableValue"><div>There are no logs to purge.</div></div>`
   }
   return Out;
}

function STGpurgeNowButtonHtml(){
   let Purge = STGeditPermission();
   let PurgeTable = STGpurgeDataSet?.Dataset?.length;
   let Classes = (Purge && PurgeTable !== 0) ? "BUTTONaction STGpurgeNow" : "BUTTONdisabled";
   let title =  !Purge ? `title="You do not have permission to perform this."` : "";
   return /*html*/`<div class="${Classes} BUTTONstandard" ${title}>PURGE NOW</div>`;
}

function STGtestPurgeInsertOtherDirHtml(){
   let PurgeOtherResult = STGpurgeTestOthersData();
   if (PurgeOtherResult.length <= 0) { return ""; }
   console.warn("have other dir data!");
   return /*html*/`
   <div class="STGtestPurgeOtherDirsContainer">
      The following non-component directories and files were found in the logs folder and will be archived when a purge occurs:
      <div class="STGtestPurgeOtherDirs"></div>
   </div>`
}



function STGtestPurgeCardHtml(){
   let PurgeTestResult = STGpurgeDataSet;
   let UsingAltDir = STGaltEnabled();
   console.log(UsingAltDir);
   return /*html*/`
   <div class="STGtestPurgeSummary">Based on the purge rules, these logs will be purged:</div>
   <div class="STGtestPurgeTableContainer">
      <div class="STGtestPurgeTable">
         <div class="STGtestPurgeHeader">
            <div class="STGtestPurgeHeading STGtestPurgeHeadingComponent STGsortDsc">Component</div>
            <div class="STGtestPurgeHeading STGtestPurgeHeadingRule">Rule</div>
            <div class="STGtestPurgeHeading STGtestPurgeHeadingTotalSize">Log Size</div>
            <div class="STGtestPurgeHeading STGtestPurgeHeadingFreed">Space Freed</div>
            <div class="STGtestPurgeHeading STGtestPurgeHeadingBackupSize">${UsingAltDir ? "Backup Size" : ""}</div>
            <div class="STGtestPurgeHeading STGtestPurgeHeadingBackupSize">${UsingAltDir ? "Backup Freed" : ""}</div>
         </div>
         <div class="STGtestPurgeRows">
         </div>
      </div>
   </div>
   ${STGtestPurgeInsertOtherDirHtml()}
   <div class="STGtestPurgeBottom">
      <div class="STGtotalPurged">Total Purged: ${STGfuzzySize(PurgeTestResult.TotalPurged)} ${UsingAltDir ? "/" + STGfuzzySize(PurgeTestResult.BackupPurged) : ""}</div>
      <div class="BUTTONgroup">
         <div class="BUTTONstandard BUTTONcancel STGpurgeNowCancel">CANCEL</div>
         ${STGpurgeNowButtonHtml()}
      </div>
   </div>
`;
}


function STGtestPurgeCardLoadingHtml(){
   return /*html*/`
   <div class="STGtestPurgeContent">
      <div class="STGtestPurgeTableLoading"><div class="STGtestPurgeSpinnerText">Calculating log usage, please wait...</div><div class="INITspinner"></div></div>
   </div>
`;
}

function STGinsertPurgeTable(){
   const Target = document.querySelector('.STGtestPurgeTableLoading');
   if (Target){ // Replaces the loading spinner with the table headings and purge buttons. Rows get added later
      let NewTable = document.createElement("div");
      NewTable.classList.add("STGpurgeTestTable");
      NewTable.innerHTML = STGtestPurgeCardHtml();
      Target.parentNode.replaceChild(NewTable, Target);
   }
}

function STGupdatePurgeTableRows(){
   let PurgeTestResult = STGpurgeDataSet.Dataset;
   const Target = document.querySelector(".STGtestPurgeRows");
   if (Target) {
      Target.innerHTML = STGtestAddPurgeRows(PurgeTestResult);
   }
}

function STGupdatePurgeOtherDirs(){
   let PurgeOtherResult = STGpurgeTestOthersData();
   if (PurgeOtherResult.length <= 0){ return; }
   const Target = document.querySelector(".STGtestPurgeOtherDirs");
   if (Target) {
      Target.innerHTML = PurgeOtherResult.join(", ");
   }
}

function STGtestPurgeListeners(){
   const Parent = document.querySelector('.STGpurgeTestCard');
   Parent.removeEventListener('click', STGpurgeClickListeners);
   Parent.addEventListener   ('click', STGpurgeClickListeners);
}

function STGpurgeClickListeners(e){
   if (SETtargetClassClicked(e, "STGpurgeNow"))                  STGtestPurgeNow();
   if (SETtargetClassClicked(e, "STGpurgeNowCancel"))            STGtestPurgeNowCancel();
   if (SETtargetClassClicked(e, "STGtestPurgeHeadingComponent")) STGpurgeSortAndUpdate("name");
   if (SETtargetClassClicked(e, "STGtestPurgeHeadingTotalSize")) STGpurgeSortAndUpdate("size_raw"); // important to use raw values - don't want 12 B to be greater than 1 kB
   if (SETtargetClassClicked(e, "STGtestPurgeHeadingRule"))      STGpurgeSortAndUpdate("rule");
   if (SETtargetClassClicked(e, "STGtestPurgeHeadingFreed"))     STGpurgeSortAndUpdate("freed_raw");
}

function STGtestPurgeNow(){
   CONFIRMaction(`Are you sure you want to purge logs now?<br/><br/><b>Warning:</b> This action cannot be reversed.`, function() {
      SNCKsnackbar('success', "Purge started", 6000);
      POPpopoutClose(STGtestPurgeCard);
      APIcall("log/purge_rules/purge", {}, function(R) {
         if(!R.success){ SNCKsnackbar('error', "Failed to purge logs"); }
         else { window.location.hash = "logs?search=%23purge"; }
      });
   }, "DELETE");
}

function STGtestPurgeNowCancel() { POPpopoutClose(STGtestPurgeCard); }
function STGtestPurgeOnCardClose(){
   STGtestPurgeCard = "";
   STGclearPurgeTestGlobals();
}

function STGbuildPurgeDatasetRow(StatsRow, PurgeRow, Out){
   let Temp       = {}
   Temp.name      = StatsRow['name'];
   Temp.rule      = PurgeRow.rule;
   Temp.size_raw  = Number(StatsRow.log_size);
   Temp.size      = STGfuzzySize(StatsRow.log_size);
   Temp.freed_raw = Number(PurgeRow.amount_freed);
   Temp.freed     = STGfuzzySize(PurgeRow.amount_freed);
   Temp.alt_size_raw  = Number(StatsRow.alt_log_size);
   Temp.alt_size      = STGfuzzySize(StatsRow.alt_log_size);
   Temp.alt_freed_raw = Number(PurgeRow.alt_amount_freed);
   Temp.alt_freed     = STGfuzzySize(PurgeRow.alt_amount_freed);
   Out.Dataset.push(Temp);
}

function STGbuildPurgeDataset(Stats, PurgeData){
   let Out = {}
   Out.Dataset = [];
   Out.TotalPurged = 0;
   Out.BackupPurged = 0;
   const StatsKeys = Object.keys(Stats);
   const PurgeKeys = Object.keys(PurgeData);
   for (key of StatsKeys){
      if (PurgeKeys.includes(key)){ // log/stats and /log/purge_rules/test can disagree.
         STGbuildPurgeDatasetRow(Stats[key], PurgeData[key], Out);
         Out.TotalPurged += Number(PurgeData[key].amount_freed);
         if (PurgeData[key].alt_amount_freed) {
            Out.BackupPurged += Number(PurgeData[key].alt_amount_freed);
         }
      }
   }
   if (Out.BackupPurged === 0) { delete Out.BackupPurged; }
   console.log(Out);
   return Out;
}

function STGtestPurgeRules(){
   STGtestPurgeCard = POPpopoutOpen({
      title:'Purge Logs',
      content: STGtestPurgeCardLoadingHtml(),
      width: '80%',
      maxWidth: '1200px',
      height: '80%',
      className: 'STGpurgeTestCard',
      esc_close : true,
      callback: STGtestPurgeOnCardClose
   });
   STGtestPurgeListeners();
   STGpurgeTestModelInit();
}