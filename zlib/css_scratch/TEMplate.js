function TEMconvertString(V){
   if (!V) { return ''} else { return V.toString(); }
}

function TEMexpandLine(Tokens, Row, Array){
   console.log(Tokens);
   for (var i =0; i < Tokens.length-1; i+=2){
      Array.push(Tokens[i]);
      Array.push(TEMconvertString(Row[Tokens[i+1]]).replaceAll("&", "&amp;").replaceAll("<", "&lt;").replaceAll(">", "&gt;"))
   }
   Array.push(Tokens[Tokens.length-1]);
}

function TEMexpand(Data, Template){
   var Array = [];
   var Tokens = Template.replaceAll("$$", "_FuNkYDolARAM").split('$');
   for (var i =0; i < Data.length; i++){
      TEMexpandLine(Tokens, Data[i], Array);
   }
   console.log(Array);
   return Array.join('').replaceAll("_FuNkYDolARAM", "$");
}