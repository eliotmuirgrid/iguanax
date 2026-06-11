let SESStabGuid = null;

function SESSshortID(len) {
  var chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  var id = "";
  for (var i = 0; i < len; i++) {
    var r = Math.floor(Math.random() * chars.length);
    id += chars.charAt(r);
  }
  return id;
}

function SESSgetTabGuid() {
  if (!SESStabGuid) SESStabGuid = SESSshortID(6);
  return SESStabGuid;
}
