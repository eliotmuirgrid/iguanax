function CYTOclearNodeContentOverlay(instance = CYTOinstance) {
   if(!instance) { return; }
   if(instance.nodeContentSyncRaf) { cancelAnimationFrame(instance.nodeContentSyncRaf); }
   if(instance.nodeContentSyncTimeout) { clearTimeout(instance.nodeContentSyncTimeout); }
   instance.nodeContentSyncRaf = null;
   instance.nodeContentSyncTimeout = null;
   if(instance.unpinnedHeaderEl) { instance.unpinnedHeaderEl.remove(); }
   if(instance.nodeContentOverlay) { instance.nodeContentOverlay.remove(); }
   instance.nodeContentOverlay = null;
   instance.nodeContentEls     = Object.create(null);
   instance.unpinnedHeaderEl   = null;
}

function CYTOensureNodeContentOverlay(instance = CYTOinstance) {
   if(!instance) { return null; }
   if(instance.nodeContentOverlay?.isConnected) { return instance.nodeContentOverlay; }
   const container = instance.container();
   if(!container) { return null; }
   let overlay = container.querySelector('.CYTOnodeContentOverlay');
   if(!overlay) {
      overlay = document.createElement('div');
      overlay.className = 'CYTOnodeContentOverlay';
      container.appendChild(overlay);
   }
   instance.nodeContentOverlay = overlay;
   if(!instance.nodeContentEls) { instance.nodeContentEls = Object.create(null); }
   return overlay;
}

function CYTOremoveNodeContent(instance, nodeId) {
   if(!instance?.nodeContentEls?.[nodeId]) { return; }
   instance.nodeContentEls[nodeId].remove();
   delete instance.nodeContentEls[nodeId];
}

function CYTOensureUnpinnedHeader(instance, overlay) {
   if(!instance || !overlay) { return null; }
   if(instance.unpinnedHeaderEl?.isConnected) { return instance.unpinnedHeaderEl; }
   const header = document.createElement('div');
   header.className = 'CYTOunpinnedHeader';
   header.innerHTML = '<span class="CYTOunpinnedHeaderLine CYTOunpinnedHeaderLineShort"></span><span class="CYTOunpinnedHeaderLabel">Staging Area (Drag below to save)</span><span class="CYTOunpinnedHeaderLine"></span>';
   overlay.appendChild(header);
   instance.unpinnedHeaderEl = header;
   return header;
}

function CYTOremoveUnpinnedHeader(instance = CYTOinstance) {
   if(!instance?.unpinnedHeaderEl) { return; }
   instance.unpinnedHeaderEl.remove();
   instance.unpinnedHeaderEl = null;
}

function CYTOnodesAboveOrigin(instance = CYTOinstance) {
   if(!instance) { return []; }
   return instance.nodes().toArray().filter(node => Number(node.position().y) < 0);
}

function CYTOhasNodesAboveOrigin(instance = CYTOinstance) {
   return CYTOnodesAboveOrigin(instance).length > 0;
}

function CYTOunpinnedLineFrame(instance, viewW) {
   const zoom = instance.zoom();
   const pan = instance.pan();
   const header = instance.unpinnedHeaderEl;
   const label = header?.querySelector('.CYTOunpinnedHeaderLabel');
   const shortLine = header?.querySelector('.CYTOunpinnedHeaderLineShort');
   const screenNudgeY = 12;
   const gap = 10;
   const labelWidth = Math.max(220, Number(label?.offsetWidth) || 0);
   const shortLineWidth = Math.max(220, Number(shortLine?.offsetWidth) || 0);
   const labelLeftOffset = (CYTOgridConst.nodeW / 2) + shortLineWidth + gap;
   const left = pan.x - (labelLeftOffset * zoom);
   const top = pan.y - (CYTOgridConst.nodeH / 2) * zoom - screenNudgeY;
   const width = Math.max(labelWidth + shortLineWidth + 1200, ((viewW - left) / zoom) + 320);
   return {
      left,
      top,
      width,
      zoom
   };
}

function CYTOsyncUnpinnedHeader(instance, overlay, viewW, viewH) {
   if(!CYTOisEditMode() || !CYTOhasNodesAboveOrigin(instance)) {
      CYTOremoveUnpinnedHeader(instance);
      return false;
   }

   const header = CYTOensureUnpinnedHeader(instance, overlay);
   if(!header) { return false; }
   const {left, top, width, zoom} = CYTOunpinnedLineFrame(instance, viewW);
   const renderedBounds = {
      left,
      top,
      right : left + width * zoom,
      bottom: top + 16 * zoom
   };

   header.style.left = `${left}px`;
   header.style.top = `${top}px`;
   header.style.width = `${width}px`;
   header.style.transform = `scale(${zoom})`;
   const isVisible = CYTOnodeBoundsVisible(renderedBounds, viewW, viewH);
   header.style.display = isVisible ? 'flex' : 'none';
   return isVisible;
}

