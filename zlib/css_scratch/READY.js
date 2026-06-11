function READYevent(Func){
   //console.log(document.readyState);
   if(document.readyState !== 'complete') {
      window.onload = Func;
      return;
   } else { 
      Func();
   } 
}