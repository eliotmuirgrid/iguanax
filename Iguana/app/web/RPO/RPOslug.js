function RPOslugData() { return MODELdata?.RPOslugData?.data; }
function RPOslugDataError() { return MODELdata?.RPOslugData?.error; }
function RPOslugIsPermissionError(ErrMsg) { return ErrMsg === "Your credentials lack one or more required privilege scopes."; }

function RPOupdateSlugExists() {
   let SlugDiv   = document.querySelector('.RPOslug');
   let SlugError = document.querySelector('.RPOfeedback');
   let SlugData  = RPOslugData();
   if(!SlugDiv || !SlugData || !SlugError) { return; }
   let SlugExists = SlugData.slug_exists && SlugDiv.value.trim() !== '';
   let SshUrl = RPOsshUrl();
   let WebUrl = RPOwebUrl();
   let ErrorHtml  = SlugExists ? RPOslugExists(SshUrl, WebUrl) : '';
   if(SlugError.innerHTML !== ErrorHtml) { SlugError.innerHTML = ErrorHtml; }
}

function RPOslugExists(SshUrl, WebUrl) {
   if(RPOallowConnect) {
      return `Component will connect to existing repo: <a class="RPOlink" href="${WebUrl}" target="_blank">${SshUrl}</a>`;
   }
   return `<p style="color: red">This git url is already in use: <a class="RPOlink" href="${WebUrl}" target="_blank">${SshUrl}</a></p>`;
}

function RPOupdateSlug(){
   let ErrorMessage = RPOverifySlug();
   let SlugError    = document.querySelector('.RPOslugFeedback');
   if(!SlugError) { return; }
   RPOslugError = ErrorMessage !== "";
   if(SlugError.innerHTML !== ErrorMessage) SlugError.innerHTML = ErrorMessage;
   // Also check for a permission error
   let SlugPermError = RPOslugDataError()
   if (RPOslugIsPermissionError(SlugPermError)) {
      console.log("slugpermerror found");
      RPOslugError = true;
      SlugError.innerHTML = SlugPermError;
   }
}

function RPOverifySlug(){
   let GitData = RPOgitData();
   let HostData = RPOownersData()?.host;
   let ErrorMessage = "";
   let SlugDiv = document.querySelector('.RPOslug');
   if(!SlugDiv) return "There was an issue rendering";
   let Slug = SlugDiv.value.trim();
   if(Slug.length > 62  && GitData.platform === "bitbucket") ErrorMessage = "Bitbucket repository cannot exceed 62 characters";
   if(Slug.length > 100 && GitData.platform === "github")    ErrorMessage = "Github repository cannot exceed 100 characters";
   if(Slug.length === 0) ErrorMessage = "Repository must not be empty";
   if(Slug[0] === ".") ErrorMessage = "Repository cannot start with a period";
   if (HostData && HostData != 'GitLab'){
      if (!/^[a-z0-9_.-]+$/g.test(Slug)) ErrorMessage = "Repository must be lowercase, alphanumerical, and may also contain underscores, dashes, or dots.";
   }
   if (Slug.endsWith(".git"))         ErrorMessage = "Repository cannot end with .git.";
   return ErrorMessage;
}
