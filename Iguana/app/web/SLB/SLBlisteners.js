function SLBlisteners(){
   let div = document.querySelector(".SLBmodal");
   if(!div) return;
   div.addEventListener("click", SLBclick);
}

function SLBdetailListeners(){
   let div = document.querySelector(".LBUdetails");
   if(!div) return;
   div.addEventListener("click", SLBdetailClick);
   div.addEventListener("change", LBUchange);
}

function SLBclick(e){
   if(e.target.closest(".LBUaheadHref"))  return SLBhrefClick(e, LBUaheadUpdate);
   if(e.target.closest(".LBUbehindHref")) return SLBhrefClick(e, LBUbehindUpdate);
   if(e.target.closest(".LBUusageHref"))  return SLBhrefClick(e, LBUusageUpdate);
}

function SLBdetailClick(e){
   if(e.target.closest(".LBUaheadHref"))  return SLBdetailHrefClick(LBUaheadUpdate);
   if(e.target.closest(".LBUbehindHref")) return SLBdetailHrefClick(LBUbehindUpdate);
   if(e.target.closest(".LBUusageHref"))  return SLBdetailHrefClick(LBUusageUpdate);
   if(e.target.closest(".LBUclose"))      return POPpopoutClose(SLBdetailPopout);
   if(e.target.closest(".LBUheader .LBUselectRow"))    return LBUselectAllClicked();
   if(e.target.closest(".LBUdetRow .LBUselectRow"))    return LBUselectOneClicked();
   if(e.target.closest(".LBUupdateComp.BUTTONaction")) return SLBupdateSelected();
}

function SLBkeyFromTarget(target){
   let idx = LBUidxFromTarget(target);
   let data = SLBdataObj || {};
   return Object.keys(data)[idx];
}

function SLBkeyToInfo(key){
   let data = SLBdataObj || {};
   let render = {};
   render[key] = data[key];
   return render;
}

function SLBhrefClick(e, callback){
   let target = e.target;
   let key = SLBkeyFromTarget(target);
   let render = SLBkeyToInfo(key);
   SLBsetData(render);
}

function SLBdetailHrefClick(){
   SLBdetClear();
   SLBupdateDetails();
}

function SLBupdateSelected(){
   let div = document.querySelector(".LBUupdateComp");
   if(div.classList.contains("BUTTONdisabled")) return;
   let Libraries = LBUgetSelected();
   div.classList.remove("BUTTONaction");
   div.classList.add("BUTTONdisabled");
   APIcall("library/update", { items : Libraries }, function(R){
      let err = LBUgiveErrorFeedback(R?.data || {})
      if(err) LBUsetErrorFeedback(err);
      MODELforcePoll("SLBdetModel", function(){
         SLBdetClear();
      });
      MODELforcePoll("SLBmodel");
   });
}
