let CORpushTimer;
let CORlastPushTime = 0;
let CORlastPushInterval = 0;

function CORmodelPushInit(){
   MODELremove   ('CORmodelPush');
   MODELpollerAdd('CORmodelPush', 'CORinterval', 31, 'instance/interval');
   // MODELviewAdd  ('CORmodelPush', 'CORlastPushUpdate');
   MODELstart    ('CORmodelPush');
   CORmodelPushTimer();
}

function CORinterval(){
   let I = MODELdata?.CORinterval?.data?.interval || 0;
   return I;
}



function CORmodelPushTimer(){
   let time = 29 * 1000;
   if(!CORpushTimer) {
      setTimeout(CORmodelPushUpdate, 6000);
      MODELforcePoll('CORmodelPush');
      CORpushTimer = setInterval(CORmodelPushUpdate, time);
   }
}

function CORmodelPushUpdate(){
   let I = CORinterval();
   if(I == 0) return;
   APIcall('instance/push', { force: false, manual: false }, function(R){
      // If not successful, we want to know if it was a network error vs the spamming interval check
   });
} 
