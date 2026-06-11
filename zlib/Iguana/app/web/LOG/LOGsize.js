function LOGsizeData(){
   return MODELdata?.LOGsizeData?.data;
}

function LOGlogSizeData(){
   return MODELdata?.LOGlogComponent?.data || {};
}

function LOGcomponentLogSize(Data){
   let Id = LOGcomponentId() || "Iguana-Service";
   if(!Data[Id]) return;
   return STGfuzzySize(Data[Id]?.log_size);
}

function LOGsizeDisplay(){
   let LogData = LOGlogSizeData();
   if(!LogData) return;
   let E = document.querySelector(".LOGcomponentSizeValue");
   if(!E) return;
   let CompSize = LOGcomponentLogSize(LogData);
   if(!CompSize) return;
   if(E.innerHTML != CompSize) E.innerHTML = CompSize;
   let D = document.querySelector(".LOGtotalSizeValue");
   if(!D) return;
   let Size = LOGsizeData()?.log_dir?.dir_size;
   let TotalSize = STGfuzzySize(Size);
   if(D.innerHTML != TotalSize) D.innerHTML = TotalSize;
}

function LOGsizeDirDisplay(){
   let E = document.querySelector(".LOGlocation");
   let Data = LOGlogSizeData();
   if(!Data) return;
   let Id = LOGcomponentId() || "Iguana-Service";
   let Value = Data[Id]?.path;
   if(!E || !Value) return;
   let H = Value;
   if(E.innerHTML != H) E.innerHTML = H;
}