function PRJopenPdf(FilePath){
   let FullPath = "file:///" + TRANSprojectRoot() + FilePath;
   const newTab = window.open(FullPath, '_blank');
   if (!newTab) {
      SNCKsnackbar('error', "Could not open pdf: the browser might have blocked the new tab due to popup blockers");
   }
}

