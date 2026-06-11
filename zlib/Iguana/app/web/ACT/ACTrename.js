let ACTrenameInProgress = false;

function ACTrename(node, apiFunction){
   ACTrenameInProgress = true;
   MENUrow.draggable   = false;
   MENUrow.innerHTML += /*html*/`<input class="FORMinput ACTnameInput">`;
   let ItemName    = MENUrow.querySelector('.PRJitemName');
   let ItemArrow   = MENUrow.querySelector('.PRJlibLink');
   const NameInput = MENUrow.querySelector('.ACTnameInput');
   NameInput.value = MENUitem.name;
   NameInput.focus();
   NameInput.setSelectionRange(0, NameInput.value.length);
   ItemName && (ItemName.hidden = true);
   ItemArrow && (ItemArrow.hidden = true);
   MENUremoveTooltip();
   NameInput.addEventListener("click", function(Event) { Event.stopPropagation() });
   NameInput.addEventListener("focusout", function(Event) {
      Event.stopPropagation();
      if(!ACTrenameInProgress) return TRANSsetCode();
      NameInput.closest('.PRJrow').draggable = true;
      const DirName = NameInput.value.trim();
      if(apiFunction) { apiFunction(node.path, DirName) }
      NameInput.remove();
      TRANSsetCode();
      ItemName && (ItemName.hidden = false);
      ItemArrow && (ItemArrow.hidden = false);
      ACTrenameInProgress = false;
   });
   NameInput.addEventListener("keypress", function(Event) {
      if (Event.key === "Enter") {
         Event.preventDefault();
         NameInput.blur();
      }
   });
   NameInput.addEventListener("keydown", function(Event) {
      if (Event.key === "Escape") {
         ItemName.hidden                        = false;
         ItemArrow.hidden                       = false;
         ACTrenameInProgress                    = false;
         NameInput.closest('.PRJrow').draggable = true;
         NameInput.remove();
      }
   });
}