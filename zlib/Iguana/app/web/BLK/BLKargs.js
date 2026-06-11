let BLKstartArgs = {};
let BLKendArgs   = {};

function BLKsingleResubmit(){ return PAGEhash() == "logs/details"; }

function BLKsingleArgs(Target){
   let Args = {};
   Args.component = LOGcomponentId();
   Args.limit     = 1;
   Args.start     = BLKstartArgs.time;
   Args.start_id  = BLKstartArgs.id;
   if(Target) Args.target = Target;
   return Args;
}

function BLKmiddleArgs(){
   if(BLKsingleResubmit()) return BLKsingleArgs();
   let Args = BLKbasicArgs();
   Args.component = LOGcomponentId();
   Args.limit     = 1001;
   Args.start     = BLKstartArgs.time;
   Args.start_id  = BLKstartArgs.id;
   Args.end       = BLKendArgs.time;
   Args.end_id    = BLKendArgs.id;
   return Args;
}

function BLKresubmitArgs(Target){
   if(BLKsingleResubmit()) return BLKsingleArgs(Target);
   let Args       = {};
   Args.component = LOGcomponentId();
   Args.search    = PAGEhashParameter("search");
   Args.tags      = PAGEhashParameter("tags");
   Args.end       = BLKstartArgs.time;
   Args.end_id    = BLKstartArgs.id;
   Args.start     = BLKendArgs.time;
   Args.start_id  = BLKendArgs.id;
   Args.target    = Target;
   return Args;
}

function BLKgetStart(){
   if(BLKsingleResubmit()) return BLKsingleGetStart();
   BLKgetBulkStart();
}

function BLKsingleGetStart(){
   let Args = {};
   Args.component = PAGEhashParameter("component");
   Args.time = PAGEhashParameter("time");
   Args.id   = PAGEhashParameter("id");
   BLKstartArgs = Args;
}

function BLKgetBulkStart(){
   let Val  = BLKstartDiv()?.value || "";
   let Args = PAGEargs(Val);
   let To = PAGEhashParameter("to");
   if(!Args.time && To){
      Args.time = To;
      Args.id = "";
   }
   BLKstartArgs = Args;
}

function BLKgetEnd(){
   if(BLKsingleResubmit()) return BLKsingleGetEnd();
   BLKgetBulkEnd();
}

function BLKsingleGetEnd(){
   let Args = {};
   BLKendArgs = Args;
}

function BLKgetBulkEnd(){
   let Val  = BLKendDiv()?.value || "";
   let Args = PAGEargs(Val);
   let From = PAGEhashParameter("from");
   if(!Args.time && From){
      Args.time = From;
      Args.id = "";
   }
   BLKendArgs = Args;
}

function BLKbasicArgs(){
   let Args = {};
   Args.component = LOGcomponentId();
   Args.formatted = true;
   Args.exact     = PAGEhashParameter("exact");
   Args.search    = PAGEhashParameter("search");
   Args.tags      = PAGEhashParameter("tags");
   Args.only_messages = true;
   return Args;
}

function BLKlast(){ 
   let Args = BLKbasicArgs();
   Args.start    = BLKendArgs.time;
   Args.start_id = BLKendArgs.id;
   Args.forward  = true;
   Args.limit     = 1;
   return Args;
}

