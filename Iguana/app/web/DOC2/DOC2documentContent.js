const DOC2fileTypes = {
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

function DOC2renderContent(Doc){
   if (DOC2paintableAttachment(Doc.meta.source)){
      DOC2paintAttachment(Doc.meta.source, Doc.meta.id, Doc.meta.created);
   }else{
      DOC2paintDescription(false);
      DOC2replaceInnerTextIfChanged(document.querySelector(".DOC2content"),Doc.markdown); 
   } 
}

function DOC2paintableAttachment(Source){
   return DOC2fileTypes.iframe.hasOwnProperty(Source.toLowerCase()) ||
   DOC2fileTypes.image.hasOwnProperty(Source.toLowerCase()) ||
   DOC2fileTypes.video.hasOwnProperty(Source.toLowerCase()) ||
   DOC2fileTypes.audio.hasOwnProperty(Source.toLowerCase());
}

function DOC2paintAttachment(Source, Id, Created) {
  const Data  = { Created, Id, Component: PAGEhashParameter("component"), inline: true };
  const Url   = "doc/download" + PAGEformatParams(Data);
  const Mount = document.querySelector(".DOC2content");
  const KeyValue = `${Source}:${Id}:${Created}`;

  if (DOC2fileTypes.iframe.hasOwnProperty(Source.toLowerCase())){
    DOC2paintIframe(Url,Mount,KeyValue);
  }
  if (DOC2fileTypes.image.hasOwnProperty(Source.toLowerCase())){
    DOC2paintImg(Url,Mount,KeyValue)  
  }
  if (DOC2fileTypes.video.hasOwnProperty(Source.toLowerCase())){
    DOC2paintVideo(Url,Mount,KeyValue)
  }
  if (DOC2fileTypes.audio.hasOwnProperty(Source.toLowerCase())){
    DOCpaintAudio(Url,Mount,KeyValue)
  }
  DOC2paintDescription(true);
} 

function DOC2paintDescription(ShouldPaint){
  if (!ShouldPaint){
    document.querySelector(".DOC2content").classList.add("DOC2md");
    document.querySelector(".DOC2description").classList.remove("DOC2md");
    document.querySelector(".DOC2description").innerHTML = "";
    return;
  }
  document.querySelector(".DOC2content").classList.remove("DOC2md");
  document.querySelector(".DOC2description").classList.add("DOC2md");
  document.querySelector(".DOC2description").innerHTML = MODELdata.DOCdocument.data.markdown;
}

async function DOC2paintIframe(Url,Mount,KeyValue){
  if (Mount.dataset.KeyValue === KeyValue) return;
  Mount.textContent = "Loading…";
  if (Mount.dataset.BlobUrl) { delete Mount.dataset.BlobUrl; }
  try {
    const Iframe = document.createElement("iframe");
    Iframe.className = "DOC2iframe";
    Iframe.src = Url + "#toolbar=1&navpanes=0&zoom=100";
    Mount.dataset.KeyValue = KeyValue;
    Mount.replaceChildren(Iframe);
  } catch (Err) {
    console.error(Err);
    Mount.textContent = "Unable to display document.";
  }
}

function DOC2paintImg(Url,Mount,KeyValue){
  if (Mount.dataset.KeyValue === KeyValue) return;
  Mount.textContent = "Loading…";
  try {
    const Img = document.createElement("img");
    Img.className = "DOC2image";
    Img.src      = Url;          
    Mount.dataset.KeyValue = KeyValue;
    Mount.replaceChildren(Img);
  } catch (Err) {
    console.error(Err);
    Mount.textContent = "Unable to display image.";
  }
}

function DOC2paintVideo(Url,Mount,KeyValue) {
  if (Mount.dataset.KeyValue === KeyValue) return;
  Mount.textContent = "Loading…";
  try {
    const Video = document.createElement("video");
    Video.className = "DOC2video";
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

function DOC2paintAudio(Url,Mount,KeyValue) {
  if (Mount.dataset.KeyValue === KeyValue) return;
  Mount.textContent = "Loading…";
  try {
    const Audio = document.createElement("audio");
    Audio.className = "DOC2audio";
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