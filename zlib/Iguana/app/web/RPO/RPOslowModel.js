function RPOslowModelInit(){
   MODELpollerAdd('RPOslowModel', 'RPOtokenValid', 20, 'git/token/valid', {});
   MODELviewAdd  ('RPOslowModel', 'RPOtokenValidCheck');
   MODELforcePoll('RPOslowModel');
}

function RPOtokenValidCheck(){
   if(RPOtokenValid()) return;
   MODELremove("RPOslowModel");
   MODELremove("RPOmodel");
   RPOsetUpBitbucket();
}

function RPOtokenValid(){
   return MODELdata?.RPOtokenValid?.data?.valid || false;
}