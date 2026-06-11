function GITHupdateOwners(){
   let Data = {};
   Data.force = true;
   APIcall("git/owners/update", Data, function(){
      
   });
}