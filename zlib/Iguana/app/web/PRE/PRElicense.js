/* ==========================================================================
   PRE License
   - Shared command/authorized-peer route checks
========================================================================== */

function PRElicenseBoolParse(Value) {
   if (Value === true || Value === false) return Value;
   if (Value === 1 || Value === 0) return Value === 1;
   if (typeof Value === "string") {
      var normalized = Value.trim().toLowerCase();
      if (normalized === "1" || normalized === "true") return true;
      if (normalized === "0" || normalized === "false") return false;
   }
   return null;
}

function PRElicenseStateFromPayload(Data) {
   if (typeof CORglobalDashboardLicenseStateFromPayload === "function") {
      return CORglobalDashboardLicenseStateFromPayload(Data);
   }

   var license = (Data && Data.license) || {};
   var state = PRElicenseBoolParse(license.federated_dashboard);
   if (state === true || state === false) return state;
   state = PRElicenseBoolParse(license.global_dashboard);
   if (state === true || state === false) return state;

   var status = String(license.license_status || "").toLowerCase();
   if (status === "no license found" || status === "license expired") return false;
   return null;
}

function PRElicenseDashboardState() {
   if (PREpeerGridLicenseDataReady()) {
      var state = PRElicenseStateFromPayload(PREpeerGridLicenseDataGet());
      if (state === true || state === false) {
         CORglobalDashboardLicenseCacheWrite(state);
         return state;
      }
   }
   return CORglobalDashboardLicenseState();
}

function PREcommandRouteAllowed() { return PRElicenseDashboardState() === true; }
function PREauthorizedPeersRouteAllowed() { return PRElicenseDashboardState() !== true; }
