/** @license
 * Copyright (c) 2010-2020 iNTERFACEWARE Inc.  All rights reserved.
 */

//julian may want to trap error in the future???
// Throws a "QuotaExceededError" DOMException exception if the new value couldn't be set. 
// (Setting could fail if, e.g., the user has disabled storage for the site, or if the quota has been exceeded.)
function STORElocalCreate(name, value) {
   try {
      localStorage.setItem(name, value); 
   } catch(e) {
      SNCKsnackbar("error", "Browser local storage exceeded. The value was unable to be set.", 0);
   }
}

// returns null if item does not exist
function STORElocalRead(name) {
   return localStorage.getItem(name);
}

function STORElocalErase(name) {
   return localStorage.removeItem(name);
}

function STOREsaveJsonVariable(Name, Json){
   STORElocalCreate(Name, JSON.stringify(Json));
}

function STOREreadJsonVariable(Name){
   let ObjectString = STORElocalRead(Name);
   let Object = {};
   try   { Object = JSON.parse(ObjectString); } 
   catch { STOREsaveJsonVariable(Name, {}); }
   if(!Object) Object = {};
   return Object;
}

function STOREcreateFileJump(lineNumber) {
   STORElocalCreate("TRANSfileJump", lineNumber);
}

function STOREreadFileJump() {
   const data = STORElocalRead("TRANSfileJump");
   STORElocalErase("TRANSfileJump"); // remove immediately after use to not conflict with other cursor positions
   return data;
}

function STOREcreateErrorJumpStore() {
   const fileInfo = document.querySelector('.STOREfileLink');
   if(!fileInfo) { return; }
   fileInfo.addEventListener('click', () => {
      const data = fileInfo.innerText.split(':');
      STOREcreateFileJump(data[1]);
   });
}