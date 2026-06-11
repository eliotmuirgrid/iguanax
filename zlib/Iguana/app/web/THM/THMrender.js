let THMcachedRadioButton = "THMcachedRadioButton1234$5$ds";

function THMthemeDiv(){
   return /*html*/`
   <div class="THMthemeContainer">
      ${THMthemeContainerImage()}
   </div>`;
}

function THMrenderPreview(target){
   let div = target.closest(".CORheaderUser");
   if(div) div.classList.add("CORshow");
   let Div = target.closest(".THMthemeContainer");
   if(!Div) return;
}

function THMgetUserTheme(username){
   const themes = JSON.parse(localStorage.getItem("CORuserThemes")) || {};
   return themes[username] || "light";
}

function THMsetUserTheme(username, theme){
   const themes = JSON.parse(localStorage.getItem("CORuserThemes")) || {};
   themes[username] = theme;
   localStorage.setItem("CORuserThemes", JSON.stringify(themes));
}

function THMthemePopTooltopHtml() {
   const username = SESScache?.username;
   const storedState = THMgetUserTheme(username);
   let isDarkMode = storedState === "dark";
   let isLightMode = storedState === "light";
   let isBrowserMode = storedState === "browser";
   return /*html*/`
   <div class="THMradioContainer">
      <label class="THMthemeDiv" for="THMdarkMode">
         <input type="radio" id="THMdarkMode" name="THMbrowser" value="dark" ${isDarkMode ? "checked" : ""}>
         ${THMdarkImage()}
         <label class="THMradio" for="THMdarkMode">Dark Mode</label>
      </label>
      <label class="THMthemeDiv" for="THMlightMode">
         <input type="radio" id="THMlightMode" name="THMbrowser" value="light" ${isLightMode ? "checked" : ""}>
         ${THMlightImage()}
         <label class="THMradio" for="THMlightMode">Light Mode</label>
      </label>
      <label class="THMthemeDiv" for="THMbrowser">
         <input type="radio" id="THMbrowser" name="THMbrowser" value="browser" ${isBrowserMode ? "checked" : ""}>
         ${THMmatchDeviceImage()}
         <label class="THMradio" for="THMbrowser">Match Device</label>
      </label>
   </div>`;
}
   
function THMdarkImage(){
   return /*html*/`
   <svg width="60px" height="40px" viewBox="0 0 60 40" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" class="THMradio" for="THMdarkMode">
      <title>Dark Theme</title>
      <g id="Dark-Theme" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
         <rect id="Rectangle-Copy-81" fill="#212D39" x="1" y="1" width="58" height="38" rx="3"></rect>
         <path d="M4,1.00470748 L19,1.00470748 L19,1.00470748 L19,39.0047075 L4,39.0047075 C2.34314575,39.0047075 1,37.6615617 1,36.0047075 L1,4.00470748 C1,2.34785323 2.34314575,1.00470748 4,1.00470748 Z" id="Rectangle-Copy-91" fill="#36414C"></path>
         <path d="M4,1 L6,1 L6,1 L6,39 L4,39 C2.34314575,39 1,37.6568542 1,36 L1,4 C1,2.34314575 2.34314575,1 4,1 Z" id="Rectangle-Copy-83" fill="#00609F"></path>
         <g id="Group-3">
            <rect id="Rectangle-Copy-84" stroke="#C9D0D5" x="0.5" y="0.5" width="59" height="39" rx="4"></rect>
            <rect id="Rectangle-Copy-85" fill="#6B7A86" x="6" y="1" width="1" height="38"></rect>
            <rect id="Rectangle-Copy-86" fill="#6B7A86" x="18" y="1" width="1" height="38"></rect>
            <rect id="Rectangle-Copy-87" fill="#5CB533" x="10" y="4" width="7" height="2" rx="1"></rect>
            <polyline id="Path-3-Copy-2" stroke="#6B7A86" points="30.9947889 18.2947236 34.5509403 23.1694555 41.8251954 23.1694555 46.5437228 16.2947236"></polyline>
            <rect id="Rectangle-Copy-88" fill="#9AA5B2" x="21" y="16" width="10" height="4" rx="2"></rect>
            <rect id="Rectangle-Copy-89" fill="#9AA5B2" x="33" y="22" width="10" height="4" rx="2"></rect>
            <rect id="Rectangle-Copy-90" fill="#9AA5B2" x="46" y="14" width="10" height="4" rx="2"></rect>
         </g>
      </g>
   </svg>`
}

