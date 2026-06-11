let ADCcard;
let ADCnewCollectionGuid = "NewCollection$1234$";
let ADCselectedCollection = "ADCselectedCollection";
let ADCaddedData = {};

function ADCrender(AddedData, SelectedCollection){
   ADCaddedData = AddedData;
   if(SelectedCollection) STORElocalCreate(ADCselectedCollection, SelectedCollection);
   APIcall('git/catalogue', {}, function(R){
      if(!R.success) return;
      let Content = ADChtml(R.data);
      ADCcard = POPpopoutOpen({title: 'Add to collection', width : "700px", content: Content});
      document.querySelector('.ADCselectCollection').addEventListener('change', function() { ADCsaveSelectedCollection(); });
      document.querySelector('.ADCselect').addEventListener('click', function() { ADCrequest(); });
      document.querySelector('.ADCcancel').addEventListener('click', function() { POPpopoutClose(ADCcard);  });
   });
}

function ADCsaveSelectedCollection(){
   let Selected = document.querySelector('.ADCselectCollection');
   if(!Selected) return;
   STORElocalCreate(ADCselectedCollection, Selected.value);
}

function ADCrequest(){
   let SelectedCollection = document.querySelector('.ADCselectCollection');
   if(!SelectedCollection) return;
   let Collection = SelectedCollection.value;
   if(Collection == ADCnewCollectionGuid) ADCnewCollection();
   else                                   EDCinit(Collection, ADCaddedData);
}

function ADCnewCollection(){
   // CRTrender();
   ADCcreateRender();
   POPpopoutClose(ADCcard);
}

function ADCcollectionRow(CollectionData){
   let SelectedCollection = STORElocalRead(ADCselectedCollection);
   let Selected = SelectedCollection === CollectionData.git ? "selected" : "";
   return /*html*/`<option value='${CollectionData.git}' ${Selected}>${CollectionData.name}</option>`;
}

function ADCcollectionRows(CollectionsData){
   let html = /*html*/`<option value="${ADCnewCollectionGuid}">+ Create a Collection +</option>`;
   // let html = /*html*/``;
   for(let i=0; i < CollectionsData.length; i++) html = html + ADCcollectionRow(CollectionsData[i]);
   return html;
}

function ADChtml(R){
   return /*html*/`
   <div class="ADCgridRow">
      <div class="FORMlabel">Collection:</div>
      <div><select class="FORMselectBox ADCselectCollection">${ADCcollectionRows(R)}</select></div>
   </div>
   <div class="BUTTONgroup ADCbuttons">
      <div class='ADCcancel BUTTONcancel BUTTONstandard'>CANCEL</div>
      <div class='ADCselect BUTTONaction BUTTONstandard'>SELECT</div>
   </div>
   `;
}