var MAINtemplate=
`<p>$Name$ has $$$Amount$ in her bank account.</p>$`;

function MAINrun(){
   var Body = document.querySelector('body');
   var Data = [{Amount : 55, Name : " Fred " }, {Amount : 45443 , Name : "Eliana"}];
   Body.innerHTML = TEMexpand(Data, MAINtemplate);
}

READYevent(MAINrun);
