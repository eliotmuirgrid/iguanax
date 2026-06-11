function UPGRADEchangelogHtml(Data){
   let Div = document.querySelector(".UPGRADEchangelog-container");
   if (!Div) return;
   Div.innerHTML = Data;
}

function UPGRADEgetChangelog(Version){
   APIcall('release/changelog', { "version" : Version }, function(R){
      if(!R.success) { UPGRADEchangelogHtml(R.error); }
      UPGRADEchangelogHtml(R.data.changelog);
   });
}

function UPGRADEupgrade(){
   COL_TRC("Upgrade screen.");
   PAGEresetPage();
   CORpreparePage();
   CORheader();
   CORsetNavIcon('CORsettingsIcon');
   SETrender();
   let Div = document.querySelector('.SETcontentBody');
   if(!Div) return;
   APIcall('release/info', {}, function(R) {
      if(!R.success) { return; } // TODO error screen
      Div.innerHTML = UPGRADEmainHtml(R.data);
      UPGRADEgetChangelog(R.data.version);
      document.querySelector(".UPGRADEdoit").addEventListener("click", function() { UPGRADEexecute(); });
   })
}

function UPGRADEmainHtml(Data) {
   return /*html*/`
   <div class="SETsettingsPaneLayout SETsettingsPane">
      <div class="SETdatabaseTitle">
         <div class="SETupgradeLabel">Update IguanaX</div>
         <div class="SETdatabaseActions BUTTONgroup">
            <div class="BUTTONaction BUTTONstandard UPGRADEdoit">UPGRADE</div>
         </div>
      </div>
		<div class="SETversionPanel ">
         <div class="SETversionLabel">Upgrade to version:</div>
         <div class="SETversionDetail STSdescription">${Data.version}</div>
         <br>
      <div class="UPGRADEabout">
            <div class="UPGRADEchangelog-container">
               <div class="UPGRADEspinner-container">
                  <div class="UPGRADEspinner"></div><div class="UPGRADEspinnerText">Loading changelog data...</div>  
               </div>
            </div>
         </div>
		</div>
   </div>`;
}