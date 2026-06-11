function LFDselectedLineFeed(){
   let div =  document.querySelector(".LFDlineFeedSelect");
   if(!div) return;
   return div.value;
}

function LFDsetSelectedLineFeed(Content){
   let hexVal = LFDdefaultLineFeed(Content);
   let div =  document.querySelector(".LFDlineFeedSelect");
   if(!div) return;
   div.value = hexVal;
}

function LFDdefaultLineFeed(Content){
   const lines = Content.split('\n');
   if(lines.length <=0)             return LFDlf;
   const firstLine = lines[0];
   if (firstLine.endsWith(LFDcrlf)) return LFDcrlf;
   if (firstLine.endsWith(LFDcr))   return LFDcr;
   return LFDlf;
}