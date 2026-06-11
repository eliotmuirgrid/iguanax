function DEPtooltips(){
   let Deps = document.querySelectorAll('.cm-DEPdeprecated');
   Deps.forEach(dep => {
      let FunctionName = dep.innerHTML;
      DEPhelpDocData(FunctionName, dep);
   });
}

function DEPfunctionToPath(FunctionName){
   let Data = DEPhelpIndexData();
   for(idx in Data){
      if(Data[idx].name == FunctionName){
         return Data[idx].location;
      }
   }
   return null;
}

function DEPhelpDocData(FunctionName, dep){
   let Path = DEPfunctionToPath(FunctionName);
   if(!Path) return null;
   APIcall("component/help", {path: Path, name : FunctionName, component: TRANScomponentGuid()}, function(R){
      if(!R.success) return;
      let Data = R.data;
      Data.Name = FunctionName;
      let Content = DEPtooltipHtml(Data);
      POPtooltip({
         target: dep,
         content: Content, 
         type: POPtooltipType.INFO, 
         style: 'DEPtooltip', 
         autoShow: false,
         event: POPtooltipEvent.CLICK,
         position: POPtooltipPosition.AUTO,
         keepExisting: true,
         yOffset: -15
      });
   });
}

