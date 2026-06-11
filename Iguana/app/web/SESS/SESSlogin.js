let SESSadminUnsafe;

function SESSsupportedBrowser() {
   const isIPad = /iPad/.test(navigator.userAgent) || 
                  (navigator.platform === 'MacIntel' && navigator.maxTouchPoints > 1);
   const isChromeOnIOS = /CriOS/.test(navigator.userAgent);
   const isMobileChrome = /Chrome/.test(navigator.userAgent) && /Mobile/.test(navigator.userAgent);
   const isDesktopChrome = /Chrome/.test(navigator.userAgent) && !/Mobile/.test(navigator.userAgent);
   const isSafari = /^((?!chrome|android).)*safari/i.test(navigator.userAgent);
   
   return isDesktopChrome || isMobileChrome || (isIPad && isChromeOnIOS) || isSafari;
}

function SESSshowError(message) {
   COL_TRC(message);
   const errorElement = document.querySelector('.SESSerror');
   if(!errorElement) return;
   errorElement.textContent = message;
   errorElement.style.display = 'block';
}

function SESShideError() {
   const errorElement = document.querySelector('.SESSerror');
   if(!errorElement) return;
   errorElement.textContent = "";
   errorElement.style.display = 'none';
}

function SESSshowWarning() {
   const errorElement = document.querySelector('.SESSpasswordWarning');
   if(!errorElement) return;
   errorElement.style.display = 'block';
}

function SESSshowLoginSpinner(){
   const LoginElement = document.querySelector('.SESSloginButton');
   if(!LoginElement) return;
   LoginElement.innerHTML = "<div class='SESSloginSpinner'></div>"
   LoginElement.style.pointerEvents = "none"
}

function SESShideLoginSpinner(){
   const LoginElement = document.querySelector('.SESSloginButton');
   if(!LoginElement) return;
   LoginElement.innerHTML = "LOG IN"
   LoginElement.style.pointerEvents = "auto"
}

function SESSloginScreen() {
   COL_TRC("Display login screen");
   if(SESSsupportedBrowser()) {
      SESSloginFrame(SESSloginHtml());
      SESScheckForUnsafeAdminPassword();
      SSOinit();
      SESScheckForSamlError(); 
      document.querySelector('.SESSloginButton').addEventListener('click', function() {
         SESShideAllErrors();
         const Username = document.querySelector('.SESSusername').value.trim();
         const Password = document.querySelector('.SESSpassword').value;
         SESSlogin(Username, Password);
         SESSshowLoginSpinner();
      });
      window.removeEventListener('keydown', SESScheckForEnterLogin);
      window.addEventListener('keydown', SESScheckForEnterLogin);
   } else {
      SESSloginFrame(SESSunsupportedBrowserHtml());
   }
   SESSsetupIntervals()
}

function SESScheckForUnsafeAdminPassword(){
   APIcall("user/admin/unsafe", {}, function(R){
      SESSadminUnsafe = false;
      if(!R.success) return;
      if(R?.data?.unsafe){
         SESSadminUnsafe = true;
         SESSshowWarning();
      }
      if(R?.data?.only_admin){
         // default to admin and password if there is only one account and it is admin/password
         document.querySelector('.SESSusername').value = "admin"; 
         document.querySelector('.SESSpassword').value = "password";
      }
   });
}

function SESScheckForEnterLogin(event) {
   const loginButton = document.querySelector('.SESSloginButton');
   if (event.key === 'Enter' && loginButton) { loginButton.click(); }
}

function SESSonLoginSuccess(Username,R) {
   COL_TRC(Username, R);
   COL_TRC("We need to setup the page table.");
   CORpageTable();
   if(Username == "admin" && SESSadminUnsafe){
      location.hash = '#settings/users'; 
      PAGEroute();
   }
   else if (PAGEhash() === ''){
      COL_TRC("We need to redirect to the dashboard.");
      location.hash = '#dashboard'; 
   } else {
      PAGEroute();
   }
   SESSinitialize();
   SESSstartHeartbeat();
}           

function SESSonLogin(R, Username){
   COL_TRC("Logged In:", Username, R);
   if (R.success){
      SESSclearLoginIntervals();
      SESScache = R.data;   // login returns the same information as /session/info so we can get it immediately.
      SESSonLoginSuccess(Username, R);
   } else {
      SESSshowError(R.error);
   }
}

