function TRANSautocompleteEnd(data) {
   if(!data.autocomplete) { return; }
   const cursor  = TRANSeditor.getCursor();
   const vars    = TRANSprepareLuaPostInfo();
   const option = {
      dt: `insert end <span class="ITSintellisenseOptionValue">`,
      ft: `\nend`,
      nt: 6,
      pt: 0,
      s : true
   };
   const args = {
      line_number      : cursor.line + 1,
      cursor_position  : cursor,
      options          : [option],
      options_truncated: false,
      search_string    : "",
      full_expression: "\nend"
   };
   ITSintellisenseOptionsHandler(vars, args);
}

function TRANSautocompleteIndent(line, cursorPos) {
   TRANSeditor.indentLine(line + 1);
   const lineContent = TRANSeditor.getLine(line + 1);
   const indentation = lineContent.search(/\S/);
   const newIndent   = Array(indentation + 4).join(" "); // 4 for new indent and cursor
   const replacement = newIndent + "\n";
   TRANSeditor.replaceRange(replacement, { line: line, ch: 0 }, { line: line + 1, ch: 0 });
   TRANSeditor.setCursor({ line: line, ch: cursorPos });
}