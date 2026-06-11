function LBUbuttonUpdate(){
   let div = document.querySelector(".TRGTlibraryUsage");
   if(!div) return;
   let ShowDot = LBUlibsAheadOrBehind();
   TRGTtoggleClass('.TRGTlibraryUsage', 'TRGTblueDot', ShowDot); 
}