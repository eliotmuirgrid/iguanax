function DEVhtmlRendered(){ return document.querySelector('.DEVbody'); }

function DEVscreenHtml(){
   return /*html*/`
   <div class="DEVContainer">
      <div class="DEVheader">
         <div class="DEVheading">Development Tools</div>
      </div>
      <div class="DEVwrapper">
         <div class="DEVbody">
            <p>These are tools we find useful but are not customer facing</p>
            <p><a href="#upgrade_special">Upgrade to a beta hash release</a></p>
            <p><a href="#unittests">Run javascript unit tests</a></p>
         </div>
      </div>
   </div>`;
}

function DEVscreenImp(){
   PAGEresetPage();
   CORpreparePage();
   CORheader();
   CORsetNavIcon('CORsettingsIcon');
   if (!DEVhtmlRendered()){
      document.querySelector('.CORmain').innerHTML = DEVscreenHtml();
   }

}

function DEVscreen(){
   DEVscreenImp();
}
