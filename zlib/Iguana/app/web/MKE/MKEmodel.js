function MKEinitModel(callback) {
   MODELremove('MKEmodel');
   MODELpollerAdd('MKEmodel', 'MKEcomponent', 10,  'component/detail', MKEdetailArgs);
   MODELstart('MKEmodel');
   MODELforcePoll('MKEmodel', function(){
      if(callback) { callback(); }
      MODELpollerAdd('MKEmodel', 'MKEwritable', 10, 'git/writable', MKEmodelArgs);
      MODELviewAdd  ('MKEmodel', 'MKEupdateUpstreamWritable');
      MODELforcePoll('MKEmodel');
   });
}

function MKEmodelArgs(){
   let ApiData = {};
   ApiData.repo = MKEurl();
   return ApiData;
}

function MKEdetailArgs(){
   let ApiData = {};
   ApiData.component = MKEcomponent();
   return ApiData;
}

function MKEcomponent(){ return MKEcomponentId || ""; }
function MKEurl()      { return MKEgitUrl      || ""; }

function MKEdata(){ return MODELdata?.MKEcomponent?.data || {}; }

function MKEresetGlobals(){
   if(MODELdata.MKEwritable) {
      MODELdata.MKEwritable = undefined;
   }
}