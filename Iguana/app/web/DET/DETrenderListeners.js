/* ------------------------------------------------------------------
   Config – tweak these two numbers if you need a different cadence
-------------------------------------------------------------------*/
const DET_POLL_WINDOW = 500;   // minimum gap (ms) between immediate polls
const DET_DELAYED_GAP = 500;  // delayed poll fires this long after last scroll

/* ------------------------------------------------------------------
   Internal state – simple globals
-------------------------------------------------------------------*/
let DET_lastImmediatePoll = 0;   // timestamp (ms) of the most-recent immediate poll
let DET_delayedTimer = null;

/* ------------------------------------------------------------------
   Attach listener once
-------------------------------------------------------------------*/
function DETsetRenderListeners() {
   const body = document.querySelector('.DETdetailContent');
   body.addEventListener('scroll', DETselectTopAndBottom);
}

/* ------------------------------------------------------------------
   Scroll handler: throttled immediate poll + debounced delayed poll
-------------------------------------------------------------------*/
function DETselectTopAndBottom() {
   DETselectBottom();                               // your existing highlight logic

   /* ---- THROTTLE: fire immediately, but no more often than DET_POLL_WINDOW ---- */
   const now = performance?.now?.() ?? Date.now();
   if (now - DET_lastImmediatePoll >= DET_POLL_WINDOW) {
      DET_lastImmediatePoll = now;
      MODELforcePoll('DETmessageModel');
   }

   /* ---- DEBOUNCE: clear & reset the trailing 1 s poll ---- */
   clearTimeout(DET_delayedTimer);
   DET_delayedTimer = setTimeout(() => {
      MODELforcePoll('DETmessageModel');
   }, DET_DELAYED_GAP);
}

/* ------------------------------------------------------------------
   Improved visible-row detector.
   – viewTop / viewBottom bracket the viewport (± a 100 px buffer)
   – topRow  = first row whose bottom edge crosses viewTop
   – botRow  = last  row whose top    edge crosses viewBottom
-------------------------------------------------------------------*/
function DETselectBottom() {
   const parentDiv      = document.querySelector('.DETdetailContent');
   const viewTop        = parentDiv.scrollTop - 100;                 // add buffer
   const viewBottom     = parentDiv.scrollTop + parentDiv.clientHeight + 100;
   const rows           = parentDiv.querySelectorAll('.DETmessageRow');
 
   let topRow  = null;   // earliest visible row
   let botRow  = null;   // last     visible row
 
   rows.forEach(row => {
     const rowTop    = row.offsetTop;
     const rowBottom = rowTop + row.offsetHeight;
 
     /* Is any part of the row inside the buffered viewport? */
     const visible = rowBottom >= viewTop && rowTop <= viewBottom;
     if (!visible) return;
 
     /* Track first & last visible rows */
     if (!topRow || rowTop < topRow.offsetTop)      topRow = row;
     if (!botRow || rowBottom > botRow.offsetTop + botRow.offsetHeight) botRow = row;
   });
 
   /* ---------- apply / clear the highlight classes ---------- */
   rows.forEach(r => {
     r.classList.toggle('DETtopVisible',    r === topRow);
     r.classList.toggle('DETbottomVisible', r === botRow);
   });
 
   /* Return whether we saw anything at all (for the watchdog) */
   return !!(topRow || botRow);
 }