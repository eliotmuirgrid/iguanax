function CYTOhideMenu(selector) {
   const menu = document.querySelector(selector);
   if(menu) { menu.classList.remove('CYTOmenuVisible'); }
}

function CYTOhideSelectionMenu()  { CYTOhideMenu('#cyto-selection-menu'); }
function CYTOhideNodeMenu()       { CYTOhideSelectionMenu(); }
function CYTOhideEdgeMenu()       { CYTOhideMenu('#cyto-edge-menu'); }
function CYTOhideBackgroundMenu() { CYTOhideMenu('#cyto-bg-menu'); }

function CYTOgetOrCreateMenu(id) {
   let menu = document.getElementById(id);
   if(!menu) {
      menu           = document.createElement('div');
      menu.id        = id;
      menu.className = 'CYTOmenu';
      document.body.appendChild(menu);
   }
   return menu;
}

function CYTOpositionMenu(menu, event, onHide) {
   menu.style.left    = `${event.pageX  }px`;
   menu.style.top     = `${event.pageY  }px`;
   menu.classList.add('CYTOmenuVisible');
   document.addEventListener('click', onHide, {once: true});
}

function CYTObuildMenuItems(menu, items) {
   menu.innerHTML = items.map(i => {
      if(i.type === 'header') { return `<div class="CYTOmenuHeader">${i.label}</div>`; }
      if(i.type === 'divider') { return '<div class="CYTOmenuDivider"></div>'; }
      const title = i.disabled && i.disabledTitle ? ` title="${i.disabledTitle}"` : '';
      const extraClasses = ['CYTOmenuItem'];
      if(i.disabled) { extraClasses.push('CYTOmenuItemDisabled'); }
      if(i.className) { extraClasses.push(i.className); }
      return `<div class="${extraClasses.join(' ')}" data-action="${i.action}"${title}>${i.label}</div>`;
   }).join('');
   menu.querySelectorAll('.CYTOmenuItem').forEach(el => {
      const item = items.find(i => i.action === el.dataset.action);
      if(item && !item.disabled) { el.onclick = item.onClick; }
   });
}

function CYTOselectionMenuHide() {
   CYTOhideSelectionMenu();
   CYTOhideBackgroundMenu();
}

function CYTOgetComponentLabel(componentId) {
   const node  = CYTOgetNodeById(componentId);
   const label = node && node.length ? (node.data('label') || '').trim() : '';
   return label || componentId;
}

function CYTOselectionMenuHeader(componentIds) {
   if(componentIds.length === 1) { return CYTOgetComponentLabel(componentIds[0]); }
   return `${componentIds.length} Selected`;
}

function CYTOselectionOperationMenuItems(componentIds) {
   return [{
      action : 'startMarked',
      label  : 'Start',
      disabled: !CYTOcheckBulkPermission('start', componentIds),
      disabledTitle: 'You do not have permission to start one or more of the selected components.',
      onClick: () => { CYTOselectionMenuHide(); CYTObulkAction.start(componentIds); }
   }, {
      action : 'stopMarked',
      label  : 'Stop',
      disabled: !CYTOcheckBulkPermission('stop', componentIds),
      disabledTitle: 'You do not have permission to stop one or more of the selected components.',
      onClick: () => { CYTOselectionMenuHide(); CYTObulkAction.stop(componentIds); }
   }, {
      action : 'clearQueueMarked',
      label  : 'Clear Queue',
      disabled: !CYTOcheckBulkPermission('clear_queue', componentIds),
      disabledTitle: 'You do not have permission to clear the queue for one or more of the selected components.',
      onClick: () => { CYTOselectionMenuHide(); CYTObulkAction.clearQueue(componentIds); }
   }, {
      action : 'clearErrorsMarked',
      label  : 'Clear Errors',
      disabled: !CYTOcheckBulkPermission('clear_errors', componentIds),
      disabledTitle: 'You do not have permission to clear errors for one or more of the selected components.',
      onClick: () => { CYTOselectionMenuHide(); CYTObulkAction.clearErrors(componentIds); }
   }];
}

