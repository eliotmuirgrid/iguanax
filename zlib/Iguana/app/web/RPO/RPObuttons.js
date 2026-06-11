function RPOisDisabled() {
   let ConfirmBtn = document.querySelector('.RPOconfirm');
   return ConfirmBtn?.classList.contains('BUTTONdisabled') || false;
}

function RPOisCustomDisabled() {
   let ConfirmBtn = document.querySelector('.RPOconfirmCustom');
   return ConfirmBtn?.classList.contains('BUTTONdisabled') || false;
}