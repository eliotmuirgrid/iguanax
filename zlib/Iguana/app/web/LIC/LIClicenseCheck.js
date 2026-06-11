function LICattachSettingsLink(){
   return " <a href='#settings/license'>[Learn more]</a>";
}

function LICisLicenseError(ErrorMessage){
   const CheckMe = ErrorMessage.toLowerCase();
   return CheckMe.includes("license");
}

// Returns false if error detected
function LICcheckComponentStartError(Api, Result, Callback){
   if(Api == 'component/start' && !Result.success && LICisLicenseError(Result.error)) {
      let ErrorMessage = Result.error + LICattachSettingsLink();
      SNCKsnackbar('error', ErrorMessage, 3000, false); // Component start could fail if over the license limit
      if (Callback) { Callback(); }
      return false;
   }
   return true;
}

// Returns false if error detected
function LICcheckComponentCreateError(Result){
   if(!Result.success && Result.error && LICisLicenseError(Result.error)) {
      let ErrorMessage = Result.error + LICattachSettingsLink();
      SNCKsnackbar('error', ErrorMessage, 3000, false); // Component create could fail if over the license limit
      return false;
   }
   return true;
}