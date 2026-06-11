function FORMfocus(el, highlight = false) {
   if (!el || typeof el.focus !== "function") return;
   el.focus();
   if (el.type === "number") return;
   if (highlight && typeof el.select === "function") {
      el.select();
      return;
   }
   if (typeof el.value !== "string") return;
   if (typeof el.selectionStart !== "number" || typeof el.selectionEnd !== "number") return;
   el.selectionStart = el.selectionEnd = el.value.length;
}
 
function FORMfocusDelay(el){
   setTimeout(function(){ FORMfocus(el); }, 200);
}

function FORMenterListener(div, saveFunc) {
   if (!div) return;
   function handler(e) { FORMkeydownListeners(e, saveFunc); }
   div.removeEventListener("keydown", handler);
   div.addEventListener("keydown", handler);
}
 
function FORMkeydownListeners(e, saveFunc) {
   if (e.key === "Enter" && !e.shiftKey) {
     e.preventDefault();
     return saveFunc();
   }
}

function FORMindexFromClickToData(target, Data, Selector){
   if(!target) return;
   let ListData = Data;
   const Row = target.closest(Selector);
   if(!Row) { return; }
   const parentElement = Row.parentNode;
   const allRows = Array.from(parentElement.querySelectorAll(Selector));
   const Index = allRows.indexOf(Row);
   if(Index >= ListData.length) { return; }
   return Index;
}


function FORMisMAC() {
	return navigator.userAgent.includes("Mac");
}

function FORMcheckForCommandPlusKey(Event, Key, callback) {
	const IsMac = FORMisMAC();
	const KeyPressed = Event.key.toLowerCase();
	if (KeyPressed === Key && ((!IsMac && Event.ctrlKey) || (IsMac && Event.metaKey))) {
		Event.preventDefault();
      if(callback) callback();
	}
}
