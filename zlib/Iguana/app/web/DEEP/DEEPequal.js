function DEEPequal(object1, object2) {
   if(object1 == null || object2 == null) {
      return false;
   }
   if (!DEEPisObject(object1) || !DEEPisObject(object2)){
      return object1 === object2;
   }
   const keys1 = Object.keys(object1);
   const keys2 = Object.keys(object2);
   if (keys1.length !== keys2.length) { return false; }
   for (const key of keys1) {
      const val1       = object1[key];
      const val2       = object2[key];
      const areObjects = DEEPisObject(val1) && DEEPisObject(val2);
      if (areObjects && !DEEPequal(val1, val2) || !areObjects && val1 !== val2) { return false; }
   }
   return true;
}

function DEEPisObject(object) { return object != null && typeof object === 'object'; }
