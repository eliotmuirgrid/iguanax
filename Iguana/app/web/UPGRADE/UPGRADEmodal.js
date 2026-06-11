function UPGRADEgetGuid(){
   let Guid = document.querySelector(".UPGRADEguid")?.value?.trim();
   COL_TRC(Guid);
   if (Guid) { // we're on #upgrade_special, rely on the hash provided on this page
      return Guid;
   }
}

function UPGRADEgetVersion(){
   return MODELdata?.CORupgradable?.data?.version;
}

function UPGRADEexecuteSpecial(){
   POPpopoutOpen({title: 'Upgrade IguanaX', content: UPGRADEmodalHtml(), esc_close: false, dismissible: false});
   UPGRADEcontinueListener(UPGRADEdownloadSpecial);
}

function UPGRADEdownloadSpecial(){
   let Guid = UPGRADEgetGuid();
   COL_TRC(Guid);
   if(!Guid) {
      console.warn("aborting update - unable to get guid");
      return;
   }
   APIcall("upgrade/download/special", {guid : Guid}, function(R) { UPGRADEdownloadDone(R, Guid); });
}

function UPGRADEdownload(){
   let Version = UPGRADEgetVersion();
   document.querySelector('.POPpopoutCard--close').remove();
   APIcall("upgrade/download", {version : Version}, function(R) { UPGRADEdownloadDone(R, Version); });
}

function UPGRADEexecute() {
   COL_TRC("Let's do the upgrade.");
   POPpopoutOpen({title: 'Upgrade IguanaX', content: UPGRADEmodalHtml(), esc_close: false, dismissible: true});
   UPGRADEcontinueListener(UPGRADEdownload);
}

function UPGRADEdownloadDone(R, Version) {
   COL_TRC(R);
   if(!R.success) { return UPGRADEappendError(`Download failed ❌\n${R.error}`); }
   UPGRADEappendSuccess('Download complete ✔️');
   APIcall("upgrade/replace", {version: Version}, function(R) { UPGRADEdone(R); });
}

function UPGRADEdone(R) {
   COL_TRC(R);
   R.success ? UPGRADEinstallSuccess() : UPGRADEappendError(`Install failed ❌\n${R.error}`)
}

function UPGRADEmodalHtml() {
   return `
    <div class="UPGRADEmodal-container">
        <h2 class="UPGRADEmodal-text">Upgrade in progress...</h2>
        <h4 class="UPGRADEmodal-text-info">The updater will download the new version of IguanaX and install it. During this process, you will be redirected to the settings page and may notice some flickering. To complete this process, IguanaX will restart and you will be logged out.</h4>
        <div class="UPGRADEcontinue BUTTONstandard BUTTONaction">CONTINUE</div>
        <div class="UPGRADEsuccess-container"></div>
        <div class="UPGRADEerror-container"></div>
    </div>`;
}

function UPGRADEcontinueListener(Callback){
   const Button = document.querySelector('.UPGRADEcontinue');
   if (!Button) { return; }
   Button.addEventListener('click', function(){
      Button.remove();
      UPGRADEaddSpinner();
      Callback();
   });
}

function UPGRADEappendSuccess(SuccessText) {
   const SuccessContainer = document.querySelector('.UPGRADEsuccess-container');
   const Success          = document.createElement('p');
   Success.textContent    = SuccessText;
   Success.classList.add('UPGRADEsuccess-text');
   SuccessContainer.appendChild(Success);
}

function UPGRADEappendError(ErrorText) {
   UPGRADEremoveSpinner();
   UPGRADEmodalText('Upgrade Failed', true);
   const ErrorContainer = document.querySelector('.UPGRADEerror-container');
   const Error          = document.createElement('p');
   Error.textContent    = ErrorText;
   Error.classList.add('UPGRADEerror-text');
   ErrorContainer.appendChild(Error);
   UPGRADEfailedButton();
}

function UPGRADEinstallSuccess() {
   UPGRADEremoveSpinner();
   UPGRADEappendSuccess('Upgrade successful ✔️');
   window.location.hash = '';
   SESSlogout();
   CORrefresh();
}

function UPGRADEmodalText(Text, isError) {
   const ModalText = document.querySelector('.UPGRADEmodal-text');
   if(!ModalText) { return; }
   isError ? ModalText.classList.add('UPGRADEerror-text') : ModalText.classList.remove('.UPGRADEerror-text');
   ModalText.innerHTML = Text;
}

function UPGRADEfailedButton() {
   const FinishButton       = document.createElement('div');
   FinishButton.textContent = 'CLOSE';
   FinishButton.classList.add('BUTTONstandard', 'BUTTONaction', 'UPGRADEfailedBtn');
   const ErrorContainer = document.querySelector('.UPGRADEerror-container');
   ErrorContainer.appendChild(FinishButton);
   document.querySelector('.UPGRADEfailedBtn').addEventListener('click', UPGRADEremoveCard);
}
function UPGRADEremoveCard()    { POPpopoutRemoveAll(); }
function UPGRADEremoveSpinner() { document.querySelector('.UPGRADEspinner-container')?.remove(); }
function UPGRADEaddSpinner()    {
   let Spinner = document.createElement("div");
   Spinner.classList.add("UPGRADEspinner-container");
   Spinner.innerHTML = `<div class="UPGRADEspinner"></div><div class="UPGRADEspinnerText"></div>`;
   document.querySelector('.UPGRADEmodal-text-info')?.after(Spinner)
}
