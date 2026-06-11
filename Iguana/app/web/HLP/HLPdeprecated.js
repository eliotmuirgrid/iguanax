let HLPshowDeprecated = false;

function HLPdepToggle(e) {
   const toggleButton = document.querySelector('.HLPdepToggle');
   if(!toggleButton) { return; }
   const checkbox = toggleButton.children[0];
   if(e.target === checkbox) { return HLPtoggleDeprecated(); }
   checkbox.checked = !checkbox.checked;
   e.preventDefault();
   HLPtoggleDeprecated();
}

function HLPsetupDepToggleListener() {
   const toggleButton   = document.querySelector('.HLPdepToggle');
   toggleButton.removeEventListener('click', HLPdepToggle);
   toggleButton.addEventListener('click', HLPdepToggle);
}

function HLPtoggleDeprecated() {
   const deprecatedFunctions = MODELdata?.DEPdata?.data;
   if(!deprecatedFunctions) { return; }

   const checkbox    = document.querySelector('.HLPdepToggle').children[0];
   HLPshowDeprecated = checkbox.checked;
   const keyword     = document.querySelector('.HLPsearch').value.toLowerCase();
   const selected    = HLPgetSelected();

   HLPgetAllFunctions().forEach(link => {
      const funcName = link.innerText;
      if(!deprecatedFunctions.hasOwnProperty(funcName)) { return; }
      link.classList.toggle('HLPdep', funcName !== selected && HLPshowDeprecated);

      const shouldShow = HLPshowDeprecated ? funcName.toLowerCase().includes(keyword) : false;
      link.classList.toggle('HLPhide', !shouldShow);
   });

   if(HLPshowDeprecated) {
      const selected = HLPgetSelected();
      const left = document.querySelector('.HLPlistBody');
      if(!selected && left) left.scrollTo({ top: 0, behavior: 'auto' });
   }
}