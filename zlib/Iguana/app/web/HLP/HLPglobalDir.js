let HLP_GLOBAL_DIR='';

function HLPdirGlobalInit(){
   APIcall("help/dir/root", {}, function(R){
      HLP_GLOBAL_DIR = R.data.dir;
   });
}