function SESSlogin(Username, Password) {
   COL_TRC("Username =", Username);
   APIcall('session/login', { 'username': Username, 'password': Password }, function(R, Username){
      SESShideLoginSpinner();
      SESSonLogin(R, Username);
   }, function(){
      SESShideLoginSpinner();
      SESSshowError("Was not able to connect to IguanaX server.  Check if it is running and your internet connection is working.");
   });
}

function SESSshowMicrosoftError(message) {
   const errorElement = document.querySelector('.SESSMicrosoftError');
   if (errorElement) {
      errorElement.textContent = message;
      errorElement.style.display = 'block';
   }
}

function SESShideMicrosoftError() {
   const errorElement = document.querySelector('.SESSMicrosoftError');
   if (errorElement) {
      errorElement.textContent = "";
      errorElement.style.display = 'none';
   }
}

function SESSshowSamlError(message) {
   const errorElement = document.querySelector('.SESSSamlError');
   if(errorElement) {
      errorElement.innerHTML     = `${message} [<a target="_blank" href="${LINKsettingsAuthentication}">Learn more</a>]`;
      errorElement.style.display = 'block';
   }
}

function SESShideSamlError() {
   const errorElement = document.querySelector('.SESSSamlError');
   if(errorElement) {
      errorElement.textContent   = "";
      errorElement.style.display = 'none';
   }
}

function SESShideAllErrors() {
   SESShideError();
   SESShideMicrosoftError();
   SESShideSamlError();
}

function SESScheckForSamlError() {
   try {
      const samlError = sessionStorage.getItem('saml_error');
      if(samlError) {
         SESSshowSamlError(samlError);
         sessionStorage.removeItem('saml_error');
      }
   } catch(e) {
      COL_TRC("Failed to read SAML error from sessionStorage:", e);
   }
}

