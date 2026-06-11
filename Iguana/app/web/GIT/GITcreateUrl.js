const GIT_URL_TYPE = Object.freeze({ HTTP: 'http', HTTPS: 'https', SSH: 'ssh' });

function GITcreateUrls(url, desiredType, callback) {
   if(!Object.values(GIT_URL_TYPE).includes(desiredType)) { return url; }
   APIcall('git/url/parse', {url}, (R) => {
      const Link = R.success ? R.data.links[desiredType] : url;
      if(callback) { callback(Link); }
   });
}