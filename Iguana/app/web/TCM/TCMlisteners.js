function TCMsetListeners(){
   let Modal = document.querySelector('.TRANSwindow');
   if(!Modal) return;
   Modal.addEventListener("mouseover", COMcommitListMouseover);
}

function TCMgetCommitDiffs(row){
   let data = {};
   data.commit = COMgetCommitId() || "";
   data.relative_path = COMgetRelPath();
   let Api = "component/commit_diffs";
   APIcall(Api, data, function(R){
      COMcommitDiffData = R?.data || {};
      COMrenderDiffPopout(row);
   });
}
