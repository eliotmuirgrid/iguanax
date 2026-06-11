function DETannotationRequestImpl() {
   const Data = {use_default: DETuseDefault(), vmd: DETgetVmd(), full_argument_requests: [], data: DETrawData().content};
   APIcall('log/viewer/annotation', Data, function(R) {
      DETrenderSegments();
      const ErrorDiv = document.querySelector('.DETsegmentError');
      if(!R.success) {
         ErrorDiv.innerText = `Error loading segment view: ${R.error}`;
         return DETsetupMessageToggle(true);
      }
      DETrenderTree(R.data?.FullArgumentResponses);
   });
}

function DETannotationRequest() {
   let div = document.querySelector(".DETsegmentToggleContainer");
   if(div) return;
   DETannotationRequestImpl();
}

function DETsegmentSettings() {
   const guid = DETcomponentId();
   if(!guid) return {use_default: true, vmd: ""};

   let Map = DETgetSegmentSettingsMap();
   if(!(guid in Map)) {
      Map[guid] = {use_default: "true", vmd: ""};
      DETsaveSegmentSettingsMap(Map);
   }

   const State = Map[guid] || {};
   return {
      use_default: State.use_default !== "false",
      vmd: State.vmd || ""
   };
}

function DETrenderSegments() {
   const Settings = DETsegmentSettings();
   const currentVmd = Settings.vmd;
   const useDefault = Settings.use_default;
   const rawChecked = document.getElementById('DETshowMessageCheckbox')?.checked;
   const messageDiv = document.querySelector('.DETmessageDiv');
   messageDiv.style.whiteSpace = 'nowrap';
   messageDiv.style.padding = '0';
   messageDiv.innerHTML = '';
   messageDiv.insertAdjacentHTML('beforeend', DETsegmentHtml(currentVmd, useDefault));
   document.querySelector('.DETbrowse').addEventListener('click', DETbrowse);
   document.querySelector('.DETvmdPath').addEventListener('change', DETrunAnnotation);
   DETsetupMessageToggle(rawChecked);
   DETdefaultVmdToggle();
   DETsegmentInfoToggle();
}

function DETrenderTree(data) {
   if(!data) return;
   // use jquery selector cause ITM relies on it
   const Root             = data[Object.keys(data)[0]];
   const containerWrapper = $('<div>', {class: 'DETcontainerWrapper'});
   const containerDiv     = $('<div>', {class: 'DETcontainerDiv'});
   const messageDiv       = $('.DETmessageDiv');
   messageDiv.append(containerWrapper);
   containerWrapper.append(containerDiv);
   ITMrenderTreeviewView(Root, containerDiv);
   const dropdown = containerWrapper.find('.emptyNodeDropdown');
   if (dropdown.length) {
      const container = $('.DETshowMessageContainer');
      dropdown.insertAfter(container); // Move the dropdown next to the VMD browser
   }
}

function DETbrowse() {
   let input = document.querySelector('.DETvmdPath');
   FILbrowser({
      value      : input.value ? input.value : DETcomponentData().source,
      new_folder : false,
      show_hidden: false,
      dir_only   : false,
      file_types : ["vmd"],
      callback   : function(T) {
         input.value = T;
         input.dispatchEvent(new Event('change'));
      }
   });
}

function DETsetupMessageToggle(forceCheck = false) {
   const checkbox   = document.getElementById('DETshowMessageCheckbox');
   const messageDiv = document.querySelector('.DETshowMessage');
   if(checkbox) {
      checkbox.addEventListener('change', function() {
         if(this.checked) {
            messageDiv.innerHTML     = DETmessageData()?.Chunks[0]?.Value;
            messageDiv.style.display = 'block';
         } else {
            messageDiv.style.display = 'none';
         }
      });
   }
   if(forceCheck) {
      checkbox.checked = true;
      checkbox.dispatchEvent(new Event('change'));
   }
}

function DETrunAnnotation(e) { 
   DETsetSegmentSettings(DETuseDefault(), e.target.value);
   if(e.target.value.trim()) DETannotationRequestImpl(); 
}

function DETgetVmd() {
   const path = document.querySelector('.DETvmdPath');
   if(path) return path.value;
   return DETsegmentSettings().vmd || '';
}

function DETuseDefault() {
   const checkbox = document.getElementById('DETdefaultVmdCheckbox');
   if(checkbox) return checkbox.checked;
   return DETsegmentSettings().use_default;
}

function DETsegmentHtml(vmd ='', useDefault = true) {
   return /*html*/`
   <div class="DETsegmentToggleContainer">
      <div class="BUTTONdefault BUTTONstandard DETdefaultVmdToggle">
         <input type="checkbox" id="DETdefaultVmdCheckbox" class="DETdefaultVmdCheckbox" ${useDefault ? 'checked' : ''}>
         <label for="DETdefaultVmdCheckbox">Use Default Vmd</label>
      </div>
      <div class="BUTTONdefault BUTTONstandard DETsegmentToggle">
         <input type="checkbox" id="DETsegmentToggleCheckbox" class="DETsegmentToggleCheckbox">
         <label for="DETsegmentToggleCheckbox">Show Info</label>
      </div>
   </div>
   <div class="DETsegmentInfo DETinfoHide">
      <p>After selecting a VMD file:</p>
      <ol>
         <li>The system will attempt to parse your message using the selected VMD</li>
         <li>If successful, a structured view of your message will be displayed below</li>
         <li>If there are any parsing errors, they will be shown in the error section below</li>
         <li>A default vmd is used at the start. You can use the "Browse" button to select a different VMD file</li>
      </ol>
   </div>
   <div class="DETvmdBrowser FORMinputGroup ${useDefault ? 'DETinfoHide' : ''}">
      <input class="FORMinput DETvmdPath" type="text" value="${vmd}" placeholder="Path to VMD">
      <div class="DETbrowse BUTTONstandard BUTTONcancel">BROWSE...</div>
   </div>
   <div class="DETsegmentError"></div>
   <div class="DETshowMessageContainer">
      <input type="checkbox" id="DETshowMessageCheckbox" class="DETshowMessageCheckbox">
      <label for="DETshowMessageCheckbox">Show Raw Message Preview</label>
      <div class="DETshowMessage" style="display: none;"></div>
   </div>
   <br>`;
}

function DETdefaultVmdToggle() {
   const browser  = document.querySelector('.DETvmdBrowser');
   const checkbox = document.getElementById('DETdefaultVmdCheckbox');

   function updateDisplay() {
      browser.classList.toggle('DETinfoHide', checkbox.checked);
      DETsetSegmentSettings(checkbox.checked, DETgetVmd());
      if(checkbox.checked || DETgetVmd().trim()) { DETannotationRequestImpl(); }
   }

   document.querySelector('.DETdefaultVmdToggle')?.addEventListener('click', function(e) {
      if(e.target === checkbox) {
         return updateDisplay();
      }
      checkbox.checked = !checkbox.checked;
      e.preventDefault();
      updateDisplay();
   });
}

function DETsegmentInfoToggle() {
   const infoEl   = document.querySelector('.DETsegmentInfo');
   const checkbox = document.querySelector('.DETsegmentToggleCheckbox');

   function updateDisplay() {
      infoEl.classList.toggle('DETinfoHide', !checkbox.checked);
   }

   document.querySelector('.DETsegmentToggle').addEventListener('click', function(e) {
      if(e.target === checkbox) {
         return updateDisplay();
      }
      checkbox.checked = !checkbox.checked;
      e.preventDefault();
      updateDisplay();
   });
}