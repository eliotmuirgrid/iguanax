let STSnewPort;
let STSsecure;

function STSserverEditScreen(){
   APIcall("web/server", {}, function(R){
      if(!R.success) STSerror     (R.error);
      else           STSrenderEdit(R.data);
   });
}

function STSerror(Error){
   SNCKsnackbar('error', "Could not get server information: " + Error);
   window.location.hash = "#settings/webserver";
}

function STSrenderEdit(Data){
   SETrender();
	document.querySelector('.STSserver').classList.add('selected');
   let Div = document.querySelector('.SETcontentBody');
   if(!Div) return;
   Div.innerHTML = STSserverEditScreenHtml(Data);
   STSsecureClicked();
   STScolorChanged();
   document.querySelector('.STSsave')             .addEventListener('click',   function(){ STSsave(); });
   document.querySelector('.STSbrowseCertificate').addEventListener('click',   function(){ STSbrowseCertificate(); });
   document.querySelector('.STSbrowsePrivateKey') .addEventListener('click',   function(){ STSbrowsePrivateKey(); });
   document.querySelector('.STSbrowseColor')      .addEventListener('click',   function(){ STScolorClicked(); });
   document.querySelector('.STSsecureInput')      .addEventListener('change',  function(){ STSsecureClicked(); });
   document.querySelector('.STScolorInput')       .addEventListener('keydown', function(){ STScolorChanged(); });
   document.querySelector('.STScolorInput')       .addEventListener('keyup',   function(){ STScolorChanged(); });
   FORMenterListener(document.querySelector('.STSdescriptionInput'), STSsave);
   FORMenterListener(document.querySelector('.STSportInput'),        STSsave);
   FORMfocus(document.querySelector('.STSdescriptionInput'));
}

function STSsaveArgs(){
   let Description = document.querySelector('.STSdescriptionInput');
   let Port        = document.querySelector('.STSportInput');
   let Color       = document.querySelector('.STScolorInput');
   let Secure      = document.querySelector('.STSsecureInput');
   let Certificate = document.querySelector('.STScertificateInput');
   let PrivateKey  = document.querySelector('.STSprivateKeyInput');
   
   let Args = {};
   Args.description = Description.value.trim();
   Args.port        = Port.value.trim();
   Args.color       = Color.value.trim();
   Args.secure      = Secure.checked;
   Args.cert_key    = Certificate.value.trim();
   Args.private_key = PrivateKey.value.trim();
   return Args;
}

function STSsave(){
   let Args = STSsaveArgs();
   STSnewPort = Args.port;
   STSsecure  = Args.secure;
   APIcall("web/server/set", STSsaveArgs(), function(R){
      if(!R.success) return STShandleError(R);
      let Message = GINsaveWebServer();
      GINaddAndCommit({message: Message, settings: true});
      if(!R?.data?.restart) {
         window.location.hash = "#settings/webserver";
         return SNCKsnackbar("success", "Saved changes to server settings");
      }
      STSrestart();
   });
}

function STShandleError(R){
   let Data = R.data;

   let DescriptionError = document.querySelector(".STSdescriptionError");
   let PortError        = document.querySelector(".STSportError");
   let CertificateError = document.querySelector(".STScertificateError");
   let PrivateKeyError  = document.querySelector(".STSprivateKeyError");
   
   if(DescriptionError && Data.description_error) DescriptionError.innerHTML = Data.description_error;
   if(PortError        && Data.port_error)        PortError.innerHTML        = Data.port_error;
   if(CertificateError && Data.certificate_error) CertificateError.innerHTML = Data.certificate_error;
   if(PrivateKeyError  && Data.key_error)         PrivateKeyError.innerHTML  = Data.key_error;
   
   SNCKsnackbar("error", R.error);
}

function STSbrowseCertificate(){
   let Div = document.querySelector(".STScertificateInput");
   FILbrowser({
      value : Div.value,
      new_folder : false,
      show_hidden: true,
      callback   : function(T) {
         Div.value = T;
      }
   });
}

function STSbrowsePrivateKey(){
   let Div = document.querySelector(".STSprivateKeyInput");
   FILbrowser({
      value : Div.value,
      new_folder : false,
      show_hidden: true,
      callback   : function(T) {
         Div.value = T;
      },
   });
}

function STSsecureClicked(){
   let Div = document.querySelector('.STSsecureSection');
   let SecureDiv = document.querySelector('.STSsecureInput');
   if(!SecureDiv) return;
   let Visible = SecureDiv.checked;
   STSupdateVisible(Div, Visible);
}

function STSserverEditScreenHtml(Data){
   return /*html*/`
   <div class="SETsettingsPaneLayout SETsettingsPane">
      <div class="SETnotificationsTitle">
         <div class="SETnotificationsLabel">Web Server</div>
         <div class="SETnotificationActions BUTTONgroup">
            <div class="STSsave BUTTONaction BUTTONstandard">SAVE</div>
            <a href="#settings/webserver" class="STNcancel BUTTONcancel BUTTONstandard">CANCEL</a>
         </div>
      </div>
      <div class="SETpageDescription">Customize the settings for the web server.</div>
      <div class=" SETversionPanel">
         <div class="STStable">
            <div class="FORMlabel">Description:</div><input class="FORMinput STSdescriptionInput" value="${ ESChtml(Data.description) || "" }">
            <div></div><div class="STSdescriptionError"></div>
            <div class="FORMlabel">Port:</div>       <input class="FORMinput STSportInput"        value="${ Data.port        || "" }">
            <div></div><div class="STSportError"></div>
            <div class="FORMlabel">Color:</div>      
            <div class="STScolorRow">
               <div class="FORMinputGroup STSvalueColor">
                  <input class="FORMinput STScolorInput" value="${ Data.color || "" }">
                  <div class="STSbrowseColor BUTTONstandard BUTTONcancel">...</div>
                  <div class="STScolor FORMinput"></div>
               </div>
            </div>
            <div></div><div class="STScolorError"></div>
         </div>
         <div class="STStable">
            <div class="FORMlabel">Use HTTPS:</div>
            <div ><input class="STSsecureInput" type="checkbox" ${Data.secure ? "checked" : ""}></div>
         </div>
         <div class="STSsecureSection">
            <br>
            <div class="STStable">
               <div class="FORMlabel">Certificate File:</div>
               <div class="FORMinputGroup STSvalue ">
                  <input type="text" value="${Data.cert_key || ""}" class="FORMinput STScertificateInput">
                  <div class="STSbrowseCertificate BUTTONstandard BUTTONcancel">...</div>
               </div>
               <div></div><div class="STScertificateError"></div>
               <div class="FORMlabel">Private Key File:</div>
               <div class="FORMinputGroup STSvalue ">
                  <input type="text" value="${Data.private_key || ""}" class="FORMinput STSprivateKeyInput">
                  <div class="STSbrowsePrivateKey BUTTONstandard BUTTONcancel">...</div>
               </div>
               <div></div><div class="STSprivateKeyError"></div>
            </div>
         </div>
      </div>
   </div>`;
}