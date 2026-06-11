const DOCfileTypes = {
  "iframe": {
    "html": "text/html",
    "htm":  "text/html",
    "pdf":  "application/pdf",
    "txt":  "text/plain",
    "csv":  "text/csv",
    "json": "application/json",
    "xml":  "application/xml"
  
  },
  "image": {
    "png":  "image/png",
   "jpg":  "image/jpeg",
   "jpeg": "image/jpeg",
   "gif":  "image/gif",
   "svg":  "image/svg+xml",
   "webp": "image/webp"

  },
  "video": {
    "mp4":  "video/mp4",
    "webm": "video/webm",
    "ogg":  "video/ogg"
  },
  "audio": {
    "mp3":  "audio/mpeg",
    "wav":  "audio/wav"
  }
 };
 
function DOCpaintableAttachment(Source){
   return DOCfileTypes.iframe.hasOwnProperty(Source.toLowerCase()) ||
   DOCfileTypes.image.hasOwnProperty(Source.toLowerCase()) ||
   DOCfileTypes.video.hasOwnProperty(Source.toLowerCase()) ||
   DOCfileTypes.audio.hasOwnProperty(Source.toLowerCase());
}

function DOCpaintAttachment(Source, Id, Created) {
  const Data  = { Created, Id, Component: PAGEhashParameter("component"), inline: true };
  const Url   = "doc/download" + PAGEformatParams(Data);
  const Mount = document.querySelector(".DOCcontent");
  const KeyValue = `${Source}:${Id}:${Created}`;

  if (DOCfileTypes.iframe.hasOwnProperty(Source.toLowerCase())){
    DOCpaintIframe(Url,Mount,KeyValue);
  }
  if (DOCfileTypes.image.hasOwnProperty(Source.toLowerCase())){
    DOCpaintImg(Url,Mount,KeyValue)  
  }
  if (DOCfileTypes.video.hasOwnProperty(Source.toLowerCase())){
    DOCpaintVideo(Url,Mount,KeyValue)
  }
  if (DOCfileTypes.audio.hasOwnProperty(Source.toLowerCase())){
    DOCpaintAudio(Url,Mount,KeyValue)
  }
  DOCpaintDescription();
} 

function DOCpaintDescription(){
  document.querySelector(".DOCcontent").classList.remove("DOCmd");
  document.querySelector(".DOCdescription").classList.add("DOCmd");
  document.querySelector(".DOCdescription").innerHTML = MODELdata.DOCdocument.data.markdown;
}

async function DOCpaintIframe(Url,Mount,KeyValue){
  if (Mount.dataset.KeyValue === KeyValue) return;
  Mount.textContent = "Loading…";
  if (Mount.dataset.BlobUrl) { delete Mount.dataset.BlobUrl; }
  try {
    const Iframe = document.createElement("iframe");
    Iframe.className = "DOCiframe";
    Iframe.src = Url + "#toolbar=1&navpanes=0&zoom=100";
    Mount.dataset.KeyValue = KeyValue;
    Mount.replaceChildren(Iframe);
  } catch (Err) {
    console.error(Err);
    Mount.textContent = "Unable to display document.";
  }
}

function DOCpaintImg(Url,Mount,KeyValue){
  if (Mount.dataset.KeyValue === KeyValue) return;
  Mount.textContent = "Loading…";
  try {
    const Img = document.createElement("img");
    Img.className = "DOCimage";
    Img.src      = Url;          
    Mount.dataset.KeyValue = KeyValue;
    Mount.replaceChildren(Img);
  } catch (Err) {
    console.error(Err);
    Mount.textContent = "Unable to display image.";
  }
}

function DOCpaintVideo(Url,Mount,KeyValue) {
  if (Mount.dataset.KeyValue === KeyValue) return;
  Mount.textContent = "Loading…";
  try {
    const Video = document.createElement("video");
    Video.className = "DOCvideo";
    Video.src = Url;            
    Video.controls = true;       
    Video.preload = "metadata";   
    Video.playsInline = true;     

    Mount.dataset.KeyValue = KeyValue;
    Mount.replaceChildren(Video);
  } catch (Err) {
    console.error(Err);
    Mount.textContent = "Unable to display video.";
  }
}

function DOCpaintAudio(Url,Mount,KeyValue) {
  if (Mount.dataset.KeyValue === KeyValue) return;
  Mount.textContent = "Loading…";
  try {
    const Audio = document.createElement("audio");
    Audio.className = "DOCaudio";
    Audio.src = Url;           
    Audio.controls = true;      
    Audio.preload = "metadata"; 

    Mount.dataset.KeyValue = KeyValue;
    Mount.replaceChildren(Audio);
  } catch (Err) {
    console.error(Err);
    Mount.textContent = "Unable to play audio.";
  }
}