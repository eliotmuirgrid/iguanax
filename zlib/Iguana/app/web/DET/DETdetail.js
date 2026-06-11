function DETresubmitUpdate(Type) {
   const IsDataMessage = Type === "message";
   const ResubmitBtn = document.querySelector('.DETresubmit');
   const ResubmitOrigBtn = document.querySelector('.DETresubmitOrig');
   if(!ResubmitBtn || !ResubmitOrigBtn) return;
   let Perm = DETpermissionData()?.resubmit_message;
   if(IsDataMessage && Perm) {
      ResubmitBtn.classList.remove("DETdisabled");
      ResubmitBtn.title = "Edit and Resubmit Message";
      ResubmitOrigBtn.classList.remove("DETdisabled");
      ResubmitOrigBtn.title = "Resubmit Message";
      return;
   } else {
      ResubmitBtn.classList.add("DETdisabled");
      ResubmitOrigBtn.classList.add("DETdisabled");
   }
   let title = !IsDataMessage ? "Must be a data message" : "You do not have permission to do this.";
   ResubmitBtn.title = title;
   ResubmitOrigBtn.title = title;
}

function DETsetMessageSize(Size){
   const S = document.querySelector('.DETmessageSize');
   S.innerHTML = STGfuzzySize(Size);
}

function DETsetMessageLocation(file){
   const S = document.querySelector('.DETlocation');
   if(S.innerHTML != file) S.innerHTML = file;
}

function DETupdateSampleDataExport(){
   const S = document.querySelector('.DETexport');
   if(!S) return;
   let Perm = DETpermissionData()?.export_sample_data;
   if(Perm) S.classList.remove("DETdisabled");
   else     S.classList.add("DETdisabled");
   let title = Perm ? "Export as Sample Data" : "You do not have permission to do this.";
   if(S.title != title) S.title = title;
}

function DETupdateDownloadButton(){
   const S = document.querySelector('.DETdownloadLink');
   if(!S) return;
   let Perm = DETpermissionData()?.download_logs;
   if(Perm) S.classList.remove("DETdisabled");
   else     S.classList.add("DETdisabled");
   let title = Perm ? "Download Log" : "You do not have permission to do this.";
   if(S.title != title) S.title = title;
   let href = Perm ? `log/download${PAGEformatParams(DETdetailArgs())}` : "";
   if(S.href != href) S.href = href;
}

function DETupdateRelatedMessages(Data){
   const Btn = document.querySelector('.DETrelated');
   if(!Btn) return;
   const IsDataMessage = LOGhasRelated(Data);
   let Perm = DETpermissionData()?.resubmit_message;
   let href = Perm && IsDataMessage ? `#logs/related${PAGEformatParams(DETdetailArgs())}` : "";
   if(Btn.href != href) Btn.href = href;
   
   if( href && Btn.href != href) Btn.href = href;
   if(!href && Btn.href != href) Btn.removeAttribute("href");

   if(IsDataMessage && Perm) {
      Btn.classList.remove("DETdisabled");
      Btn.title = "Context view";
      return;
   } else {
      Btn.classList.add("DETdisabled");
   }
   let title = !IsDataMessage ? "Must be a data message" : "You do not have permission to do this.";
   Btn.title = title;
}