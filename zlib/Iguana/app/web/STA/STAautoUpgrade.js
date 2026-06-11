function STAautoSave(Message) {
   const data = {time: STAautoGetEpochTime(), day: STAautoGetDay(), enabled: STAautoEnabled()};
   APIcall('upgrade/set_auto', data, function(R) {
      if(!R.success) { return STAautoSetFeedback(R.error); }
      if(Message) { GINaddAndCommit({message: Message, settings: true}); }
      MODELforcePoll('STAmodel');
      MODELforcePoll('CORmodel');
      MODELforcePoll('CORmodelSlow');
   });
}

function STAdata() {
   let data = {day: MODELdata?.STAautoUpgradeTime?.data?.day, enabled: MODELdata?.STAautoUpgradeTime?.data?.enabled,};
   return { ...STAautoEpochToTime(MODELdata?.STAautoUpgradeTime?.data?.time), ...data };
}

function STAupdateAutoUpgrade() {
   const selector = document.querySelector('.STAautoUpgrade');
   if(!selector) { return; }
   const data = STAdata();
   let linkHtml    = data.enabled ? `- [<a class="STAsetAutoUpgrade SETlink">Change Time</a>]` : `- [<a class="STAsetAutoUpgrade SETlink">Setup Time</a>]`;
   let Permission = STAupgradePermission();
   if(!Permission) linkHtml = "";
   const displayText = data.enabled ? `${data.day}: ${data.hour}:${data.min} ${data.period} ${linkHtml}` : `Not Configured ${linkHtml}`;
   if(selector.innerHTML !== displayText) { selector.innerHTML = displayText; }
}

function STAautoUpgradeModal() {
   POPpopoutOpen({title : `Setup Automatic Updates`, width :"550px", content: STAautoUpgradeHtml()});
   STAUautoListeners();
   STAautoPopulateDays();
   STAautoPopulateHours();
   STAautoPopulateMinutes();
   STAautoGet();
}

function STAUautoListeners() {
   document.querySelector('.STAautoCancel')?.addEventListener('click', function() { POPpopoutRemoveAll(); });
   document.querySelector('.STAautoSave')?.addEventListener('click', function(){ STAautoSave(GINsetAutoUpdate()); });
   document.getElementById('STAenabled')?.addEventListener('click', function(){ 
      let Message = STAautoEnabled() ? GINsetAutoUpdateOn() : GINsetAutoUpdateOff();
      STAautoSave(Message); 
   });
}

function STAautoGet() {
   const converted = STAautoEpochToTime(MODELdata?.STAautoUpgradeTime?.data?.time);
   const day       = MODELdata?.STAautoUpgradeTime?.data?.day;
   const enabled   = MODELdata?.STAautoUpgradeTime?.data?.enabled;
   if(!converted) { return; }
   STAselectOption(document.querySelector('.STAday'), day);
   STAselectOption(document.querySelector('.STAhour'), converted.hour);
   STAselectOption(document.querySelector('.STAminute'), converted.min);
   STAselectOption(document.querySelector('.STAperiod'), converted.period);
   document.getElementById('STAenabled').checked = enabled || false;
}

function STAselectOption(selectElement, optionValueToSelect) {
   if(!selectElement || !optionValueToSelect) { return; }
   for(let i = 0; i < selectElement.options.length; i++) {
      if(selectElement.options[i].value === optionValueToSelect) {
         selectElement.selectedIndex = i;
         break;
      }
   }
}

function STAautoPopulateDays() {
   const daySelect = document.querySelector('.STAday');
   const days      = ['Any day', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday', 'Sunday'];
   for(let i = 0; i < days.length; i++) { daySelect?.add(new Option(days[i], days[i])); }
}

function STAautoPopulateHours() {
   const hourSelect = document.querySelector('.STAhour');
   for(let i = 1; i <= 12; i++) { hourSelect?.add(new Option(i.toString().padStart(2, '0'), i)); }
}

function STAautoPopulateMinutes() {
   const minuteSelect = document.querySelector('.STAminute');
   for(let i = 0; i <= 45; i += 15) { minuteSelect?.add(new Option(i.toString().padStart(2, '0'), i)); }
}

function STAautoGetDay()  { return document.querySelector('.STAday')?.value; }
function STAautoEnabled() { return document.getElementById('STAenabled')?.checked; }

function STAautoGetEpochTime() {
   const hour   = document.querySelector('.STAhour').value;
   const min    = document.querySelector('.STAminute').value;
   const period = document.querySelector('.STAperiod').value;
   let hour24;
   if(hour === '12') { hour24 = period === 'AM' ? '00' : '12'; }
   else              { hour24 = period === 'PM' ? parseInt(hour) + 12 : hour; }
   const time   = new Date();
   time.setHours(hour24, min, 0, 0);
   return time.getTime().toString();
}

function STAautoEpochToTime(epochTime) {
   if(!epochTime) { return; }
   const date   = new Date(parseInt(epochTime));
   let hours    = date.getHours();
   let minutes  = date.getMinutes();
   const period = hours >= 12 ? 'PM' : 'AM';
   hours        = (hours % 12) || 12;
   hours        = hours.toString().padStart(2, '0');
   minutes      = minutes.toString().padStart(2, '0');
   return {hour: `${hours}`, min: `${minutes}`, period: `${period}`}; // want strings
}

function STAautoSetFeedback(feedback) {
   const selector = document.querySelector('.STAautoFeedback');
   if(selector) { selector.innerHTML = feedback; }
}

function STAautoUpgradeHtml() {
   return `
<div class="STAautoInfo">
  Select a time when you would like Iguana to automatically update:
</div>
<br>
<div class="STAtime-picker">
  <select class="STAday"></select>

  <select class="STAhour"></select>

  <select class="STAminute"></select>

  <select class="STAperiod">
    <option value="AM">AM</option>
    <option value="PM">PM</option>
  </select>
  
  <div class="STAswitch">
     <input type="checkbox" id="STAenabled">
     <label for="STAenabled"></label>
  </div>
</div>
<br>
<div class="STAautoFeedback"></div>
<br>
<div class="BUTTONgroup STAautoButtonGroup">
  <div class="BUTTONaction BUTTONstandard STAautoSave">SET TIME</div>
  <div class="BUTTONcancel BUTTONstandard STAautoCancel">CLOSE</div>
</div>`;
}
