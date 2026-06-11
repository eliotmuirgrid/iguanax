// Routines to validate file names for Windows and create valid file names for Windows provided a file name.
// Adapted from https://gist.github.com/doctaphred/d01d05291546186941e1b7ddc02034d3

// Returns a valid Windows file name provided a file name.
function FILmakeValidFileName(FileName){
   // https://stackoverflow.com/a/31976060
   // https://gist.github.com/doctaphred/d01d05291546186941e1b7ddc02034d3
   // resolve multi-line, whitespace trimming
   FileName = FileName.split(/[\r\n]/).map(s => s.trim()).filter(s => s.length).join("  ");
   // remove forbidden characters
   // (after multi-line, because new-line-chars are themselves forbidden characters)
   // also forbidding ; as that delimits the PATH
   FileName = FileName.replaceAll(/[`<>:;"\/\\\|?*\x00-\x1F]/g, "");
   // advanced trim
   FileName = FileName.replace(/\.$/, "");
   // empty filename
   if (!FileName.length) { FileName = '_'; }
   // forbidden filenames
   if (FileName.match(/^(CON|PRN|AUX|NUL|COM1|COM2|COM3|COM4|COM5|COM6|COM7|COM8|COM9|LPT1|LPT2|LPT3|LPT4|LPT5|LPT6|LPT7|LPT8|LPT9)(\..+)?$/)) {
      FileName = `_${FileName}`;
   }
   return FileName;
}