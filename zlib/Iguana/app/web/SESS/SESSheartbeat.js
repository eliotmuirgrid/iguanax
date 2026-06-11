let SESScache = null;
let SESSheartbeatId = null;


function SESSheartbeatStop(){
   if (SESSheartbeatId){
      COL_TRC("Stop my beating heart.");
      clearInterval(SESSheartbeatId);
      SESSheartbeatId = null;
   } else {
      COL_TRC("Heartbeat was not going.");
   }
}

function SESSonHeartbeat(R){
   COL_TRC(R);
   if (R.success || !SESSsupportedBrowser()){
      SESScache = R.data;
      SSOrefreshToken();
   } else {
      SESSswitchToLogin();
      SESSshowError('Session was logged out');   
   }
}

function SESSswitchToLogin(){
   COL_TRC("Switching to login screen.");
   SESScache = null;
   if (PEERisRemote()){
      COL_TRC("Redirecting to root");
      window.location.pathname = "/";
   }
   PAGEresetPage();
   SESSloginScreen();
   SESSheartbeatStop();
}

function SESSonHeartbeatFail(){
   COL_TRC("Heartbeat call failed.");
   SESSswitchToLogin();
   SESSshowError('Connection to server failed.  Check if it is running and that your internet connection to it is working.');
}

function SESSstartHeartbeat() {
   COL_TRC("Start heartbeat.  Stop previous one if it is running.");
   SESSheartbeatStop();  
   SESSheartbeatId = setInterval(function(){
      APIhostCall('/session/heartbeat', {}, SESSonHeartbeat, SESSonHeartbeatFail);
   }, 5000);
}

function SESSinit(){
   COL_TRC("On init we check if we are logged in.");
   APIhostCall('/session/heartbeat', {}, SESSonInit);
}

function SESSonInit(R){
   COL_TRC(R);
   if (R.success){
      SESScache = R.data;
      SESSstartHeartbeat();
      CORpageTable();  // TODO this is copy pasted - maintenance issue!
      PAGEroute();
   } else {
      SESSloginScreen();   
   }
}

function SESSlogout(){
   COL_TRC("Log out");
   PAGEresetPage();
   SESSswitchToLogin();
   APIcall("session/logout", {}, SESSonLogout);
}

async function SESSonLogout(R){
   COL_TRC(R);
   await SSOlogout()
   CORpageTableLoggedOut();
   // CORrefresh(); // do we need this here? causes flicker on logout
   SESSinit();
}