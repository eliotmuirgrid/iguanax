function LINEScheckForCommentOut(Event) {
   FORMcheckForCommandPlusKey(Event, "/", function () {
      LINEStoggleLineComments();
   });
}
function LINESgetDoc() {
   return TRANSeditor.getDoc();
}

function LINESgetSelectionIndices(doc) {
   const startIdx = doc.indexFromPos(doc.getCursor('start'));
   const endIdx = doc.indexFromPos(doc.getCursor('end'));
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
// Line Processing
// =========================
function LINESsplitLines(text) {
   return text.split('\n');
}

function LINESjoinLines(lines) {
   return lines.join('\n');
}

function LINESgetNonBlankLines(lines) {
   return lines.filter(line => line.trim() !== '');
}

function LINESisCommented(line) {
   const noIndent = line.replace(/^\s+/, '');
   return noIndent.startsWith('-- ');
}

function LINESuncommentLine(line) {
   return line.replaceAll('-- ', '');
}

function LINEScommentLine(line) {
   return '-- ' + line;
}

function LINESuncommentLines(lines) {
   return lines.map(line => {
      if (line.trim().length === 0) return line;
      return LINESuncommentLine(line);
   });
}

function LINEScommentLines(lines) {
   return lines.map(line => {
      if (line.trim().length === 0) return line;
      return LINEScommentLine(line);
   });
}

// =========================
// Main Toggle Function
// =========================
function LINEStoggleLineComments() {
   const doc = LINESgetDoc();
   const { startIdx, endIdx } = LINESgetSelectionIndices(doc);
   const selectedText = LINESgetSelectedText(doc);

   // If text is selected, handle multiple lines
   if (selectedText && selectedText.length > 0) {
      LINEStoggleMultipleLines(doc, selectedText, startIdx, endIdx);
      return;
   }

   // Otherwise, handle the *current line* where the cursor is
   const cursorPos = doc.getCursor();            // e.g. { line: 5, ch: 10 }
   const lineNumber = cursorPos.line;
   const lineText = doc.getLine(lineNumber) || ""; // getLine() returns a string

   LINEStoggleSingleLine(doc, lineNumber, lineText);
}

// =========================
// Helper: Multi-line toggle
// =========================
function LINEStoggleMultipleLines(doc, selectedText, startIdx, endIdx) {
   const lines = LINESsplitLines(selectedText);
   const nonBlank = LINESgetNonBlankLines(lines);
   const allCommented = nonBlank.every(line => LINESisCommented(line));
   const toggledLines = allCommented
      ? LINESuncommentLines(lines)
      : LINEScommentLines(lines);

   const toggledText = LINESjoinLines(toggledLines);
   LINESreplaceSelection(doc, toggledText, startIdx, endIdx);
   LINESresetSelection(doc, startIdx, toggledText);
}

// =========================
// Helper: Single-line toggle
// =========================
function LINEStoggleSingleLine(doc, lineNumber, lineText) {
   // We'll treat this single line as an array of 1, apply the same logic:
   const lines = [lineText];
   const isBlank = (lineText.trim().length === 0);

   // If it's blank, do nothing or comment it (your choice). 
   // Let's skip toggling if it's empty/blank:
   if (isBlank) return;

   const allCommented = LINESisCommented(lineText); // single line check
   const toggledLines = allCommented
      ? LINESuncommentLines(lines)
      : LINEScommentLines(lines);

   const toggledLine = toggledLines[0];
   // Replace the entire line in the editor from ch=0 to ch=Infinity
   doc.replaceRange(
      toggledLine,
      { line: lineNumber, ch: 0 },
      { line: lineNumber, ch: Infinity }
   );

   // Optionally, move the cursor to the end of this line or keep it where it was
   // We'll move it to the same column if possible, or just set it to the length:
   doc.setCursor({ line: lineNumber, ch: Math.min(doc.getLine(lineNumber).length, 9999) });
}