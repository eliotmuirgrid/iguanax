function COMupdateView(Div, Index, View, Data){
   let Key = Object.keys(Data)[Index];
   let FileData = Data[Key];
   let Max = FileData?.summary?.maxLinesReached;
   let DiffHtml = HSVdiffBlock(FileData, View, Max);
   let div = Div.querySelector('.HSVdiff');
   div.innerHTML = DiffHtml;
}

function COMcommitDetails(Data){
   let Message = Data?.message || "";
   let Author = Data?.author || "";
   let TimeStamp = Data?.timestamp || "";
   let Day = LOGdayFormat(TimeStamp + '000');
   let FullTimeHtml = LOGtimeFormat(TimeStamp + "000", false, false);
   let Time = FullTimeHtml.split(' ').slice(-1)[0];
   let Hash = Data?.hash || "";
   let Path = Data?.path ? Data.path : "";
   let PathTitle = Path ? Path : "";
   let PathClass = Path ? " PRJicon-closedDir-library-unlocked" : "";
   let FolderHtml = !Path ? "" : /*html*/`
   <div class="COMfolderDetails">
      <div class="COMcommitListType${PathClass}"></div>
      <div class="COMcommitListType" title="${PathTitle}">${Path}</div>
   </div>
   `;
   let RevertButton = COMonDashboard() ? "" : `<div class="BUTTONdefault BUTTONstandard COMrestore">Restore to this Commit</div>`;
   return /*html*/`
   <div class="COMdetails">
      <div class="COMrowDate">
         <div class="COMcommitRowDate">${Day} at ${Time}</div>
         ${RevertButton}
      </div>
      <div class="COMcommitRowDetails">
         <div class="COMmessage">${Message}</div>
         <div class="COMcommitHash">${Hash}</div>
      </div>
      <div class="COMauthor">${Author}</div>
      ${FolderHtml}
   </div>
   `;
}

function COMcloseButton(){
   return /*html*/`
   <div class="COMcloseButtonContainer">
      <div class="COMcloseButton">
         <img src="/DASH/DASHclearSearch.svg">
      </div>
   </div>
   `;
}

function COMdiffHtml(Data, Data2){
   let Html = /*html*/`${COMcommitDetails(Data2)}<br><div class="COMdivHalf">`;
   let View = STORElocalRead('COMviewSelected');
   if(View !== "condensed") View = "full";
   for(let file in Data){
      let diffData = Data[file];
      Html += HSVdiffTemplate(diffData, file, View);
   }
   Html += '</div>';
   return Html;
}