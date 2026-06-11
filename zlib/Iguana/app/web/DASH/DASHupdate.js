function DASHupdate(){
   DASHupdateSearch();
   DASHupdateArrows();
   DASHupdateWholeWord();
}

function DASHupdateSearch(){
   if(document.activeElement.closest(".EDITfilter")) return;
   let div = document.querySelector(".EDITfilter");
   if(!div) return;
   let Search =  PAGEhashParameter("filter") || "";
   if(div.value !== Search) div.value = Search;
}

function DASHremoveArrow(className){
   let div = document.querySelector("."+className);
   if(div) div.classList.remove(className);
}

function DASHremoveArrows(){
   DASHremoveArrow("DASHsortAsc")
   DASHremoveArrow("DASHsortDsc")
}

function DASHupdateArrows(){
   DASHremoveArrows();
   const Sort = PAGEhashParameter("sort");
   let Headers = document.querySelectorAll('.DASHtableHeader');
   let className = PAGEhashParameter("order") == "Dsc" ? "DASHsortDsc" : "DASHsortAsc";
   for (let i = 0; i < Headers.length; i++) {
      let Header = Headers[i];
      if(Header.innerHTML == Sort) Header.classList.add(className);
   }
}

function DASHupdateWholeWord(){
   let div = document.querySelector(".DASHfilterResultsCheckbox");
   if(!div) return;
   let Checked = PAGEhashParameter("whole_word") === "true";
   if(div.checked !== Checked) div.checked = Checked;  
}
