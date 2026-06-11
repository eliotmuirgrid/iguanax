function SSOrefreshToken() {
   const Revalidate = SESScache?.should_revalidate;
   const Provider   = SESScache?.sso_provider;
   const Mode       = SESScache?.sso_mode;
   if(Provider === 'Microsoft' && Mode === 'Client' && (Revalidate || SSO_MSAL == null)) { return SSOacquireMicrosoftTokenSilent(); }
   if(Mode === 'SAML' && Revalidate) { return SSOrevalidateSaml(); }
}

function SSOrevalidateSaml() {
   APIcall('auth/saml/revalidate', {hash: window.location.hash}, function(Response) {
      if(Response?.success && Response?.data?.redirect_url) {
         window.location.href = Response.data.redirect_url;
      } else if(Response?.error) {
         console.error('Failed to revalidate SAML session.');
      }
   });
}
