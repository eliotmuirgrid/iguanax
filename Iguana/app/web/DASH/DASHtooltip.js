function DASHqueueCountTooltip(target) {
   POPtooltipRemoveByClass('LGEtooltipWrap');
   if(target.textContent === '0')   { return; }
   if(target.textContent === 'N/A') { return; }
   let guid = DASHrowGuid( DASHrowIndex(target.closest('.DASHcomponentRow')) );
   POPtooltip({
      target: target,
      content: "<a role='button' class='DASHclearQueue'>Clear Queue</a>",
      style: 'LGEtooltipWrap',
      position: POPtooltipPosition.RIGHT,
      xOffset: -20
   });
   document.querySelector('.DASHclearQueue').addEventListener('click', function() { CARDclearQueueModal([guid]) });
}