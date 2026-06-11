function CARDcomponentId(){
   return PAGEhashParameter('component');
}

function CARDfocusAtTheEnd(el){
   el.focus();
   el.selectionStart = el.selectionEnd = el.value.length;
}