function CYTOselectionStructureMenuItems(componentIds) {
   const selectionCount = componentIds.length;
   const singleId       = selectionCount === 1 ? componentIds[0] : '';
   const canRename      = !!singleId && !!DASHuserPermissionAll()[singleId]?.edit_name;
   const items          = [];

   if(CYTOisEditMode()) {
      items.push({
         action : 'connectMarked',
         label  : 'Connect to...',
         disabled: !CYTOcheckBulkPermission('edit_connections', componentIds),
         disabledTitle: 'You do not have permission to connect one or more of the selected components.',
         onClick: () => { CYTOselectionMenuHide(); CYTObulkAction.connect(componentIds); }
      });
   }

   if(selectionCount === 1) {
      items.push({
         action : 'renameMarked',
         label  : 'Rename',
         disabled: !canRename,
         disabledTitle: 'You do not have permission to rename this component.',
         onClick: () => { CYTOselectionMenuHide(); CYTOrenameDialog(singleId); }
      });
   }

   items.push({
      action : 'editTagsMarked',
      label  : 'Edit Tags',
      disabled: !CYTOcheckBulkPermission('edit_tags', componentIds),
      disabledTitle: 'You do not have permission to edit tags for one or more of the selected components.',
      onClick: () => { CYTOselectionMenuHide(); CYTObulkAction.editTags(componentIds); }
   }, {
      action : 'deleteMarked',
      label  : 'Delete',
      className: 'CYTOmenuItemDanger',
      disabled: !CYTOcheckBulkPermission('delete_component', componentIds),
      disabledTitle: 'You do not have permission to delete one or more of the selected components.',
      onClick: () => { CYTOselectionMenuHide(); CYTObulkAction.delete(componentIds); }
   });
   return items;
}

function CYTOselectionMenuItems(componentIds) {
   const items = [{
      type  : 'header',
      label : CYTOselectionMenuHeader(componentIds)
   }];
   const operationItems = CYTOselectionOperationMenuItems(componentIds);
   const structureItems = CYTOselectionStructureMenuItems(componentIds);

   if(CYTOisEditMode() && structureItems[0]?.action === 'connectMarked') {
      const connectItem = structureItems.shift();
      return items.concat([connectItem], [{type: 'divider'}], operationItems, [{type: 'divider'}], structureItems);
   }

   return items.concat(operationItems, [{type: 'divider'}], structureItems);
}

function CYTOshowSelectionMenu(event, componentIds) {
   if(!componentIds || componentIds.length === 0) { return; }
   const menu = CYTOgetOrCreateMenu('cyto-selection-menu');
   CYTOhideBackgroundMenu();
   CYTObuildMenuItems(menu, CYTOselectionMenuItems(componentIds));
   CYTOpositionMenu(menu, event, CYTOhideSelectionMenu);
}

function CYTOresolveSelectionMenuIds(nodeId) {
   const markedNodeIds = CYTOgetMarkedNodeIds();
   if(markedNodeIds.length > 1) { return markedNodeIds; }
   if(markedNodeIds.length === 1 && markedNodeIds[0] === nodeId) { return markedNodeIds; }
   return [nodeId];
}

function CYTOshowNodeMenu(event, nodeId) {
   if(CYTOinstance?.connectMode) { return; }
   CYTOshowSelectionMenu(event, CYTOresolveSelectionMenuIds(nodeId));
}

function CYTOshowEdgeMenu(event, edgeId, sourceId, targetId) {
   if(!CYTOisEditMode()) { return; }
   const menu       = CYTOgetOrCreateMenu('cyto-edge-menu');
   const sourcePerm = DASHuserPermissionAll()[sourceId] || {};
   CYTOhideSelectionMenu();
   CYTOhideBackgroundMenu();
   CYTObuildMenuItems(menu, [{
      type : 'header',
      label: `${CYTOgetComponentLabel(sourceId)} → ${CYTOgetComponentLabel(targetId)}`
   }, {
      action: 'remove',
      label: 'Remove Connection',
      className: 'CYTOmenuItemDanger',
      disabled: !sourcePerm.edit_connections,
      disabledTitle: 'You do not have permission to remove this connection.',
      onClick: () => { CYTOremoveConnection(targetId, sourceId); CYTOhideEdgeMenu(); }
   }]);
   CYTOpositionMenu(menu, event, CYTOhideEdgeMenu);
}