function THMlightImage(){
   return /*html*/`
   <svg width="60px" height="40px" viewBox="0 0 60 40" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
      <title>Light Theme</title>
      <g id="Light-Theme" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
         <rect id="Rectangle-Copy-81" fill="#D2E4EC" x="1" y="1" width="58" height="38" rx="3"></rect>
         <path d="M4,1.00470748 L19,1.00470748 L19,1.00470748 L19,39.0047075 L4,39.0047075 C2.34314575,39.0047075 1,37.6615617 1,36.0047075 L1,4.00470748 C1,2.34785323 2.34314575,1.00470748 4,1.00470748 Z" id="Rectangle-Copy-91" fill="#EBF3F6"></path>
         <path d="M4,1 L6,1 L6,1 L6,39 L4,39 C2.34314575,39 1,37.6568542 1,36 L1,4 C1,2.34314575 2.34314575,1 4,1 Z" id="Rectangle-Copy-83" fill="#00609F"></path>
         <g id="Group-3">
            <rect id="Rectangle-Copy-84" stroke="#C9D0D5" x="0.5" y="0.5" width="59" height="39" rx="4"></rect>
            <rect id="Rectangle-Copy-85" fill="#C9D0D5" x="6" y="1" width="1" height="38"></rect>
            <rect id="Rectangle-Copy-86" fill="#C9D0D5" x="18" y="1" width="1" height="38"></rect>
            <rect id="Rectangle-Copy-87" fill="#5CB533" x="10" y="4" width="7" height="2" rx="1"></rect>
            <polyline id="Path-3-Copy-2" stroke="#6B7A86" points="30.9947889 18.2947236 34.5509403 23.1694555 41.8251954 23.1694555 46.5437228 16.2947236"></polyline>
            <rect id="Rectangle-Copy-88" fill="#FFFFFF" x="21" y="16" width="10" height="4" rx="2"></rect>
            <rect id="Rectangle-Copy-89" fill="#FFFFFF" x="33" y="22" width="10" height="4" rx="2"></rect>
            <rect id="Rectangle-Copy-90" fill="#FFFFFF" x="46" y="14" width="10" height="4" rx="2"></rect>
         </g>
      </g>
   </svg>`
}

function THMmatchDeviceImage(){
   return /*html*/`
   <svg width="60px" height="40px" viewBox="0 0 60 40" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
      <title>Match Device</title>
      <g id="Match-Device" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
         <rect id="Rectangle-Copy-81" fill="#212D39" x="1" y="1" width="58" height="38" rx="3"></rect>
         <path d="M4,1.00470748 L38,1.00470748 L38,1.00470748 L24,39.0047075 L4,39.0047075 C2.34314575,39.0047075 1,37.6615617 1,36.0047075 L1,4.00470748 C1,2.34785323 2.34314575,1.00470748 4,1.00470748 Z" id="Rectangle-Copy-82" fill="#D2E4EC"></path>
         <path d="M4,1.00470748 L19,1.00470748 L19,1.00470748 L19,39.0047075 L4,39.0047075 C2.34314575,39.0047075 1,37.6615617 1,36.0047075 L1,4.00470748 C1,2.34785323 2.34314575,1.00470748 4,1.00470748 Z" id="Rectangle-Copy-91" fill="#EBF3F6"></path>
         <path d="M4,1 L6,1 L6,1 L6,39 L4,39 C2.34314575,39 1,37.6568542 1,36 L1,4 C1,2.34314575 2.34314575,1 4,1 Z" id="Rectangle-Copy-83" fill="#00609F"></path>
         <g id="Group-3">
            <rect id="Rectangle-Copy-84" stroke="#C9D0D5" x="0.5" y="0.5" width="59" height="39" rx="4"></rect>
            <rect id="Rectangle-Copy-85" fill="#C9D0D5" x="6" y="1" width="1" height="38"></rect>
            <rect id="Rectangle-Copy-86" fill="#C9D0D5" x="18" y="1" width="1" height="38"></rect>
            <rect id="Rectangle-Copy-87" fill="#5CB533" x="10" y="4" width="7" height="2" rx="1"></rect>
            <polyline id="Path-3-Copy-2" stroke="#6B7A86" points="30.9947889 18.2947236 34.5509403 23.1694555 41.8251954 23.1694555 46.5437228 16.2947236"></polyline>
            <rect id="Rectangle-Copy-88" fill="#FFFFFF" x="21" y="16" width="10" height="4" rx="2"></rect>
            <rect id="Rectangle-Copy-89" fill="#9AA5B2" x="33" y="22" width="10" height="4" rx="2"></rect>
            <rect id="Rectangle-Copy-90" fill="#9AA5B2" x="46" y="14" width="10" height="4" rx="2"></rect>
        </g>
      </g>
   </svg>`
}

