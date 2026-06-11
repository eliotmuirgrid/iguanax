let CORslowTimer;
let CORgitSshValidData = {};
let CORgitTokenValidData = {};

function CORmodelSlowInit(){
   // MODELremove('CORmodelSlow');
   MODELpollerAdd('CORmodelSlow', 'CORgitTokenValid', 100, 'git/token/valid');
   MODELviewAdd  ('CORmodelSlow', 'CORsettingsIconDot');
   MODELviewAdd  ('CORmodelSlow', 'CORsettingsMyGitDot');
   // MODELforcePoll('CORmodelSlow');
   MODELstart    ('CORmodelSlow');
   CORmodelSlowTimer();
}

function CORmodelSlowTimer(){
   let time = 111 * 1000;
   if(!CORslowTimer) {
      setTimeout(CORmodelUpdateSlow, 8000);
      CORslowTimer = setInterval(CORmodelUpdateSlow, time);
   }
}

function CORmodelUpdateSlow(){
   CORsshValidInterval();
   CORtokenValidInterval();
}

function CORsshValidInterval(){
   APIcall("git/ssh/valid", {}, function(R){
      if(!R?.success) return;
      CORgitSshValidData = R;
      MODELupdate("CORmodelSlow");
   });
}

function CORtokenValidInterval(){
   APIcall("git/token/valid", {}, function(R){
      if(!R?.success) return;
      CORgitTokenValidData = R;
      MODELupdate("CORmodelSlow");
   });
}

function CORgitTokenValid(){
   let Data = CORgitTokenValidData?.data;
   if(!Data) return true;
   return Data?.valid;
}

function CORgitTokenDeprecated(){
   let Data = CORgitTokenValidData?.data;
   if(!Data) return false;
   return Data?.deprecated;
}

function CORtokenWarning(){
   return !CORgitTokenValid() || CORgitTokenDeprecated();
}

function CORgitSshValid(){
   let Data = CORgitSshValidData?.data;
   if(!Data || Data?.busy) return true;
   return Data?.valid;
}

function CORsshWarning(){
   return !CORgitSshValid();
}

function CORownersList(){
   return MODELdata?.CORorgsSet?.data?.owners || [];
}

function CORorgsWarning(){
   return CORownersList().length == 0;
}