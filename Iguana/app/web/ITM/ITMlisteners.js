function ITMdecodeHTMLEntities(str) {
   const textarea = document.createElement("textarea");
   textarea.innerHTML = str;
   return textarea.value;
}

function ITMcopyContent(e) {
   let Div = e.target.closest(".ANNpopup");
   let InnerList = Div.querySelectorAll(".chunk");

   // Aggregate text content from all .chunk elements
   let textToCopy = Array.from(InnerList)
       .map(inner => ITMdecodeHTMLEntities(inner.innerHTML))
       .join("\n"); // Join with newline for better readability
   ITMsafeCopy(textToCopy, Div);
}

function ITMsafeCopy(text, fallbackDiv) {
   // Try to use clipboard API first
   if(navigator.clipboard && navigator.clipboard.writeText) {
      navigator.clipboard.writeText(text)
         .then(() => ITMdisplayCopied(fallbackDiv))
         .catch(err => console.error('Failed to copy text: ', err));
   } else {
      // Fallback to execCommand (older browsers)
      try {
         const textArea          = document.createElement('textarea');
         textArea.value          = text;
         textArea.style.position = 'fixed';  // Avoid scrolling to bottom
         document.body.appendChild(textArea);
         textArea.focus();
         textArea.select();
         const successful = document.execCommand('copy');
         document.body.removeChild(textArea);
         if(successful) { ITMdisplayCopied(fallbackDiv); }
      } catch(err) {
         console.error('Fallback copy method failed: ', err);
      }
   }
}

function ITMdisplayCopied(Div){
   let CopyText = Div.querySelector(".ITMcopyContent");
   if(!CopyText) return;
   CopyText.innerHTML = 'Copied!';
}

function ITMresetCopiedHtml(){
   let CopyDivs = document.querySelectorAll(".ITMcopyContent");
   if(!CopyDivs || CopyDivs.length == 0) return;
   for(let i=0; i < CopyDivs.length; i++) {
      let Html = "Copy Content";
      let Div = CopyDivs[i];
      if(Div.innerHTML != Html) Div.innerHTML = Html; 
   }
}

function ITMhandleCopyHideShow(Event, ClassName){
   if(ClassName.includes("string_et")) return ITMhideCopy(Event);
   if(ClassName.includes("string_hx")) return ITMhideCopy(Event);
   ITMshowCopy(Event)
}

function ITMhideCopy(e){
   let Div = e.target.closest(".ANNpopup").querySelector(".ITMbannerContainer");
   Div.classList.add("ITMhideContent");
}

function ITMshowCopy(e){
   let Div = e.target.closest(".ANNpopup").querySelector(".ITMbannerContainer");
   Div.classList.remove("ITMhideContent");
}
