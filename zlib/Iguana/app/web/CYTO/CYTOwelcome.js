function CYTOwelcomeRender() {
   const map     = document.querySelector('.CYTOcontainer');
   const welcome = document.querySelector('.CYTOwelcomeOverlay');
   if(!map || welcome) { return; }
   map.insertAdjacentHTML('beforeend', CYTOwelcomeHtml());
}

function CYTOwelcomeHtml() {
   return `
<div class="CYTOwelcomeOverlay">
    <div class="CYTOwelcomeModal">
      <h2>Welcome to IguanaX</h2>
      <p>Are you ready to take control of your data?</p>
      <p>Let us <a href="${LINKdashWelcomeBegin}" target="_blank">begin</a>.</p>
    </div>
</div>`;
}

function CYTOwelcomeRemove() {
   const Welcome = document.querySelector('.CYTOwelcomeOverlay');
   if(Welcome) { Welcome.remove(); }
}