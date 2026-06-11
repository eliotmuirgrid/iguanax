function PRJdrawersClickListeners(e){
   if(e.target.closest('.FILTdrawer')) return FILTclickListeners(e);
   if(e.target.closest('.GITTdrawer')) return GITTclickListeners(e);
}

function FILTclickListeners(e){
   if(e.target.closest('.FILTdrawerHeader')) return FILTtoggleDrawer(e);
   // if(e.target.closest('.FILTpathButton')) return FILTopenInFsButton(e);
}

function FILTtoggleDrawer(e){
   let div = document.querySelector('.FILTdrawer');
   div.classList.toggle('is-open');
}