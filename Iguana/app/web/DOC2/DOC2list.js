let DOCshowListData = [];
let DOC2minListWidth = 410;
const MAX_DOC_ENTRIES = 200;

function DOC2listInit(){  
   let Created = PAGEhashParameter("created");
   let Id = PAGEhashParameter("id"); 
   if(!Created) return DOCrenderNewestList();
   DOCinitRenderDown(Created,Id);
}

function DOCrenderNewestList(){
   DOCstartOfDocs = true;
   DOCendOfDocs = false;
   let Args = DOCrenderNewestListArgs();

   APIcall("doc/search/threaded",Args, function(R){
      DOCshowListData = R.data;
      DOCshowListRender();
      DOCselectTopAndBottom();
      DOCdocumentModelInit();
   });
}

function DOCclearDocs(){
   document.querySelector(".DOC2list").innerHTML = "";
   DOCshowListData = [];
}

function DOCinitRenderDown(Created,Id){
   DOCinProgress = true;
   DOCclearDocs();
   let Args = DOCinitRenderDownArgs(Created, Id);
   APIcall("doc/search/threaded",Args, function(R){
      let Data = R.data || [];
      Data.shift();
      DOCshowListAppend(Data);
      DOCendOfDocs = Data.length < Args.limit
      DOCinProgress  = false;
      DOCinitRenderUp(Created,Id);
      DOCselectTopAndBottom();
   });
}

function DOCinitRenderUp(Created,Id){
   DOCinProgress = true;
   let Args = DOCinitRenderUpArgs(Created, Id);
   APIcall("doc/search/threaded",Args, function(R){
      let Data = R.data || [];
      Data.reverse();
      let TopDoc = document.querySelectorAll(".DOC2listRow")[0];
      DOCshowListPrepend(Data);
      DOCstartOfDocs = Data.length === 0;
      DOCinProgress = false;
      if (Id) {TopDoc = DOCtopDocWithId(Id)};
      DOCentryClicked(TopDoc);
      DOCdocumentModelInit();
      if(TopDoc) return TopDoc.scrollIntoView({ behavior: "instant", block: "center"});
      let DocRows = document.querySelectorAll(".DOC2listRow");
      let BottomDoc = DocRows[DocRows.length-1];
      if(!BottomDoc) return;
      BottomDoc.scrollIntoView({ behavior: "instant", block: "center"});
      DOCentryClicked(BottomDoc);
   });
}

function DOCtopDocWithId(Id){
   const Idx = DOCshowListData.findIndex(el => el.id === Id);
   return document.querySelectorAll(".DOC2listRow")[Idx];
}

function DOCshowListRender() {
   const Div = document.querySelector(".DOC2list");
   if (!Div) return;

   const H = DOCshowListData.map((_, i) => DOClistItemIdx(i, DOCshowListData)).join('');
   if (Div.innerHTML != H) {
      Div.innerHTML = H;
      let Selected = Div.querySelector('.DOC2listRowSelected');
      if (!Selected) {
         const firstRow = Div.querySelector('.DOC2listRow');
         if (firstRow) {
            firstRow.classList.add('DOC2listRowSelected');
            DOCscrollbarLineSelectedPosition = parseInt(DOCshowListData[0].created,16);
         }
      }
      DOCselectTopAndBottom();
      MODELupdate("DOC2scrollBarModel");
      DOC2documentModelRefresh();
   }
}

function DOClistItemIdx(Idx, Data){
   const I = Data[Idx];
   const DateStamp = DOC2convertDate(I.created);
   const Id = PAGEhashParameter("id");
   const Created = PAGEhashParameter("created");
   return DOCrowHtml(I,Id,Created,DateStamp);
}

function DOCrowHtml(I,Id,Created,DateStamp){
   let Selected = "";
   if(Created && Id && Created == I.created && Id == I.id){
      Selected = " DOC2listRowSelected";
      DOCscrollbarLineSelectedPosition = parseInt(Created,16);
   } 
   return `
   <div class="DOC2listRow${Selected}">
      <div class="DOC2listRowCheckBox DOC2listRowElm">
         <input type="checkbox" class="DOC2selectListRow">
      </div>
      <div class="DOC2listRowTitle DOC2listRowElm">${I.title}</div>
      <div class="DOC2listRowDate DOC2listRowElm">${DateStamp}</div>
   </div>`;
}

function DOColdestListItem(){
   return DOCshowListData[DOCshowListData.length - 1] || {};
}
function DOCnewestListItem(){
   return DOCshowListData[0] || {};
}

const DOCshowListAppend = (Data) => {
   const Div = document.querySelector('.DOC2list');
   if (!Div) return;
   DOCremoveTopRows(DOCnumberOfDocsToRemove(Data, DOCshowListData));
   DOCshowListData = DOCcombineListsBackward(Data, DOCshowListData);
   const StartIdx = DOCshowListData.length - Data.length;
   const H = DOCshowListData.slice(StartIdx).map((_, i) => DOClistItemIdx(i + StartIdx, DOCshowListData)).join('');
   Div.insertAdjacentHTML('beforeend', H);
}

const DOCshowListPrepend = (Data) => {
   const Div = document.querySelector('.DOC2list');
   if (!Div) return;
   DOCremoveBottomRows(DOCnumberOfDocsToRemove(Data, DOCshowListData));
   DOCshowListData = DOCcombineListsForward(Data, DOCshowListData);
   const H = Data.map((_, i) => DOClistItemIdx(i, DOCshowListData)).join('');
   Div.insertAdjacentHTML('afterbegin', H);
};

function DOCnumberOfDocsToRemove(arr1, arr2) {
   return Math.max(arr1.length + arr2.length - MAX_DOC_ENTRIES, 0);
}

const DOCremoveTopRows = (n) => {
   const Docs = document.querySelectorAll('.DOC2listRow');
   for (let i = n - 1; i >= 0; i--) {
       Docs[i]?.remove();
   }
}

const DOCremoveBottomRows = (n) => {
   const Docs = document.querySelectorAll('.DOC2listRow');
   const DocsLength = Docs.length - 1;
   for (let i = DocsLength; i > DocsLength - n; i--) {
       Docs[i]?.remove();
   }
}

function DOCcombineListsBackward(NewArray, OldArray) {
   const CombinedArray = OldArray.concat(NewArray);
   return CombinedArray.slice(-MAX_DOC_ENTRIES);
}

function DOCcombineListsForward(NewArray, OldArray) {
   const CombinedArray = NewArray.concat(OldArray);
   return CombinedArray.slice(0, MAX_DOC_ENTRIES);
}