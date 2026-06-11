function REPQhideButton(){
   let Button = document.querySelector('.REPQrepositionAction');
   if (Button.style.display != "none") { Button.style.display = "none"; }
}

function REPQshowButton(){
   let Button = document.querySelector('.REPQrepositionAction');
   if (Button.style.display != "inline-flex") { Button.style.display = "inline-flex"; }
}

function REPQshowFeedback(){
   let Feedback = document.querySelector('.REPQfeedback');
   Feedback.innerHTML="No log messages found. Try a later time.";
}

function REPQhideFeedback(){
   let Feedback = document.querySelector('.REPQfeedback');
   Feedback.innerHTML="";
}

function REPQshowMessageSelect(){
   let MessageSelect = document.querySelectorAll('.REPQselectContainer');
   MessageSelect.forEach((e)=>{
      if (e.style.display != "block") { e.style.display = "block"; }
   })
}

function REPQhideMessageSelect(){
   let MessageSelect = document.querySelectorAll('.REPQselectContainer');
   MessageSelect.forEach((e)=>{
      if (e.style.display != "none") { e.style.display = "none"; }
   })
}

function REPQnoMessagesFound(){
   REPQhideButton();
   REPQhideMessageSelect();
   REPQshowFeedback();
}

function REPQmessagesFound(){
   REPQshowButton();
   REPQshowMessageSelect();
   REPQhideFeedback();
}

function REPQaddLogMessages(Data){
   let MessageSelect = document.querySelector('.REPQselectMessage');
   MessageSelect.innerHTML = "";
   if (!Data.length){ REPQnoMessagesFound(); }
   else             { REPQmessagesFound();   }
   for(i in Data){
      let Option   = document.createElement("option");
      Option.value = Data[i].time + "-" + Data[i].id;
      Option.text  = Data[i].content;
      MessageSelect.appendChild(Option);
      REPQshowButton();
   }
}

function REPQgetLogsAroundTime(DefaultSource){ // called every time the time field changes and every time source changes
   const SelectedTime   = document.querySelector('.REPQselectTime');
   let SelectedSource = document.querySelector('.REPQselectSource').value;
   if (DefaultSource) { 
      COL_TRC("Default source provided - this is the first render of the reposition modal");
      SelectedSource = DefaultSource; 
   }

   if (!SelectedTime || !SelectedSource) return;
   let TimeInMs = new Date(SelectedTime.value);
   if (!isNaN(TimeInMs.getTime())) TimeInMs = TimeInMs.getTime();

   APIcall("logs/retrieve", { component : SelectedSource, start : TimeInMs.toString(), limit : 10, only_messages : true}, function(R){  // IMPORTANT: must keep type: "message" - we can only reposition to data messages
      if(!R.success) { 
         let ErrorDisplay = document.querySelector(".REPQrepositionError");
         if (ErrorDisplay) { ErrorDisplay.innerHTML = R.error; }
      }
      REPQmessages = R.data;
      let MessageSelect = document.querySelector('.REPQselectMessage');
      if(!MessageSelect) return; // handle error;
      REPQaddLogMessages(R.data);
   });
}