function CYTObackgroundEditModeMenuItem() {
   const editMode = CYTOisEditMode();
   return {
      action : 'toggleEditMode',
      label  : editMode ? 'Finish Editing' : 'Edit Layout',
      onClick: () => {
         CYTOtoggleEditMode();
         CYTOhideBackgroundMenu();
      }
   };
}

function CYTOshowBackgroundMenu(event) {
   if(CYTOinstance?.connectMode) { return; }
   const menu        = CYTOgetOrCreateMenu('cyto-bg-menu');
   const markedNodes = CYTOgetMarkedNodeIds();
   const editMode    = CYTOisEditMode();
   if(markedNodes.length > 0) { return CYTOshowSelectionMenu(event, markedNodes); }
   const items = [{
      action : 'reset',
      label  : 'Reset View',
      onClick: () => { CYTOresetView(); CYTOhideBackgroundMenu(); }
   }, {
      type: 'divider'
   }];
   if(editMode) {
      items.push({
         action : 'manageTags',
         label  : 'Manage Tags',
         onClick: () => { DTAGrender(); CYTOhideBackgroundMenu(); }
      }, {
         action : 'autoLayout',
         label  : 'Auto Layout',
         onClick: () => { CYTOconfirmAutoLayout(); CYTOhideBackgroundMenu(); }
      }, {
         action : 'rawCoordinates',
         label  : 'Edit Exact Coordinates',
         onClick: () => { PINSrender(); CYTOhideBackgroundMenu(); }
      }, {
         type: 'divider'
      }, CYTObackgroundEditModeMenuItem());
   } else {
      items.push(CYTObackgroundEditModeMenuItem());
   }
   CYTObuildMenuItems(menu, items);
   CYTOpositionMenu(menu, event, CYTOhideBackgroundMenu);
}

function CYTOinputDialog({title, label, confirmText, className, initialValue, onSubmit}) {
   const content = `
      <div class="FORMlabel">${label}</div>
      <input class="FORMinput CYTOdialogInput" type="text">
      <div class="BUTTONgroup CONFIRMbuttonGroup">
         <div class="POPcancelAction BUTTONstandard BUTTONdefault">CANCEL</div>
         <div class="BUTTONstandard BUTTONaction CYTOdialogConfirm">${confirmText}</div>
      </div>
   `;
   const card    = POPpopoutOpen({title, content, className, maxWidth: '500px'});
   const input   = card.element.querySelector('.CYTOdialogInput');
   const confirm = card.element.querySelector('.CYTOdialogConfirm');
   if(!input || !confirm) { return; }

   input.value = initialValue;
   input.focus();
   input.select();

   const submit = () => {
      const result = onSubmit(input.value.trim(), card);
      if(result !== false) { POPpopoutClose(card); }
   };

   confirm.addEventListener('click', submit);
   input.addEventListener('keydown', e => { if(e.key === 'Enter') { submit(); } });
}

function CYTOrenameDialog(nodeId) {
   const node = CYTOgetNodeById(nodeId);
   if(!node || node.length === 0) { return; }
   const oldName = (node.data('label') || '').trim();
   CYTOinputDialog({
      title        : 'Rename Component',
      label        : 'New component name',
      confirmText  : 'RENAME',
      className    : 'CYTOrenameModal',
      initialValue : oldName,
      onSubmit     : (newName) => {
         if(!newName) { SNCKsnackbar('error', 'Name cannot be empty.'); return false; }
         if(newName !== oldName) { CYTOrename(nodeId, oldName, newName); }
      }
   });
}

function CYTOeditTagsDialog(nodeId) {
   const node = CYTOgetNodeById(nodeId);
   if(!node || node.length === 0) { return; }
   const componentName = (node.data('label') || '').trim();
   const oldValue      = (node.data('tags') || []).join(' ');
   CYTOinputDialog({
      title        : 'Edit Tags',
      label        : 'Edit component tags',
      confirmText  : 'SAVE',
      className    : 'CYTOtagsModal',
      initialValue : oldValue,
      onSubmit     : (newValue) => {
         if(newValue === oldValue) { return; }
         CYTOsetTags(nodeId, componentName, newValue ? newValue.split(/\s+/) : []);
      }
   });
}

function CYTOtagSetDialog(nodeId) { CYTOeditTagsDialog(nodeId); }
