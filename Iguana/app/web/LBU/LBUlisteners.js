function LBUlisteners(){
   let div = document.querySelector(".LBUcontainer");
   if(!div) return;
   div.addEventListener("click", LBUclick);
   div.addEventListener("change", LBUchange);
}

function LBUclick(e){
   if(e.target.closest(".LBUclose")) POPpopoutClose(LBUpopoutCard);
   if(e.target.closest(".LBUhref"))  LBUhrefClick(e.target);
   if(e.target.closest(".LBUheader .LBUselectRow")) return LBUselectAllClicked();
   if(e.target.closest(".LBUdetRow .LBUselectRow")) return LBUselectOneClicked();
   if(e.target.closest(".LBUupdateComp.BUTTONaction")) return LBUupdateSelected();
   if(e.target.closest(".LBUlink"))                    return LBUlinkClicked(e);
}

function LBUkeyFromTarget(target){
   let idx = LBUidxFromTarget(target);
   let data = LBUdata() || {};
   return Object.keys(data)[idx];
}

function LBUidxFromTarget(target){
   const row = target.closest('.LBUrow');
   if (!row) return;
   const rows = Array.from(row.parentNode.children).filter(node => node.classList.contains('LBUrow'));
   return rows.indexOf(row);   
}

function LBUhrefClick(target){
   let key = LBUkeyFromTarget(target);
   let data = LBUdata() || {};
   let clickData = data[key];
   if(target.closest(".LBUaheadHref"))  return LBUaheadUpdate (clickData, key);
   if(target.closest(".LBUbehindHref")) return LBUbehindUpdate(clickData, key);
   if(target.closest(".LBUusageHref"))  return LBUusageUpdate (clickData, key);
}

function LBUupdateSelected(){
   let div = document.querySelector(".LBUupdateComp");
   if(div.classList.contains("BUTTONdisabled")) return;
   let Libraries = LBUgetSelected();
   div.classList.remove("BUTTONaction");
   div.classList.add("BUTTONdisabled");
   APIcall("library/update", { items : Libraries }, function(R){
      let err = LBUgiveErrorFeedback(R?.data || {})
      if(err) LBUsetErrorFeedback(err);
      MODELforcePoll("LBUmodel", function(){
         LBUclearDetails();
      });
   });
}

function LBUgiveErrorFeedback(Data){
   let arr = [];
   for(lib of Data){
      let component = lib?.component || "";
      if(lib.success == false && lib.error.includes("unresolved conflict")) {
         arr.push(`${LBUcomponentName(component)} (contains uncommitted changes)`);
      }
   }
   if(arr.length == 0) return "";
   return `Could not pull: ${arr.join(", ")}.`;
}

function LBUsetErrorFeedback(err){
   let div = document.querySelector(".LBUerrorFeedback");
   if(!div) return;
   div.innerHTML = err;
}

function LBUselectAllClicked(){
   let sel = LBUgetSelected()?.length || 0;
   let total = LBUtotalBehindRows();
   if(sel == total && sel != 0) return LBUselectAll(false);
   LBUselectAll(true);
   LBUselectOneClicked();
}

function LBUselectOneClicked(){
   let sel = LBUgetSelected()?.length || 0;
   let total = LBUtotalBehindRows();
   if(sel == total && sel != 0) return LBUselectHeader(true);
   return LBUselectHeader(false);
}

function LBUtotalBehindRows(){
   let divs = document.querySelectorAll(".LBUdetRow .LBUselectRow");
   return divs.length;
}

function LBUgetSelected(){
   let divs = document.querySelectorAll(".LBUdetRow .LBUselectRow");
   let selected = [];
   for(let i=0; i < divs.length; i++){
      let div = divs[i];
      if(!div?.checked) continue;
      let data = LBUbehindData[i];
      let arg = {};
      arg.component = data?.component || "";
      arg.path      = data?.path || "";
      selected.push(arg);
   }
   return selected;
}

function LBUselectAll(selectAll){
   let divs = document.querySelectorAll(".LBUdetRow .LBUselectRow");
   for(let i=0; i < divs.length; i++){
      let div = divs[i];
      if(!div) continue;
      div.checked = selectAll;
   }
}

function LBUselectHeader(selected){
   let div = document.querySelector(".LBUheader .LBUselectRow");
   div.checked = selected;
}

function LBUchange(){
   let checked = LBUgetSelected();
   let enabled = checked.length;
   let div = document.querySelector(".LBUupdateComp");
   if(!div) return;
   if( enabled && !div.classList.contains("BUTTONaction"))   div.classList.add   ("BUTTONaction");
   if( enabled && div.classList.contains("BUTTONdisabled"))  div.classList.remove("BUTTONdisabled");
   if(!enabled && div.classList.contains("BUTTONaction"))    div.classList.remove("BUTTONaction");
   if(!enabled && !div.classList.contains("BUTTONdisabled")) div.classList.add   ("BUTTONdisabled");
}

function LBUlinkClicked(e){
   let link = e.target.closest(".LBUlink");
   const key  = link.dataset.pathvar; 
   const comp = link.dataset.compvar; 
   TRANSopenDir = key;
   if(comp == TRANScomponentGuid()){
      POPpopoutClose(LBUpopoutCard);
      MODELforcePoll('TRANSmodel');
   }
}