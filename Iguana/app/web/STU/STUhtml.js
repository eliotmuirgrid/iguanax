function STUloggingHtml() {
   let UsingAltDir = STUaltLogEnabled();
   return /*html*/`
   <div class="SETsettingsPaneLayout SETsettingsPane">
      <div class="STGlogUsagePage">
         <div class="SETloggingTitle">Log Storage</div>
         <div class="SETpageDescription">Review disk usage and choose where Iguana writes its logs. [<a target="_blank" href="${LINKsettingsLogUsage}">Learn more</a>]</div>
         ${STUnewHtml()}
         <div class="SETloggingTableWrap">
            <div class="SETloggingTable">
               <div class="SETloggingHeading">
                  <div class="SETloggingTableHeading">Component</div>
                  <div class="SETloggingTableHeading">Path</div>
                  <div class="SETloggingTableHeading">Log Size</div>
                  <div class="SETloggingTableHeading">${UsingAltDir ? "Backup Size" : ""}</div>
                  <div class="SETloggingTableHeading">Action</div>
               </div>
               ${STGsortBySizeDesc().map(Stat => {return STGlogSizeRowHtml(Stat, UsingAltDir);}).join('')}
            </div>
         </div>
      </div>
   </div>`;
}


function STUlogsDirHtml(){
   return /*html*/`
   <div class="STUsection">
      <div class="STUtitleRow">
         <div class="STUtitle">Log folder</div>
         <div class="STUdesc">Active logs written while Iguana is running.</div>
         <div class="BUTTONstandard BUTTONdefault STUchange">Change folder</div>
      </div>
      <div class="STUtable">
         <div class="STUtableTitle">Current path</div>
         <div class="STUsubTable">
            <div class="STUtableVal STUdirPath"></div>
            <div class="STUtableVal STUlogSize"></div>
         </div>
      </div>
   </div>`;
}

function STUarchiveDirHtml(){
   return /*html*/`
   <div class="STUsection STUsectionBottom">
      <div class="STUtitleRow">
         <div class="STUtitle">Archive folder</div>
         <div class="STUdesc">Logs and data from deleted components.</div>
         <div class="STUclearArchive BUTTONstandard BUTTONwarning BUTTONdefault">Clear archive</div>
      </div>
      <div class="STUtable">
         <div class="STUtableTitle">Current path</div>
         <div class="STUsubTable">
            <div class="STUtableVal STUarchivePath"></div>
            <div class="STUtableVal STUarchiveSize"></div>
         </div>
      </div>
   </div>`;
}

function STUusageHtml(){
   return /*html*/`
   <div class="STUsectionTop">
      <div class="STUfreeBar">
         <div class="STUfree"></div>
         <div class="STUstatus"></div>
      </div>
      <div class="STUprogress">
         <div class="STUprogressUsed"></div>
      </div>
   </div>
   `;
}

function STUdupLogsDirHtml(){
   if (!STUaltLogEnabled()) return ""
   return /*html*/`
   <div class="STUsection">
      <div class="STUtitleRow">
         <div class="STUtitle">Backup log folder</div>
         <div class="STUdesc">This folder should be on another host.</div>
         <div class="BUTTONstandard BUTTONdefault STUchangeBackup">Change folder</div>
      </div>
      <div class="STUtable">
         <div class="STUtableTitle">Current path</div>
         <div class="STUsubTable">
            <div class="STUtableVal STUaltDirPath"></div>
            <div class="STUtableVal STUaltLogSize"></div>
         </div>
      </div>
   </div>`;
}

function STUnewHtml(){
   return /*html*/`
   <div class="STUbox">
      ${STUusageHtml()}
      ${STUlogsDirHtml()}
      ${STUdupLogsDirHtml()}
      ${STUarchiveDirHtml()}
   </div>`;
}
