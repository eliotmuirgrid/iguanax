let NWCsourceComponentName = "";

function NWCupdateSourceComponent(){
   let SourceComponent = PAGEhashParameter("source");
   if(!SourceComponent) return;
   if(NWCsourceComponentName) return;
   let data = MODELdata?.NWCcomponentList?.data?.component_data;
   if(!data) return;
   for(let component of data){
      if(component.guid == SourceComponent){
         NWCsourceComponentName = component.name;
         break;
      }
   }
   let Html = NWCsourceComponentHtml();
   let div = document.querySelector('.NWCsourceComponentDiv');
   if(!div) return;
   if(div.innerHTML) return;
   div.innerHTML = Html;
}

function NWCsourceComponentHtml(){
   return /*html*/`
   <div class="NWCsourceComponent">
      <div class="NWCsourceComponentName"><b>${NWCsourceComponentName}</b> will be the source for the components you add.
      </div>
   </div>
   `;
}