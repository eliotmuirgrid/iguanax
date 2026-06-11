let HLPnavigationHistory = [];
let HLPcurrentIndex = -1;

function HLPnavTo(targetElement) {
   const filepath              = targetElement.getAttribute('data-location');
   const name                  = targetElement.innerText;
   const current               = HLPnavigationHistory[HLPcurrentIndex];
   targetElement.style.display = '';
   targetElement.style.color   = '';
   if (!current || current.name !== name || current.location !== filepath) {
      if (HLPcurrentIndex < HLPnavigationHistory.length - 1) {
         HLPnavigationHistory = HLPnavigationHistory.slice(0, HLPcurrentIndex + 1);
      }
      HLPnavigationHistory.push({ name, location: filepath });
      if (HLPnavigationHistory.length > 5) HLPnavigationHistory.shift();
      HLPcurrentIndex = HLPnavigationHistory.length - 1;
   }
}

function HLPnavigate() {
   const entry = HLPnavigationHistory[HLPcurrentIndex];
   const link = HLPgetAllFunctions().find(el => el.innerText === entry.name);
   if (link) {
      link.style.display = '';
      HLPhandleClick(link, false);
      HLPjumpToFunction(HLPgetSelected())
   }
}

function HLPprev() {
   if(HLPcurrentIndex > 0) {
      HLPcurrentIndex--;
      HLPnavigate();
   }
   HLPupdateNavButtons();
}

function HLPnext() {
   if(HLPcurrentIndex < HLPnavigationHistory.length - 1) {
      HLPcurrentIndex++;
      HLPnavigate();
   }
   HLPupdateNavButtons();
}

function HLPupdateNavButtons() {
   const prevBtn = document.querySelector('.HLPprev');
   const nextBtn = document.querySelector('.HLPnext');
   prevBtn.classList.toggle('HLPnavButtonDisabled', HLPcurrentIndex <= 0)
   nextBtn.classList.toggle('HLPnavButtonDisabled', HLPcurrentIndex >= HLPnavigationHistory.length - 1);
}

function HLPsetupNavListeners() {
   document.querySelector('.HLPprev').addEventListener('click', HLPprev);
   document.querySelector('.HLPnext').addEventListener('click', HLPnext);
}