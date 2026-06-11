function DBMcheckSqlite(FilePath) {
   const FIleName = TRANSprojectRoot() + FilePath;
   APIcall('database/check_sqlite', {filename: FIleName, component: TRANScomponentGuid(), development: TRANSdevelopment()}, function(R) {
      if(!R.success && R.error) { SNCKsnackbar('error',R.error, 20000); }
      if(!R.success || R.data?.is_sqlite === false) { return TRANSfileChange(FilePath); }
      WNDwindow("DBMsqliteWindow", DBMtableWindow(FilePath, R.data.tables), `${FilePath}`);
      DBMsetWindowDimensions();
      DBMcopyCodeToClipboardListener();
      DBMconnectHelp();
   })
}

function DBMtableWindow(filePath, tables) {
   return `
<div class="DBM-popup">
<div>This is a SQLlite database file.</div>
    <div class="DBM-code-examples">
        <p>Here is some example code you can copy paste to get started to query this database:</p>
        <div style="position: relative;">
            <pre id="codeSnippet">
            
local D = db.connect{api=db.SQLITE, name=component.projectRoot()..'${ESChtmlEscape(filePath)}'}
local R = D:query{sql='SELECT * FROM ${tables?.length ? tables[0] : 'xyz'} LIMIT 50;'}
            </pre>
            <div class="DBMcopyFeedback"></div>
            <div class="DBIMcopyCode" id="DBMcopyButton">
                <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" style="position: absolute; top: 5px; right: 5px;">
                    <path d="M3,4 L3,5 C2.48716416,5 2.06449284,5.38604019 2.00672773,5.88337887 L2,6 L2,13 C2,13.5128358 2.38604019,13.9355072 2.88337887,13.9932723 L3,14 L10,14 C10.5128358,14 10.9355072,13.6139598 10.9932723,13.1166211 L11,13 L12,13 C12,14.1045695 11.1045695,15 10,15 L3,15 C1.8954305,15 1,14.1045695 1,13 L1,6 C1,4.8954305 1.8954305,4 3,4 Z M13,1 C14.1045695,1 15,1.8954305 15,3 L15,10 C15,11.1045695 14.1045695,12 13,12 L6,12 C4.8954305,12 4,11.1045695 4,10 L4,3 C4,1.8954305 4.8954305,1 6,1 L13,1 Z M13,2 L6,2 C5.48716416,2 5.06449284,2.38604019 5.00672773,2.88337887 L5,3 L5,10 C5,10.5128358 5.38604019,10.9355072 5.88337887,10.9932723 L6,11 L13,11 C13.5128358,11 13.9355072,10.6139598 13.9932723,10.1166211 L14,10 L14,3 C14,2.48716416 13.6139598,2.06449284 13.1166211,2.00672773 L13,2 Z" fill="#FFFFFF" fill-rule="nonzero"/>
                </svg>
            </div>
        </div>
    </div>
    <p>For more information <a href="${LINKtransSqlite}" target="_blank">read the manual.</a></p>
    <div class="DBM-button">See connect</div>
    <div class="DBM-button">See query</div>
    <br>
    <br>
    <div class="DBM-table-container">
        <table class="DBM-table">
            <thead>
                <tr>
                    <th>Available Tables</th>
                </tr>
            </thead>
            <tbody>
                ${DBMshowtables(tables)}
            </tbody>
        </table>
    </div>
</div>
`;
}


function DBMshowtables(tables) {
   if (!tables?.length) { return ``; }
   return tables.map(table => { return `<tr><td>${ESChtmlEscape(table)}</td></tr>`; }).join('');
}

function DBMsetWindowDimensions() {
   const sqliteWindow = document.getElementById('DBMsqliteWindow');
   const codeSnippet  = document.getElementById("codeSnippet");
   if(!sqliteWindow || !codeSnippet) { return; }
   const codeLength             = codeSnippet.textContent.length;
   const baseHeight             = 400;
   const table                  = document.querySelector(".DBM-table");
   const rows                   = table.querySelectorAll("tbody tr");
   const numRows                = rows.length;
   const rowHeight              = 30;
   const maxRowsBeforeScrollbar = 5; // Adjust the number of rows before adding a scrollbar
   const heightInPixels         = numRows <= maxRowsBeforeScrollbar ? numRows * rowHeight : maxRowsBeforeScrollbar * rowHeight;
   sqliteWindow.style.height    = baseHeight + heightInPixels + "px";
   sqliteWindow.style.width     = `${codeLength * 4}px`;
}

function DBMcopyCodeToClipboardListener() {
   const CopyBtn = document.querySelector('.DBIMcopyCode');
   if(!CopyBtn) { return; }
   CopyBtn.addEventListener('click', function() {
      const CodeSnippet = document.getElementById('codeSnippet');
      const TextArea    = document.createElement('textarea');
      TextArea.value    = CodeSnippet.textContent.trim();
      document.body.appendChild(TextArea);
      TextArea.select();
      document.execCommand('copy');
      document.body.removeChild(TextArea);
      const CopyButton = document.getElementById('DBMcopyButton');
      CopyButton.classList.add('DBMflash-animation');
      setTimeout(() => { CopyButton.classList.remove('DBMflash-animation'); }, 500);
      const Feedback = document.querySelector('.DBMcopyFeedback');
      Feedback.innerText = 'Copied!';
      setTimeout(() => { Feedback.innerText = ''; }, 2000);
   })
}

function DBMresolveHelp(text) {
   switch(text) {
      case 'connect': return 'db.connect';
      case 'query'  : return 'dbconnection.query';
      default       : return '';
   }
}

function DBMconnectHelp() {
   const DBbuttons = document.querySelectorAll('.DBM-button');
   DBbuttons.forEach(btn => {
      btn.addEventListener('click', function() {
         let functionName = DBMresolveHelp(btn.innerText.split(' ')[1]);
         HLPjumpToFunction(functionName)
      });
   });
}
