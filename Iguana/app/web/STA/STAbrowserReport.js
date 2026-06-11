function SETbrowserReport() {
	COL_TRC("Browser report");
	// source: https://stackoverflow.com/a/11219680
   const nVer = navigator.appVersion;
   const nAgt = navigator.userAgent;
   let browserName = navigator.appName;
   let fullVersion = '' + parseFloat(navigator.appVersion);
   let nameOffset, verOffset, ix;

   // In Opera, the true version is after "Opera" or after "Version"
	if ((verOffset = nAgt.indexOf("Opera")) != -1) {
		browserName = "Opera";
		fullVersion = nAgt.substring(verOffset + 6);
		if ((verOffset = nAgt.indexOf("Version")) != -1)
			fullVersion = nAgt.substring(verOffset + 8);
	}
	// In MSIE, the true version is after "MSIE" in userAgent
	else if ((verOffset = nAgt.indexOf("MSIE")) != -1) {
		browserName = "Microsoft Internet Explorer";
		fullVersion = nAgt.substring(verOffset + 5);
	}
	// In Chrome, the true version is after "Chrome" 
	else if ((verOffset = nAgt.indexOf("Chrome")) != -1) {
		browserName = "Chrome";
		fullVersion = nAgt.substring(verOffset + 7);
	}
	// In Safari, the true version is after "Safari" or after "Version" 
	else if ((verOffset = nAgt.indexOf("Safari")) != -1) {
		browserName = "Safari";
		fullVersion = nAgt.substring(verOffset + 7);
		if ((verOffset = nAgt.indexOf("Version")) != -1) 
			fullVersion = nAgt.substring(verOffset + 8);
	}
	// In Firefox, the true version is after "Firefox" 
	else if ((verOffset = nAgt.indexOf("Firefox")) != -1) {
		browserName = "Firefox";
		fullVersion = nAgt.substring(verOffset + 8);
	}
	// In most other browsers, "name/version" is at the end of userAgent 
	else if ((nameOffset = nAgt.lastIndexOf(' ') + 1) <
		(verOffset = nAgt.lastIndexOf('/'))) {
		browserName = nAgt.substring(nameOffset, verOffset);
		fullVersion = nAgt.substring(verOffset + 1);
		if (browserName.toLowerCase() == browserName.toUpperCase()) {
			browserName = navigator.appName;
		}
	}
	// trim the fullVersion string at semicolon/space if present
	if ((ix = fullVersion.indexOf(";")) != -1)
		fullVersion = fullVersion.substring(0, ix);
	if ((ix = fullVersion.indexOf(" ")) != -1)
		fullVersion = fullVersion.substring(0, ix);

   const browserOutput = '<div class="SETversionRow"><div class="SETversionLabel">Browser:</div><div class="SETversionDetail STAbrowser">' + browserName + '</div></div>'
       + '<div class="SETversionRow"><div class="SETversionLabel">Version:</div><div class="SETversionDetail STAbrowserVersion">' + fullVersion + '</div></div>'
       + '<div class="SETversionRow"><div class="SETversionLabel">navigator.appName:</div><div class="SETversionDetail STAnavName">' + navigator.appName + '</div></div>'
       + '<div class="SETversionRow"><div class="SETversionLabel">navigator.userAgent:</div><div class="SETversionDetail STAnavAgent">' + navigator.userAgent + '</div></div>';
   document.querySelector('.SETbrowserInformation').innerHTML = browserOutput;
}