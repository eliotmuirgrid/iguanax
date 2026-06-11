function SMPLtoolbarHtml() {
   return /*html*/`
   <div class="TRANSsection SMPLmodal">
      <div class="TRANStoolbarButton SMPLmsgFirst" title="First Message"></div>
      <div class="TRANStoolbarButton SMPLmsgPrev"  title="Previous Message"></div>
      <div class="TRANStoolbarText   SMPLmsgGoto"  title="Go To Message">
         <input class="SMPLgoToInput FORMinput" autocomplete="off">
         <div class="SMPLtotalCount"></div>
      </div>
      <div class="TRANStoolbarButton SMPLmsgNext" title="Next Message"></div>
      <div class="TRANStoolbarButton SMPLmsgLast" title="Last Message"></div>
      <div class='TRANStoolbarButton SMPLmsgAdd'  title="Add sample data"></div>
      <div class="TRANStoolbarButton SMPLmsgEdit" title='Edit selected sample data'></div>
   </div>
`;
}