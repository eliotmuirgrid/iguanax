let SESSserverInterval;
let SESSmodeInterval;
let SESScacheInterval;

// DO NOT REMOVE THIS -- it is updated by the webpacker -- see IX-2374
let APP_HASH;

function SESSclearLoginIntervals() {
   clearInterval(SESSserverInterval);
   clearInterval(SESSmodeInterval);
   clearInterval(SESScacheInterval);
}

function SESSsetupIntervals() {
   SESSclearLoginIntervals();
   SESSserverInterval = setInterval(SESSserverInfo, 5000);
   SESSmodeInterval   = setInterval(SESSgetMode, 5000);
   SESScacheInterval   = setInterval(SESScheckCache, 5000);
   SESSserverInfo();
   SESSgetMode();
   SESScheckCache();
}

function SESSserverInfo() {
   APIcall('header', {}, SESSserverCallback);
   APIcall('os', {}, SESSsetupForgotPassword);
}

function SESSserverCallback(R) {
   SESSupdateVersion(R);
   SESStabTitle(R);
}

function SESSupdateVersion(R) {
   const VersionDiv = document.querySelector('.SESSloginVersion');
   if(VersionDiv && R.success && VersionDiv.innerText !== `v${R.data.version}`) { VersionDiv.innerText = `v${R.data.version}`; }
}

function SESStabTitle(R) {
   const ServerLabel = R.data.server_description;
   const Title       = ServerLabel || "IguanaX";
   if(document.title !== Title) { document.title = Title; }
}

function SESSsetupForgotPassword(R) {
   const passDiv = document.querySelector('.SESSforgotPasswordDiv');
   if(!R?.data || !passDiv) { return; }
   const Links = {
      "macos"  : LINKsessMacForgotPassword,
      "windows": LINKsessWindowsForgotPassword,
      "linux"  : LINKsessLinuxForgotPassword
   };
   passDiv.innerHTML = `<a class="SESSforgotPassword" target="_blank" href="${Links[R.data]}">I forgot my password</a>`;
}

function SESSgetMode() { APIcall('mode', {}, SESSmodeCallback); }

// TODO there should be a better way to do this but seems sufficient for now
function SESSmodeCallback(R) {
   if(SESSisInitializationMode(R)) {
      SESSclearLoginIntervals();
      return STARTUPsetup();
   }
   if(SESSisEncryptionMode(R)) { location.reload(); }
}

function SESSisInitializationMode(R) { return R.success && R.data.mode === 'initialization'; }
function SESSisEncryptionMode(R) { return R.success && R.data.mode === 'log_unlock'; }

function SESScheckCache() {
   APIcall('hash', {}, function(R) {
      if(R?.data?.hash && R.data.hash !== APP_HASH) { location.reload(); }
   })
}