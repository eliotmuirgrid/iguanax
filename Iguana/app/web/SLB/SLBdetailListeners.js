// function SLBsetDetailListeners(){
//    let div = document.querySelector(".SLBcontainer");
//    if(!div) return;
//    div.addEventListener("click", SLBdetClick)
// }

// function SLBdetClick(e){
//    if(e.target.closest(".SLBupdateComp")) return SLBupdateSelected();
//    if(e.target.closest(".SLBheader .SLBselectRow")) return SLBselectAllClicked();
//    if(e.target.closest(".SLBdetRow .SLBselectRow")) return SLBselectOneClicked();
// }

// function SLBupdateSelected(){
//    let Libraries = SLBgetSelected();
//    APIcall("library/update", { items : Libraries }, function(R){
//       console.log(R);
//       MODELforcePoll("SLBmodel");
//    });
   
// }

// function SLBselectAllClicked(){
//    let sel = SLBgetSelected()?.length || 0;
//    let total = SLBtotalBehindRows();
//    if(sel == total && sel != 0) return SLBselectAll(false);
//    SLBselectAll(true);
//    SLBselectOneClicked();
// }

// function SLBselectOneClicked(){
//    let sel = SLBgetSelected()?.length || 0;
//    let total = SLBtotalBehindRows();
//    if(sel == total && sel != 0) return SLBselectHeader(true);
//    return SLBselectHeader(false);
// }

// function SLBcompFromCheckbox(div){
//    let par = div.closest(".SLBdetRow");
//    return par.querySelector(".SLBcomp")?.innerHTML || "";
// }

// function SLBpathFromCheckbox(div){
//    let par = div.closest(".SLBdetRow");
//    return par.querySelector(".SLBpath")?.innerHTML || "";
// }

// function SLBtotalBehindRows(){
//    let divs = document.querySelectorAll(".SLBdetRow .SLBselectRow");
//    return divs.length;
// }

// function SLBgetSelected(){
//    let divs = document.querySelectorAll(".SLBdetRow .SLBselectRow");
//    let selected = [];
//    for(let i=0; i < divs.length; i++){
//       let div = divs[i];
//       if(!div?.checked) continue;
//       let arg = {};
//       arg.component = SLBcompFromCheckbox(div);
//       arg.path      = SLBpathFromCheckbox(div);
//       selected.push(arg);
//    }
//    return selected;
// }

// function SLBselectAll(selectAll){
//    let divs = document.querySelectorAll(".SLBdetRow .SLBselectRow");
//    for(let i=0; i < divs.length; i++){
//       let div = divs[i];
//       if(!div) continue;
//       div.checked = selectAll;
//    }
// }

// function SLBselectHeader(selected){
//    let div = document.querySelector(".SLBheader .SLBselectRow");
//    div.checked = selected;
// }