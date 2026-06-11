// Fast escaping routine I (a mystery programmer) found of stack overflow
//https://stackoverflow.com/questions/6234773/can-i-escape-html-special-chars-in-javascript
function ESChtml(unsafe) {
   return unsafe.replace(/[&<"'\r]/g, function (m) {
      switch (m) {
         case '&'  : return '&amp;';
         case '<'  : return '&lt;';
         case '"'  : return '&quot;';
         case '\r' : return '&#13;';
         default   : return '&#039;';
      }
   });
 }

function ESChtmlEscape(Text) {
   if (Text === null || Text === undefined) return '';
   // Force everything to a string first (Fixes crash on Numbers)
   return String(Text)
      .replace(/;/g, '&#59;')
      .replace(/&(?!#59;)/g, '&amp;')
      .replace(/</g, '&lt;')
      .replace(/>/g, '&gt;')
      .replace(/"/g, '&quot;');
}

function ESCstripHtml(html){
   let Html = html;
   if(!Html) Html = "";
   const tmp = document.createElement('DIV');
   tmp.innerHTML = Html;
   return tmp.textContent || tmp.innerText || "";
}

function ESCsafeHtml(html){
   let Html = html;
   if(!Html) Html = "";
   const tmp = document.createElement('DIV');
   tmp.innerHTML = Html;
   return tmp.innerHTML || "";
}