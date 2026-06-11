function ACTviewPdfFile(FilePath){
   MENUremoveTooltip();
   const Data = { file: FilePath, component: TRANScomponentGuid(), development : TRANSdevelopment()};
   let Url = "file/pdf_view" + PAGEformatParams(Data);
   window.open(Url, "_blank");
}