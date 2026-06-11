let CORautoUpgradeInterval;

function CORautoUpgradeWarning() {
   clearInterval(CORautoUpgradeInterval)
   let autoUpgrade = MODELdata?.CORautoUpgrade?.data;
   if(!CORupgradeAvailable() || !CORisService()) { return CORremoveBanner(); }
   if(CORautoUpgradeCheck()) { return CORautoUpgradeCountdown(autoUpgrade.warning); }
   CORremoveBanner();
}

function CORautoUpgradeCheck() {
   let autoUpgrade = MODELdata?.CORautoUpgrade?.data;
   if(!autoUpgrade?.enabled) { return false; }
   if(autoUpgrade?.day === 'Any day') { return true; }
   const daysOfWeek = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"];
   const currentDay = daysOfWeek[new Date().getDay()];
   return (autoUpgrade?.day === currentDay);
}

function CORautoUpgradeBannerText(text) {
   const banner = document.querySelector('.CORwarningBanner');
   if(banner) { banner.innerHTML = text; }
}

function CORautoUpgradeCountdown(timeStr) {
   if(!timeStr) { return; }
   try {
      const [minutes, seconds] = timeStr.split(':').map(Number);
      let totalSeconds         = minutes * 60 + seconds;
      CORautoUpgradeInterval   = setInterval(function() {
         const minutesRemaining = Math.floor(totalSeconds / 60);
         const secondsRemaining = totalSeconds % 60;
         const timeRemaining    = `${String(minutesRemaining).padStart(2, '0')}:${String(secondsRemaining).padStart(2, '0')}`;
         CORautoUpgradecreateBanner();
         CORautoUpgradeBannerText(`Warning - IguanaX will automatically upgrade in ${timeRemaining}`);
         if(totalSeconds === 0) {
            clearInterval(CORautoUpgradeInterval);
            return CORautoUpgradeNow();
         }
         totalSeconds--;
      }, 1000);
   } catch(error) { console.error('Invalid time format. Please use mm:ss.'); }
}

function CORautoUpgradecreateBanner() {
   if(document.querySelector('.CORwarningBanner')) { return; }
   const warningBanner = document.createElement('div');
   warningBanner.className = 'CORwarningBanner';
   document.body.appendChild(warningBanner);
}

function CORremoveBanner() { document.querySelector('.CORwarningBanner')?.remove(); }

function CORautoUpgradeNow() {
   CORremoveBanner();
   POPpopoutRemoveAll();
   POPpopoutOpen({title : `Updating IguanaX`, width :"550px", content: CORautoUpdateHtml()});
   CORstopAllComponents(CORdownloadUpdate);
}

function CORstopAllComponents(callback) {
   APIcall("component/stop_all", {}, function(R) { if(callback) { callback(); } })
}

function CORdownloadUpdate() {
   APIcall("upgrade/download", {version: CORreleaseVersion()}, function(R) {
      document.querySelector('.CORdownloadingContainer')?.remove();
      if(!R.success) { return CORfeedback(R.error); }
      const modal = document.querySelector('.CORupdateModal');
      if(modal) {
         const newElement     = document.createElement("div");
         newElement.className = "STAdownloadComplete STAloading";
         newElement.innerHTML = "<br>Downloaded! Starting your update ...";
         modal.appendChild(newElement);
      }
      setTimeout(function() {
         APIcall("upgrade/replace", {version: CORreleaseVersion()}, function(R) {
            // it does not reach here because iguana shuts down during the restart
         })
      }, 500);
   });
}

function CORfeedback(text) {
   const feedback = document.querySelector('.CORfeedback');
   if(feedback) { feedback.innerHTML = text; }
}

function CORautoUpdateHtml() {
   return /*html*/`
<div class="CORupdateModal">
   To update, Iguana will start by shutting down all running components and downloading the latest version. After that, it will log you out and restart the service.
   <br>
   <div class="CORdownloadingContainer">
      <div class="CORdownloadingDiv">
      <br>
         <div class="CORupdateRow">
            <div class="CORspinner"></div>
            <div class="CORloading">Downloading Version ${CORreleaseVersion()} ...</div>
         </div>
      </div>
   </div>
   <div class="CORfeedback"></div>
</div>`;
}