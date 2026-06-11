const LFDcr = "\u240D";
const LFDlf = "\u240A";
const LFDcrlf = LFDcr + LFDlf;

function LFDcontentWithNewLines(Content){
   return Content.replace(
      // Match \u240D\u240A first, OR \u240D alone, OR \u240A alone,
      // only if NOT followed by a real newline (\n) OR \u240A.
      /(\u240D\u240A|\u240D|\u240A)(?![\n])/g,
      '$1\n'
      );
}

function LFDcontentWithLineFeeds(Content) {
   const dialogue = document.querySelector('.SMPLaddDataDialog');
   let LineFeed;
   if(dialogue) {
      const hasCR   = Content.includes("\r");
      const hasLF   = Content.includes("\n");
      const hasCRLF = Content.includes("\r\n");
      if (hasCRLF) {
         LineFeed = LFDcrlf;
      } else if (hasCR) {
         LineFeed = LFDcr;
      } else if (hasLF) {
         LineFeed = LFDlf;
      } else {
         LineFeed = LFDselectedLineFeed();
      }
   } else { LineFeed = LFDselectedLineFeed(); }
   return Content.replace(
      /(?<![\u240A\u240D])\n/g,
      `${LineFeed}\n`
   );
}

// on load convert \n to Lf and \r to Cr

function LFDsaveEncoded(Code){
   let Content = Code.replace(/\n/g, "");
   Content = Content.replaceAll(LFDcr, "\r");
   Content = Content.replaceAll(LFDlf, "\n");
   return Content;
}

function LFDloadDecoded(Code){
   let Content = LFDcontentWithNewLines(Code);
   Content = Content.replaceAll("\r", LFDcr);
   Content = Content.replaceAll("\n", LFDlf);
   Content = LFDcontentWithNewLines(Content);
   return Content;
}
