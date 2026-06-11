function CYTOtagColorClass(tag) {
   const color = HASHTAGgetHashtagColor(tag);
   const colorClasses = {
      'rgba(106,27,154,1.0)': 'CYTOtagColor0',
      'rgba(40,53,147,1.0)' : 'CYTOtagColor1',
      'rgba(21,101,192,1.0)': 'CYTOtagColor2',
      'rgba(0,105,92,1.0)'  : 'CYTOtagColor3',
      'rgba(46,125,50,1.0)' : 'CYTOtagColor4',
      'rgba(0,77,64,1.0)'   : 'CYTOtagColor5',
      'rgba(74,20,140,1.0)' : 'CYTOtagColor6',
      'rgba(55,71,79,1.0)'  : 'CYTOtagColor7',
      'rgba(198,40,40,1.0)' : 'CYTOtagColorError',
      'rgba(230,74,25,1.0)' : 'CYTOtagColorWarning',
      'rgba(2,119,189,1.0)' : 'CYTOtagColorInfo'
   };
   return colorClasses[color] || 'CYTOtagColor0';
}

function CYTOtoolbarContainerHtml() {
   const tags  = DASHtagData();
   const pills = tags.map(tag => {
      const colorClass = CYTOtagColorClass(tag);
      return `<a class="CYTOtagPill ${colorClass}" data-tag="${tag}" title="Filter by ${tag}">${tag}</a>`;
   });
   const list = tags.length > 0 ? `<div class="CYTOtagsList">${pills.join('')}</div>` : '';
   return `<div class="CYTOtoolbar">
  ${list}
</div>`;
}

function CYTOgetRenderedTags() { return [...document.querySelectorAll('.CYTOtagPill')].map(p => p.dataset.tag); }

function CYTOeditButtonHtml() {
   return '<a class="CYTOeditFloating BUTTONaction BUTTONstandard" title="Turn on canvas editing to move nodes">Edit Layout</a>';
}

function CYTOautoLayoutButtonHtml() {
   return '<a class="CYTOautoLayoutFloating BUTTONstandard BUTTONdefault" title="Run Auto Layout" hidden>Auto Layout</a>';
}

function CYTOtagsManageButtonHtml() {
   return '<a class="CYTOtagsManageFloating BUTTONstandard BUTTONdefault" title="Manage Tags" hidden>Manage Tags</a>';
}

function CYTOfloatingControlsHtml() {
   return '<div class="CYTOfloatingControls"></div>';
}

function CYTOeditButtonClick(e) {
   e.preventDefault();
   CYTOtoggleEditMode();
}

function CYTOautoLayoutButtonClick(e) {
   e.preventDefault();
   CYTOconfirmAutoLayout();
}

function CYTOautoLayoutConfirmHtml() {
   return /*html*/`
   <div class="CYTOautoLayoutModalContent">
      <div class="CYTOautoLayoutModalIntro">
         <div class="CYTOautoLayoutModalQuestion">Run Auto Layout on this canvas?</div>
         <div class="CYTOautoLayoutModalDescription">
            This reorganizes the current component layout.
         </div>
      </div>
      <div class="CYTOautoLayoutModalNotes">
         <div class="CYTOautoLayoutModalNote">You can still move components around afterward.</div>
         <div class="CYTOautoLayoutModalNote CYTOautoLayoutModalHint">
            Runs using the Auto Layout Utility component's <code>user</code> custom field. Multiple users can be comma-separated.
         </div>
      </div>
      <div class="BUTTONgroup CYTOautoLayoutModalButtons">
         <div class="POPcancelAction BUTTONstandard BUTTONdefault">Cancel</div>
         <div class="BUTTONstandard BUTTONaction CYTOautoLayoutConfirmButton">Run Auto Layout</div>
      </div>
   </div>`;
}

function CYTOtagsManageButtonClick(e) {
   e.preventDefault();
   DTAGrender();
}

function CYTOensureFloatingControls() {
   const container = document.querySelector('.DASHdashboard');
   if(!container) { return null; }
   let controls = container.querySelector('.CYTOfloatingControls');
   if(!controls) {
      container.insertAdjacentHTML('beforeend', CYTOfloatingControlsHtml());
      controls = container.querySelector('.CYTOfloatingControls');
   }
   return controls;
}

function CYTOensureEditButton() {
   const controls = CYTOensureFloatingControls();
   if(!controls) { return null; }
   let button = controls.querySelector('.CYTOeditFloating');
   if(!button) {
      controls.insertAdjacentHTML('beforeend', CYTOeditButtonHtml());
      button = controls.querySelector('.CYTOeditFloating');
   }
   if(button) {
      button.removeEventListener('click', CYTOeditButtonClick);
      button.addEventListener('click', CYTOeditButtonClick);
   }
   return button;
}

function CYTOensureAutoLayoutButton() {
   const controls = CYTOensureFloatingControls();
   if(!controls) { return null; }
   let button = controls.querySelector('.CYTOautoLayoutFloating');
   if(!button) {
      controls.insertAdjacentHTML('beforeend', CYTOautoLayoutButtonHtml());
      button = controls.querySelector('.CYTOautoLayoutFloating');
   }
   if(button) {
      button.removeEventListener('click', CYTOautoLayoutButtonClick);
      button.addEventListener('click', CYTOautoLayoutButtonClick);
   }
   return button;
}

