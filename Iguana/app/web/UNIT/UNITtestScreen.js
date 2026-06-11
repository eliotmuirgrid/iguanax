function UNITtestScreen() {
   COL_TRC("Upgrade screen.");
   PAGEresetPage();
   CORpreparePage();
   CORheader();
   CORsetNavIcon('CORsettingsIcon');
   document.querySelector('.CORmain').innerHTML = UNITtestHtml();
   document.querySelector(".UNITrun").addEventListener("click", function(E) { UNITrunTests(); });
}

function UNITtestHtml() {
   return `
  <div class="UNITtestContainer">
    <div class="BUTTONaction BUTTONstandard UNITrun">RUN TESTS</div>
    <div class="UNITtestColumns">
      <div class="UNITtestColumn">
      <div class="UNITheader">SUCCESSFUL TESTS</div>
        <div class="UNITsuccess"></div>
      </div>
      <div class="UNITtestColumn">
      <div class="UNITheader">FAILED TESTS</div>
        <div class="UNITfail"></div>
        <!-- Right column content goes here -->
      </div>
    </div>
  </div>`
}