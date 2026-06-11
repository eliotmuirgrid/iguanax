const SMPLtoolbarHexByteHtml = /*html*/`
<div class="SMPLhexCodeInput" title="Hex Byte(00 - FF)">
   <label for="SMPLhexInput">0x</label>
   <input id="SMPLhexInput" class="SMPLhexInput FORMinput" type="text" size="2" maxlength="2" autocomplete="off">
</div>
<div class="TRANStoolbarButton SMPLinsertHex" title='Insert Hex Byte'></div>`;

function SMPLinvalidHexCode(Code) {
   if (Code[0] === 'x' || Code[0] === 'X') {
      Code = Code.substring(1);
   }
   return !/^[a-fA-F0-9]{1,2}$/.test(Code);
}

function SMPLhexCodeToChar(Code){
   if (SMPLinvalidHexCode(Code)) {
      throw new Error("Bad Hex Code.");
   }
   if(Code.length === 1){
      Code = "0" + Code;
   }
   let charCode = parseInt(Code, 16);
   return String.fromCharCode(charCode);
}

function SMPLinsertHex(e, Editor, EolTokens) {
   if (e.keyCode !== undefined && e.keyCode !== 13) { return; }
   e.preventDefault();

   let Input = document.querySelector('.SMPLhexInput');
   let HexVal = Input.value.trim().toUpperCase();
   if (SMPLinvalidHexCode(HexVal)){
      SNCKsnackbar("error","Valid hex codes range from 00 to FF");
      FORMfocus(Input);
      return;
   }
   if (HexVal.length == 1){
      console.log("Hex value is only one digit, so padding with zero.");
      HexVal = "0" + HexVal
   }
   console.log("Inserting:", HexVal);

   let Cursor = Editor.getCursor();
   let LineNumber = Cursor.line, Pos = Cursor.ch;

   if (HexVal >= "20" && HexVal <= "7E"){
      let Char = SMPLhexCodeToChar(HexVal);
      Editor.replaceRange(Char, {line: LineNumber, ch: Pos});
   } else {
      let HexTokenObject = EolTokens.hexToken(HexVal);
      Editor.replaceRange(HexTokenObject.ServerToken, {line: LineNumber, ch: Pos});
      Editor.markText(
         {line: LineNumber, ch: Pos},
         {line: LineNumber, ch: Pos + HexTokenObject.ServerToken.length},
         {replacedWith: HexTokenObject.newNode(), clearWhenEmpty: false}
      );
   }
   TRANSfileCurrentSave(function(){
      setTimeout(TRANScleanDirtyLine, 100);
   });
}

function SMPLtoolbarRenderHexByte() {
   // const hexByteTools = document.querySelector(".SMPLtoolbarHexByte");
   // if (SMPLeditingSampleData()) {
   //    hexByteTools.classList.remove("SMPLhidden");
   // } else {
   //    hexByteTools.classList.add("SMPLhidden");
   // }
}

function SMPLtoolbarSetUpHexByte() {
   document.querySelector(".SMPLtoolbarHexByte").innerHTML = SMPLtoolbarHexByteHtml;
   document.querySelector(".SMPLhexInput").addEventListener('keypress', function(e){ SMPLinsertHex(e, TRANSeditor, SMPLeolTokens); });
   document.querySelector(".SMPLinsertHex").addEventListener('click', function(e){ SMPLinsertHex(e, TRANSeditor, SMPLeolTokens); });
}
