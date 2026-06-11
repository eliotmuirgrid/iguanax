function PERmodelInit(StartModel){
   PERserversData = undefined;
   MODELpollerAdd("PERmodel", 'PEReditPermission', 10, 'component/permission', {});
   MODELpollerAdd("PERmodel", 'PERservers',        10, 'peers/status',     {});
   MODELviewAdd  ("PERmodel", "PERupdateAddButton");
   MODELviewAdd  ("PERmodel", "PERupdateServers");
   MODELforcePoll("PERmodel");
   if(StartModel) MODELstart("PERmodel");
}

function PEReditPermission(){
   return MODELdata?.PEReditPermission?.data?.edit_gitservers;
}

function PERdata(){
   let Data = {};
   let guid1 = "MyNameguid123";
   let guid2 = "guid234";
   Data[guid1] = { "connected" : true };
   Data[guid2] = { "connected" : false, "error" : "Could not blah blah" };
   return Data;
   // return MODELdata?.PERservers?.data || {};
}

