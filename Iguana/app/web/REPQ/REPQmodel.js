function REPQmodelInit(){
   REPQclearGlobals();
   MODELpollerAdd('REPQmodel', 'REPQsources',   5, 'log/queue/sources', REPQcomponentId);
   MODELviewAdd  ('REPQmodel', 'REPQupdateSources');
   MODELforcePoll('REPQmodel');
   MODELstart    ('REPQmodel');
   REPQlisteners();
}

function REPQcomponentId(){
   return {component : PAGEhashParameter('component')};
}

function REPQsources(){ return MODELdata?.REPQsources?.data; }

function REPQclearGlobals(){
   REPQmessages = [];
}

function REPQlisteners(){
   let Card = document.querySelector('.REPQmodal');
   Card.removeEventListener('click',   REPQclickListeners);
   Card.addEventListener   ('click',   REPQclickListeners);
   Card.removeEventListener('change',  REPQchangeListeners);
   Card.addEventListener   ('change',  REPQchangeListeners);
   Card.removeEventListener('input',   REPQinputListener);
   Card.addEventListener   ('input',   REPQinputListener);
}

// This catches control changes in the datepicker
function REPQinputListener(e){
   if(e.target.closest('.REPQselectTime'))   { return REPQgetLogsAroundTime(); }
}

function REPQchangeListeners(e){
   if(e.target.closest('.REPQselectTime'))   { return REPQgetLogsAroundTime(); }
   if(e.target.closest('.REPQselectSource')) { return REPQgetLogsAroundTime(); }
}

function REPQclickListeners(e){
   if(e.target.closest('.REPQrepositionAction')) { return REPQdoReposition(); }
   if(e.target.closest('.REPQrepositionCancel')) { return REPQclose(); }
}