// Deliberately using inline SVG to make sure we load them at initialization.
const SESSiguanaWordmark=`<svg class="SESSiguanaWordmark" width="270px" height="94px" viewBox="0 0 270 94" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<defs></defs>
<g id="Page-1" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
   <g id="Fleet-Manager---Log-In" transform="translate(-597.000000, -401.000000)">
      <g id="Modal---Log-In" transform="translate(420.000000, 342.000000)">
         <g id="Logo---Iguana" transform="translate(177.000000, 59.000000)">
            <g id="Spike" transform="translate(207.000000, 0.000000)">
               <path d="M40.9707594,15.2430091 C37.9343768,18.3620818 33.3172251,22.8705273 28.3255137,24.8318636 C25.6006596,25.9539636 22.5261862,26.6248182 19.4118081,27.0301818 C25.3657662,32.5899 29.2288103,40.2869091 29.7149217,48.8743818 C31.8838312,44.9722 33.3575833,41.6188182 36.2629153,36.1245818 C36.7884779,34.9272 37.3290048,33.7333818 37.8899375,32.5462455 C41.8364187,24.1819455 46.6281533,15.4742 51.8697224,9.52470909 C52.7530667,8.52199091 53.761113,7.65023636 54.7433121,6.75487273 C55.4629565,6.10673636 56.1721713,5.49423636 56.8578061,4.92182727 C60.8119961,1.63303636 62.1247691,0.522518182 62.4712142,0.227627273 C46.404143,7.41548182 43.0625804,13.0945818 40.9707594,15.2430091" id="Fill-6" fill="#34AA48"></path>
               <path d="M28.3685926,18.1620727 L28.2783536,18.1620727 C34.5048425,7.79857273 44.1082641,1.30562727 44.1082641,1.30562727 C23.3582892,4.4688 13.2964174,17.9807727 13.2964174,17.9807727 L13.0288747,18.3139727 C17.9244522,10.0338636 24.7463369,4.66034545 24.7463369,4.66034545 C12.3355311,6.55219091 4.58178164,13.9302545 1.49687858,16.8319455 C0.797639936,17.4881 0.342363929,17.9776545 0.120167472,18.2262182 C7.56420225,19.0191273 14.2668264,22.2255091 19.4118081,27.0301818 C22.5261862,26.6248182 25.6006596,25.9539636 28.3255137,24.8318636 C33.3172251,22.8705273 37.9343768,18.3620818 40.9707594,15.2430091 C43.0625804,13.0945818 46.404143,7.41548182 62.4712142,0.227627273 C62.5478493,0.162145455 62.5777778,0.136309091 62.5777778,0.136309091 C39.0571491,3.37877273 28.3685926,18.1620727 28.3685926,18.1620727" id="Fill-8" fill="#7BBB42"></path>
            </g>
            <path d="M203,30 C191.971872,30 183,38.971532 183,49.9997806 C183,61.028468 191.971872,70 203,70 C206.329238,70 209.468033,69.1781032 212.232086,67.733092 L212.232086,58.4184082 C209.941946,60.927541 206.657028,62.5142888 203,62.5142888 C196.099741,62.5142888 190.486068,56.900554 190.486068,49.9997806 C190.486068,43.099446 196.099741,37.4857112 203,37.4857112 C209.89982,37.4857112 215.513932,43.099446 215.513932,49.9997806 C215.513932,50.0958806 215.502084,50.188909 215.49989,50.2841314 L215.513932,50.2841314 L215.513932,66.256925 C215.513932,68.3241731 217.190179,70 219.256966,70 C221.324191,70 223,68.3241731 223,66.256925 L223,49.9997806 C223,38.971532 214.028128,30 203,30" id="A" fill="#343534"></path>
            <path d="M175.196256,70 C173.095256,70 171.392067,68.3243638 171.392067,66.2568095 L171.392067,46.9518825 C171.392067,43.2947121 168.841966,37.4927448 162.000669,37.4874783 C155.158479,37.4927448 152.608379,43.2947121 152.608379,46.9518825 L152.608379,66.2568095 C152.608379,68.3243638 150.90519,70 148.80419,70 C146.703189,70 145,68.3243638 145,66.2568095 L145,46.9518825 C145,38.6272444 151.350276,30.0173357 161.977478,30.0006583 C161.982384,29.9997806 161.985952,29.9997806 161.991749,30.0006583 C161.996209,30.0010972 162.002899,30.0010972 162.009143,30.0006583 C162.013602,30.0006583 162.017616,30.0006583 162.022968,30.0006583 C172.65017,30.0173357 179,38.6272444 179,46.9518825 L179,66.2568095 C179,68.3243638 177.297257,70 175.196256,70" id="N" fill="#343534"></path>
            <path d="M117.999561,30 C106.971872,30 98,38.971532 98,49.9997806 C98,61.028468 106.971872,70 117.999561,70 C121.328799,70 124.467594,69.1781032 127.231647,67.733092 L127.231647,58.4184082 C124.941507,60.927541 121.656589,62.5142888 117.999561,62.5142888 C111.099302,62.5142888 105.485629,56.900554 105.485629,49.9997806 C105.485629,43.099446 111.099302,37.4857112 117.999561,37.4857112 C124.89982,37.4857112 130.513932,43.099446 130.513932,49.9997806 C130.513932,50.0958806 130.501646,50.188909 130.499451,50.2841314 L130.513932,50.2841314 L130.513932,66.256925 C130.513932,68.3241731 132.189741,70 134.256966,70 C136.323753,70 138,68.3241731 138,66.256925 L138,49.9997806 C138,38.971532 129.027689,30 117.999561,30" id="A" fill="#343534"></path>
            <path d="M76.0086966,70 C76.0033448,70 75.9962092,70 75.9913034,70 C75.9863977,70 75.9823839,70 75.9774781,70 C65.3493842,69.9828835 59,61.372833 59,53.0480579 L59,33.743691 C59,31.6756638 60.7027428,30 62.8037436,30 C64.9047444,30 66.6079332,31.6756638 66.6079332,33.743691 L66.6079332,53.0480579 C66.6079332,56.7057275 69.1580335,62.5077902 75.999777,62.512618 C82.8415205,62.5077902 85.3916209,56.7057275 85.3916209,53.0480579 L85.3916209,33.743691 C85.3916209,31.6756638 87.0948096,30 89.1958104,30 C91.2968113,30 93,31.6756638 93,33.743691 L93,53.0480579 C93,61.372833 86.6501699,69.9828835 76.0220759,70 C76.0176161,70 76.0131563,70 76.0086966,70" id="U" fill="#343534"></path>
            <path d="M32.9927769,63.8607672 C26.0865792,63.8607672 20.4682893,58.0095158 20.4682893,50.8185302 C20.4682893,43.6275446 26.0865792,37.7762931 32.9927769,37.7762931 C39.8994123,37.7762931 45.5177022,43.6275446 45.5177022,50.8185302 C45.5177022,58.0095158 39.8994123,63.8607672 32.9927769,63.8607672 Z M52.9750473,50.372256 C52.7452201,39.099161 43.8734528,30 32.9927769,30 C21.9689514,30 13,39.3393923 13,50.8185302 C13,62.2976681 21.9689514,71.6370604 32.9927769,71.6370604 C37.7381612,71.6370604 42.0996246,69.9034744 45.5317107,67.0170515 L45.5317107,71.1333495 C45.5317107,79.5724441 42.3001215,84.3524552 35.2660961,86.0573228 L35.2660961,94 C49.7272608,91.4194504 53,80.4905198 53,71.1333495 L53,50.6311771 C53,50.5422869 52.9803005,50.4593228 52.9750473,50.372256 Z" id="G" fill="#343534"></path>
            <path d="M3.5,70 C1.56699883,70 0,68.3188616 0,66.2450615 L0,33.7549385 C0,31.6811384 1.56699883,30 3.5,30 C5.43300117,30 7,31.6811384 7,33.7549385 L7,66.2450615 C7,68.3188616 5.43300117,70 3.5,70" id="I" fill="#343534"></path>
         </g>
      </g>
   </g>
</g>
</svg>`;

