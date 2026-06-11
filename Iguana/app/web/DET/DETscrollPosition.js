let DETsavedScroll = 0;

/** Save the current scrollTop of a container */
function DETrememberScroll() {
   let container = document.querySelector(".DETdetailContent");
  DETsavedScroll = container.scrollTop || 0;
}

function DETresetScroll(){
   DETsavedScroll = 0;
}

/** Get the saved scrollTop */
function DETgetSavedScroll() {
  return DETsavedScroll;
}

/** Apply the saved scrollTop to a container */
function DETapplyScroll() {
   let container = document.querySelector(".DETdetailContent");
   container.scrollTop = DETsavedScroll | 0;
}
