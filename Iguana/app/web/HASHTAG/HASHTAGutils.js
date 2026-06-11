// High-contrast base colors
const HASHTAGcolors = [
   'rgba(106,27,154,1.0)',   // deep violet (purple 700)
   'rgba(40,53,147,1.0)',    // deep indigo (700)
   'rgba(21,101,192,1.0)',   // deep blue (700)
   'rgba(0,105,92,1.0)',     // deep teal (700)
   'rgba(46,125,50,1.0)',    // deep green (700)
   'rgba(0,77,64,1.0)',      // deep cyan/green-ish (900)
   'rgba(74,20,140,1.0)',    // deep magenta/purple (800)
   'rgba(55,71,79,1.0)'      // deep slate / blue-grey (800)
 ];
 
 
 const HASHTAGreserved = {
   error:   'rgba(198,40,40,1.0)',   // red 700, error only
   warning: 'rgba(230,74,25,1.0)',   // deep Orange 800 (optional)
   info:    'rgba(2,119,189,1.0)'    // info blue (strong)
 };
 
function HASHTAGrenderTags(tags, classPassed = "", roleData = undefined, href = {}, searchText = "") {
   if (!tags) return;
   let result = "";
   for (const tag of tags) {
     const shouldUseClass = roleData === undefined || roleData[tag];
     result += HASHTAGcreateTagElement(tag, shouldUseClass ? classPassed : "", href[tag], searchText);
   }
   return result;
}

function HASHTAGhashStringToNumber(str) {
   return Array.from(str).reduce((hash, char) => {
      hash = ((hash << 5) - hash) + char.charCodeAt(0);
      return hash & hash; // Convert to 32bit integer
   }, 0);
}

function HASHTAGgetHashtagColor(tag) {
   if (!tag) return HASHTAGcolors[0];

   const normalized = tag.toLowerCase();

   // reserve red strictly for #error
   if (normalized === "#error") return HASHTAGreserved.error;
   if (normalized === "#warning") return HASHTAGreserved.warning;
   if (normalized === "#info") return HASHTAGreserved.info;
   if (normalized === "#autostart") return HASHTAGreserved.info;

   // for all other hashtags, hash only against NON-red colors
   return HASHTAGcolors[Math.abs(HASHTAGhashStringToNumber(normalized)) % HASHTAGcolors.length];
}

function HASHTAGcreateTagElement(word, className = "", href = "", searchText = "") {
   const color = HASHTAGgetHashtagColor(word);
   const element = document.createElement('a');
   
   Object.assign(element.style, {
      backgroundColor: color.replace(/[^,]+(?=\))/, '0.1'),
      border: `1px solid ${color.replace(/[^,]+(?=\))/, '0.5')}`,
      color: color
   });
   let AddClass = className == "" ? "DASHhashtag" : className;
   element.classList.add(AddClass, 'HASHTAGstyle', 'HASHTAGtagStyle');
   // element.classList.add(AddClass.trim());
   if (href) element.href = href;
   element.setAttribute('word', word);
   
   if (searchText && word.toLowerCase().includes(searchText.toLowerCase())) {
      const regex = new RegExp(`(${searchText})`, 'gi');
      element.innerHTML = word.replace(regex, match => `<span class="HASHTAGbold" style="${color};">${match}</span>`);
   } else {
      element.textContent = word;
   }
   
   return element.outerHTML;
}

function HASHTAGrenderHashtags(text, options = {}) {
   if (!text) return "";
   const { toIcon = true, search = "", renderHashtags = true } = options;
   const searchText = search.toLowerCase();
   return text.split('\n').map(line => 
      HASHTAGprocessWords(line, word => {
         if (!searchText.includes(word.toLowerCase()) && EDITisHashtag(word) && renderHashtags) {
            return HASHTAGcreateTagElement(word);
         } else if (toIcon && DASHisValidHttpUrl(word)) {
            const Inner = word.startsWith("https://man.interfaceware.com") ? "Learn more" : word;
            return `<a class="DASHrenderHashtagInfoSymbol" target="_blank" href="${word}">${Inner}</a>`;
         }
         return word;
      })
   ).join('\n');
}

function HASHTAGrenderLinks(text) {
   return text.split('\n').map(line => 
      HASHTAGprocessWords(line, word => 
         DASHisValidHttpUrl(word)
            ? `<a class="DASHrenderHashtagInfoSymbol" target="_blank" href="${word}">Learn more</a>`
            : word
      )
   ).join('\n');
}

function HASHTAGprocessWords(line, processWord) {
   return line.split(/([\s\t]+)/).map(processWord).join('');
}

function EDITisHashtag(word) {
   const hashtagRegex = /^-?#[A-Za-z0-9\:\-._*]+$/;
   return hashtagRegex.test(word);
}
