let SLBdataObj = {};
let SLBpageDataRendered = undefined;

function SLBmodelInit(){
   MODELpollerAdd('SLBmodel', 'SLBdata', 30, 'library/info', {});
   MODELviewAdd('SLBmodel',   'SLBtableUpdate');
   MODELforcePoll('SLBmodel');
   MODELstart('SLBmodel');
}

function SLBdata(){ return MODELdata?.SLBdata?.data || {}; }