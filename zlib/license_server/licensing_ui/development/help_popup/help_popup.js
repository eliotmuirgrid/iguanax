/**
 * Copyright (c) 2010-2013 iNTERFACEWARE Inc.  All rights reserved.
 */

// Requires jQuery.
//
// Any page that uses this must have the following HTML defined in its body:
// <div id="helpTooltipDiv" class="helpTooltip">
//   <b id="helpTooltipTitle"></b>
//   <em id="helpTooltipBody"></em>  
//   <input type="hidden" name="helpTooltipId" id="helpTooltipId" value="0">
// </div>
//

function HLPpopUpinitialize(refresh_func) {
   // Attaches an onclick event to any element with the helpIcon class.
   // These items now open a modal help window using their title/rel attributes as content
   // If the same element is clicked twice, it closes - but if a different one is clicked, the modal window moves
   // Modal position is tied to mouse click - and clicking anywhere in the title will close it.
   $('.helpIcon').each(
      function () {
         $(this).click(
            function (e) {
               if ($("#helpTooltipDiv").is(":hidden") || document.getElementById('helpTooltipId').value != this.id) {
                  document.getElementById('helpTooltipTitle').innerHTML = this.title;
                  document.getElementById('helpTooltipBody').innerHTML = this.rel;
                  document.getElementById('helpTooltipId').value = this.id;
                  $("#helpTooltipDiv").css("left", e.pageX);
                  $("#helpTooltipDiv").css("top", e.pageY);
                  $("#helpTooltipDiv").fadeIn("slow");

                  document.getElementById('helpTooltipDiv').style.left = HLPpopUpCalculatePosition(WINgetWindowLeft(), e.clientX, WINgetWindowWidth(), document.getElementById('helpTooltipDiv').offsetWidth) + 'px';
                  document.getElementById('helpTooltipDiv').style.top = HLPpopUpCalculatePosition(WINgetWindowTop(), e.clientY, WINgetWindowHeight(), document.getElementById('helpTooltipDiv').offsetHeight) + 'px';
                  if (refresh_func) {
                     refresh_func(this.id.replace('_Icon', ''));
                  }
               }
               else {
                  $("#helpTooltipDiv").fadeOut("slow");
               }
            }
         );
      }
   );
   $("#helpTooltipDiv b").click(
      function (e) {
         $("#helpTooltipDiv").fadeOut("slow");
      }
   );
}

function HLPpopUpCalculatePosition(WindowPos, MousePos, WindowSize, PopUpSize) {
   var Position = MousePos;
   if ((Position + PopUpSize) > (WindowSize)) {
      Position = Math.max(10, (WindowPos + WindowSize - (PopUpSize + 10) ));
   }

   return Position + WindowPos;
}

