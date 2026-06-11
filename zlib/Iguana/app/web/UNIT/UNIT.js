let UNITtests = {};

function UNITaddTest(description, testFunction) { UNITtests[description] = testFunction; }

function UNITassertEquals(expected, actual) {
   const UNITisEqual = (a, b) => {
      if(Array.isArray(a) && Array.isArray(b)) {
         // Deep compare arrays
         if(a.length !== b.length) { return false; }
         for(let i = 0; i < a.length; i++) { if(!UNITisEqual(a[i], b[i])) { return false; } }
         return true;
      } else if(typeof a === 'object' && typeof b === 'object') {
         // Deep compare objects
         const keysA = Object.keys(a);
         const keysB = Object.keys(b);
         if(keysA.length !== keysB.length) { return false; }
         for(const key of keysA) { if(!UNITisEqual(a[key], b[key])) { return false; } }
         return true;
      } else { return a === b; }
   };

   if(!UNITisEqual(expected, actual)) {
      throw new Error(`\nExpected:\n${JSON.stringify(expected)}\nActual:\n${JSON.stringify(actual)}`);
   }
}

function UNITrunTests() {
   const results = [];
   for(const [description, testFunction] of Object.entries(UNITtests)) {
      try {
         testFunction();
         results.push({description, result: '✅'});
      } catch(error) {
         results.push({description, result: `❌ ${error.message || 'Unknown error'}`});
      }
   }

   PAGEhash() === 'unittests' ? UNITdisplay(results) : UNITprintToConsole(results);
}

function UNITprintToConsole(results) {
   const successfulTests = results.filter(({result}) => result.startsWith('✅'));
   if(successfulTests.length > 0) {
      successfulTests.forEach(({description, result}) => { console.log(`${description} SUCCESS`); });
   }
   const failedTests = results.filter(({result}) => result.startsWith('❌'));
   if(failedTests.length > 0) {
      failedTests.forEach(({description, result}) => { console.error(`${description}: ${result}`); });
   }
}

function UNITdisplay(results) {
   const successDiv     = document.querySelector('.UNITsuccess');
   const failDiv       = document.querySelector('.UNITfail');
   successDiv.innerHTML = '';
   failDiv.innerHTML   = '';

   results.forEach(({description, result}) => {
      const resultElement       = document.createElement('p');
      resultElement.textContent = `${description}: ${result}`;

      if(result.includes('✅')) {
         successDiv.appendChild(resultElement);
      } else {
         failDiv.appendChild(resultElement);
      }
   });
}