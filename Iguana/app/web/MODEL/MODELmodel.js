/*  This is part of critical architecture of our product.  Do not change it without having
a thoughtful architectural discussion with Eliot.  Thanks!
*/
let MODELdata={};
let MODELpollers=[];
let MODELviews=[];
let MODELlastUpdate = {};
let MODELpendingView = {};

let MODELupdateThreshold = 500;

function MODELpollerExists(ModelName, Name){
   for(let i = 0; i < MODELpollers.length; i++){
      if(MODELpollers[i].model === ModelName && MODELpollers[i].name === Name) return true;
   }
   return false;
}

//MODELpollerAdd('INITmodel','INITgitInfo', 2.5, 'component/git/info', {component : CARCcomponentId()}}
function MODELpollerAdd(ModelName, Name, PollTimeInSeconds, ApiPath, Params, Condition){
   if(MODELpollerExists(ModelName, Name)){
      COL_TRC("Poller with name "+ Name + " already exists for model " + ModelName);
      return;
   }
   let Entry ={
      time : PollTimeInSeconds,
      condition : Condition,
      name : Name,
      path : ApiPath,
      params : Params, 
      model : ModelName, 
      id : 0,
      request : 0
   }
   COL_TRC(Entry);
   MODELpollers.push(Entry);
} 

function MODELschedulePaint(ModelName, When){
   if (MODELpendingView[ModelName]){
      COL_TRC("Paint scheduled already.");
      return;
   }
   let Difference = When;
   COL_TRC("Schedule update in ",Difference, " milliseconds");
   MODELpendingView[ModelName] = setTimeout(function() { 
      MODELpendingView[ModelName] = 0;
      MODELupdate(ModelName, "scheduledPaint"); 
   }, Difference);
}

function MODELclearPendingView(ModelName){
   if(!MODELpendingView[ModelName]) return;
   COL_TRC("Clear pending view ", ModelName);
   clearTimeout(MODELpendingView[ModelName]);
   delete MODELpendingView[ModelName];
}

function MODELclearPollers(IdxArray){
   let DscArray = IdxArray.sort(function(a, b) { return b - a; });
   // Important we go in descending order for item removal
   COL_TRC("Removing pollers ", IdxArray);
   for (let i = 0; i < DscArray.length; i++){
      let Idx = DscArray[i];
      MODELclearPollerTimer(Idx);
      MODELclearPollerRequest(Idx);
      MODELpollers.splice(Idx, 1);
   }
}

function MODELclearPollerTimer(i){
   if (!MODELpollers[i].id) return;
   COL_TRC("Cancel poller timer ", i, MODELpollers[i].path, MODELpollers[i].id);
   clearInterval(MODELpollers[i].id);
}

function MODELclearPollerRequest(i){
   if (!MODELpollers[i].request) return;
   COL_ERR("Abort HTTP request:", MODELpollers[i].path);
   MODELpollers[i].request.abort();
}

function MODELclearData(ModelName){
   COL_TRC("Clear data for model "  + ModelName);
   delete MODELdata[ModelName];
}

function MODELclearViews(ModelName){
   COL_TRC("Clear views for model "  + ModelName);
   for(let i = MODELviews.length - 1; i >= 0; i--){
      if (MODELviews[i].model === ModelName){
         MODELviews.splice(i, 1);
      }    
   }
}

function MODELremove(ModelName){
   COL_TRC("Request model removal of "  + ModelName);
   MODELclearPendingView(ModelName);
   let ModelPollers = MODELgetPollers(ModelName);
   MODELclearPollers(ModelPollers);
   MODELclearViews(ModelName);
   MODELclearData(ModelName);
}

function MODELshutdownAll(){
   COL_TRC("Shutting down all models.");
   for (let ModelName in MODELpendingView){
      MODELclearPendingView(ModelName);
   }
   let AllPollers = Object.keys(MODELpollers);
   MODELclearPollers(AllPollers);
   MODELdata = {};
   MODELviews = [];
} 

function MODELonPollerData(P, R, Callback, saveResp = true){
   COL_TRC(P.path, R);
   if(saveResp) MODELdata[P.name] = R;
   P.request = 0;  
   if (Callback) { Callback(P); }
}