const SESSiguanaWordmarkWhite=`<svg class="SESSiguanaWordmarkWhite" width="270px" height="93px" viewBox="0 0 270 93" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<defs>
	 <polygon id="path-1" points="0 0 178.874156 0 178.874156 41.0196592 0 41.0196592"></polygon>
	 <polygon id="path-3" points="4.796883e-15 0 257.246662 0 257.246662 92.5354553 4.796883e-15 92.5354553"></polygon>
</defs>
<g id="Page-1" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
	 <g id="Logo_Iguana_COLOURWHT-Copy-4" transform="translate(-0.000000, 0.000000)">
		  <path d="M118.933265,29.3878332 C107.622618,29.3878332 98.420313,38.5894634 98.420313,49.90011 C98.420313,61.2114317 107.622618,70.4137371 118.933265,70.4137371 C122.347927,70.4137371 125.567485,69.5705356 128.402235,68.0886883 L128.402235,58.5346552 C126.053558,61.1088163 122.684128,62.7358104 118.933265,62.7358104 C111.856178,62.7358104 106.09824,56.9778717 106.09824,49.90011 C106.09824,42.8230235 111.856178,37.0657599 118.933265,37.0657599 C126.010352,37.0657599 131.768965,42.8230235 131.768965,49.90011 C131.768965,49.9993499 131.756138,50.0938641 131.754113,50.1924289 L131.768965,50.1924289 L131.768965,66.5751113 C131.768965,68.6949292 133.487773,70.4137371 135.607591,70.4137371 C137.728084,70.4137371 139.446892,68.6949292 139.446892,66.5751113 L139.446892,49.90011 C139.446892,38.5894634 130.244587,29.3878332 118.933265,29.3878332" id="Fill-1" fill="#FFFFFE"></path>
		  <path d="M204.503025,29.3878332 C193.192379,29.3878332 183.990074,38.5894634 183.990074,49.90011 C183.990074,61.2114317 193.192379,70.4137371 204.503025,70.4137371 C207.917688,70.4137371 211.137246,69.5705356 213.971996,68.0886883 L213.971996,58.5346552 C211.623318,61.1088163 208.253888,62.7358104 204.503025,62.7358104 C197.425939,62.7358104 191.668,56.9778717 191.668,49.90011 C191.668,42.8230235 197.425939,37.0657599 204.503025,37.0657599 C211.580112,37.0657599 217.338726,42.8230235 217.338726,49.90011 C217.338726,49.9993499 217.325899,50.0938641 217.323874,50.1924289 L217.338726,50.1924289 L217.338726,66.5751113 C217.338726,68.6949292 219.057534,70.4137371 221.177352,70.4137371 C223.297845,70.4137371 225.016652,68.6949292 225.016652,66.5751113 L225.016652,49.90011 C225.016652,38.5894634 215.814347,29.3878332 204.503025,29.3878332" id="Fill-2" fill="#FFFFFE"></path>
		  <path d="M76.1574986,70.4076611 C76.1520978,70.4076611 76.1453468,70.4076611 76.139946,70.4076611 C76.1358954,70.4076611 76.1311697,70.4076611 76.126444,70.4076611 C65.4011102,70.3901085 59,61.5611342 59,53.0238036 L59,33.2278092 C59,31.106641 60.7111817,29.3885083 62.8316748,29.3885083 C64.9521678,29.3885083 66.6703005,31.106641 66.6703005,33.2278092 L66.6703005,53.0238036 C66.6703005,56.7746662 69.2444617,62.7243337 76.1487223,62.7297345 C83.0529829,62.7243337 85.626469,56.7746662 85.626469,53.0238036 L85.626469,33.2278092 C85.626469,31.106641 87.3452768,29.3885083 89.4657699,29.3885083 C91.5862629,29.3885083 93.3050708,31.106641 93.3050708,33.2278092 L93.3050708,53.0238036 C93.3050708,61.5611342 86.8963345,70.3901085 76.1710007,70.4076611 C76.1662749,70.4076611 76.1622243,70.4076611 76.1574986,70.4076611" id="Fill-3" fill="#FFFFFE"></path>
		  <g id="Group-7" transform="translate(0.000000, 29.388002)">
				<mask id="mask-2" fill="white">
					 <use xlink:href="#path-1"></use>
				</mask>
				<g id="Clip-5"></g>
				<path d="M175.03553,41.0196592 C172.915037,41.0196592 171.196229,39.3008514 171.196229,37.1810334 L171.196229,17.3843639 C171.196229,13.6335013 168.623419,7.68315872 161.719158,7.67843301 C154.813547,7.68315872 152.240736,13.6335013 152.240736,17.3843639 L152.240736,37.1810334 C152.240736,39.3008514 150.521928,41.0196592 148.401435,41.0196592 C146.281617,41.0196592 144.562809,39.3008514 144.562809,37.1810334 L144.562809,17.3843639 C144.562809,8.8470333 150.970871,0.0180589588 161.695529,0.000506325949 C161.70093,0 161.704306,0 161.709706,0.000506325949 C161.714432,0.00118142721 161.721183,0.00118142721 161.727259,0.000506325949 C161.73266,0.000506325949 161.736035,0.000506325949 161.741436,0.000506325949 C172.466095,0.0180589588 178.874156,8.8470333 178.874156,17.3843639 L178.874156,37.1810334 C178.874156,39.3008514 177.155348,41.0196592 175.03553,41.0196592" id="Fill-4" fill="#FFFFFE" mask="url(#mask-2)"></path>
				<path d="M3.79879482,40.9042169 C1.67830175,40.9042169 -0.0405060759,39.1854091 -0.0405060759,37.0655911 L-0.0405060759,3.84790844 C-0.0405060759,1.72741536 1.67830175,0.00860754113 3.79879482,0.00860754113 C5.91861279,0.00860754113 7.63742061,1.72741536 7.63742061,3.84790844 L7.63742061,37.0655911 C7.63742061,39.1854091 5.91861279,40.9042169 3.79879482,40.9042169" id="Fill-6" fill="#FFFFFE" mask="url(#mask-2)"></path>
		  </g>
		  <path d="M248.554058,15.2633645 C245.541081,18.4140621 240.957819,22.9696454 236.003926,24.9510677 C233.29947,26.0852378 230.248012,26.7623644 227.156724,27.1721508 C233.06656,32.7896685 236.90046,40.566835 237.383157,49.2432365 C239.53538,45.29997 240.998325,41.9123118 243.881682,36.3609541 C244.403536,35.1511727 244.939566,33.9454418 245.496524,32.7457869 C249.413462,24.2948692 254.168875,15.4969495 259.371206,9.48584788 C260.248162,8.47252088 261.249337,7.59151373 262.223509,6.68687803 C262.938441,6.0320298 263.642571,5.41363705 264.322398,4.83507526 C268.247437,1.51222683 269.550383,0.390208531 269.893334,0.0918137721 C253.947442,7.35455318 250.63067,13.0929139 248.554058,15.2633645" id="Fill-8" fill="#36AA48"></path>
		  <g id="Group-12" transform="translate(12.753338, 0.000000)">
				<mask id="mask-4" fill="white">
					 <use xlink:href="#path-3"></use>
				</mask>
				<g id="Clip-10"></g>
				<path d="M223.293119,18.2122068 L223.204006,18.2122068 C229.383207,7.74206131 238.914287,1.18142721 238.914287,1.18142721 C218.320323,4.3773566 208.334225,18.0292544 208.334225,18.0292544 L208.06891,18.3661299 C212.927614,10.000275 219.698205,4.57111067 219.698205,4.57111067 C207.380307,6.48232235 199.684828,13.9367905 196.623243,16.8680802 C195.928564,17.5317048 195.476922,18.0258789 195.255488,18.2776917 C202.645147,19.0783618 209.29692,22.3181727 214.403386,27.1721508 C217.494674,26.7623644 220.546132,26.0852378 223.250588,24.9510677 C228.204481,22.9696454 232.787743,18.4140621 235.80072,15.2633645 C237.877332,13.0929139 241.194104,7.35455318 257.139996,0.0918137721 C257.216958,0.0263289493 257.246662,0 257.246662,0 C233.902335,3.27626644 223.293119,18.2122068 223.293119,18.2122068" id="Fill-9" fill="#7ABB43" mask="url(#mask-4)"></path>
				<path d="M20.5541331,62.8188478 C13.4547682,62.8188478 7.67792669,57.0420063 7.67792669,49.9426414 C7.67792669,42.8426014 13.4547682,37.0657599 20.5541331,37.0657599 C27.6548482,37.0657599 33.4310147,42.8426014 33.4310147,49.9426414 C33.4310147,57.0420063 27.6548482,62.8188478 20.5541331,62.8188478 Z M41.0981397,49.5011252 C40.8618543,38.3714057 31.7405611,29.3878332 20.5541331,29.3878332 C9.22120818,29.3878332 4.796883e-15,38.6083663 4.796883e-15,49.9426414 C4.796883e-15,61.2762414 9.22120818,70.4967745 20.5541331,70.4967745 C25.43309,70.4967745 29.9171126,68.7853928 33.4458669,65.9351153 L33.4458669,69.9999 C33.4458669,78.3319998 30.1230185,83.0509576 22.8920088,84.7339851 L22.8920088,92.5759614 C37.7590889,90.0281292 41.1237936,79.2373106 41.1237936,69.9999 L41.1237936,49.7569885 C41.1237936,49.6699005 41.1042156,49.5875381 41.0981397,49.5011252 L41.0981397,49.5011252 Z" id="Fill-11" fill="#FFFFFE" mask="url(#mask-4)"></path>
		  </g>
	 </g>
</g>
</svg>`;

