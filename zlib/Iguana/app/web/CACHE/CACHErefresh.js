let CACHErefreshVar = undefined;

function CACHErefreshPoller(){
   let time = 200 * 1000;
   if(!CACHErefreshVar) CACHErefreshVar = setInterval(function(){ CACHErefresh(false); }, time);
}

function CACHErefresh(force){
   let Args = {};
   Args.force = force;
   APIcall('git/cache/refresh', Args, function(R){});
}