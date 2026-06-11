function STSrestart(){
   APIcall("server/restart", {}, function(R){
      if(R.success) STSchangeURL(STSnewPort, "#settings/webserver", STSsecure);
   });
}

function STSchangeURL(newPort, newHref, secure) {
   // Get the current hostname and pathname
   let currentHostname = window.location.hostname;
   let currentPathname = window.location.pathname;

   // Build the new URL with the specified port
   let protocol = secure ? 'https://' : 'http://';

   // Build the new URL with the specified protocol, port, and path
   let newURL = protocol + currentHostname + ':' + newPort;

   // Check if the newHref is specified and not empty
   if (newHref && newHref.trim() !== '') {
      newURL += '/' + newHref.trim();
   } else {
      newURL += currentPathname;
   }

   // Redirect to the new URL
   window.location.href = newURL;
}