const SESSinterfacewareLogo = `<?xml version="1.0" encoding="UTF-8"?>
<svg class="SESSinterfacewareLogo" width="190px" height="36px" viewBox="0 0 667 127" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
    <title>Logo_InterfacewareHorizontal_WHT</title>
    <defs>
        <polygon id="path-1" points="0 0 178.436 0 178.436 126.541 0 126.541"></polygon>
    </defs>
    <g id="Logo_InterfacewareHorizontal_WHT" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
        <polyline id="Fill-1" fill="#FFFFFE" points="190.98 46.882 197.925 46.882 213.967 67.962 213.967 46.882 221.401 46.882 221.401 81.117 214.994 81.117 198.415 59.354 198.415 81.117 190.98 81.117 190.98 46.882"></polyline>
        <polyline id="Fill-2" fill="#FFFFFE" points="242.482 53.827 232.065 53.827 232.065 46.882 260.432 46.882 260.432 53.827 250.015 53.827 250.015 81.117 242.482 81.117 242.482 53.827"></polyline>
        <polyline id="Fill-3" fill="#FFFFFE" points="271.096 46.882 296.919 46.882 296.919 53.583 278.579 53.583 278.579 60.527 294.718 60.527 294.718 67.228 278.579 67.228 278.579 74.417 297.163 74.417 297.163 81.117 271.096 81.117 271.096 46.882"></polyline>
        <path d="M323.919,63.511 C327.586,63.511 329.689,61.554 329.689,58.67 L329.689,58.572 C329.689,55.343 327.439,53.681 323.772,53.681 L316.29,53.681 L316.29,63.511 L323.919,63.511 Z M308.757,46.882 L324.407,46.882 C328.761,46.882 332.135,48.105 334.384,50.354 C336.292,52.262 337.319,54.952 337.319,58.18 L337.319,58.278 C337.319,63.805 334.336,67.277 329.983,68.89 L338.346,81.117 L329.542,81.117 L322.207,70.162 L316.29,70.162 L316.29,81.117 L308.757,81.117 L308.757,46.882 L308.757,46.882 Z" id="Fill-4" fill="#FFFFFE"></path>
        <polyline id="Fill-5" fill="#FFFFFE" points="349.01 46.882 375.077 46.882 375.077 53.73 356.542 53.73 356.542 61.017 372.877 61.017 372.877 67.864 356.542 67.864 356.542 81.117 349.01 81.117 349.01 46.882"></polyline>
        <path d="M401.735,66.787 L397.186,55.686 L392.638,66.787 L401.735,66.787 Z M393.811,46.638 L400.756,46.638 L415.428,81.117 L407.554,81.117 L404.424,73.439 L389.948,73.439 L386.818,81.117 L379.14,81.117 L393.811,46.638 L393.811,46.638 Z" id="Fill-6" fill="#FFFFFE"></path>
        <path d="M421.888,64.098 L421.888,64 C421.888,54.267 429.223,46.296 439.739,46.296 C446.195,46.296 450.059,48.448 453.237,51.578 L448.444,57.104 C445.804,54.708 443.114,53.24 439.689,53.24 C433.919,53.24 429.762,58.033 429.762,63.901 L429.762,64 C429.762,69.868 433.821,74.759 439.689,74.759 C443.602,74.759 445.999,73.195 448.688,70.749 L453.482,75.591 C449.961,79.357 446.048,81.704 439.445,81.704 C429.371,81.704 421.888,73.929 421.888,64.098" id="Fill-7" fill="#FFFFFE"></path>
        <polyline id="Fill-8" fill="#FFFFFE" points="464.39 46.882 490.213 46.882 490.213 53.583 471.874 53.583 471.874 60.527 488.012 60.527 488.012 67.228 471.874 67.228 471.874 74.417 490.457 74.417 490.457 81.117 464.39 81.117 464.39 46.882"></polyline>
        <polyline id="Fill-9" fill="#FFFFFE" points="500.145 46.882 504.351 46.882 514.131 75.395 523.521 46.785 526.7 46.785 536.091 75.395 545.872 46.882 549.932 46.882 537.705 81.362 534.428 81.362 525.038 53.583 515.599 81.362 512.372 81.362 500.145 46.882"></polyline>
        <path d="M579.622,68.597 L571.846,51.137 L564.021,68.597 L579.622,68.597 Z M570.086,46.638 L573.704,46.638 L589.305,81.117 L585.148,81.117 L581.138,72.07 L562.504,72.07 L558.445,81.117 L554.485,81.117 L570.086,46.638 L570.086,46.638 Z" id="Fill-10" fill="#FFFFFE"></path>
        <path d="M615.327,64.342 C620.462,64.342 624.13,61.702 624.13,57.299 L624.13,57.202 C624.13,52.996 620.903,50.452 615.377,50.452 L604.812,50.452 L604.812,64.342 L615.327,64.342 Z M600.948,46.882 L615.67,46.882 C619.876,46.882 623.25,48.154 625.402,50.307 C627.065,51.969 628.043,54.365 628.043,57.055 L628.043,57.153 C628.043,62.827 624.13,66.152 618.751,67.13 L629.266,81.117 L624.521,81.117 L614.594,67.814 L604.812,67.814 L604.812,81.117 L600.948,81.117 L600.948,46.882 L600.948,46.882 Z" id="Fill-11" fill="#FFFFFE"></path>
        <polyline id="Fill-12" fill="#FFFFFE" points="641.201 46.882 665.948 46.882 665.948 50.403 645.065 50.403 645.065 62.093 663.747 62.093 663.747 65.614 645.065 65.614 645.065 77.596 666.192 77.596 666.192 81.117 641.201 81.117 641.201 46.882"></polyline>
        <polygon id="Fill-13" fill="#FFFFFE" points="171.001 53.481 178.436 53.481 178.436 46.878 171.001 46.878"></polygon>
        <g id="Group-17" transform="translate(0.000000, 0.399000)">
            <mask id="mask-2" fill="white">
                <use xlink:href="#path-1"></use>
            </mask>
            <g id="Clip-15"></g>
            <polygon id="Fill-14" fill="#FFFFFE" mask="url(#mask-2)" points="171.001 80.719 178.436 80.719 178.436 57.438 171.001 57.438"></polygon>
            <path d="M129.63,121.382 L66.466,121.382 L66.466,111.418 L83.515,111.418 L83.515,58.218 L129.63,58.218 L129.63,121.382 Z M66.466,106.029 L78.126,106.029 L78.126,58.218 L66.466,58.218 L66.466,106.029 Z M5.389,106.029 L5.389,33.293 L61.123,33.293 L61.123,48.453 L78.126,48.453 L78.126,52.999 L61.247,52.999 L61.247,106.029 L5.389,106.029 Z M66.59,42.986 L78.125,42.986 L78.125,33.293 L66.59,33.293 L66.59,42.986 Z M66.59,5.467 L104.109,5.467 L104.109,42.986 L83.515,42.986 L83.515,27.903 L66.59,27.903 L66.59,5.467 Z M83.515,52.999 L83.515,48.453 L109.576,48.453 L109.576,0 L61.123,0 L61.123,27.903 L0,27.903 L0,111.418 L61.247,111.418 L61.247,126.601 L134.849,126.601 L134.849,52.999 L83.515,52.999 L83.515,52.999 Z" id="Fill-16" fill="#FFFFFE" mask="url(#mask-2)"></path>
        </g>
    </g>
</svg>`;

