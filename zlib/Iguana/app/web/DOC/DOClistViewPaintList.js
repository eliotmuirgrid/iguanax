function DOClistRowHeaderHtml(){
   return  `
   <div class="DOClistRowHeader DOClistRow">
      <div class="DOClistRowHeaderCheckBox DOClistRowElm">
         <input type="checkbox" class="DOCselectListRow">
      </div>
      <div class="DOClistRowTitle DOClistRowElm">
         Title
      </div>
      <div class="DOClistRowHeaderDate DOClistRowElm">
         Date
      </div>
   </div>`;
}


function DOClistRowHtml(){
   return `<div class="DOClistRow">
           <div class="DOClistRowCheckBox DOClistRowElm">
              <input type="checkbox" class="DOCselectListRow">
           </div>
           <div class="DOClistRowTitle DOClistRowElm"></div>
           <div class="DOClistRowDate DOClistRowElm"></div>
      </div>`;
}

function DOCpaintList(DocArray){
   let CurrentId = PAGEhashParameter("id");
   let H = DOClistRowHeaderHtml();
   for (let i=0; i< DocArray.length; i++){
      H+=`<div class="DOClistRow`     
      if (CurrentId === DocArray[i].id){
         H+= " DOChighlight";
      }
      H+=`">
            <div class="DOClistRowCheckBox DOClistRowElm">
               <input type="checkbox" class="DOCselectListRow">
            </div>
         <div class="DOClistRowTitle DOClistRowElm">${DocArray[i].title}</div>
            <div class="DOClistRowDate DOClistRowElm">${DOCconvertDate(DocArray[i].created)}</div>
      </div>`
   }
      return H;
}

function DOClistSetup(W){
   document.querySelector(".DOClist").innerHTML = DOClistRowHeaderHtml() + DOClistRowHtml().repeat(W);
}

function DOClistPopulate(L, Start, W){
   COL_TRC(L);
   let T = document.querySelectorAll(".DOClistRowTitle");
   let D = document.querySelectorAll(".DOClistRowDate");
   for(let i=Start; i < Start+W; i++){
      if (T[i-Start].innerText !== L[i].title)                  { T[i-Start].innerText = L[i].title; }
      if (D[i-Start].innerText !== DOCconvertDate(L[i].created)){ D[i-Start].innerText = DOCconvertDate(L[i].created); }
   }
}

function DOCtest(){
   let X = MODELdata.DOClist.data.list;
   MODELshutdownAll();
   let W=30;
   DOClistSetup(W);
   DOClistPopulate(X, 13, W);
   DOCtestSelect(3);
}

function DOCtestSelect(i){
   document.querySelector(".DOChighlight")?.classList.remove("DOChighlight");
   document.querySelectorAll(".DOClistRow")[i+1].classList.add("DOChighlight");
}

function DOCselected(D, Id){
   for(let i = 0; i < D.length; i++){
      if (D[i].id === Id){
         return i;
      }
   }
}

// Not used currently.
function DOClistUpdate(){
   document.querySelector(".DOClist").innerHTML = DOCpaintList(MODELdata.DOClist.data.list);
}

function DOClistUpdateNew(){
   let D = MODELdata.DOClist.data.list;
   if (!document.querySelector(".DOClistRowHeader")) {
      DOClistSetup(D.length);
   }
   DOClistPopulate(D, 0, D.length);
   DOCtestSelect(DOCselected(D, PAGEhashParameter("id")));
}

