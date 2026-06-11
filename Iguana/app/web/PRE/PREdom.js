/* ==========================================================================
   PRE DOM
   - Shared DOM update helpers
========================================================================== */

function PREsetText(Selector, Value) {
   var el = document.querySelector(Selector);
   PREsetElementTextIfChanged(el, Value);
}

function PREsetElementTextIfChanged(Element, Text) {
   if (!Element) return;
   var next = String(Text == null ? "" : Text);
   if (Element.textContent === next) return;
   Element.textContent = next;
}

function PREsetElementClassIfChanged(Element, ClassName) {
   if (!Element) return;
   var next = String(ClassName == null ? "" : ClassName);
   if (Element.className === next) return;
   Element.className = next;
}

function PREsetElementAttrIfChanged(Element, Name, Value) {
   if (!Element) return;
   var next = String(Value == null ? "" : Value);
   if (Element.getAttribute(Name) === next) return;
   Element.setAttribute(Name, next);
}

function PREsetElementValueIfChanged(Element, Value) {
   if (!Element) return;
   var next = String(Value == null ? "" : Value);
   if (Element.value === next) return;
   Element.value = next;
}

function PREsetElementHTMLIfChanged(Element, Html) {
   if (!Element) return;
   var next = String(Html == null ? "" : Html);
   if (Element.innerHTML === next) return;
   Element.innerHTML = next;
}

function PREsetElementClassToggle(Element, ClassName, Enabled) {
   if (!Element) return;
   if (Enabled) {
      if (!Element.classList.contains(ClassName)) Element.classList.add(ClassName);
      return;
   }
   if (Element.classList.contains(ClassName)) Element.classList.remove(ClassName);
}

function PREcopyToClipboard(Text) {
   if (navigator.clipboard && navigator.clipboard.writeText) {
      navigator.clipboard.writeText(Text);
      return;
   }
   var ta = document.createElement("textarea");
   ta.value = String(Text || "");
   document.body.appendChild(ta);
   ta.select();
   document.execCommand("copy");
   document.body.removeChild(ta);
}

function PREflashCopied(Btn) {
   if (!Btn) return;
   var original = Btn.textContent;
   Btn.textContent = "Copied!";
   Btn.classList.add("PREisCopied");
   setTimeout(function() {
      Btn.textContent = original;
      Btn.classList.remove("PREisCopied");
   }, 1500);
}
