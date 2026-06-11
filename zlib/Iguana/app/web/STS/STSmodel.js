function STSmodelInit(){
   MODELpollerAdd('STSmodel', 'STSpermission', 5, 'component/permission', {});
   MODELpollerAdd('STSmodel', 'STSserverData', 5, 'web/server',  {});
   MODELviewAdd  ('STSmodel', 'STSupdateDescription');
   MODELviewAdd  ('STSmodel', 'STSupdateColour');
   MODELviewAdd  ('STSmodel', 'STSupdatePort');
   MODELviewAdd  ('STSmodel', 'STSupdateSecure');
   MODELviewAdd  ('STSmodel', 'STSupdatePermissionEdit');
   MODELviewAdd  ('STSmodel', 'STSupdateCertificate');
   MODELviewAdd  ('STSmodel', 'STSupdatePrivateKey');
   MODELviewAdd  ('STSmodel', 'STSupdateSecureVisible');
   MODELforcePoll('STSmodel');
   MODELstart    ('STSmodel');
}

function STSeditPermission(){
   return MODELdata?.STSpermission?.data?.edit_webserver || false;
}

function STSserverData(){
   return MODELdata?.STSserverData?.data;
}

function STSupdateDescription(){
   let Div = document.querySelector('.STSdescription');
   if(!Div) return;
   let Description = STSserverData().description;
   if(Div.innerHTML !== Description) Div.innerHTML = Description;
}

function STSupdateColour(){
   let Div = document.querySelector('.STScolorValue');
   let Div2 = document.querySelector('.STScolorView');
   if(!Div || !Div2) return;
   let Color = STSserverData().color;
   if(Div.innerHTML !== Color) Div.innerHTML = Color;
   if(Div2.style.backgroundColor != Color) Div2.style.backgroundColor = Color;
}

function STSupdatePort(){
   let Div = document.querySelector('.STSport');
   if(!Div) return;
   let Port = STSserverData().port;
   if(Div.innerHTML !== Port) Div.innerHTML = Port;
}

function STSupdateSecure(){
   let Div = document.querySelector('.STSuseHttps');
   if(!Div) return;
   let Secure = STSserverData().secure ? "Yes" : "No";
   if(Div.innerHTML !== Secure) Div.innerHTML = Secure;
}

function STSupdateCertificate(){
   let Div = document.querySelector('.STScertificate');
   if(!Div) return;
   let Certificate = STSserverData().cert_key;
   if(Div.innerHTML !== Certificate) Div.innerHTML = Certificate;
}

function STSupdatePrivateKey(){
   let Div = document.querySelector('.STSprivateKey');
   if(!Div) return;
   let PrivateKey = STSserverData().private_key;
   if(Div.innerHTML !== PrivateKey) Div.innerHTML = PrivateKey;
}

function STSupdateVisible(Div, Visible){
   if(!Div) return;
   let HiddenAlready = Div.classList.contains("STShidden");
   if( Visible &&  HiddenAlready) Div.classList.remove("STShidden");
   if(!Visible && !HiddenAlready) Div.classList.add   ("STShidden");
}

function STSupdateSecureVisible(){
   let Div = document.querySelector('.STSsecureDiv');
   let Visible = STSserverData().secure;
   STSupdateVisible(Div, Visible);
}
