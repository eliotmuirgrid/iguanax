function SETtemplateViewHtmlMakeRow(Item){
   let Link = Item.web;
   return /*html*/`<div class="SETcatalogueRow">
      <div class="SETcatalogueData">
         ${Link ? `<a class="SETlink" href="${Link}" target="_blank">${ESChtmlEscape(Item.git)}</a>` : `${ESChtmlEscape(Item.git)}` }         
      </div>
      <div class="SETcatalogueData SETcatalogueDataName">${ESChtmlEscape(Item.name)}</div>
      <div class="SETcatalogueData">${ESChtmlEscape(Item.description)}</div>
      <div class="SETcatalogueData SETeditCollection">Modify</div>
   </div>`;
}

function SETcreateNewCollectionHtml(){
   let Edit = STCeditPermission();
   let Classes = Edit ? "SETcreateAndAddCollection" : "STAlinkDisabled STCnoAddCollection";
   let title = !Edit ? `title="You do not have permission to do this."` : ""
   return /*html*/`
   <div class="${Classes}" ${title}>+ Create a Collection +</div>`;
}

function SETtemplateViewHtml(RefreshInfo){
   let CollectionData = MODELdata.SETcollection.data;
   let Rows = "";
   for (let i in CollectionData){
      Rows += SETtemplateViewHtmlMakeRow(CollectionData[i]);
   }
   return /*html*/ `<div class="SETsettingsPaneLayout SETsettingsPane SETgitCollectionView">
      <div class="SETgitTitle">Collections
         <div class="BUTTONgroup">
            <div class="SETeditCatalogue BUTTONaction BUTTONstandard">EDIT</div>
         </div>
      </div>
      <div class="SETpageDescription">${STCcollectionDescription()}</div>
      ${ SETcreateNewCollectionHtml() }
      <br>
      <br>
      <div class="SETcatalogueTable">
         <div class="SETcatalogueHeading">
            <div class="SETcatalogueTableHeading">Repository</div>
            <div class="SETcatalogueTableHeading">Name</div>
            <div class="SETcatalogueTableHeading">Description</div>
            <div ></div>
         </div>
         ${Rows}
      </div>
      ${SETemptyList('collections', CollectionData.length)}
      <div class="SETtotalRepos">TOTAL COLLECTIONS: ${CollectionData?.length}</div>
      <div class="SETdefaultRepoRef"></div>
      <div class="SETrefreshCacheGrid">
         <div class="SETrefreshCache">Refresh cache</div>
         <div class="SETrefreshCacheInfo">${RefreshInfo}</div>
      </div>
   </div>`
}

function STCcollectionDescription(){
   return /*html*/`Collections are curated lists of git repositories containing reusable components and libraries, allowing users to easily access frequently used resources. 
   [<a href="${LINKsettingsCollectionsHeader}" target="_blank">Learn more</a>]`;
}

function SETtemplateEditHtml(Data){
   let Html = /*html*/`
   <div class="SETsettingsPaneLayout SETsettingsPane SETgitCollectionEdit">
      <div class="SETgitTitle">Collections
         <div class="BUTTONgroup">
            <div class="SETsaveCatalogue BUTTONaction BUTTONstandard">SAVE</div>
            <div class="SETcancelCatalogue BUTTONcancel BUTTONstandard">CANCEL</div>
         </div>
      </div>
      <div class="SETpageDescription">${STCcollectionDescription()}</div>
      ${ SETcreateNewCollectionHtml() }
      <br>
      ${SETresetToDefaultHtml("Add the Default Collections", "SETresetToDefaultCollectionsLink")}
      <br>
      <div class="SETcatalogueTable">
         <div class="SETcatalogueHeading">
            <div class="SETcatalogueTableHeading">Href</div>
         </div>`
   for(i in Data){
      Html += /*html*/`
         <div class="SETcatalogueRow"  data-href="${ESChtmlEscape(i)}">
            <div class="SETcatalogueData"><input class="FORMinput SETcatalogueEditValue"   type="text" placeholder="Enter href"    value="${ESChtmlEscape(i)}"></div>
            <div class="SETcatalogueData">
               <div class="button-group">
                  <div class="FORMbutton BUTTONcancel BUTTONwarning SETdeleteRow">
                     ${SETtrashIcon()}
                  </div>
               </div>
            </div>
         </div>`
   }
   Html += /*html*/`
      </div>
      <div class="SETaddRow">
         <svg class="SETaddIcon" width="14" height="14">
            <use href="SET/SETiconAdd.svg#SETiconAdd"></use>
         </svg>
         COLLECTION
      </div>
   </div>`
   return Html;
}

const SETtemplateNewRowHtml = /*html*/`
<div class="SETcatalogueRow">
   <div class="SETcatalogueData"><input class="FORMinput SETcatalogueEditValue" type="text" placeholder="Enter href"></div>
   <div class="SETcatalogueData">
      <div class="button-group">
         <div class="FORMbutton BUTTONcancel BUTTONwarning SETdeleteRow">
            ${SETtrashIcon()}
         </div>
      </div>
   </div>
</div>
`
