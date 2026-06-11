let CMTcommitEditPop = null;
let CMTcomponentSelected = null;

function CMTdetetmineComponentSelected(){
   let Component = CARDrunFromComponent();
   if(Component) return Component;
   return CARDcomponentId();
}

function CMTrender(){
   CMTcomponentSelected = CMTdetetmineComponentSelected();
   CMTcommitEditPop = POPpopoutOpen({
      title : "Select a Commit",
      content : CMTskeleton(),
      className: 'CMTcommitHelpPopWindow',
      width : "800px",
      height : "90%",
   });
   
   document.querySelector('.CMTcommitHelpPopWindow .POPpopoutCard-content').classList.add('CMTcommitHelpPop');
   CMTmodel();
   CMTsetListeners();   
}

function CMTcommitHelpRowsHtml(Commits) {
   let Html = "";
   for (let Commit of Commits) {
      let   DateData      = Commit?.date;
      const ConvertedDate = DateData ? CMTconvertedDate(DateData) : "";
      const CommitTime    = CMTformatTime(DateData);
      const CommitDate    = CMTformatDate(DateData);
      Html += /*html*/`
      <div class='CMTcommitHelpRow'>
         <div class='CMTcommitHelpHash'>${Commit?.hash}</div>
         <div>${Commit?.message}</div>
         <div title="${CommitDate} at ${CommitTime}">${ConvertedDate}</div>
      </div>`;
   }
   return Html;
}

function CMTcommitHeaderHtml(){
   return /*html*/`
   <div class='CMTcommitHelpTable'>
      <div class='CMTcommitHelpHeader'>
         <div class='CMTcommitHelpHeading'>Hash</div>
         <div class='CMTcommitHelpHeading'>Message</div>
         <div class='CMTcommitHelpHeading'>Date</div>
      </div>
      <div class="CMTcommitTarget">
      </div>
   </div>`;
}

function CMTselectComponentHtml(){
   if(!CARDgitPlain()) return "";
   return /*html*/`            
   <div class="CMTcomponentBox">
      <div class="FORMlabel">Component:</div>
      <select class="FORMselectBox CMTcomponentSelectBox"></select>
   </div>`;
}

function CMTupdateComponentCheckboxHtml(){
   if(!CARDgitPlain()) return "";
   let storedState = !(STORElocalRead(CMTcachedCheckbox) === 'false') ? "checked" : "";
   return /*html*/`
   <div class='CMTcommitHelpRunFromComponentBox'>
      <input type='checkbox' class='CMTcommitHelpRunFromComponent' ${storedState}>
      <div class='CMTupdateComponentLabel'>Update component to run from to the selected component.</div>
   </div>`;
}

function CMTskeleton(){ 
   let div = document.querySelector(".CARDcommitInputValue");
   let Commit = div ? div.value?.trim() : "";
   return /*html*/`
   <div class="CMTcommitHelpModalContent">
      <div class="CMTcommitHelpModal ">
         <div>
            ${CMTselectComponentHtml()}
            ${CMTupdateComponentCheckboxHtml()}
         </div>
         ${CMTcommitHeaderHtml()}
         <div class="FORMlabel CMTcommitHelpLabel">Commit Selected:</div>
         <input type='text' class='FORMinput CMTcommitHelpValue' value="${Commit}">
         <div class='BUTTONgroup CMThelpFooter'>
            <button class='BUTTONstandard BUTTONdefault CMTcancelAction'>CANCEL</button>
            <button class='BUTTONaction BUTTONstandard CMTokAction'>CONFIRM</button>
         </div>
      </div>
   </div>`;
}