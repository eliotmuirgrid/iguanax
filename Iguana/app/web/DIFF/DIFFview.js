var DIFFwidgets = [];
var DIFFhighlightedLines = [];

/* ---------- Entry ---------- */
function DIFFshowConflicts() {
  DIFFclearAll();
  if (!TRANSeditor) return;

  let startCursor = DIFFfindConflictStart();
  while (startCursor.findNext()) {
    const splitCursor = DIFFfindConflictSplit(startCursor);
    if (!splitCursor) continue;

    const endCursor = DIFFfindConflictEnd(splitCursor);
    if (!endCursor) continue;

    const start = [startCursor.from(), startCursor.to()];
    const split = [splitCursor.from(), splitCursor.to()];
    const end = [endCursor.from(), endCursor.to()];

    DIFFshowDiffBox(start, split, end);
  }
}

/* ---------- Finders ---------- */
function DIFFfindConflictStart() {
  return TRANSeditor.getSearchCursor(/^<<<<<<< HEAD/, { line: 0, ch: 0 }, false);
}

function DIFFfindConflictSplit(startCursor) {
  const split = TRANSeditor.getSearchCursor(/^=======$/, startCursor.to(), false);
  return split.findNext() ? split : null;
}

function DIFFfindConflictEnd(splitCursor) {
  // Match the entire ">>>>>>>" header line
  const end = TRANSeditor.getSearchCursor(/^>>>>>>>.*$/, splitCursor.to(), false);
  return end.findNext() ? end : null;
}

/* ---------- UI Builders ---------- */
function DIFFcreateWidgetTemplate(idBase) {
  const idCur = idBase + "Cur";
  const idInc = idBase + "Inc";
  const idBoth = idBase + "Both";

  return `
    <a class="DIFFoption" id="${idCur}">Use Current</a>
    <span class="DIFFoption">|</span>
    <a class="DIFFoption" id="${idInc}">Use Incoming</a>
    <span class="DIFFoption">|</span>
    <a class="DIFFoption" id="${idBoth}">Use Both</a>
  `;
}

function DIFFshowDiffBox(start, split, end) {
  DIFFhighlightLines(start, split, end);

  const currentText = TRANSeditor.getRange(
    { line: start[1].line + 1, ch: 0 },
    split[0]
  );
  const incomingText = TRANSeditor.getRange(
    { line: split[1].line + 1, ch: 0 },
    end[0]
  );

  const lineNumber = start[0].line - 1;
  const idBase = btoa(JSON.stringify(start[0])).replace(/=/g, "");
  const toolTip = document.createElement("div");
  toolTip.innerHTML = DIFFcreateWidgetTemplate(idBase);
  toolTip.classList.add("DIFFwidget");

  const widget = TRANSeditor.addLineWidget(lineNumber + 1, toolTip, { above: true });
  DIFFwidgets.push(widget);

  DIFFbindWidgetEvents(idBase, start, end, currentText, incomingText);
}

/* ---------- Event Binding ---------- */
function DIFFbindWidgetEvents(idBase, start, end, currentText, incomingText) {
  const idCur = "#" + idBase + "Cur";
  const idInc = "#" + idBase + "Inc";
  const idBoth = "#" + idBase + "Both";

  const endReplace = { line: Math.min(end[1].line + 1, TRANSeditor.lineCount() - 1), ch: 0 };

  document.querySelector(idCur).addEventListener("click", () => {
    DIFFapplyChange(currentText, start[0], endReplace);
  });
  document.querySelector(idInc).addEventListener("click", () => {
    DIFFapplyChange(incomingText, start[0], endReplace);
  });
  document.querySelector(idBoth).addEventListener("click", () => {
    DIFFapplyChange(currentText + incomingText, start[0], endReplace);
  });
}

/* ---------- Actions ---------- */
function DIFFapplyChange(newText, start, end) {
  DIFFclearAll();
  TRANSeditor.replaceRange(newText, start, end);
  DIFFshowConflicts();
}

function DIFFhighlightLines(start, split, end) {
  TRANSeditor.addLineClass(start[0].line, "background", "DIFFcurrentHeader");
  DIFFhighlightedLines.push(start[0].line);

  for (let i = start[0].line + 1; i < split[0].line; i++) {
    TRANSeditor.addLineClass(i, "background", "DIFFcurrentText");
    DIFFhighlightedLines.push(i);
  }

  for (let i = split[0].line + 1; i < end[0].line; i++) {
    TRANSeditor.addLineClass(i, "background", "DIFFincomingText");
    DIFFhighlightedLines.push(i);
  }

  TRANSeditor.addLineClass(end[0].line, "background", "DIFFincomingHeader");
  DIFFhighlightedLines.push(end[0].line);
}

/* ---------- Cleanup ---------- */
function DIFFclearAll() {
  while (DIFFwidgets.length) DIFFwidgets.pop().clear();
  while (DIFFhighlightedLines.length) {
    const line = DIFFhighlightedLines.pop();
    TRANSeditor.removeLineClass(line, "background", "DIFFcurrentHeader");
    TRANSeditor.removeLineClass(line, "background", "DIFFcurrentText");
    TRANSeditor.removeLineClass(line, "background", "DIFFincomingText");
    TRANSeditor.removeLineClass(line, "background", "DIFFincomingHeader");
  }
}
