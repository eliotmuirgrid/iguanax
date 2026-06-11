function LGGgeneratorModelInit(){
   MODELpollerAdd('LGGgeneratorModel', 'LGGgeneratorStatus', 1, 'log_generator/status', {});
   MODELpollerAdd('LGGgeneratorModel', 'LGGpermissionData',  11, 'component/permission', {});
   MODELviewAdd  ('LGGgeneratorModel', 'LGGpermissionError');
   MODELviewAdd  ('LGGgeneratorModel', 'LGGgeneratorUpdateOutput');
   MODELforcePoll('LGGgeneratorModel');
   MODELstart    ('LGGgeneratorModel');
}

function LGGpermissionData(){
   return MODELdata?.LGGpermissionData?.data?.log_generator;
}

function LGGgeneratorData(){
   return MODELdata?.LGGgeneratorStatus?.data;
}

function LGGgeneratorAdd(ComponentList){
   let ConsumerSelect = document.querySelector('.LGGgeneratorComponents');
   for(i in ComponentList){
      let Option   = document.createElement("option");
      Option.value = ComponentList[i].guid;
      Option.text  = ComponentList[i].name;
      ConsumerSelect.appendChild(Option);
   }
}

function LGGgeneratorUpdateComponents(){
   APIcall('component/list', {}, function(R){
      if (!R.success) { return; }
      let ComponentList = R.data.component_data;
      let ConsumerSelect = document.querySelector('.LGGgeneratorComponents');
      if(!ConsumerSelect || !ComponentList ) return;
      LGGgeneratorAdd(ComponentList);
   });
}

function LGGgeneratorScreenListeners(){
   let Container = document.querySelector('.LGGgeneratorContent');
   Container.removeEventListener('click',   LGGgeneratorClickListeners);
   Container.addEventListener   ('click',   LGGgeneratorClickListeners);
   Container.removeEventListener('change',  LGGgeneratorChangeListeners);
   Container.addEventListener   ('change',  LGGgeneratorChangeListeners);
}

function LGGgeneratorChangeListeners(e){
   if(e.target.closest('.LGGvolumeToGenerate')) { return LGGgeneratorUpdateSizeEstimate(); }
   if(e.target.closest('.LGGmonthsToGenerate')) { return LGGgeneratorUpdateSizeEstimate(); }
   if(e.target.closest('.LGGsizeToGenerate'))   { return LGGgeneratorUpdateSizeEstimate(); }
}

function LGGgeneratorClickListeners(e){
   if(e.target.closest('.LGGgeneratorRun')) { return LGGrunGenerator(); }
}

function LGGgetDaysDifference(Months){
   const CurrentDate = new Date();
   const NewDate = new Date(CurrentDate);
   NewDate.setMonth(NewDate.getMonth() - Months);
   // Calculate the difference in days
   const OneDay = 24 * 60 * 60 * 1000; // One day in milliseconds
   const TimeDifference = CurrentDate.getTime() - NewDate.getTime();
   return Math.floor(TimeDifference / OneDay);
}

function LGGgeneratorUpdateSizeEstimate(){
   const SizeEstimate = document.querySelector('.LGGgeneratorSizeEstimate');
   const Months = document.querySelector('.LGGmonthsToGenerate');
   const Volume = document.querySelector('.LGGvolumeToGenerate');
   const LogSize = document.querySelector('.LGGsizeToGenerate');
   let Size = (LGGgetDaysDifference(Months.value) * Volume.value * LogSize.value)/1073741824; // (# of months * 30 days * <logs per day> * LogSize characters per log message) / number of bytes in a gigabyte
   Size = Size * 1.15; // Add in some wiggle room for file metadata, folder metadata, etc. Better to overestimate than underestimate.
   Size = Size.toFixed(3); 
   SizeEstimate.innerHTML = Size + " gb";
}

function LGGgeneratorOutput(){
   return /*html*/ `
   <div class='LGGgeneratorOutput'> 
      <div class='LGGgeneratorDownloadDone'>Downloading</div>
      <div class='LGGgeneratorGenerationMonth'></div>
      <div class='LGGgeneratorGenerationDone'></div>
      <div class='LGGgeneratorComplete'></div>
   </div>`;
}

function LGGgeneratorUpdateOutput(){
   const Data = LGGgeneratorData();
   const OutputDiv = document.querySelector('.LGGgeneratorOutput');
   if (!OutputDiv) return;
   const DownloadFeedback = document.querySelector('.LGGgeneratorDownloadDone');
   Data.download ? DownloadFeedback.innerHTML = "Download complete" : DownloadFeedback.innerHTML = "Downloading...";

   const GenerationFeedback = document.querySelector('.LGGgeneratorGenerationMonth');
   GenerationFeedback.innerHTML = "Month " + Data.progress + " complete";
   const GenerationComplete = document.querySelector('.LGGgeneratorGenerationDone');
   Data.generation ? GenerationComplete.innerHTML = "Generation of log data complete" : GenerationComplete.innerHTML = "Generating logs...";
   
   const AllDone = document.querySelector('.LGGgeneratorComplete');
   if (Data.complete) {
      AllDone.innerHTML = "Done";
   }
   if (Data.error){
      AllDone.innerHTML = "Error: " + Data.error;
      GenerationFeedback.innerHTML = "";
      GenerationComplete.innerHTML = "";
   }
}

