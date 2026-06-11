function TUNscreen(){
   PAGEresetPage();
   COL_TRC("We have freshly come to the tunnel page.");
   CORpreparePage();
   CORheader();
   CORsetNavIcon("CORclusterIcon");
   TUNsetupHtml();
   TUNmodelInit();
}

function TUNsetupHtml(){
   COL_TRC("Setting up tunnel scaffold for the first time.");
   let Div = document.querySelector('.CORmain');
   if(!Div) return;
   let Content = document.querySelector('.TUNlistView');
   if(Content) return;
   Div.innerHTML = TUNlistHtml();
   TUNlistenersInit();
   TUNresetGlobals();
}

function TUNmodelInit(){
   MODELpollerAdd('TUNmodel','TUNstatus',    2,    'tunnel/status',     {});
   MODELpollerAdd('TUNmodel','TUNpublicKey', 1000, 'tunnel/key/public', {});
   
   MODELviewAdd('TUNmodel', 'TUNstatus');
   MODELviewAdd('TUNmodel', 'TUNkey');
   MODELviewAdd('TUNmodel', 'TUNlistUpdate');
 
   MODELforcePoll('TUNmodel');
   MODELstart('TUNmodel');
}

function TUNpeerServerData(){
   return MODELdata.TUNstatus.data.peers || {};
}