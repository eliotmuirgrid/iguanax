function LSTupdateBar(){
   let Line = document.querySelector(".LSTcomplete");
   if(!Line) return;
   let Percent = LSTpercentComplete();
   if(Percent > 97 ) Percent = 100;
   let PercentString = Percent + "%";
   if(Line.style.left != PercentString) Line.style.left = PercentString;
}

function LSTpercentComplete(){
   let Time = LSTtime();
   let StartTime = LSTstartTime();
   let EndTime = LSTendTime();
   let Percent = ( Time - StartTime ) / (EndTime - StartTime) * 100;
   let Done = LSTdone();
   if(Done) Percent = 100;
   return Percent;
}