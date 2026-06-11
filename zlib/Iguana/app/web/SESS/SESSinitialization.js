// list of functions we want to call on login
function SESSinitialize(){
   CACHErefresh(false);
   APIcall('git/owners/update', {}, function(R) {});
}