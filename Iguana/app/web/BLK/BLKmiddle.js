let BLKmax = 1001;

function BLKgetMiddle(){
   let Args = BLKmiddleArgs();
   APIcall('logs/retrieve', Args, function(R){
      BLKmiddleData = R?.data || [];
      BLKrenderMiddle();
      BLKrenderCount();
   });
}

function BLKellipsis(){
   let Data = {};
   Data.content = "...";
   return Data;
}

function BLKrenderMiddle(){
   let Data = BLKmiddleData;
   let Html = BLKmiddleHtml(Data);
   let Div = document.querySelector(".BLKmiddle");
   if(Div) Div.innerHTML = Html;
   if(Div && Data.length) Div.classList.add("BLKborder");
}

function BLKmiddleHtml(Data){
   let Html = "";
   if(Data.length == 0) return "<div class='BLKellipsis'>No logs found matching your criteria.</div>";
   for(let i = 0; i < Data.length && i < 1000; i++){
      let LogData = Data[i];
      const Time = LogData?.time || 0;
      const CurrentDate = LOGyearMonthDateTime({
          epoch: Time,
          includeTime: false,
          simpleTime: false,
          shortDayLabel: false
      });
      const DateRow = LOGneedsDateRow(i, Data) || i == 0 ? LOGrowDateHtml(CurrentDate) : "";
      let Row = LOGrowHtml(LogData, {}, DateRow, "", false);
      Html += Row;
   }
   if(Data.length > 1000) {
      let EndData = BLKlastData();
      const Time = EndData?.time || 0;
      const CurrentDate = LOGyearMonthDateTime({
          epoch: Time,
          includeTime: false,
          simpleTime: false,
          shortDayLabel: false
      });
      const DateRow = LOGrowDateHtml(CurrentDate);
      let EndHtml = `<div class='BLKellipsis'>...</div>` + LOGrowHtml(EndData, {}, DateRow, "", false);
      Html += EndHtml;
   }
   return Html;
}

function BLKrenderCount(){
   let Data = BLKmiddleData;
   let Count = Data.length;
   if(Count >= BLKmax) Count = "More than 1000";
   let Html = `<li>${Count} message${Count == 1 ? "" : "s"}</li>`;
   Html += "<li>Resubmission will proceed in chronological order <i>(starting with the oldest)</i></li>"
   let Div = document.querySelector(".BLKcount");
   if(Div) Div.innerHTML = Html;
}