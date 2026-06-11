function LBUargs(){
   let libs = LBUlibs();
   let args = {};
   args.key = libs;
   return args;
}

function LBUlibs(){
   let List = TRGTlibraryList();
   let Libs = [];
   for(path in List){
      let lib = List[path];
      let libUrl = lib?.url || "";
      Libs.push(libUrl);
   }
   return Libs;
}

function LBUlibsExist(){
   let libs = LBUlibs() || [];
   return libs.length;
}

function LBUdata(){
   return MODELdata?.LBUdata?.data || {};
}

function LBUblankData(){
   return DEEPequal(LBUdata(), {});
}

function LBUlibsAheadOrBehind(){
   let data = LBUdata();
   let aheadOrBehind = false;
   for(url in data){
      let found = LBUlibAheadOrBehind(data[url]);
      if(found) aheadOrBehind = true;
   }
   return aheadOrBehind;
}

function LBUlibAheadOrBehind(libdata){
   let usage = libdata?.usage;
   let behind = LBUcount(usage, "behind");
   let ahead  = LBUcount(usage, "ahead");
   return behind || ahead;
}

function LBUcount(UsageData, param){
   let Count = 0;
   for(Use of UsageData) if(Use[param]) Count = Count + 1;
   return Count;
}

function LBUempty(Data){
   return !Object.keys(Data)?.length;
}

function LBUemptyHtml(){
   return /*html*/`<div class="LBUempty LBUinfo">No libraries are currently being used by any components.</div>`;
}

function LBUnameFromKey(raw){
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

function LBUname(Name, Key){
   if(Name) return Name;
   let keyToName = LBUnameFromKey(Key);
   if(keyToName) return keyToName;
   return "N/A";
}

function LBUlink(Component, Path){
   if(!Component) return ``;
   let href = `#translator?component=${Component}&amp;development=true`;
   let path = ESChtmlEscape(Path);
   return /*html*/`
   <div class="LBUopen">Edit →</div>
   <a class="LBUlink BUTTONdefault BUTTONstandard" href="${href}" data-compvar="${Component}" data-pathvar="${path}" title="Go to the component's development editor.">
      <div class="LBUlinkDiv"></div>
   </a>`;
}

function LBUcount(UsageData, param){
   let Count = 0;
   if(!UsageData) return;
   for(Use of UsageData) if(Use[param]) Count = Count + 1;
   return Count;
}

function LBUzero(){ return `<div class="LBUzero">—</div>`; }

function LBUformat(Count, Warning, href){
   if(Count == 0) return LBUzero();
   let Warninghtml = Warning ? " LBUwarningIcon" : "";
   let CountsDisplay = `${Count} ${Count == 1 ? "comp" : "comps"}`;
   if(href) return /*html*/`
   <div class="LBUhref${Warninghtml}">
      <a ${href}>${CountsDisplay}</a>
   </div>`;
   return /*html*/`<div class="LBUhref${Warninghtml}">${CountsDisplay}</div>`;
}

function LBUupdate(selector, value){
   let div = document.querySelector(selector);
   if(!div) return;
   if(div.innerHTML == value) return;
   div.innerHTML = value;
}

function LBUheader(Library, action){
   let words = "";
   if(action == "behind") words = `<b>Outdated</b> components:`;
   if(action == "ahead")  words = `<b>Customized</b> components:`;
   if(action == "usage")  words = `<b>Used in</b> components:`;
   return `<div class="LBUheaderDesc">${words}</div>`
}

function LBUclearDetails(){
   let div = document.querySelector(".LBUdetailsDiv");
   if(!div) return;
   div.innerHTML = "";
   LBUbehindData = [];
}

let LBUcomponentNames = {};

function LBUcalcComponentNames(){
   let components = LBUcomponents();
   LBUcomponentNames = Object.fromEntries(components.map(({ guid, name }) => [guid, name]));
}

function LBUcomponentName(guid){ return LBUcomponentNames[guid] ?? guid; };