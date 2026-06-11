function ACTdownloadFile(CurrentTreeNode){
   console.log(CurrentTreeNode);
   const Filepath = CurrentTreeNode.path;
   const FileName = CurrentTreeNode.name;
   MENUremoveTooltip();
   const Data = { name : FileName,file: Filepath, component: TRANScomponentGuid(), development : TRANSdevelopment()};
   let Url = "file/download" + PAGEformatParams(Data);
   window.open(Url, "_blank");
}