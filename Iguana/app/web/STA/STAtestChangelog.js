let STAtestChanglog = false;

// call STAupdatePopout() in the console to show the changelog
let STAtestChanglogHtml = `
<!-- ROOT: vertical flex layout, constrained to viewport -->
<div style="
  display:flex;
  flex-direction:column;
  max-height:calc(100vh - 260px);
  overflow:hidden;
">

  <!-- HEADER (fixed) -->
  <div style="
    display:flex;
    justify-content:space-between;
    align-items:center;
    padding:0 12px 20px 12px;
    flex:0 0 auto;
  ">
    <span style="font-size:18px;font-weight:700;line-height:1;">Version&nbsp;118</span>
    <a href="https://man.interfaceware.com/code/2661580801"
       target="_blank"
       style="font-size:14px;color:#0066cc;line-height:1;text-decoration:underline;">
      Changelog
    </a>
  </div>

  <!-- BODY (scrollable) -->
  <div style="
    flex:1 1 auto;
    min-height:0;      /* critical */
    overflow:auto;
    padding:0 16px;
  ">
    <p style="margin:0 0 16px 0;">
      This <b>maintenance release</b> includes reliability, performance, and compatibility improvements across Git, component APIs, logs, networking, and code-editing workflows.
    </p>

    <h2 style="margin:0 0 8px 0;">Git &amp; Integration Improvements</h2>
    <ul style="margin:0 0 16px 20px;padding:0;line-height:1.5;">
      <li style="margin-bottom:6px;"><strong>IX-4329</strong> – Improved Git instance reset performance for instances with many components.</li>
      <li style="margin-bottom:6px;"><strong>IX-4332</strong> – Improved Windows Git process cleanup after operations.</li>
      <li style="margin-bottom:6px;"><strong>IX-4348</strong> – Added additional permission checks for manual pushes to the instance repository.</li>
      <li style="margin-bottom:6px;"><strong>IX-4374</strong> – Improved support for starting components by name as well as GUID.</li>
      <li style="margin-bottom:6px;"><strong>IX-4378</strong> – Refined component API permission checks and validation messages.</li>
      <li style="margin-bottom:6px;"><strong>IX-4384</strong> – Improved checkout handling for earlier commits in uncommitted libraries.</li>
      <li style="margin-bottom:6px;"><strong>IX-4393</strong> – Fixed SSH key validation and registration checks for Git services when servers return broader permission-denied messages or emit SSH warnings before the final auth error.</li>
      <li style="margin-bottom:6px;"><strong>IX-4394</strong> – Updated Bitbucket integration to use the current API.</li>
    </ul>

    <h2 style="margin:0 0 8px 0;">Runtime, Logs &amp; Editor Improvements</h2>
    <ul style="margin:0 0 8px 20px;padding:0;line-height:1.5;">
      <li style="margin-bottom:6px;"><strong>IX-4226</strong> – Improved UTF-8 handling when exporting sample data from logs.</li>
      <li style="margin-bottom:6px;"><strong>IX-4306</strong> – Improved FTP and SFTP handling for file failures and names with spaces.</li>
      <li style="margin-bottom:6px;"><strong>IX-4360</strong> – Improved <code>net.tcp</code> connection cleanup when a component is stopped.</li>
      <li style="margin-bottom:6px;"><strong>IX-4362</strong> – Improved path parsing for shell commands such as <code>netstat</code>.</li>
      <li style="margin-bottom:6px;"><strong>IX-4369</strong> – Improved resilience in module loading and annotation handling while editing code.</li>
      <li style="margin-bottom:6px;"><strong>IX-4382</strong> – Improved responsiveness when fetching last-error messages in logs.</li>
      <li style="margin-bottom:6px;"><strong>IX-4383</strong> – Improved log browser performance for logs with large messages.</li>
      <li style="margin-bottom:6px;"><strong>IX-4389</strong> – Improved HTTP/2 redirect handling.</li>
    </ul>
  </div>
</div>
`;
