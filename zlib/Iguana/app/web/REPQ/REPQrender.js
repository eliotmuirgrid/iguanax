let REPQcard = "";
let REPQmessages = [];

function REPQhtml(){
   return /*html*/`
   <div class="REPQmodal">
      <div class="REPQcontainingDiv">
         <div class="REPQchooseTimeAndMessage"> Specify a timeframe to get a list of messages the queue can be repositioned to, then select the message you want to use.
            <a target="_blank" class="REPQmoreInfo" href="${LINKrepositionInfo}">ⓘ</a>
            <br>
            <br>
            <div class="REPQcontrols">
               <div>Select source queue to reposition:</div>                           <select class="REPQselectSource"></select>
               <div>Set reposition time:</div>                                         <input class="REPQselectTime" type="datetime-local"/>
               <div class="REPQselectContainer">Select message to reposition to:</div> <select class="REPQselectContainer REPQselectMessage"></select>
            </div>
         </div>
         <div class="REPQfeedback"></div>
         <br>
         <br>
         <div class="BUTTONgroup REPQbuttonGroup">
            <div class="BUTTONstandard BUTTONcancel REPQrepositionCancel">CANCEL</div>
            <div class="BUTTONstandard BUTTONaction REPQrepositionAction">AFTER THIS MESSAGE</div>
         </div>
         <div class="REPQrepositionError"></div>
      </div>
   </div>
   `;
}

function REPQhtmlNoSources(){
   return /*html*/`
   <div class="REPQmodal">
      <div class="REPQcontainingDiv">
         <div class="REPQnoSources">To use this feature, first connect one or more upstream components to this component.</div>
         <br>
         <br>
         <div class="REPQbuttonGroup">
            <a class="BUTTONstandard BUTTONdefault" href="#dashboard/details?component=${PAGEhashParameter('component')}">CLOSE</a>
         </div>
      </div>
   </div>
   `;
}

function REPQscreen() { REPQrender(); }

function REPQrender(){
   if(PAGEhash() === "dashboard/reposition_queue") DASHdashboardScreen();
   APIcall("log/queue/sources",  {component : PAGEhashParameter('component')}, function(R){
      REPQfinishRender(R);
   });
}

function REPQfinishRender(Response){
   let CardBody = "";
   let Empty = Response.data.length == 0;
   let Success = Response.success;
   if (Empty) CardBody = REPQhtmlNoSources();
   else       CardBody = REPQhtml(); 
   if (!Success) CardBody = Response.error;
   let Width = Empty ? "600px" : "50%";
   if(document.querySelector('.REPQmodal')) { return; }
   REPQcard = POPpopoutOpen({
      title: 'Reposition Queue',
      width: Width,
      minWidth: '800px',
      maxWidth: '800px',
      content: CardBody,
      esc_close : true,
      callback : REPQclose
   });
   if(Empty || !Success) return;
   APIcall("log/queue/oldest", {component : Response.data[0].guid}, function(R){
      if (!R.success) {
         document.querySelector('.REPQcontainingDiv').innerHTML = "Error getting time information: " + R.error;
         return;
      }
      REPQsetCurrentTime(R.data);
      REPQgetLogsAroundTime(Response.data[0].guid);
      REPQmodelInit();
   });
}

function REPQclose() {
   MODELremove('REPQmodel');
   let Filter = PAGEhashParameter("filter");
   let Url = '#dashboard/details?component=' + REPQcomponentId().component;
   if(Filter) Url = Url + "&filter=" + Filter;
   window.location.hash = Url;
}

function REPQsetCurrentTime(Data){
   let SelectTime = document.querySelector('.REPQselectTime');
   if (!SelectTime) return;
   const Newest     = new Date(Number(Data.newest));
   const Oldest     = new Date(Number(Data.oldest));
   SelectTime.value = LOGepochToTimeSelect(Newest);
   SelectTime.min   = LOGepochToTimeSelect(Oldest);
}
