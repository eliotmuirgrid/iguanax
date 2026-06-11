function LOGisOnlyMessageType(){
   return PAGEhashParameter("type") === "message";
 }
 
 function LOGneedsGapRow (Idx, Data) {
   if (Idx === 0) return false;
   if (PAGEhashParameter("filter") == 'true')  return false;
   if (PAGEhashParameter("type")   == 'error') return false;
 
   // In messages-only mode, only report if there's a queued gap.
   if (LOGisOnlyMessageType()) return LOGdetermineQidDiff(Idx, Data) > 0;
 
   const currentSid = parseInt(Data[Idx]?.sid || "0", 10);
   const oldSid     = parseInt(Data[Idx - 1]?.sid || "0", 10);
   if (currentSid === 0 || oldSid === 0) return false;
   const sidDiff = LOGdetermineDiff(Idx, Data);          // total gap size
   if (sidDiff < 1) return false;                        // no missing rows
   return currentSid !== (oldSid - 1);                   // true ⇢ show gap row
 }
 
 function LOGdetermineDiff(Idx, Data){
   const I  = Data[Idx];
   const I2 = Data[Idx - 1];
   const CurrentSid = parseInt(I?.sid  || "0");
   const OldSid     = parseInt(I2?.sid || "0");
   const Diff = OldSid - CurrentSid - 1;
   return Diff;
 }
 
 function LOGdetermineQidDiff(Idx, Data){
   const I  = Data[Idx];
   const I2 = Data[Idx - 1];
   const CurrentSid = parseInt(I?.qid  || "0");
   const OldSid     = parseInt(I2?.qid || "0");
   let Diff = OldSid - CurrentSid;
   if (I2.type == "message") Diff = Diff - 1;
   return Diff;
 }
 
 function LOGformatGapMessage (total, queued) {
   // In messages-only mode, only talk about queued gaps.
   if (LOGisOnlyMessageType()) {
   const entriesWord = queued === 1 ? 'entry' : 'entries';
     return `${queued} log ${entriesWord} missing in this time range — all ${queued} are queued messages`;
   }
 
   const other = total - queued;
   const entryWord = total === 1 ? 'entry' : 'entries';
 
   let breakdown;
   if (queued === total) {
     breakdown = `all ${queued} are queued messages`;
   } else if (queued === 0) {
     breakdown = `${other} other (info/error/debug etc); no queued messages lost`;
   } else {
     breakdown = `${queued} queued, ${other} other (info/error/debug etc)`;
   }
 
   return `${total} log ${entryWord} missing in this time range — ${breakdown}.`;
 }
 
 /* -------------------------------------------------------------------
  *  Row renderer used by the log viewer
  * ------------------------------------------------------------------*/
 function LOGrowGapHtml (Idx, Data) {
   const queuedMissing = LOGdetermineQidDiff(Idx, Data);
   // In messages-only mode, treat "total" as the queued gap count.
   const totalMissing  = LOGisOnlyMessageType()
     ? queuedMissing
     : LOGdetermineDiff(Idx, Data);

   const message = LOGformatGapMessage(totalMissing, queuedMissing);
 
   return /*html*/ `
     <div class="LOGrowGapEntry">${message}</div>
   `;
 }
 