function SESSloginFrame(html) {
   document.body.innerHTML = `
   <div id="SESSlogin" class="SESSbackground">
    <div class="SESSmodal">
        <div class="SESSmodalMainArea">
            <div class="SESSmainLogoArea">
                ${SESSiguanaWordmark}
                <div class="SESSloginVersion"></div>
            </div>
            ${html}
        </div>
        <div class="SESSfooter">
            <div class="SESSfooterText">developed by</div>
                <!--<img src="/SESS/images/SESSinterfacewareLogo.svg" class="SESSinterfacewareLogo">-->
                ${SESSinterfacewareLogo}
        </div>
    </div>
</div>`
}

function SESSloginHtml() {
   return /*html*/ `
      <div class="SESSloginForm">
          <div class="SESSloginInputs">
               <div class="SESSlabel">Username</div>
               <input type="text" autofocus="autofocus" class="SESSusername SESSloginFormInput">
               <div class="SESSlabel">Password</div>
               <input type="password" class="SESSpassword SESSloginFormInput">
               <div class="SESSpasswordWarning">The admin password set as 'password' is considered insecure. [
                  <a target="_blank" href="${LINKsessLogin}">See more</a> ]
               </div>
               <div class="SESSerror"></div>
          </div>
          <div class="SESSloginButtonContainer">
              <div class="SESSloginButton BUTTONaction" tabindex="0">LOG IN</div>
          </div>
          
          <div class="SESSloginDivider" style="display: none;">
              <div class="SESSdividerLine"></div>
              <div class="SESSdividerText">or</div>
              <div class="SESSdividerLine"></div>
          </div>
      </div>
      
      <div class="SESSssoContainer">
          <div class="SESSmicrosoftSignInContainer" style="display: none;"><div class="SESSmicrosoftButton"></div></div>
          <div class="SESSSamlSignInContainer" style="display: none;"><div class="SESSsamlButton"></div></div>
      </div>

      <div class="SESSgoogleError"></div>
      <div class="SESSMicrosoftError"></div>
      <div class="SESSSamlError"></div>
      <div class="SESSloginLegalStatement">
          This application may provide you with access to sensitive information, including Protected Health
          Information (PHI). By logging into this application, you acknowledge your responsibility to adhere to
          the privacy policies, processes, and procedures of your organization or the organization you are acting
          on the behalf of.
      </div>
      <div class="SESSforgotPasswordDiv"></div>
      `;
}

function SESSunsupportedBrowserHtml() {
   return `
   <div class="SESSunsupported">
    <p>So that we can focus on completing IguanaX to solve the core problems of being an interface 
    engine at this stage we are only supporting the Chrome browser.</p>
   <p>This enables us to focus our quality assurance resources on the core work flows of the product.</p>
   </div>`;
}