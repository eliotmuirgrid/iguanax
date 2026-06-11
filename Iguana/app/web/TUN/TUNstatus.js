function TUNstatus(){
   if (!MODELdata?.TUNstatus?.success){
      return;
   }
   let Data = MODELdata.TUNstatus.data;
   let H;
   let Connected = Data.connected;
   if(Connected) H = `<div class="TUNconnected"><b>SUCCESS:</b> Connected to tunnel router</div>`;
   else          H = `<div class="TUNdisconnected"><b>ERROR:</b> Not connected to tunnel router</div>`;
   let Div = document.querySelector('.TUNconnectedDiv');
   if(!Div) return;
   if(Div.innerHTML != H ) Div.innerHTML = H;
}