// if a poller has not returned, don't call it again
function MODELpollerRun(P, Callback){
   COL_TRC("Model pollers ", MODELpollers);
   COL_TRC(P);
   // I store the request object so we can cancel it.
   COL_TRC("Request " + P.path);
   let Params =  P.params;
   let Condition = P.condition;
   if(Condition && !Condition()) return MODELonPollerData(P, null, Callback, false);
   // parameter values need to be populated when we are calling the api for some
   if(typeof Params === "function") {
      COL_TRC("Parameters argument is a function!");
      Params = Params();
   }
   if(P.request) {
      COL_TRC("Don't need to call the poller, one is already running for it!");
      return;
   }
   P.request = APIcall(P.path, Params, function(R){
      MODELonPollerData(P, R, Callback); // For tracing to work I call a different function
   });
}

function MODELgetPollers(ModelName){
   let Set = [];
   for (let i=MODELpollers.length-1; i >= 0; i--){
      if (MODELpollers[i].model === ModelName){
         Set.push(i);
      }
   }
   return Set;
}

function MODELforcePoll(ModelName, Callback){
   COL_TRC(ModelName);
   var PollerSet = MODELgetPollers(ModelName);
   COL_TRC(PollerSet);
   var Count = PollerSet.length; // count all the pollers matching the model
   var Func = function(P) {
      Count--;
      COL_TRC(P.path + " completed.");
      if (0 == Count){
         COL_TRC("Completed all polls.");
         if (Callback){
            Callback();
         }
         MODELupdate(ModelName, "forcePoll");
      }
   }
   for (let i = 0; i < PollerSet.length; i++){
      let P = MODELpollers[PollerSet[i]];
      MODELpollerRun(P, Func);
   }
}

function MODELviewExists(ModelName, ViewFunc){
   for(let i = 0; i < MODELviews.length; i++){
      if(MODELviews[i].model === ModelName && MODELviews[i].name === ViewFunc) return true;
   }
   return false;
}

function MODELviewAdd(ModelName, ViewFunc){
   if(MODELviewExists(ModelName, ViewFunc)){
      COL_TRC("View "+ ViewFunc + " already exists for model " + ModelName);
      return;
   }
   let Entry={
      model : ModelName,
      call : window[ViewFunc],
      name : ViewFunc
   };
   COL_TRC(Entry);
   MODELviews.push(Entry);
}

function MODELgetTime(){
   const Time = new Date();
   return Time.getTime();
}

function MODELupdate(ModelName, Who){
   COL_TRC("Request update for " + ModelName + " because of " + Who);
   let CurrentTime = MODELgetTime(); 
   if (MODELlastUpdate[ModelName] + MODELupdateThreshold > CurrentTime){
      COL_TRC("Last update was too recent.  Not updating views now.");
      MODELschedulePaint(ModelName, MODELlastUpdate[ModelName] + MODELupdateThreshold - CurrentTime);
      return;
   }
   MODELlastUpdate[ModelName] = CurrentTime;
   
   COL_TRC("Update views for " + ModelName);
   for(let i=0; i< MODELviews.length; i++){
      if (MODELviews[i].model === ModelName){
         if(MODELviews[i].call) MODELviews[i].call();
         else console.log("Cannot find the model view: " + MODELviews[i].name);
      }    
   }
}

function MODELpollerFromId(PollerId){
   for (let i = 0; i < MODELpollers.length; i++){
      let P = MODELpollers[i];
      if(P.id === PollerId) {
         COL_TRC("Found the poller " + P.name + " with the corresponding poller id " + PollerId);
         return P;
      }
   }
   COL_TRC("Could NOT find the poller with the corresponding poller id " + PollerId);
}

function MODELstart(ModelName){
   COL_TRC(ModelName);
   let PollerSet = MODELgetPollers(ModelName);
   for (let i = 0; i < PollerSet.length; i++){
      let idx = PollerSet[i];
      let P = MODELpollers[idx];
      if(P.id) {
         COL_TRC("This poller has already been set and started");
         continue;
      }
      let Time = P.time * 1000;
      const PollerId = setInterval(function(){
         // Since the index can change we need to compute the i based on the id (since that doesn't change)
         let Poller = MODELpollerFromId(PollerId);
         if(!Poller) return;
         MODELpollerRun(Poller, function(){ MODELupdate(ModelName, Poller.path)});
      }, Time);
      P.id = PollerId;
   }
}
