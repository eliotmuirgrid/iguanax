function CYTOcssColorVar(variableName, fallback) {
   if(typeof window === 'undefined' || !window.getComputedStyle) { return fallback; }
   const value = window.getComputedStyle(document.documentElement).getPropertyValue(variableName).trim();
   return value || fallback;
}

const CYTOstyleColors = {
   nodeBorder      : '#ced4da',
   nodeText        : '#374151',
   templateBg      : CYTOcssColorVar('--grey-300', '#e5e7eb'),
   templateBorder  : CYTOcssColorVar('--grey-400', '#cbd5e1'),
   templateText    : CYTOcssColorVar('--grey-700', '#374151'),
   running         : '#28a745',
   inProgress      : '#facc15',
   error           : '#ef4444',
   hoveredBg       : '#e8f0fe',
   connectSource   : '#3b82f6',
   edgeLine        : '#9ca3af',
   edgeHover       : '#2563eb',
   edgeOverlay     : '#1f2937',
   markedBg        : '#e5f3fa',
   markedText      : '#2b3133',
   markedGlow      : '#2590cd',
   processingGlow  : '#60a5fa'
};

const CYTOnodeTransitionStyle = {
   'transition-property'       : 'underlay-opacity, underlay-padding, outline-opacity, outline-offset',
   'transition-duration'       : '320ms',
   'transition-timing-function': 'ease-in-out'
};

const CYTOedgeTransitionStyle = {
   'transition-property'       : 'width, line-color, target-arrow-color, overlay-opacity, overlay-padding',
   'transition-duration'       : '180ms',
   'transition-timing-function': 'ease-out'
};

const CYTObaseNodeStyle = {
   'background-color': '#ffffff',
   'border-width'    : 2,
   'border-color'    : CYTOstyleColors.nodeBorder,
   'label'           : '',
   'width'           : 160,
   'height'          : 40,
   'shape'           : 'round-rectangle',
   'corner-radius'   : 999,
   'text-valign'     : 'center',
   'text-halign'     : 'center',
   'font-size'       : '12px',
   'color'           : CYTOstyleColors.nodeText,
   'text-wrap'       : 'ellipsis',
   'text-max-width'  : '120px',
   'overlay-opacity' : 0,
   'underlay-color'  : CYTOstyleColors.processingGlow,
   'underlay-opacity': 0,
   'underlay-padding': 0,
   'outline-width'   : 1,
   'outline-color'   : CYTOstyleColors.processingGlow,
   'outline-opacity' : 0,
   'outline-offset'  : 0,
   ...CYTOnodeTransitionStyle
};

const CYTObaseEdgeStyle = {
   'width'             : 1,
   'line-color'        : CYTOstyleColors.edgeLine,
   'target-arrow-color': CYTOstyleColors.edgeLine,
   'target-arrow-shape': 'triangle',
   'curve-style'       : 'bezier',
   'arrow-scale'       : 0.8,
   'overlay-color'     : CYTOstyleColors.edgeOverlay,
   'overlay-opacity'   : 0,
   'overlay-padding'   : 0,
   ...CYTOedgeTransitionStyle
};

function CYTOstyleRule(selector, style) { return {selector, style}; }

function CYTOborderStateStyle(borderColor, borderWidth = 2) {
   return {'border-color': borderColor, 'border-width': borderWidth};
}

function CYTOstylesheet() {
   return [
      CYTOstyleRule('node', CYTObaseNodeStyle),
      CYTOstyleRule('node.CYTOTemplate', {
         'background-color': CYTOstyleColors.templateBg,
         'border-color'    : CYTOstyleColors.templateBorder,
         'border-width'    : 1,
         'color'           : CYTOstyleColors.templateText
      }),
      CYTOstyleRule('node.CYTORunning', CYTOborderStateStyle(CYTOstyleColors.running, 2)),
      CYTOstyleRule('node.CYTOInProgress', CYTOborderStateStyle(CYTOstyleColors.inProgress, 1)),
      CYTOstyleRule('node.CYTOError', CYTOborderStateStyle(CYTOstyleColors.error, 2)),
      CYTOstyleRule('node[borderColor]', CYTOborderStateStyle('data(borderColor)', 2)),
      CYTOstyleRule('node.CYTOHovered', {'background-color': CYTOstyleColors.hoveredBg}),
      CYTOstyleRule('node.CYTOConnectSource', {
         ...CYTOborderStateStyle(CYTOstyleColors.connectSource, 3),
         'underlay-color'  : CYTOstyleColors.connectSource,
         'underlay-opacity': 0.18,
         'underlay-padding': 5,
         'outline-width'   : 1,
         'outline-color'   : CYTOstyleColors.connectSource,
         'outline-opacity' : 0.18,
         'outline-offset'  : 2
      }),
      CYTOstyleRule('edge', CYTObaseEdgeStyle),
      CYTOstyleRule('edge.CYTOHovered', {
         'line-color'        : CYTOstyleColors.edgeHover,
         'target-arrow-color': CYTOstyleColors.edgeHover,
         'width'             : 2,
         'overlay-opacity'   : 0.08,
         'overlay-padding'   : 8
      }),
      CYTOstyleRule('.CYTOHighlighted', {'opacity': 1, 'z-index': 10}),
      CYTOstyleRule('.CYTODimmed', {'opacity': 0.15}),
      CYTOstyleRule('.CYTOMarked', {
         'background-color': CYTOstyleColors.markedBg,
         'color'           : CYTOstyleColors.markedText,
         'underlay-color'  : CYTOstyleColors.markedGlow,
         'underlay-opacity': 0.16,
         'underlay-padding': 4,
         'outline-width'   : 2,
         'outline-color'   : CYTOstyleColors.markedGlow,
         'outline-opacity' : 0.45,
         'outline-offset'  : 3,
         'opacity'         : 1
      }),
      CYTOstyleRule('node.CYTOProcessing', {
         'underlay-color'  : CYTOstyleColors.processingGlow,
         'underlay-opacity': 0.14,
         'underlay-padding': 3,
         'outline-width'   : 1,
         'outline-color'   : CYTOstyleColors.processingGlow,
         'outline-opacity' : 0.06,
         'outline-offset'  : 1
      }),
      CYTOstyleRule('node.CYTOProcessing.CYTOProcessingPulse', {
         'underlay-opacity': 0.24,
         'underlay-padding': 6,
         'outline-opacity' : 0.14,
         'outline-offset'  : 3
      })
   ];
}