function CYTOnodeContentHtml(node) {
   const name       = ESChtmlEscape(String(node.data('label') || node.id()));
   const isTemplate = !!node.data('isTemplate');
   const queueCount = Math.max(0, Number(node.data('queueCount') || 0));
   const errorCount = Math.max(0, Number(node.data('errorCount') || 0));
   const queueHtml  = queueCount > 0 ? `<span class="CYTOqueuePill">${queueCount}</span>` : '';
   const errorHtml  = errorCount > 0 ? `<span class="CYTOerrorPill">${errorCount}</span>` : '';
   const classes    = ['CYTOnodeContentInner'];
   if(isTemplate) { classes.push('CYTOnodeTemplate'); }
   if(queueCount > 0) { classes.push('CYTOnodeHasQueue'); }
   if(errorCount > 0) { classes.push('CYTOnodeHasError'); }
   return `<div class="${classes.join(' ')}">${queueHtml}<span class="CYTOnodeName">${name}</span>${errorHtml}</div>`;
}

function CYTOrenderedNodeBounds(node, zoom) {
   const modelWidth       = Number(node.width());
   const modelHeight      = Number(node.height());
   const renderedPosition = node.renderedPosition();
   const renderedWidth    = modelWidth * zoom;
   const renderedHeight   = modelHeight * zoom;
   const left             = renderedPosition.x - renderedWidth / 2;
   const top              = renderedPosition.y - renderedHeight / 2;
   return {
      left,
      top,
      right : left + renderedWidth,
      bottom: top + renderedHeight,
      width : renderedWidth,
      height: renderedHeight
   };
}

function CYTOnodeBoundsVisible(bounds, viewW, viewH) {
   return !(bounds.left > viewW || bounds.top > viewH || bounds.right < 0 || bounds.bottom < 0);
}

function CYTOsyncNodeContentOverlay(instance = CYTOinstance) {
   if(!instance) { return; }
   const overlay = CYTOensureNodeContentOverlay(instance);
   if(!overlay) { return; }

   const contentEls = instance.nodeContentEls || (instance.nodeContentEls = Object.create(null));
   const viewW      = instance.container().clientWidth;
   const viewH      = instance.container().clientHeight;
   const zoom       = instance.zoom();
   const activeIds  = Object.create(null);

   instance.nodes().forEach(node => {
      const modelWidth = Number(node.width());
      const modelHeight = Number(node.height());
      const bounds = CYTOrenderedNodeBounds(node, zoom);
      const {left, top, width: renderedWidth, height: renderedHeight} = bounds;

      if(!CYTOnodeBoundsVisible(bounds, viewW, viewH)) {
         CYTOremoveNodeContent(instance, node.id());
         return;
      }

      let el = contentEls[node.id()];
      if(!el) {
         el = document.createElement('div');
         el.className = 'CYTOnodeContentWrap';
         el.dataset.nodeId = node.id();
         overlay.appendChild(el);
         contentEls[node.id()] = el;
      }

      const html = CYTOnodeContentHtml(node);
      if(el.innerHTML !== html) { el.innerHTML = html; }

      el.style.left = `${left}px`;
      el.style.top = `${top}px`;
      el.style.width = `${modelWidth}px`;
      el.style.height = `${modelHeight}px`;
      el.style.transform = `scale(${zoom})`;
      el.style.opacity = `${node.style('opacity') || 1}`;
      activeIds[node.id()] = true;
   });

   Object.keys(contentEls).forEach(nodeId => {
      if(!activeIds[nodeId]) { CYTOremoveNodeContent(instance, nodeId); }
   });
   const hasUnpinnedHeader = CYTOsyncUnpinnedHeader(instance, overlay, viewW, viewH);
   overlay.hidden = Object.keys(instance.nodeContentEls).length === 0 && !hasUnpinnedHeader;
}

function CYTOscheduleNodeContentSync(instance = CYTOinstance, {debounceMs = 0} = {}) {
   if(!instance) { return; }
   if(debounceMs > 0) {
      if(instance.nodeContentSyncTimeout) { clearTimeout(instance.nodeContentSyncTimeout); }
      instance.nodeContentSyncTimeout = setTimeout(() => {
         instance.nodeContentSyncTimeout = null;
         CYTOscheduleNodeContentSync(instance);
      }, debounceMs);
      return;
   }
   if(instance.nodeContentSyncTimeout) {
      clearTimeout(instance.nodeContentSyncTimeout);
      instance.nodeContentSyncTimeout = null;
   }
   if(instance.nodeContentSyncRaf) { return; }
   instance.nodeContentSyncRaf = requestAnimationFrame(() => {
      instance.nodeContentSyncRaf = null;
      CYTOsyncNodeContentOverlay(instance);
   });
}