function THMthemeContainerImage(){
   return /*html*/`
   <svg width="14px" height="14px" viewBox="0 0 24 24" version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
      <title>theme-icon</title>
      <g id="theme-icon" stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
         <path d="M23.8275859,11.4277225 C23.6005618,11.8323241 23.3141358,12.2005861 22.9778664,12.5202191 C20.7735961,14.7142545 17.2093362,14.7101075 15.0101775,12.5109487 C12.8110187,10.3117899 12.8068717,6.7475301 15.0009071,4.54325983 C15.8410411,3.6872634 16.9365224,3.12736629 18.1224126,2.94786797 C18.4566134,2.90034019 18.7882282,3.04994929 18.9737586,3.33195551 C19.159289,3.61396174 19.1654102,3.97771134 18.9894734,4.26580038 C18.0574893,5.79713508 18.2939064,7.76727739 19.5617335,9.03463474 C20.4533141,9.91306467 21.7045525,10.3237275 22.9432706,10.1444726 C23.2731041,10.0913018 23.6040218,10.2331237 23.7929902,10.5086381 C23.9856315,10.7810757 23.9992012,11.1415737 23.8275859,11.4277225 Z M12.0525188,3.96701136 C12.2825172,3.96714013 12.5031324,3.87583074 12.6657658,3.71319735 C12.8283992,3.55056396 12.9197086,3.32994875 12.9195798,3.09995043 L12.9195798,0.867060794 C12.9195798,0.38819634 12.5313833,-3.55271368e-15 12.0525188,-3.55271368e-15 C11.5736544,-3.55271368e-15 11.1854579,0.38819634 11.1854579,0.867060794 L11.1854579,3.09991575 C11.1853291,3.32991406 11.2766385,3.55052928 11.4392719,3.71316267 C11.6019053,3.87579606 11.8225205,3.96710545 12.0525188,3.96701136 L12.0525188,3.96701136 Z M18.9593691,17.6799418 C18.619832,17.3471959 18.0756648,17.3499443 17.7395062,17.6861029 C17.4033475,18.0222616 17.4005991,18.5664288 17.7333451,18.9059658 L19.3116732,20.4851436 C19.6512103,20.8178896 20.1953774,20.8151412 20.5315361,20.4789826 C20.8676947,20.1428239 20.8704431,19.5986567 20.5376972,19.2591197 L18.9593691,17.6799418 Z M12.0525188,20.0329886 C11.8225205,20.0328945 11.6019053,20.1242039 11.4392719,20.2868373 C11.2766385,20.4494707 11.1853291,20.6700859 11.1854579,20.9000842 L11.1854579,23.1329392 C11.1854579,23.6118037 11.5736544,24 12.0525188,24 C12.5313833,24 12.9195798,23.6118037 12.9195798,23.1329392 L12.9195798,20.9000842 C12.9197178,20.6700799 12.8284124,20.4494558 12.665778,20.2868149 C12.5031437,20.124174 12.2825232,20.0328599 12.0525188,20.0329886 L12.0525188,20.0329886 Z M5.14650101,17.6799072 L3.56734053,19.259085 C3.34462973,19.4773414 3.25628462,19.7983862 3.33597671,20.0998578 C3.4156688,20.4013294 3.65112013,20.6367807 3.9525917,20.7164728 C4.25406328,20.7961649 4.57510813,20.7078198 4.79336449,20.485109 L6.37254232,18.9059311 C6.70528828,18.5663941 6.70253989,18.0222269 6.36638124,17.6860682 C6.03022258,17.3499096 5.4860554,17.3471612 5.14651835,17.6799072 L5.14650101,17.6799072 Z M4.0195649,11.9999827 C4.01969827,11.7699829 3.92839036,11.549365 3.76575597,11.3867307 C3.60312158,11.2240963 3.38250369,11.1327883 3.15250396,11.1329217 L0.919579643,11.1329217 C0.440715189,11.1329217 0.0525188489,11.5211182 0.0525188489,11.9999827 C0.0525188489,12.4788471 0.440715189,12.8670436 0.919579643,12.8670436 L3.15250396,12.8670436 C3.38250369,12.867177 3.60312158,12.7758691 3.76575597,12.6132347 C3.92839036,12.4506003 4.01969827,12.2299824 4.0195649,11.9999827 L4.0195649,11.9999827 Z M5.14657038,6.32005815 C5.48610743,6.65280412 6.0302746,6.65005573 6.36643326,6.31389707 C6.70259192,5.97773841 6.70534031,5.43357124 6.37259434,5.09403419 L4.79341652,3.51485636 C4.45387947,3.1821104 3.90971229,3.18485879 3.57355364,3.52101744 C3.23739498,3.8571761 3.23464659,4.40134328 3.56739255,4.74088033 L5.14657038,6.32005815 Z M11.6194046,8.53173948 C11.6182921,7.45848973 11.8551952,6.39834831 12.3130533,5.42766184 C12.2096593,5.4133177 12.1052564,5.40754654 12.0009114,5.41040733 C8.87076231,5.41337067 6.174638,7.61791021 5.55001997,10.6851067 C4.92540193,13.7523032 6.5446766,16.8356505 9.42438325,18.0624886 C12.3040899,19.2893266 15.6494671,18.3210642 17.428712,15.745772 C14.0406721,15.000104 11.6254447,12.0008602 11.6194046,8.53173948 L11.6194046,8.53173948 Z" fill="#000000" fill-rule="nonzero"></path>
      </g>
   </svg>
   Theme`
}