function CYTOensureTagsManageButton() {
   const controls = CYTOensureFloatingControls();
   if(!controls) { return null; }
   let button = controls.querySelector('.CYTOtagsManageFloating');
   if(!button) {
      controls.insertAdjacentHTML('beforeend', CYTOtagsManageButtonHtml());
      button = controls.querySelector('.CYTOtagsManageFloating');
   }
   if(button) {
      button.removeEventListener('click', CYTOtagsManageButtonClick);
      button.addEventListener('click', CYTOtagsManageButtonClick);
   }
   return button;
}

function CYTOtoolbarRender() {
   const container = document.querySelector('.CYTOcontainer');
   if(!container) { return; }
   const tags = DASHtagData();
   const div  = container.querySelector('.CYTOtoolbar');
   if(CYTOshouldShowWelcome()) { CYTOremoveToolbar(); return; }
   if(tags.join(',') === CYTOgetRenderedTags().join(',') && div) {
      CYTOtoolbarSyncState();
      return;
   }
   CYTOremoveToolbar();
   container.insertAdjacentHTML('afterbegin', CYTOtoolbarContainerHtml());
   CYTOtoolbarListeners();
   CYTOtagsUpdateSelected();
   CYTOtoolbarSyncState();
}

function CYTOconfirmAutoLayout() {
   if(!AULhasAutoLayoutUtility()) { return AULrunAutoLayout(); }
   const Card = POPpopoutOpen({
      title    : 'Run Auto Layout',
      content  : CYTOautoLayoutConfirmHtml(),
      className: 'CYTOautoLayoutModal',
      maxWidth : '460px'
   });
   const Button = Card.element.querySelector('.CYTOautoLayoutConfirmButton');
   Button.addEventListener('click', function() {
      AULrunAutoLayout();
      POPpopoutClose(Card);
   });
}

function CYTOtoolbarListeners() {
   const toolbar = document.querySelector('.CYTOtoolbar');
   if(!toolbar) { return; }

   toolbar.removeEventListener('click', CYTOtoolbarClick);
   toolbar.addEventListener('click', CYTOtoolbarClick);
}

function CYTOtoolbarClick(e) {
   const tagPill = e.target.closest('.CYTOtagPill');
   if(tagPill) { return CYTOtagsFilterClick(tagPill, e); }
}

function CYTOsetEditToggleContent(editButton, editMode) {
   editButton.textContent = editMode ? 'Finish Editing' : 'Edit Layout';
   editButton.classList.remove('BUTTONdefault');
   editButton.classList.add('BUTTONaction');
}

function CYTOtoolbarSyncState() {
   const tagsManageButton = CYTOensureTagsManageButton();
   const autoLayoutButton = CYTOensureAutoLayoutButton();
   const editButton       = CYTOensureEditButton();
   const editMode         = CYTOisEditMode();
   const hasTags          = (DASHtagData() || []).length > 0;

   if(autoLayoutButton) { autoLayoutButton.hidden = !editMode; }
   if(tagsManageButton) {
      tagsManageButton.hidden      = !editMode;
      tagsManageButton.textContent = hasTags ? 'Manage Tags' : 'Add Tags';
      tagsManageButton.title       = hasTags ? 'Manage Tags' : 'Add Tags';
   }

   if(editButton) {
      CYTOsetEditToggleContent(editButton, editMode);
      editButton.title = editMode ? 'Exit edit mode' : 'Turn on canvas editing to move nodes';
   }
}

function CYTOtagsFilterClick(pill, e) {
   e.preventDefault();
   const tagValue       = pill.dataset.tag;
   const existingFilter = PAGEhashParameter('filter') || '';
   const activeWords    = new Set(existingFilter.toLowerCase().split(/\s+/).filter(w => w));
   const tagLower       = tagValue.toLowerCase();

   if(activeWords.has(tagLower)) {
      activeWords.delete(tagLower);
      const newFilter = [...activeWords].join(' ');
      PAGEsetHashParameter('filter', newFilter);
      if(!newFilter && PAGEhashParameter('whole_word')) {
         PAGEsetHashParameter('whole_word');
      }
      return;
   }

   if(e.shiftKey && existingFilter) { PAGEsetHashParameter('filter', `${existingFilter  } ${  tagValue}`); }
   else                             { PAGEsetHashParameter('filter', tagValue); }
   if(PAGEhashParameter('whole_word') !== 'true') { PAGEsetHashParameter('whole_word', true); }
}

function CYTOtagsUpdateSelected() {
   const currentFilter = (PAGEhashParameter('filter') || '').toLowerCase();
   const activeWords   = new Set(currentFilter.split(/\s+/).filter(w => w));
   document.querySelectorAll('.CYTOtagPill').forEach(pill => {
      const tag        = (pill.dataset.tag || '').toLowerCase();
      const isSelected = activeWords.has(tag);
      pill.classList.toggle('CYTOtagSelected', isSelected);
      pill.title = isSelected ? `Remove ${pill.dataset.tag} filter` : `Filter by ${pill.dataset.tag}`;
   });
}

function CYTOremoveToolbar() {
   const div  = document.querySelector('.CYTOtoolbar');
   if(div) { div.remove(); }
   document.querySelectorAll('.CYTOeditFloating, .CYTOautoLayoutFloating, .CYTOtagsManageFloating').forEach(button => button.remove());
   document.querySelectorAll('.CYTOfloatingControls').forEach(controls => controls.remove());
}
