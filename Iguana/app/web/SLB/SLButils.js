function SLBempty(Data){
   return !Object.keys(Data)?.length;
}

function SLBemptyHtml(){
   return /*html*/`<div class="SLBempty SLBinfo">No libraries are currently being used by any components.</div>`;
}

function SLBnameFromKey(raw){
   if (typeof raw !== 'string') return null;
   let url = raw.trim()
                .replace(/[?#].*$/, '')     // chuck ?foo or #branch
                .replace(/\.git$/, '')      // ditch .git suffix
                .replace(/\/+$/, '');       // ditch trailing /
   if (/^[\w.-]+@[\w.-]+:/.test(url)) {
     url = 'ssh://' + url.replace(':', '/');
   }
   try {
     const path = new URL(url).pathname;
     const parts = path.split('/').filter(Boolean);
     return parts.length ? parts.pop() : null;
   } catch {
     // Falls back here if URL constructor still can’t parse (unlikely)
   }
   const match = url.match(/([^/]+)$/);
   return match ? match[1] : null;
}

function SLBname(Name, Key){
   if(Name) return Name;
   let keyToName = SLBnameFromKey(Key);
   if(keyToName) return keyToName;
   return "N/A";
}

function SLBlink(Link){
   if(!Link) return `<a class="SLBlink"></a>`;
   return /*html*/`
   <a class="SLBlink BUTTONdefault BUTTONstandard" href="${Link}" target="_blank" title="Open the repository.">
      <div class="SLBlinkDiv"></div>
   </a>`;
}

function SLBcount(UsageData, param){
   let Count = 0;
   for(Use of UsageData) if(Use[param]) Count = Count + 1;
   return Count;
}

function SLBformat(Count){
   if(Count == 0) return `<div class="SLBzero">—</div>`;
   return /*html*/`${Count} ${Count == 1 ? "comp" : "comps"}`;
}

function SLBupdate(selector, value){
   let div = document.querySelector(selector);
   if(!div) return;
   if(div.innerHTML == value) return;
   div.innerHTML = value;
}