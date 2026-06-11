function ROLlisteners(){
   let div = document.querySelector(".ROLmodal");
   if(!div) return;
   div.addEventListener("click", ROLclickListeners);
   div.addEventListener("change", ROLchangeListeners);
}

function ROLclickListeners(e){
   if(e.target.closest(".ROLroleName"))    return ROLheaderClicked(e.target);
   if(!document.querySelector(".ROLedit")) return;
   if(e.target.closest(".ROLsave"))        return ROLsaveRole();
   if(e.target.closest(".ROLselectAll"))   return ROLselectAll(e.target, true);
   if(e.target.closest(".ROLdeselectAll")) return ROLselectAll(e.target, false);
   if(e.target.closest(".ROLlineItem"))    return ROLlineItem(e.target);
}

function ROLchangeListeners(e){
   if(e.target.closest(".ROLlineItem input")) return ROLcheckRoles(e.target);
   if(e.target.closest(".ROLroleAppliesTo select")) return ROLcheckType(e.target.closest(".ROLroleAppliesTo"));
}

function ROLselectAll(target, set){
   let div = target.closest(".ROLsubsection");
   if(!div) return;
   let checks = div.querySelectorAll(".ROLlineItem input");
   checks.forEach(function(checkbox){
      checkbox.checked = set;
      ROLcheckRoles(checkbox);
   });
}

function ROLlineItem(target){
   if(target.closest("input")) return;
   let parent = target.closest(".ROLlineItem");
   let inp = parent.querySelector("input");
   if(!inp) return;
   if(inp.checked) inp.checked = false;
   else            inp.checked = true;
   ROLcheckRoles(parent);
}

function ROLcheckRoles(target){
   let div = target.closest(".ROLlineItem");
   ROLcontrolCheckboxes(div, ".ROLeditScript",      ".ROLviewScript");
   ROLcontrolCheckboxes(div, ".ROLeditFields",      ".ROLviewScript");
   ROLcontrolCheckboxes(div, ".ROLdownloadLogs",    ".ROLviewLogs");
   ROLcontrolCheckboxes(div, ".ROLexportSample",    ".ROLviewLogs");
   ROLcontrolCheckboxes(div, ".ROLresubmitMessage", ".ROLviewLogs");
}

function ROLcontrolCheckboxes(target, Class1, Class2){
   let Div1T = target.querySelector(Class1);
   let Div2 = document.querySelector(Class2);
   if(Div1T && Div1T.checked && !Div2.checked) Div2.checked = true;
   let Div2T  = target.querySelector(Class2);
   let Div1 = document.querySelector(Class1);
   if(Div2T && !Div2T.checked && Div1.checked) Div1.checked = false;
}