function LGGclearFormFields(){
   const Fields = document.querySelector('.LGGgeneratorConfig');
   Fields.remove();
}

function LGGaddViewLink(ComponentName, ComponentGuid){
   const Container = document.querySelector('.LGGgeneratorContentInner');
   let LogsLink = document.createElement("a");
   LogsLink.setAttribute("href", `#logs?component=${ComponentGuid}`);
   LogsLink.textContent = `View ${ComponentName} logs`;
   Container.appendChild(LogsLink);
}

function LGGaddComponentName(Name){
   let TextDiv = document.createElement("div");
   TextDiv.textContent = `Log generation: ${Name}`;
   let Target = document.querySelector(".LGGgeneratorProgress");
   let Parent = Target.parentNode;
   Parent.insertBefore(TextDiv, Target);
}

function LGGrunGenerator(){
   let Button = document.querySelector('.LGGgeneratorRun');
   Button.style.display = "none";
   let Data = {};
   const SelectedComponent = document.querySelector('.LGGgeneratorComponents');
   const Months = document.querySelector('.LGGmonthsToGenerate');
   const Volume = document.querySelector('.LGGvolumeToGenerate');
   const Rarity = document.querySelector('.LGGrarityToGenerate');
   const Size   = document.querySelector('.LGGsizeToGenerate');
   let ComponentName = SelectedComponent[SelectedComponent.selectedIndex].innerHTML;
   Data['component'] = SelectedComponent.value;
   Data['months'] = Months.value;
   Data['volume'] = Volume.value;
   Data['rarity'] = Rarity.value;
   Data['size'] = Size.value;
   LGGclearFormFields();
   LGGaddComponentName(ComponentName);
   LGGaddViewLink(ComponentName, Data['component']);

   APIcall('log_generator/start', Data, function(R){
      if (!R.success){
         let FeedbackArea = document.querySelector('.LGGgeneratorProgress');
         if (FeedbackArea) { FeedbackArea.innerHTML="Error: " + R.error; }
         return;
      }
      let Target = document.querySelector('.LGGgeneratorProgress');
      Target.innerHTML = LGGgeneratorOutput();
   });
}

function LGGgeneratorHtml(){
   return /*html*/`
   <div class="LGGgeneratorContainer">
      <div class="LOGheader">
         <div class="LOGheading">Log Generator</div>
      </div>
      <div class="LGGgeneratorContent">
      <div class="LGGgeneratorContentInner">
         <div class="LGGgeneratorUsage">This tool generates log messages that are about 1kb in size using Shakespeare's public works.</div>
         <br>
         <div class="LGGgeneratorConfig">
            <div class="FORMlabel">Generate logs for:</div>                              <select class="FORMselectBox LGGgeneratorComponents"></select>
            <div class="FORMlabel">Number of months of logs to generate:</div>           <input class="FORMinput LGGmonthsToGenerate" type="number" value="36"    min="1">
            <div class="FORMlabel">Number of logs per day to generate:</div>             <input class="FORMinput LGGvolumeToGenerate" type="number" value="1000"  min="1">
            <div class="FORMlabel">Number of logs between magic string insertions:</div> <input class="FORMinput LGGrarityToGenerate" type="number" value="10000" min="1">
            <div class="FORMlabel">Size of each log message (in bytes):</div>            <input class="FORMinput LGGsizeToGenerate"   type="number" value="1000"  min="1" max="5000000">
            <div class="FORMlabel">Approximate amount of log data generated:</div>       <div class="LGGgeneratorSizeEstimate"></div>
         </div>
         <br>
         <div class="LGGgeneratorRun BUTTONstandard BUTTONaction">RUN</div>
         <div class='LGGgeneratorProgress'></div>
      </div>
      </div>
   </div>
   `;
}

function LGGgeneratorScreen(){
   COL_TRC("Log Generator screen.");
   PAGEresetPage();
   CORpreparePage();
   CORheader();
   CORsetNavIcon('CORsettingsIcon');
   LGGgeneratorModelInit();
   document.querySelector('.CORmain').innerHTML = LGGgeneratorHtml();
   LGGgeneratorUpdateComponents();
   LGGgeneratorScreenListeners();
   LGGgeneratorUpdateSizeEstimate();
}
