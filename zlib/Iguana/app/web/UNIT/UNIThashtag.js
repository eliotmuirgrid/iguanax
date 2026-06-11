UNITaddTest("HASHTAGtestIsTag", () => { UNITassertEquals(true, EDITisHashtag('#thisIsAHashtag')); });

UNITaddTest("HASHTAGtestRender", () => {
   const removeStyles = (htmlString) => { return htmlString.replace(/style="[^"]*"/g, ''); };
   const expected     = "hello <a class=\"DASHhashtag HASHTAGstyle HASHTAGtagStyle\" word=\"#world\" >#world</a>";
   const actual       = HASHTAGrenderHashtags('hello #world');
   UNITassertEquals(removeStyles(expected), removeStyles(actual));
});
