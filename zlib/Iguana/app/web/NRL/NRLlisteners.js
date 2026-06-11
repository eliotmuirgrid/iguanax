let NRLupdateModelTimer; 
let NRLupdateUrlTimer; 
let NRLnameTyped;

function NRLkeyupListeners(e){
   if(e.target.closest('.NRLname') || e.target.closest('.NRLgit') ) { 
      clearTimeout(NRLupdateModelTimer);
      NRLupdateModelTimer = setTimeout(function() { MODELforcePoll('NRLmodel'); }, 100);
   }
   if(e.target.closest('.NRLname')) NRLnameTyped = true;
}

function NRLpasteListener(target){
   if(target.closest('.NRLgit')) return NRLguessName();
}

function NRLlisteners(){
   let card = document.querySelector('.NRLmodal');
   card.addEventListener('click', function(e){ NRLsingleClickListeners(e); });
   card.addEventListener('keyup', function(e){ NRLkeyupListeners(e); });
   card.addEventListener('paste', function(e){ NRLpasteListener(e.target); });
}

function NRLsingleClickListeners(e){
   if(e.target.closest('.NRLadd'))   NRLaddComponent();
   if(e.target.closest('.NRLclose')) DASHcloseModal();
}

function NRLaddComponent(){
   let Button = document.querySelector('.NRLadd');
   if(!Button) return;
   if(Button.classList.contains('BUTTONdisabled')) return; 
   if(NRLinProgress) return; 
   NRLinProgress = true;
   NRLupdateButtonAdd(); // update the styling
   NRLworkingFeedback();
   let ComponentData = {};
   let TagInput = document.querySelector(".NRLtags").value.trim() || "";
   let Tags = TagInput.split(/\s+/);
   let Parsed = NRLsyncParsedGitInput();
   ComponentData.name        = document.querySelector('.NRLname').value.trim();
   ComponentData.branch      = Parsed.branch;
   ComponentData.description = document.querySelector('.NRLdescription').value.trim();
   ComponentData.git         = Parsed.url;
   ComponentData.tags        = Tags;
   ComponentData.editable    = false;
   ComponentData.template    = true;
   APIcall('component/create', ComponentData, function(R){
      if(!R.success) {
         NRLinProgress = false;
         NRLworkingFeedback();
         if (!LICcheckComponentCreateError(R)) { return; } // This function will take care of creating the error snackbar
         else                                  { return SNCKsnackbar('error', R.error); }
      }
      DASHcloseModal();
      let Guid =     R.data?.components[0]?.guid;
      let Warning =  R?.data?.warning;
      if(Warning){
         SNCKsnackbar('error', Warning); 
      }
      if(Guid) {
         let Url = "#dashboard/details?component=" + Guid;
         let Filter = PAGEhashParameter("filter");
         if(Filter) Url = Url + "&filter=" + Filter;
         window.location.hash = Url;
      }  
      MODELforcePoll('DASHmodel');
      let Message = GINaddFromUrl(ComponentData.name, ComponentData.git);
      GINaddAndCommit({message: Message, component: Guid, all: true});
   });
}
