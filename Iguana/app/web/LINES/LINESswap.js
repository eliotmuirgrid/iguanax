// =========================
// Key Handler
// =========================
function LINEScheckForSwapLine(event) {
   // Check if Alt is pressed
   if (!event.altKey) return;

   // Check for ArrowUp or ArrowDown
   if (event.code === 'ArrowUp') {
      event.preventDefault();
      LINESmoveSelectedLinesUp();
   } else if (event.code === 'ArrowDown') {
      event.preventDefault();
      LINESmoveSelectedLinesDown();
   }
}

// =========================
// Doc and Selection Helpers
// =========================
function LINESgetDoc() {
   return TRANSeditor.getDoc();
}

function LINESgetSelectionIndices(doc) {
   const startIdx = doc.indexFromPos(doc.getCursor('start'));
   const endIdx   = doc.indexFromPos(doc.getCursor('end'));
   return { startIdx, endIdx };
}

function LINESgetSelectedText(doc) {
   return doc.getSelection();
}

function LINESreplaceSelection(doc, newText, startIdx, endIdx) {
   doc.replaceRange(
      newText,
      doc.posFromIndex(startIdx),
      doc.posFromIndex(endIdx)
   );
}

function LINESresetSelection(doc, startIdx, newText) {
   doc.setSelection(
      doc.posFromIndex(startIdx),
      doc.posFromIndex(startIdx + newText.length)
   );
}

// =========================
// String/Line Helpers
// =========================
function LINESsplitLines(text) {
   return text.split('\n');
}

function LINESjoinLines(lines) {
   return lines.join('\n');
}

// =========================
// Public Up/Down Functions
// =========================
function LINESmoveSelectedLinesUp() {
   const doc = LINESgetDoc();
   const { startIdx, endIdx } = LINESgetSelectionIndices(doc);
   const selectedText = LINESgetSelectedText(doc);

   // If multiple lines are selected, move them all up
   if (selectedText && selectedText.length > 0) {
      LINESmoveMultipleLines(doc, selectedText, startIdx, endIdx, 'up');
   } 
   // Otherwise, move the single current line up
   else {
      const cursorPos = doc.getCursor();  // e.g. { line: 5, ch: 10 }
      const lineNumber = cursorPos.line;
      LINESmoveSingleLine(doc, lineNumber, 'up');
   }
}

function LINESmoveSelectedLinesDown() {
   const doc = LINESgetDoc();
   const { startIdx, endIdx } = LINESgetSelectionIndices(doc);
   const selectedText = LINESgetSelectedText(doc);

   // If multiple lines are selected, move them all down
   if (selectedText && selectedText.length > 0) {
      LINESmoveMultipleLines(doc, selectedText, startIdx, endIdx, 'down');
   } 
   // Otherwise, move the single current line down
   else {
      const cursorPos = doc.getCursor();  // e.g. { line: 5, ch: 10 }
      const lineNumber = cursorPos.line;
      LINESmoveSingleLine(doc, lineNumber, 'down');
   }
}

// =========================
// Internal: Multi-line Swaps
// =========================
function LINESmoveMultipleLines(doc, selectedText, startIdx, endIdx, direction) {
   const lines = LINESsplitLines(selectedText);

   // Figure out the current line range (startLine to endLine)
   const startLine = doc.posFromIndex(startIdx).line;
   const endLine   = doc.posFromIndex(endIdx).line;

   // Edge cases: can't move past first or last line in the doc
   if (direction === 'up' && startLine === 0) return;
   if (direction === 'down' && endLine === doc.lastLine()) return;

   // Read the entire document as an array of lines
   const totalLines = doc.lineCount();
   const docLines = [];
   for (let i = 0; i < totalLines; i++) {
      docLines.push(doc.getLine(i));
   }

   // Remove the selected block from docLines
   const blockSize = lines.length;
   const block = docLines.splice(startLine, blockSize); 
   // 'block' should match our selected lines

   // Insert them one line up or down
   if (direction === 'up') {
      // Insert starting at (startLine - 1)
      docLines.splice(startLine - 1, 0, ...block);
   } else {
      // Insert starting at (startLine + 1) if we're moving down
      docLines.splice(startLine + 1, 0, ...block);
   }

   // Build the updated text
   const newDocText = LINESjoinLines(docLines);

   // Overwrite the entire doc
   doc.replaceRange(
      newDocText,
      { line: 0, ch: 0 },
      { line: doc.lastLine() + 1, ch: 0 }
   );

   // Now set the new selection block
   const newStartLine = (direction === 'up') ? startLine - 1 : startLine + 1;
   const newEndLine   = newStartLine + blockSize - 1;

   doc.setSelection(
      { line: newStartLine, ch: 0 },
      { line: newEndLine,   ch: doc.getLine(newEndLine).length }
   );
}

// =========================
// Internal: Single-line Swap
// =========================
function LINESmoveSingleLine(doc, lineNumber, direction) {
   // Edge cases
   if (direction === 'up'   && lineNumber === 0) return;
   if (direction === 'down' && lineNumber === doc.lastLine()) return;

   // Read all lines
   const totalLines = doc.lineCount();
   const docLines = [];
   for (let i = 0; i < totalLines; i++) {
      docLines.push(doc.getLine(i));
   }

   // Extract this one line
   const [ lineText ] = docLines.splice(lineNumber, 1);

   // Compute new target line
   let newLine = lineNumber + (direction === 'up' ? -1 : 1);

   // Put the line into its new spot
   docLines.splice(newLine, 0, lineText);

   // Overwrite the entire doc
   const newDocText = LINESjoinLines(docLines);
   doc.replaceRange(
      newDocText,
      { line: 0, ch: 0 },
      { line: doc.lastLine() + 1, ch: 0 }
   );

   // Move cursor to the new line
   const finalCh = doc.getLine(newLine).length;
   doc.setCursor({ line: newLine, ch: finalCh });
}
