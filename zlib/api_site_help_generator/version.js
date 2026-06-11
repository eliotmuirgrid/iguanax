$(function() {

  var sb = $('#nav_select');
  var versions = ['6.1.1', '6.1.0', '6.0.6', '6.0.5', '6.0.4', '6.0.3', '6.0.0', '5.6.20', '5.6.19', '5.6.18', '5.6.17', '5.6.16', '5.6.15', '5.6.14', '5.6.12', '5.6.11', '5.6.10', '5.6.9', '5.6.8', '5.6.7', '5.6.6', '5.6.5', '5.6.4', '5.6.3', '5.6.2', '5.6.1', '5.5.7', '5.5.6', '5.5.5', '5.5.4', '5.5.3', '5.5.2', '5.5.1']
  var options = '';
  for (x=0; x < versions.length; x++)
  {
   var newUrl = versions[x].replace(/\./g, '') + '.html';
   var url = window.location.pathname;
    var filename = url.substring(url.lastIndexOf('/')+1);
    
   options = options + '<option value="' + newUrl + '"';
   if (newUrl == filename) 
   {
      options = options + ' selected';
   }
   options = options + '>' + versions[x] + '</option>';
  }
  sb.append(options);
  
  $(window).scroll(function() {
      if ($(window).scrollTop() < 72) {
         $('body > .logo').css('top', 0 - $(window).scrollTop() + 'px');
         $('.tocify-wrapper').css('top', 72 - $(window).scrollTop() + 'px');
      }
      else
      {
         $('body > .logo').css('top', '-72px');
         $('.tocify-wrapper').css('top', '0px');
      }
  });
  
  
});