
function HSVdiffBlock(fileData, View, Max){ 
   let html;
   if(View === 'full'      && fileData?.full)      html = TGITfullDiff(fileData?.full?.from, fileData?.full?.to);
   if(View === 'condensed' && fileData?.condensed) html = `<div class='TGITcondensedDiff'>${fileData?.condensed}</div>`;
   if(!html) html = `<div class="HSVdiffBlock">${fileData?.status}</div>`;
   if(Max) html += `<div class="HSVdiffMaxLines">-- Lines truncated, only showing the first ${Max} lines --</div>`;
   return html;
}

function HSVdiffTemplate(Params, fileName, View){ 
   let Max = Params?.summary?.maxLinesReached;
   let Added = HSVgetInt(Params?.summary?.added, Max);
   let Deleted = HSVgetInt(Params?.summary?.deleted, Max);
   let Changed = HSVgetInt(Params?.summary?.changed, Max);
   let DiffHtml = HSVdiffBlock(Params, View, Max);
   let ChangesOnly = View === 'condensed' ? " selected" : "";
   let SideBySide  = View === 'full'      ? " selected" : "";
   return /*html*/`
   <div class="HSVdiffWrapper">
      <div class="HSVdiffToolbar">
         <div class="HSVdiffToolbarToggle"></div>
         <div class="HSVtoolbarFilename">${fileName}</div>
         <div class="HSVtoolbarLines">
            <span class="HSVlinesInfo">Added:
               <span class="HSVlinesAdded">${Added}</span>
            </span>
            <span class="HSVlinesInfo">Deleted:
               <span class="HSVlinesDeleted">${Deleted}</span>
            </span>
            <span class="HSVlinesInfo">Changed:
               <span class="HSVlinesChanged">${Changed}</span>
            </span>
         </div>
         <div class="HSVtoolbarDisplay">
            <select class="FORMselectBox HSVdiffViewSelect" title="Change display view">
               <option value="condensed"${ChangesOnly}>Changes Only</option>
               <option value="full"${SideBySide}>Side by Side</option>
            </select>
         </div>
      </div>
      <div class='HSVdiff'>${DiffHtml}</div>
   </div>`;
}

function HSVgetInt(value, max){
   if(!value) return "";
   if(max && value == max) return value + "+";
   return value;
}