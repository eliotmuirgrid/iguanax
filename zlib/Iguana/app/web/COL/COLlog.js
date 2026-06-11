/*
The trace library allows conditional tracing in our Javascript.  To switch on tracing 
for functions starting with say MODEL, do:

   COLtrace("MODEL*");   

To turn tracing off do
   COLtrace();

The library is relatively expensive but then hopefully our Javascript routines don't need
to do that much...We can probably write code to remove these calls in production mode later if we
want to.

If you have ideas to improve this library please have a conversation with Eliot about it.

One of the serious drawbacks of the library is it won't work well in the context of anonymous functions
like:

   APIcall('foo', {}, function(R){
      COL_TRC("This won't log"));
   });

This is because the stack of the Error object doesn't have information on file name where the function
was defined.  Same problem occurs when we assign a function to say a hash table.
*/

let COLlogRegex = null;
let COLdebugGlob = null;

console.log("From the console type COLtrace('PAG*') to trace functions starting with PAG");
console.log("To turn tracing off completely use COLtrace()");
function COLtrace(pattern, debug_pattern) {
   if (!pattern){
      COLlogRegex = null;
      console.log("Tracing is disabled.");
   } else {
      console.log("Tracing match: " + pattern)
      COLlogRegex = pattern;
   }
   if (!debug_pattern){
      COLdebugGlob = null;
      console.log("Debug tracing is disabled.");
   } else {
      console.log("Tracing match: " + debug_pattern)
      COLdebugGlob = debug_pattern;
   }
}

function COLlogPrefix(Info){
   return COLtimestamp() + " " + String(Info.functionName + ":" + Info.lineNumber).padEnd(22, ' ');
}

function COL_TRC(...args){
   if (!COLlogRegex){
      return;
   }
   let Info = COLfuncInfo();
   if (COLglobMatch(COLlogRegex, Info.functionName)){
      console.log(COLlogPrefix(Info), ...args);   // Thanks to chat GPT for this obscure part of Javascript syntax
   }
}

function COL_HEX(Label, S){
   if (!COLlogRegex){
      return;
   }
   let Info = COLfuncInfo();
   if (COLglobMatch(COLlogRegex, Info.functionName)){
      console.log(COLlogPrefix(Info), Label, "\n" + COLhexDump(S));
   }
}

function COL_DBG(...args){
   if (!COLdebugGlob){
      return;
   }
   let Info = COLfuncInfo();
   if (COLglobMatch(COLdebugGlob, Info.functionName)){
      console.log(COLlogPrefix(Info), ...args);
   }
}

function COL_ERR(...args){
   let Info = COLfuncInfo();
   console.log(COLlogPrefix(Info), ...args);   // Thanks to chat GPT for this obscure part of Javascript syntax
}

// FROM Chat GPT - I haven't optimized this code. Eliot
function COLhexDump(input) {
   if (typeof input !== 'string'){
      return input;
   }
   const bytes = new TextEncoder().encode(input); // Convert string to UTF-8 byte array
   let hexLines = '';
   for (let i = 0; i < bytes.length; i += 16) {
       const offset = i.toString(16).padStart(8, '0'); // 8-character hex offset
       const chunk = bytes.slice(i, i + 16);
       
       const hexBytes = Array.from(chunk)
           .map(byte => byte.toString(16).padStart(2, '0')) // Convert each byte to hex
           .join(' '); // Join the hex bytes with a space
       
       const ascii = Array.from(chunk)
           .map(byte => (byte >= 32 && byte <= 126 ? String.fromCharCode(byte) : '.')) // Printable ASCII or `.`
           .join('');
       
       const paddedHex = hexBytes.padEnd(48, ' '); // Pad hex bytes for alignment
       hexLines += `${offset}  ${paddedHex} |${ascii}|\n`; // Combine and format
   }
   return hexLines;
}

// IMPLEMENTATION - from Chat GPT with the right question :-)
function COLfuncInfo() {
   // Create a new Error object to capture the stack trace
   const error = new Error();
 
   // Retrieve the stack trace
   const stackTrace = error.stack;
 
   // Extract the calling function name and line number from the stack trace
   if (stackTrace) {
     const lines = stackTrace.split('\n');
     // The calling function info should be present at index 4 (assuming this function is called directly)
     if (lines.length >= 4) {
       const callerLine = lines[3].trim();
       const functionNameRegex = /at\s+(\S+)\s+\(.*:(\d+):\d+\)$/;
       const matches = callerLine.match(functionNameRegex);
       if (matches && matches.length >= 3) {
         const functionName = matches[1];
         const lineNumber = matches[2];
         return { functionName, lineNumber };
       }
     }
   }
 
   return { functionName: 'Unknown', lineNumber: 'Unknown' };
}

function COLtimestamp(){
   let date = new Date();
   // TODO - could optmimize it to cache the date string.
   const hours   = String(date.getHours())  .padStart(2, "0"); // Zero-padded hours
   const minutes = String(date.getMinutes()).padStart(2, "0"); // Zero-padded minutes
   const seconds = String(date.getSeconds()).padStart(2, "0");
   const fraction = String(date.getTime() % 1000).padEnd(3, "0");
   return `${hours}:${minutes}:${seconds}.${fraction}`;
}