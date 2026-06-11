let COMpreviousDate;

function COMlistUpdate(){
   let div = document.querySelector('.COMmodal');
   if(!div) return;
   let Data = COMcommitData();
   let Error = COMcommitDataError();
   if(Error) return div.innerHTML = /*html*/`<div class="COMerror">Cannot view Commit History: <br> <br> ${Error}</div>`;
   if(DEEPequal(Data, COMrenderedCommitMap)) return;
   let html = COMhtml();
   if(html) html = html + COMbufferHtml();
   div.innerHTML = html;
   COMrenderedCommitMap = COMcommitData();
}

function COMcommitRowHtml(commitData){
   let CommitMessage = commitData?.message || "No commit message.";
   let Time = commitData?.timestamp || '0';
   let Author = commitData?.author || "";
   let TimeStamp =  LOGtimeFormat(Time + "000", false, false);
   let Title = `${Author} committed at ${TimeStamp}` + (commitData?.path ? ` in library ${commitData?.path}.` : ".");
   let DateHtml = LOGdayFormat(Time * 1000);
   let DateRow = "";
   if (DateHtml !== COMpreviousDate) {
      DateRow = /*html*/  `${COMcommitDateHtml(DateHtml, false)}`;
      COMpreviousDate = DateHtml;
   }
   let Lib = commitData?.path ? " PRJicon-closedDir-library-unlocked" : "";
   return /*html*/`
   ${DateRow}
   <div class="COMcommitRowContainer"><div class="COMcommitRow${Lib}" title="${Title}">${CommitMessage}</div></div>`;
}

function COMbufferHtml(){
   return /*html*/`<div class="COMrowBuffer"></div>`;
}

function COMscreen(){
   return TRANSviewingTranslator() ? "translator" : "dashboard";
}

function COMhtml(){
   let html = "";
   let Data = COMcommitList[COMscreen()];
   if(!Data?.length) return html;
   COMpreviousDate = "";
   for(commit in Data){
      let commitData = Data[commit];
      html = html + COMcommitRowHtml(commitData) ;
   }
   return html;
}
