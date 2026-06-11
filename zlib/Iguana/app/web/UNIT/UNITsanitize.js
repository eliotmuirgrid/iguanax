UNITaddTest("SANsanitize", () => {
   const cases = [
      {
         input: '<div>Before<script>alert("test")</script>After</div>',
         expected: '<div>BeforeAfter</div>'
      },
      {
         input: '<script>alert("test")</script><p>Content</p>',
         expected: '<p>Content</p>'
      },
      {
         input: '<div>Start</div><script>\n// Comments\nalert("test");\n</script><div>End</div>',
         expected: '<div>Start</div><div>End</div>'
      },
      {
         input: '<script src="evil.js"></script>',
         expected: ''
      },
      {
         input: '<div><script>nested</script>content</div>',
         expected: '<div>content</div>'
      }
   ];
   cases.forEach(c => { UNITassertEquals(c.expected, SANsanitizeHTML(c.input)); });
});
