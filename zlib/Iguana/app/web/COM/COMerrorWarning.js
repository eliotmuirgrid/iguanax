function COMgitCredentialsWarning(){
   let Data = MODELdata?.COMgitCredentials?.data || {};
   return !Data?.name?.length || !Data?.email?.length;
}

function COMerrorWarning(){
   let Error = COMgitCredentialsWarning();
   let Div = document.querySelector('.DASHgitHistoryFadeSection');
   if(!Div) return;
   let Html = Error ? `<div class="COMerrorWarning">You must set your Git credentials to make commits to the instance. [<a class="COMerrorWarningLink" href="/#settings/git">Set Git Credentials</a>]</div>` : "";
   if(Html !== Div.innerHTML) Div.innerHTML = Html;
}