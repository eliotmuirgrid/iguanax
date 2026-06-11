function TCMlistUpdate(){
   let div = document.querySelector('.TRANSgitHistoryTargetScroller');
   if(!div) return;
   let Data = COMcommitData();
   if(DEEPequal(Data, COMrenderedCommitMap)) return;
   let html = COMhtml();
   div.innerHTML = html;
   let fadeDiv = document.createElement('div');
   fadeDiv.className = 'TRANScommitListFade';
   div.appendChild(fadeDiv);
   COMrenderedCommitMap = COMcommitData();
}

function TCMhtml(){
   let html = "";
   let Data = TCMcommitList;
   for(commit in Data){
      let commitData = Data[commit];
      html = html + COMcommitRowHtml(commitData) ;
   }
   return html;
}