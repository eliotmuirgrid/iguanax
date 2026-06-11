/* ==========================================================================
   PRE Screens
   - Shared screen entry points used by both Command Center and Authorized Peers
========================================================================== */

function PREaccessModelInit() {
   MODELpollerAdd('PREaccessModel', 'PREaccessList', 5, 'iguanas/list', {});
   MODELviewAdd('PREaccessModel', 'PREaccessRender');
}

function PREaddModelInit() {
   MODELpollerAdd('PREaddModel', 'PREaddData', 60, 'iguanas/me', {});
   MODELviewAdd('PREaddModel', 'PREaddRender');
   MODELforcePoll('PREaddModel');
}

function PREaccessScreen() {
   if (!document.querySelector('.PREaccessModal')) PAGEremovePopouts();
   PREaccessModelInit();
   MODELforcePoll('PREaccessModel');
}

function PREaddPeerScreen() {
   PREaddModelInit();
}
