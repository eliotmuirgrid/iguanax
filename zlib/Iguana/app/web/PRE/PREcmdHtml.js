/* ==========================================================================
   PRE Command HTML
   - Command Center HTML builders
========================================================================== */

function PREcommandUnavailableScreenHtml() {
   var state = PRElicenseDashboardState();
   if (state === null) {
      return /*html*/`
      <div class="PREscreenContainerAuthorized PREfeatureNoticeScreen">
         <div class="PREgridContentAreaAuthorized">
            ${PREfeatureNoticeHtml(
               "Checking Command Center Access",
               "Verifying this instance license and role. If this takes longer than expected, refresh the page.",
               [{ href: "#dashboard", label: "Open Dashboard", primary: true }]
            )}
         </div>
      </div>`;
   }

   return /*html*/`
   <div class="PREscreenContainerAuthorized PREfeatureNoticeScreen">
      <div class="PREgridContentAreaAuthorized">
         ${PREfeatureNoticeHtml(
            "Command Center Unavailable",
            "Your current license does not have the Federated Dashboard feature enabled.",
            [
               { href: "#settings/license", label: "Upgrade License", primary: true },
               { href: "#settings/authorized_peers", label: "Did you mean Authorized Peers?" }
            ]
         )}
      </div>
   </div>`;
}

function PREcommandGridHeaderHtml() {
   return /*html*/`
      <div class="PREgridHeader">
         <div class="PREcolPeer">Name</div>
         <div class="PREcolLink">Connection</div>
         <div class="PREcolLatency">Latency</div>
         <div class="PREcolAccess">Access</div>
         <div class="PREcolQueue">Que</div>
         <div class="PREcolErrors">Err</div>
         <div class="PREcolRate">Rate</div>
         <div class="PREcolCpu">CPU</div>
         <div class="PREcolMem">Mem</div>
         <div class="PREcolDsk">Dsk</div>
         <div class="PREcolActions">Action</div>
      </div>`;
}

function PREcommandScreenHtml() {
   return /*html*/`
   <div class="PREscreenContainer">
      <div class="PREmainHeader">
         <div class="PREheaderLeft">
            <div class="PREappIcon">
               <svg style="width:24px; height:24px; display:block;" viewBox="0 0 24 24" fill="currentColor"><path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm-1 17.93c-3.95-.49-7-3.85-7-7.93 0-.62.08-1.21.21-1.79L9 15v1c0 1.1.9 2 2 2v1.93zm6.9-2.54c-.26-.81-1-1.39-1.9-1.39h-1v-3c0-.55-.45-1-1-1H8v-2h2c.55 0 1-.45 1-1V7h2c1.1 0 2-.9 2-2v-.41c2.93 1.19 5 4.06 5 7.41 0 2.08-.8 3.97-2.1 5.39z"/></svg>
            </div>
            <div class="PREtitle">Command Center</div>
         </div>

         <div class="PREheaderActions">
            <div class="PREbtnMyDetails BUTTONstandard" title="Share My Details">
               <svg class="PREbtnMyDetailsIcon" viewBox="0 0 24 24" fill="currentColor" aria-hidden="true">
                  <path d="M18 16.08c-.76 0-1.44.3-1.96.77L8.91 12.7a2.97 2.97 0 0 0 0-1.39l7-4.11A2.99 2.99 0 1 0 15 5a2.9 2.9 0 0 0 .06.59l-7.12 4.18a3 3 0 1 0 0 4.46l7.12 4.18c-.04.18-.06.38-.06.59a3 3 0 1 0 3-2.92z"/>
               </svg>
               <span>Share My Details</span>
            </div>
            <div class="BUTTONstandard BUTTONaction PREbtnAdd" title="Add New Peer">+ Add Peer</div>
         </div>
      </div>

      <div class="PREgridContentArea">
         ${PREcommandGridHeaderHtml()}
         <div class="PREgridTarget"></div>
      </div>
   </div>`;
}
