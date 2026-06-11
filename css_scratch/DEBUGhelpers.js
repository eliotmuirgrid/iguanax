function DEBUGborders(){
   console.log("Borders on");
   var Element = document.createElement('style');
   document.head.appendChild(Element);
	var Sheet = Element.sheet;
   Sheet.insertRule("*{outline: 1px red solid;}", 0);
}

function css(el) {
   var sheets = document.styleSheets, ret = [];
   el.matches = el.matches || el.webkitMatchesSelector || el.mozMatchesSelector 
       || el.msMatchesSelector || el.oMatchesSelector;
   for (var i in sheets) {
       var rules = sheets[i].rules || sheets[i].cssRules;
       for (var r in rules) {
           if (el.matches(rules[r].selectorText)) {
               ret.push(rules[r]);
           }
       }
   }
   return ret;
}