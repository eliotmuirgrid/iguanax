let PEERinterval=0;

function PEERisRemote(){
   return (window.location.pathname.substring(0, 5) === "/peer");
}

function PEERcurrent(){
   if (!PEERisRemote()){
      return '';
   }
   return window.location.pathname.split('/')[2];
}

function PEERinit(){
   clearInterval(PEERinterval);
   if (PEERisRemote()){
      PEERinterval = setInterval(PEERheartbeat, 2000);
   }
}

function PEERheartbeat(){
   COL_TRC("Peer heartbeat.");
   APIhostCall("/tunnel/status", {}, PEERcheckStatus);
}

function PEERcheckStatus(R){
   COL_TRC(R);
   if (!R.data.peers[PEERcurrent()]["authenticated"]){
      clearInterval(PEERinterval);
      PEERscreenOffline();
   }
}

   
function PEERscreenOffline(){
   PAGEresetPage();
   COL_TRC("We have freshly come to the tunnel page.");
   let H = document.querySelector(".CORheader");
   H.innerHTML = CORpeerOfflineHeaderHtml();
   document.querySelector('.CORmain').innerHTML = `Peer ${PEERcurrent()} is not available.  Go back to host <a href="/#dashboard">dashboard.</a>`;
}