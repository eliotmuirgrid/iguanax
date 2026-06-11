function ENVlisteners(){
   let Div = document.querySelector(".SETcontentBody");
   if(!Div) return;
   Div.addEventListener("click", ENVclickListener);
   Div.addEventListener("dblclick", ENVdblclickListener);
}

function ENVclickListener(e){ 
   if(e.target.closest(".ENVhide"))      return ENVsystemToggle();
   if(!e.target.closest(".ENVeditable")) return;
   if(e.target.closest(".ENVcreate"))    return ENVcreateNew();
   if(e.target.closest(".ENVedit"))      return ENVeditVariable(e);
}

function ENVdblclickListener(e){
   if(!e.target.closest(".ENVeditable")) return;
   if(e.target.closest(".ENVrow"))       return ENVeditVariable(e);
}

function ENVcreateNew(){
   const envCard = POPpopoutOpen({title: 'Create Environment Variable', content: ENVeditHtml('', '', false)});
   ENVcardActions(envCard, null);
}

function ENVsystemToggle(){
   const systemToggle = document.querySelector('.ENVhide');
   const systemVars   = document.querySelectorAll('.ENVsection')[1];
   systemVars.querySelectorAll('.ENVrow').forEach(row => {
      row.style.display = row.style.display === 'none' ? 'grid' : 'none';
   });
   systemToggle.textContent = systemToggle.textContent.toLowerCase() === 'hide' ? 'SHOW' : 'HIDE';
}

function ENVeditVariable(e){
   let envSections = document.querySelectorAll(".ENVsection");
   const parent = e.target.closest(".ENVrow");
   const isLocalSection = e.target.closest(".ENVsection") === envSections[0];
   const originalKey = parent.querySelector(".ENVkey").textContent;
   const value       = ENVresolveValue(originalKey);
   const envCard     = POPpopoutOpen({title: 'Edit Environment Variable', content: ENVeditHtml(originalKey, value, isLocalSection)});
   ENVcardActions(envCard, originalKey);
}
