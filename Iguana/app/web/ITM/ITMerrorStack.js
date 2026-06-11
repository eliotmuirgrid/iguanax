function ITMstackFrameRow(Frame) {
   const EscapedFilename = ESChtmlEscape(Frame.fd);
   let R = `<tr>
        <td width="1%"><a class="ITMlink" title="Step into ${Frame.n} at ${EscapedFilename}:${Frame.l}"> ${Frame.n}</a></td>
        <td>(${EscapedFilename}:${Frame.l})</td>
      </tr>`;
   return R;
}

// TODO a little gross how we identify the container
function ITMgetErrorStackContainer() {
   return document.querySelector("#CONT-ERROR_STACK > .ANNpopup");
}

function ITMpopulateErrorStackTrace(Error) {
   let C = ITMgetErrorStackContainer();   
   COL_TRC(C);
   if (!C) {
      COL_TRC("No container for error stack");
      return;
   }
   if (Error && Error.CallStack) {
      const CallStack = Error.CallStack;
      let Rows ='';
      for (let i = CallStack.length - 1; i >= 0; i--) {
         Rows += ITMstackFrameRow(CallStack[i]);
      }
      C.innerHTML = `<div class="error_stack_container"><table class="ANNest">${Rows}</table></div>`;
      C.addEventListener('click', function(e) { ITMonErrorStackClick(e, CallStack);});
   } else {
      C.innerHTML = '<div class="ANNest_none">There is no error in the script for which a stack trace can be provided.</div>';
   }
}

function ITMrowIndex(e) {
   const row = e.target.closest('tr');
   if (!row) return -1;
   const table = row.closest('table');
   const rows = Array.from(table.getElementsByTagName('tr'));
   return rows.indexOf(row);
}

function ITMonErrorStackClick(e, CallStack) {
   e.preventDefault();
   COL_TRC(e);
   const index = ITMrowIndex(e);
   if (index == -1) return;
   // Convert to stack frame index since rows are in reverse order
   const stackFrameIndex = CallStack.length - 1 - index;
   COL_TRC("Stack frame index:", stackFrameIndex);
   let Call = CallStack[stackFrameIndex];
   COL_TRC("Jumping to:", Call.fd + ":" + Call.l);
   TRANSjumpToStackFrame(Call);
}
