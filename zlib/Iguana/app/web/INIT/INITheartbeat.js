let INITheartbeatHealth = 0;
let INITheartbeatInterval;

function INITheartbeat() {
   const Request = APIcall('install/heartbeat', {}, APIdoNothingCallback);
   setTimeout(function() {
      if(INITheartbeatCheck(Request)) { return INITheartbeatReset(); }
      INITheartbeatError();
   }, 1000)
}

function INITheartbeatError() {
   INITheartbeatIncrement();
   if(INITheartbeatHealth >= 3) {
      INITheartbeatClearInterval();
      window.location.hash = 'crash';
   }
}

function INITheartbeatCheck(Request)  { return Request.readyState === XMLHttpRequest.DONE && Request.status === 200; }
function INITheartbeatReset()         { INITheartbeatHealth = 0; }
function INITheartbeatIncrement()     { INITheartbeatHealth++; }
function INITheartbeatSetInterval()   { INITheartbeatInterval = setInterval(INITheartbeat, 2000); }
function INITheartbeatClearInterval() { clearInterval(INITheartbeatInterval); }