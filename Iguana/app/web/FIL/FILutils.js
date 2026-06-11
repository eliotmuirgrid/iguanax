function FILreservedCharacters()            { return "<>:\"/\\|?*"; }
function FILnameEndsWithDot(Name)           { return Name.endsWith("."); }
function FILnameStartsOrEndsWithSpace(Name) { return Name.startsWith(" ") || Name.endsWith(" "); }

function FILreservedNames() {
   return "CON, PRN, AUX, NUL, COM1, COM2, COM3, COM4, COM5, COM6, COM7, COM8, COM9, LPT1, LPT2, LPT3, LPT4, LPT5, LPT6, LPT7, LPT8, and LPT9";
}

function FILnameRequirements() {
   return `Invalid name. Must avoid:
- Reserved characters ${FILreservedCharacters()}
- Reserved names ${FILreservedNames()}
- Ending with a period`;
}


function FILnameMatchesReservedNames(Name) {
   // Reserved file names in Windows
   const ReservedNames = [
      "CON", "PRN", "AUX", "NUL",
      "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
      "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"
   ];
   return ReservedNames.includes(Name.toUpperCase());
}

function FILnameContainsReservedCharacters(Name) {
   // List of forbidden characters in Windows file names
   const ForbiddenChars = /[\\\/:*?"<>|]/;
   return ForbiddenChars.test(Name);
}


function FILnameIsInvalid(Name) {
   return FILnameStartsOrEndsWithSpace(Name) || FILnameContainsReservedCharacters(Name) ||
      FILnameMatchesReservedNames(Name) || FILnameEndsWithDot(Name);
}

function FILpathSepAdd(path) { return path.endsWith(FILsep()) ? path : path + FILsep(); }