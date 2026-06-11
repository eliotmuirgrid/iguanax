/**
 * Copyright (c) 2013 iNTERFACEWARE Inc.  All rights reserved.
 */

if (!my_ifware) var my_ifware = {};

if (!LicensingAPI) { //Assume a standard API location of '/\'.
   var url = $.url(window.location.href);
   var LicensingAPI = url.attr('protocol') + "://" + url.attr('host') +
       (url.attr('port') ? ":" + url.attr('port') : "") + "/api";
}

$(document).ready(function(){
   //Flash detection code from: http://stackoverflow.com/a/3336320
   my_ifware.hasFlash = false;
   try {
      var fo = new ActiveXObject('ShockwaveFlash.ShockwaveFlash');
      if(fo) my_ifware.hasFlash = true;
   } catch(e){
      if(navigator.mimeTypes ["application/x-shockwave-flash"] != undefined) my_ifware.hasFlash = true;
   }

   $(".IndexLink").click(function() {
      //Handling the click on the banner, bread crumb, etc. in order to preserve the query string,
      //which we use elsewhere to tell we have a fresh visit.
      window.location.hash = "";
      return false;
   });
});

my_ifware.errorDisplay = function(message, content, OnClose) {
   var curHTML = '';
   if (message) {
      curHTML += '<p>' + PAGhtmlEscape(message) + '</p>';
   }
   if (content && content.desc) {
      curHTML += '<p>' + PAGhtmlEscape(content.desc) + '</p>';
   }
   if (content && content.details) {
      curHTML += '<p>' + PAGhtmlEscape(content.details) + '</p>';
   }

   $('<div>' + curHTML + '</div>').appendTo('body:first').dialog({
      title: 'Error',
      modal: true,
      buttons: {OK:function(){$(this).dialog('close');}},
      close: function(){
         $(this).dialog('destroy').remove();
         if (OnClose) { OnClose(); }
      }
   });
};

my_ifware.withPermission = function(PermissionName, DisplayErrors, onSuccess, onFail) {
   var Permissions = Storage.getObject('Permissions');
   if (Permissions == undefined) {
      my_ifware.errorDisplay('Error in determining permissions.', content);
      if (onFail) { onFail(); }
   } else if (Permissions[PermissionName] == true) {
      if (onSuccess) { onSuccess(); }
   } else {
      if (DisplayErrors) {
         PAGwarning('Need to have ' + PermissionName + ' permission to proceed.');
      }
      if (onFail) { onFail(); }
   }
};

my_ifware.InitializePage = function() {
   $("#helpTooltipDiv").hide();
   $(".OnlyBeforeLoad").hide();
   $(".HiddenOnStart").show();

   $("#bread_crumb_specific").html('');

   if (Storage.get('Token')) {
      $(".LoggedIn").show();
      if (Storage.get('CompanyName')) {
         $("#CompanyHeader").show();
         $("#CompanyHeader .CompanyName").html(Storage.get('CompanyName'));
      } else {
         $('#CompanyHeader').hide();
      }
   }
   my_ifware.withPermission('FinancialRead', false, function() {
      $(".AccountHistorySections").show();
   }, function(){
      $(".AccountHistorySections").hide();
   });
};

// http://stackoverflow.com/a/2587398
// parse a date in yyyy-mm-dd format
my_ifware.parseDate = function(input) {
   var parts = input.match(/(\d+)/g);
   // new Date(year, month [, date [, hours[, minutes[, seconds[, ms]]]]])
   return new Date(parts[0], parts[1]-1, parts[2]); // months are 0-based
};

my_ifware.doLogin = function(afterLogin, IsSplashPage) {
   $(".OnlyBeforeLoad").hide();
   $(".HiddenOnStart").show();

   $('.LoggedIn').hide();

   if (IsSplashPage) {
      $("#content").html($("#StaticLoginSplash").html() + $("#StaticLoginPage").html());
   } else {
      $("#content").html($("#StaticLoginWarning").html() + $("#StaticLoginPage").html());
   }
   $("#content").find("#LinkToCreateAccount").click(function() {
      window.location.hash = "Page=createaccount";
      return false;
   });

   $("#content").find("#LoginForm input.enter_will_submit").keypress(function (e) {
      if ((e.which && e.which == 13) || (e.keyCode && e.keyCode == 13)) {
         $('#LoginSubmit').click();
         return false;
      } else {
         return true;
      }
   });

   $("#content").find("#LoginSubmit").click(function() {
      var $form = $("#content").find("#LoginForm"),
          username = $form.find( 'input[name="username"]' ).val(),
          password = $form.find( 'input[name="password"]' ).val();

      var posting = $.ajax({
         type: "POST",
         url: LicensingAPI,
         data: {
            method : 'session.login',
            username: username,
            password: password },
         success: function( content ) {
            if (content.status == "ok") {
               analytics.identify(username,{email: username});
               analytics.track('Licensing Login');
               Storage.set('Token',content.data.Token, 1);
               if (Storage.get('CompanyName') != undefined) {
                  Storage.remove('CompanyName');
               }
               if (content.data.Info.CompanyName != undefined) {
                  Storage.set('CompanyName', content.data.Info.CompanyName, 1);
               }
               Storage.setObject('Permissions', content.data.Info.Permissions, 1);
               afterLogin();
            } else {
               my_ifware.errorDisplay('Could not log in.', content);            
            }
         },
         complete: function(jqXHR, textStatus) {
            if (textStatus != "success") {
               my_ifware.handleFailToConnect();
            }
         },
         dataType: "json"
      });
   });
};

//checks whether current session token is valid
//if not, presents log-in
//if it logs in, executes afterLogin(); else redirects to main page
my_ifware.accessWithLogin = function(afterLogin, IsSplashPage) {
   if (!Storage.get('Token')) {
      my_ifware.doLogin(afterLogin, IsSplashPage);
   } else {
      $.ajax({
         type: "POST",
         url: LicensingAPI,
         data: {
            method: 'session.checkToken',
            token: Storage.get('Token')
         },
         success: function( content ) {
            if (content.status == "ok") {
               afterLogin();
            }  else {
               my_ifware.doLogin(afterLogin, IsSplashPage);
            }
         },
         complete: function(jqXHR, textStatus) {
            if(textStatus !== "success"){
               my_ifware.handleFailToConnect();
            }
         },
         dataType: "json"
      });
   }
};

my_ifware.handleSplash = function(PageParams) {
   my_ifware.accessWithLogin( function() {
      my_ifware.InitializePage();

      $("#content").html(
         $("#StaticIndexPage").html()
      );
   },
                             true);
};

my_ifware.handleIndex = function(PageParams) {
   my_ifware.accessWithLogin( function() {
      my_ifware.InitializePage();

      $("#content").html(
         $("#StaticIndexPage").html()
      );
   },
                             false);
};

my_ifware.handleFailToConnect = function() {
   my_ifware.InitializePage();
   $("#content").html(
      $("#StaticFailToConnectPage").html()
   );
};

my_ifware.doLogout = function(AfterLogout) {
   var vals = {
      token: Storage.get('Token'),
      method: 'session.logout'
   };
   if (!vals.token) {
      if (AfterLogout) { AfterLogout(); }
      return;
   }
   $.ajax({
      type: "POST",
      url: LicensingAPI,
      data: vals,
      success: function( content ) {},
      complete: function(jqXHR, textStatus) {
         Storage.remove('Token');
         if (AfterLogout) { AfterLogout(); }
      },
      dataType: "json"
   });
};

my_ifware.handleLogout = function(PageParams) {
   my_ifware.doLogout(function () {
      window.location.hash = "";
   });
};

my_ifware.getIguanaActivationURL = function(ProductLocation, Code) {
   if (ProductLocation.slice(-1) != '/') {
      ProductLocation += '/';
   }
   return ProductLocation + 'submit_license_settings.html?NewLicenseHex=' + Code;
};

my_ifware.processActivationPage = function(PageParams, CurLicense) {
   $(".LicenseName").html(CurLicense.name);

   $("#content").html(
      $("#StaticActivatePage").html()
   );

   var $form = $("#content").find("#RegistrationForm");

   if (!CurLicense.ExtraActivationFields) {
      $form.find(".extra_activation_fields").remove();
   }

   if (PageParams.Product.toLowerCase() == 'iguana') {
      $form.find(".chameleon_fields").remove();
      $form.find("#ActivationTableHeader").html('Get an Iguana Registration Code');
   } else {
      $form.find(".iguana_fields").remove();
      $form.find("#ActivationTableHeader").html('Get a Chameleon Registration Code');
   }

   if (PageParams.LicenseID.toLowerCase() == 'temporary') {
      $("#content").find('.TemporaryLicenseInfo').show();
   }

   if (Storage.get('product') && Storage.get('product').toLowerCase() == PageParams.Product.toLowerCase()
       && Storage.get('instanceid')) {
      $form.find( 'input[name="InstanceID"]' ).val(Storage.get('instanceid'));
   }

   HLPpopUpinitialize(); //Set up the help pop-ups for some of the activation fields.

   $("#content").find("#RegistrationSubmit").click(function() {
      $("#content").find("#RegistrationSubmit").attr("disabled", true).addClass("submit_button_disabled");

      var $form = $("#content").find("#RegistrationForm");

      var vals = {
         token: Storage.get('Token'),
         product: PageParams.Product,
         entitlementid: PageParams.LicenseID,
         instanceid: $.trim($form.find( 'input[name="InstanceID"]' ).val()),
         description: $form.find( 'input[name="Description"]' ).val(),
         method: 'license.activate'
      };

      if (CurLicense.ExtraActivationFields) {
         vals.trackingid = $form.find( 'input[name="TrackingID"]' ).val();
         vals.site = $form.find( 'input[name="Site"]' ).val();
         vals.city = $form.find( 'input[name="City"]' ).val();
         vals.state = $form.find( 'input[name="State"]' ).val();
         vals.country = $form.find( 'input[name="Country"]' ).val();
      }

      if (!vals.instanceid || !vals.description) {
         $("#content").find("#RegistrationSubmit").attr("disabled", false).removeClass("submit_button_disabled");
         PAGwarning('Required field(s) missing.');
         return;
      }

      $.ajax({
         type: "POST",
         url: LicensingAPI,
         data: vals,
         success: function( content ) {
            if (content.status !== "ok"){
               $("#content").find("#RegistrationSubmit").attr("disabled", false).removeClass("submit_button_disabled");
               my_ifware.errorDisplay("Could not activate license.", content);
            } else {
               $("#content").html($("#LicenseSummaryPage").html());

               if (PageParams.LicenseID.toLowerCase() == 'temporary') {
                  $('#content').find('.TemporaryLicenseWarning').show();
               }

               if (vals.product.toLowerCase() == 'iguana') {
                  $("#content").find('#LicenseSummaryTable').html(
                     $('#EmptyIguanaSummaryFields').find('table').html()
                  );

                  $("#content").find("#summary_maintenance_expiry").html(
                     content.data.maintenance_expiry == "none" ? "N/A" : content.data.maintenance_expiry
                  );
                  $("#content").find("#summary_numchannels").html(content.data.num_channels);

               } else {
                  $("#content").find('#LicenseSummaryTable').html(
                     $('#EmptyChameleonSummaryFields').find('table').html()
                  );
               }

               $("#content").find("#summary_instanceid").html(content.data.instanceid);
               $("#content").find("#summary_code").html(content.data.code);
               $("#content").find("#summary_license_expiry").html(
                  content.data.license_expiry == "none" ? "N/A" : content.data.license_expiry
               );
               $("#content").find("#summary_description").html(content.data.description);

               if (CurLicense.ExtraActivationFields) {
                  $("#content").find('#LicenseSummaryTable').append(
                     $('#EmptyExtraSummaryFields').find('table').html()
                  );
                  $("#content").find("#summary_trackingid").html(content.data.trackingid);
                  $("#content").find("#summary_site").html(content.data.site);
                  $("#content").find("#summary_city").html(content.data.city);
                  $("#content").find("#summary_state").html(content.data.state);
                  $("#content").find("#summary_country").html(content.data.country);

               }

               if (vals.product.toLowerCase() == 'iguana' && Storage.get('productlocation')) {
                  $("#content").find("#successful_registration_buttons").html(
                     '<form><input id="GoIntoProduct" class="submit_button" type="button" value="Activate inside Iguana" /></form>'
                  );

                  var ActivationURL = my_ifware.getIguanaActivationURL(Storage.get('productlocation'), content.data.code);
                  $("#content").find("#GoIntoProduct").click(function() {
                     window.location.href = ActivationURL;
                     return false;
                  }
                                                            );
               } else {
                  $("#content").find("#successful_registration_buttons").html(
                     '<form><input id="ReturnToLicenses" class="submit_button" type="button" value="Back to Licenses" /></form>'
                  );
                  $("#content").find("#ReturnToLicenses").click(function() {
                     window.location.hash = "Page=licenses";
                     return false;
                  }
                                                               );
               }
               //Clearing in-product use parameters after any activation.
               Storage.remove('instanceid');
               Storage.remove('productlocation');
               Storage.remove('product');
               Storage.remove('version');
            }
         },
         complete: function(jqXHR, textStatus) {
            if (textStatus != 'success') {
              $("#content").find("#RegistrationSubmit").attr("disabled", false).removeClass("submit_button_disabled");
              PAGwarning('Could not connect to license server.');
            }
         },
         dataType: "json"
      });
   });
};

my_ifware.profileSubmit = function() {
   $("#content").find("#ProfileSubmit").attr("disabled", true).addClass("submit_button_disabled");

   var $form = $("#content").find("#ProfileInfo");

   var AjaxParams = {
      token: Storage.get('Token'),
      email: $form.find( 'input[name="Email"]' ).val(),
      password: $form.find( 'input[name="Password"]' ).val(),
      firstname: $form.find( 'input[name="FirstName"]' ).val(),
      lastname: $form.find( 'input[name="LastName"]' ).val(),
      method: 'account.updateProfile'
   };

   if ($form.find( 'input[name="NewPassword"]' ).val() != '') {
      if ($form.find( 'input[name="NewPassword"]' ).val() !=
          $form.find( 'input[name="NewPasswordVerify"]' ).val()) {
         PAGwarning('New password does not match');

         $("#content").find("#ProfileSubmit").attr("disabled", false).removeClass("submit_button_disabled");
         return;
      }
      AjaxParams.newpassword = $form.find( 'input[name="NewPassword"]' ).val();
   }

   $.ajax({
      type: "POST",
      url: LicensingAPI,
      data: AjaxParams,
      success: function( content ) {
         if (content.status == "ok") {
            PAGwarning('Profile successfully updated.', function() {
               window.location.hash="";
            });
         } else {
            $("#content").find("#ProfileSubmit").attr("disabled", false).removeClass("submit_button_disabled");
            my_ifware.errorDisplay('',content);   
         }
      },
      complete: function(jqXHR, textStatus) {
         if(textStatus !== "success"){
            $("#content").find("#ProfileSubmit").attr("disabled", false).removeClass("submit_button_disabled");
            PAGwarning('Could not connect to license server.');     
         }
         //Clear password fields.
         $form.find( 'input[type="password"]' ).val('');

         $("#content").find("#ProfileSubmit").attr("disabled", false).removeClass("submit_button_disabled");

      },
      dataType: "json"
   });
};

my_ifware.handleProfile = function(PageParams) {
   my_ifware.accessWithLogin( function() {
      my_ifware.InitializePage();
      $("#bread_crumb_specific").html(" &#8250; View/Edit Profile");

      $("#content").html(
         $("#EmptyProfilePage").html()
      );
      $("#content").find("#ProfileInfo").html(
         '<div id="ProfileSpinner"><img src="images/spinner_large.gif" /></div>'
      );

      $.ajax({
         type: "POST",
         url: LicensingAPI,
         data: {
            method: 'account.getProfile',
            token: Storage.get('Token')
         },
         success: function( content ) {
            if (content.status === "ok") {
               var $form = $("#content").find("#ProfileInfo");
               $form.html(
                  $('#EmptyProfileInfo').html()
               );

               $form.find( 'input[name="Email"]' ).val(content.data.Email);
               $form.find( 'input[name="FirstName"]' ).val(content.data.FirstName);
               $form.find( 'input[name="LastName"]' ).val(content.data.LastName);

               $("#content").find("#ProfileSubmit").click(my_ifware.profileSubmit);
            } else {
               $("#content").find('#ProfileInfo').html('Could not load information.');
               my_ifware.errorDisplay('',content);
            }
         },
         complete: function(jqXHR, textStatus) {
            if (textStatus !== 'success') {
               $("#content").find('#ProfileInfo').html('Could not load information.');
               PAGwarning('Could not connect to license server.');
            }
         },
         dataType: "json"
      });
   });
};

my_ifware.handleActivate = function(PageParams) {
   my_ifware.accessWithLogin( function() {
      my_ifware.InitializePage();
      $("#bread_crumb_specific").html(' &#8250; <a href="#Page=licenses">Licenses</a> &#8250; Activate');

      var CurLicense = {};
      $.ajax({
         type: "POST",
         url: LicensingAPI,
         data: {
            method: 'license.getEntitlementInfo',
            token: Storage.get('Token'),
            product: PageParams.Product,
            entitlementid: PageParams.LicenseID
         },
         success: function( content ) {
            if (content.status === "ok") {
               my_ifware.processActivationPage(PageParams, content.data);
            } else {
               my_ifware.errorDisplay('Could not get license info.', content);      
            }
         },
         complete: function(jqXHR, textStatus) {
            if (textStatus !== 'success') {
               PAGwarning('Could not connect to license server.');
            }         
         },
         dataType: "json"
      });
   });
};

my_ifware.handleLicenses = function(PageParams, NeedSplashPage) {
   my_ifware.accessWithLogin( function() {
      my_ifware.InitializePage();
      $("#bread_crumb_specific").html(' &#8250; Licenses');

      $("#content").html(
         $("#EmptyLicensesPage").html()
      );

      var populateLicenses = function(product) {
         $.ajax({
            type: "POST",
            url: LicensingAPI,
            data: {
               method: 'license.listEntitlements',
               token: Storage.get('Token'),
               product: product
            },
            success: function( content ) {
               if (content.status == "ok") {
                  //Hide Chameleon section if there are no Chameleon licenses.
                  if (product.toLowerCase() == 'chameleon' && content.data.length == 0) {
                     $("#content").find("#ChameleonLicensesSection").hide();
                  }
                  var r = new Array();
                  var j=0;
                  if(product == 'Iguana'){
                     r[j++] = '<tr class="light"><td><b>Temporary License</b></td><td></td><td></td><td></td><td></td><td>';
                  } else {
                     r[j++] = '<tr class="light"><td><b>Temporary License</b></td><td></td><td></td><td></td><td>';
                  }
                  r[j++] = '<a class="temporary_license_activate license_entitlement_link activate_license_link" href="#Page=activate&Product=' + product;
                  r[j++] = '&LicenseID=temporary';
                  r[j++] = '">Activate</a>';
                  r[j++] = '</td>';

                  var total_used = 0;
                  for (var i=0; i<content.data.length; i++) {
                     total_used += parseInt(content.data[i].entitlements_used, 10);

                     r[j++] = (i%2 == 0 ? '<tr class="dark">' : '<tr class="light">');
                     r[j++] = '<td>';
                     r[j++] = content.data[i].name;
                     r[j++] = '</td>';
                     if(content.data[i].product != null || content.data[i].name == "Iguana Trial"){
                        r[j++] = '<td>';
                        if(content.data[i].name == "Iguana Trial"){
                           content.data[i].product = "Iguana";
                        }
                        r[j++] = content.data[i].product;
                        r[j++] = '</td>';
                     }

                     r[j++] = '<td>';
                     r[j++] = content.data[i].entitlements_total;
                     r[j++] = '</td>';
                     r[j++] = '<td>';
                     if (content.data[i].entitlements_used > 0) {
                        r[j++] = '<a class="view_activation_link license_entitlement_link" href="#Page=viewactivations&Product=' + product + '&LicenseID=';
                        r[j++] = content.data[i].id + '">';
                        r[j++] = content.data[i].entitlements_used + '</a>';
                     } else {
                        r[j++] = content.data[i].entitlements_used;
                     }
                     r[j++] = '</td>';
                     r[j++] = '<td>';
                     var entitlements_available = content.data[i].entitlements_total - content.data[i].entitlements_used;
                     if (entitlements_available < 0) {
                        entitlements_available = 0;
                     }
                     r[j++] = entitlements_available;
                     r[j++] = '</td>';
                     r[j++] = '<td>';
                     if (content.data[i].entitlements_total - content.data[i].entitlements_used > 0) {
                        r[j++] = '<a class="license_entitlement_link activate_license_link" href="#Page=activate&Product=' + product + '&LicenseID=';
                        r[j++] = content.data[i].id;
                        r[j++] = '">Activate</a>';
                     } else {
                        r[j++] = '<span class="disabled_hyperlink">Activate</span>';
                     }
                     r[j++] = '</td>';
                  }
                  $("#content").find('#' + product + 'Licenses').append(r.join(''));

                  my_ifware.withPermission('TemporaryLicense', false, undefined, function() {
                     $("#content").find('#' + product + 'Licenses .temporary_license_activate').replaceWith(
                        '<span class="disabled_hyperlink">Activate</span>'
                     );
                  });

                  my_ifware.withPermission('LicenseRead',false,function() {
                     if (total_used > 0) {
                        $("#content").find('#' + product.toLowerCase() + '_all_activations_section').show();
                        $("#content").find('#' + product.toLowerCase() + '_all_activations_link').html('View All (' + total_used + ')');
                     }
                  }, function() {
                     $("#content").find('#' + product + 'Licenses .view_activation_link').replaceWith(function() {
                        return $(this).text();
                     });
                  });

                  my_ifware.withPermission('LicenseWrite',false,undefined,function() {
                     $("#content").find('#' + product + 'Licenses .activate_license_link').replaceWith('<span class="disabled_hyperlink">Activate</span>');
                  });

               } else {
                  my_ifware.errorDisplay('Could not get license listing.', content);
               }
            },
            complete: function(jqXHR, textStatus) {
               if (textStatus != 'success') {
                  my_ifware.errorDisplay('Could not connect to license server.');
               }
               $("#content").find('#' + product + 'Spinner').remove();
            },
            dataType: "json"
         });
      };

      populateLicenses('Iguana');
      populateLicenses('Chameleon');
   },
   NeedSplashPage);
};

my_ifware.handleGetLicense = function(PageParams) {
   my_ifware.handleLicenses(PageParams, true);
};

my_ifware.handleInvoices = function(PageParams) {
   my_ifware.accessWithLogin( function() {
      my_ifware.InitializePage();
      $("#bread_crumb_specific").html(' &#8250; Invoices');

      $("#content").html(
         $("#StaticInvoicesPage").html()
      );

      $("#content").find("#InvoiceContent").html(
         '<div id="InvoiceSpinner"><img src="images/spinner_large.gif" /></div>'
      );

      $.ajax({
         type: "POST",
         url: LicensingAPI,
         data: {
            method: 'account.getInvoices',
            token: Storage.get('Token')
         },
         success: function( content ) {
            if (content.status == "ok") {
               var datacolumns = ['invoice_number','description','date_due','amount_due','amount_paid','amount_owing'];

               var getRowHTML = function(row) {
                  var arr = new Array(),
                      j=0;
                  arr[j++] = "<tr>";
                  for (var i=0; i<datacolumns.length; i++) {
                     if ($.inArray(datacolumns[i],['amount_due','amount_paid','amount_owing']) != -1) {
                        arr[j++] = "<td class='dollar_value'>";
                     } else {
                        arr[j++] = "<td>";
                     }
                     arr[j++] = row[datacolumns[i]];
                     arr[j++] = "</td>";
                  }
                  arr[j++] = "</tr>";

                  return arr.join('');
               };

               var currentdata = new Array(),
                   historydata = new Array(),
                   iCurrent = 0,
                   iHistory = 0;

               var total_owing = 0;

               for (var i=0; i<content.data.length; i++) {
                  total_owing += Number(content.data[i].amount_owing);

                  content.data[i].date_due = $.datepicker.formatDate("MM d, yy", my_ifware.parseDate(content.data[i].date_due));

                  if (content.data[i].amount_owing > 0) {
                     currentdata[iCurrent++] = getRowHTML(content.data[i]);
                  } else {
                     historydata[iHistory++] = getRowHTML(content.data[i]);
                  }
               }

               var header = "<thead><tr><th style='width:10%'>Invoice</th><th style='width:30%'>Description</th>" +
                   "<th style='width:15%'>Due</th><th style='width:15%'>Total</th><th style='width:15%'>Paid</th>" +
                   "<th style='width:15%'>Owing</th></tr></thead>";

               $("#content").find("#InvoiceContent").html($("#EmptyInvoiceTables").html());

               $("#content").find("#InvoiceCurrentTable").html(header + '<tbody>' + currentdata.join('') + '</tbody>');
               $("#content").find("#InvoiceHistoryTable").html(header + '<tbody>' + historydata.join('') + '</tbody>');

               $("#content").find("#TotalOwing").html(total_owing);

               $(".dollar_value").formatCurrency();

               $("#content").find("#InvoiceCurrentTable").dataTable({
                  "bLengthChange": false,
                  "bFilter": false,
                  "bPaginate": false,
                  "aaSorting": [[ 0, "desc" ]],
                  "sDom": 'it'
               });
               $("#content").find("#InvoiceHistoryTable").dataTable({
                  "bLengthChange": false,
                  "bFilter": false,
                  "aaSorting": [[ 0, "desc" ]],
                  "sDom": 'pit',
                  "oLanguage": {
                     "oPaginate": {
                        "sNext": "", //Removing text to not conflict with the previous/next images.
                        "sPrevious": ""
                     }
                  }
               });
            } else {
               $("#content").find("#InvoiceContent").html("Content not available.");
               my_ifware.errorDisplay('',content);
            }
         },
         complete: function(jqXHR, textStatus) {
            if (textStatus != "success") {
               $("#content").find("#InvoiceContent").html("Content not available.");
               PAGwarning("Could not connect to licensing API server.");
            }
         },
         dataType: "json"
      });
   });
};

my_ifware.handleCreateAccount = function(PageParams) {
   $(".OnlyBeforeLoad").hide();
   $(".HiddenOnStart").show();

   $('.LoggedIn').hide();
   my_ifware.doLogout();

   $("#content").html(
      $("#StaticCreateAccountPage").html()
   );

   $("#content").find("#CreateAccountSubmit").click(function () {
      $("#content").find("#CreateAccountSubmit").attr("disabled", true).addClass("submit_button_disabled");

      var $form = $("#CreateAccount"),
          email = $form.find( 'input[name="PrimaryEmail"]' ).val(),
          email2 = $form.find( 'input[name="PrimaryEVerify"]' ).val(),
          password = $form.find( 'input[name="Password"]' ).val(),
          password2 = $form.find( 'input[name="PasswordVerify"]' ).val(),
          firstname = $form.find( 'input[name="FirstName"]' ).val(),
          lastname = $form.find( 'input[name="LastName"]' ).val(),
          company = $form.find( 'input[name="Company"]' ).val(),
          state = $form.find('#StateField').val(),
          country = $form.find('#CountryField').val(),
          phone = $form.find( 'input[name="Phone"]' ).val();

      //check that all nonempty that need to be

      if (email != email2) {
         PAGwarning('Email does not match.');
         $("#content").find("#CreateAccountSubmit").attr("disabled", false).removeClass("submit_button_disabled");
         return;
      }

      if (password != password2) {
         PAGwarning('Password does not match.');
         $("#content").find("#CreateAccountSubmit").attr("disabled", false).removeClass("submit_button_disabled");
         return;
      }

      $.ajax({
         type: "POST",
         url: LicensingAPI,
         data: {
            method: 'account.create',
            email: email,
            password: password,
            firstname: firstname,
            lastname: lastname,
            company: company,
            state: state,
            country: country,
            phone: phone
         },
         success: function( content ) {
            if (content.status == "ok") {
               analytics.identify(email, {
                  email: email,
                  firstName: firstname,
                  lastName: lastname,
                  company: company,
                  state: state,
                  country: country,
                  phone: phone,
                  page:'account.create'
               });
               analytics.track('Created licensing account');
               var AccountType = 'General';
               if (Storage.get('product') && Storage.get('product').toLowerCase() == 'iguana') {
                  AccountType = 'Iguana';
               }
               $("#content").html(
                  $("#StaticAccountCreatedPage").html() 
               );
            } else {
               my_ifware.errorDisplay('Could not create account.',content);
               $("#content").find("#CreateAccountSubmit").attr("disabled", false).removeClass("submit_button_disabled");
            }
         },
         complete: function(jqXHR, textStatus) {
             if (textStatus != "success") {
               PAGwarning("Could not connect to licensing API server.");
               $("#content").find("#CreateAccountSubmit").attr("disabled", false).removeClass("submit_button_disabled");
             }
         },
         dataType: "json"
      });
   });
};

my_ifware.handleActivateAccount = function(PageParams) {
   $(".OnlyBeforeLoad").hide();
   $(".HiddenOnStart").show();

   my_ifware.doLogout();

   $("#content").html(
      '<div id="ActivateAccountSpinner"><center><img src="images/spinner_large.gif"></center></div>'
   );

   $.ajax({
      type: "POST",
      url: LicensingAPI,
      data: {
         method: 'account.activate',
         key: PageParams.Key
      },
      success: function( content ) {
         if (content.status == 'ok') {
            PAGwarning('Account successfully activated. You should now be able to log in.');
            window.location.hash = '';
         } else {
            my_ifware.errorDisplay('',content);
         }
     },
      complete: function(jqXHR, textStatus) {
         if (textStatus != "success") {
            PAGwarning("Could not connect to licensing API server.");
         }
         window.location.hash = "";
      },
      dataType: "json"
   });
};

my_ifware.handlePasswordReset = function(PageParams) {
   $(".OnlyBeforeLoad").hide();
   $(".HiddenOnStart").show();

   my_ifware.doLogout();

   $("#content").html(
      $("#StaticPasswordResetPage").html()
   );

   $("#content").find("#PasswordResetSubmit").click(function () {
      $("#content").find("#PasswordResetSubmit").attr("disabled", true).addClass("submit_button_disabled");
      $("#content").find('input[name="ResetEmail"]').attr('disabled', true);
      $("#content").find('#PasswordResetSpinner').show();

      $.ajax({
         type: "POST",
         url: LicensingAPI,
         data: {
            method: 'account.requestPasswordReset',
            email: $('input[name="ResetEmail"]').val()
         },
         success: function( content ) {
            if (content.status == 'ok') {
                PAGwarning('Password reset request has been submitted and you should receive an email shortly. Please click the password change link in the email.');
            } else {
               my_ifware.errorDisplay('',content);
               $("#content").find("#PasswordResetSubmit").attr("disabled", false).removeClass("submit_button_disabled");
               $('input[name="ResetEmail"]').attr('disabled', false);
            }
         },
         complete: function(jqXHR, textStatus) {
            if (textStatus != "success") {
               PAGwarning("Could not connect to licensing API server.");
               $("#content").find("#PasswordResetSubmit").attr("disabled", false).removeClass("submit_button_disabled");
               $('input[name="ResetEmail"]').attr('disabled', false);
            }
            $("#content").find('#PasswordResetSpinner').hide();
         },
         dataType: "json"
      });
   });
};

my_ifware.handlePasswordChange = function(PageParams) {
   $(".OnlyBeforeLoad").hide();
   $(".HiddenOnStart").show();

   $('.LoggedIn').hide();
   my_ifware.doLogout();

   $("#content").html(
      $("#StaticPasswordChangePage").html()
   );

   $("#content").find("#PasswordChangeSubmit").click(function () {
      $("#content").find("#PasswordChangeSubmit").attr("disabled", true).addClass("submit_button_disabled");

      if ($("#content").find('input[name="NewPassword"]').val() != $("#content").find('input[name="NewPasswordVerify"]').val()) {
         PAGwarning('Password does not match.');
         $("#content").find("#PasswordChangeSubmit").attr("disabled", false).removeClass("submit_button_disabled");
         return;
      }

      $.ajax({
         type: "POST",
         url: LicensingAPI,
         data: {
            method: 'account.changePassword',
            key: PageParams.Key,
            newpassword: $("#content").find('input[name="NewPassword"]').val()
         },
         success: function( content ) {
             if (content.status == 'ok') {
               Storage.remove('ResetKey');
               PAGwarning('Password successfully updated.');
               window.location.hash = "";
            } else {
               my_ifware.errorDisplay('',content);
               $("#content").find("#PasswordChangeSubmit").attr("disabled", false).removeClass("submit_button_disabled");
            }
         },
         complete: function(jqXHR, textStatus) {
            if (textStatus != "success") {
               PAGwarning("Could not connect to licensing API server.");
               $("#content").find("#PasswordChangeSubmit").attr("disabled", false).removeClass("submit_button_disabled");
            }
         },
         dataType: "json"
      });
   });
}

my_ifware.LicenseInfoFields = [
   {
      'name': 'instance_id',
      'title': 'Machine ID',
      'product': 'Chameleon',
      'showAsColumn': true
   },
   {
      'name': 'instance_id',
      'title': 'Iguana ID',
      'product': 'Iguana',
      'showAsColumn': true
   },
   {
      'name': 'license_type',
      'title': 'Type',
      'isReadOnly': true,
      'showAsColumn': true
   },
   {
      'name': 'num_channels',
      'title': 'Channels',
      'product': 'Iguana',
      'isReadOnly': true
      //        'showAsColumn': true
   },
   {
      'name': 'description',
      'title': 'Description',
      'showAsColumn': true
   },
   {
      'name': 'tracking_id',
      'title': 'Tracking ID',
      'showAsColumn': true
   },
   {
      'name': 'site',
      'title': 'Site',
      'showAsColumn': true
   },
   {
      'name': 'city',
      'title': 'City'
      //        'showAsColumn': true
   },
   {
      'name': 'state',
      'title': 'State'
   },
   {
      'name': 'country',
      'title': 'Country'
   },
   {
      'name': 'code',
      'title': 'Code',
      'isTextarea': true,
      'isReadOnly': true
   },
   {
      'name': 'license_expiry',
      'title': 'Expiry',
      'isDate' : true,
      'isReadOnly': true,
      'showAsColumn': true
   },
   {
      'name': 'maintenance_expiry',
      'title': 'Maintenance Expiry',
      'isDate': true,
      'isReadOnly': true
   },
   {
      'name': 'date_issued',
      'title': 'Date Issued',
      'isDate' : true,
      'isReadOnly': true
   },
   {
      'name': 'date_modified',
      'title': 'Date Modified',
      'isDate' : true,
      'isReadOnly': true,
      'showAsColumn': true
   },
   {
      'name': 'last_modified_by',
      'title': 'Last Modified By',
      'isReadOnly': true
      //        'showAsColumn': true
   }
];

my_ifware.isDateField = {};
for (var i=0; i<my_ifware.LicenseInfoFields.length; i++) {
   if (my_ifware.LicenseInfoFields[i].isDate) {
      my_ifware.isDateField[my_ifware.LicenseInfoFields[i].name] = true;
   }
}

my_ifware.needsWiderField = {'instance_id': true, 'description': true, 'site': true};
my_ifware.isRequiredField = {'instance_id': true, 'description': true};

my_ifware.fnFormatDetails = function( oTable, nTr ) {
   var oData = oTable.fnGetData( nTr );

   var $sOut = $('<div class="innerDetails">' +
                 $("#LicenceActivationDrillDown").html() +
                 '</div>');

   for (var i=0; i<my_ifware.LicenseInfoFields.length; i++) {
      if (my_ifware.LicenseInfoFields[i].product &&
          my_ifware.LicenseInfoFields[i].product.toLowerCase() != PageParams.Product.toLowerCase()) {
         continue;
      }
      if (oData[my_ifware.LicenseInfoFields[i].name] == undefined) {
         continue;
      }

      var RowContent = oData[my_ifware.LicenseInfoFields[i].name];

      if (my_ifware.LicenseInfoFields[i].name == 'code') {
         RowContent = '<textarea class="registration_code" readonly="readonly" rows="1" cols="12"' +
            'onfocus="javascript:this.select();" onclick="javascript:this.select();">' +
            RowContent +
            '</textarea>';
      }

      if (my_ifware.LicenseInfoFields[i].name == 'maintenance_expiry') {
         RowContent = RowContent + $('#StaticUpdateMaintenanceSpan').html();
      }

      RowContent = '<tr><th><span class="RequiredFieldMarker" style="display: none">*&nbsp;</span>' + my_ifware.LicenseInfoFields[i].title + ':</th>' +
         '<td id="Field_' + my_ifware.LicenseInfoFields[i].name + '">' + RowContent + '</td></tr>';

      if (my_ifware.LicenseInfoFields[i].isReadOnly) {
         $sOut.find('#LicenseInfoReadOnly tbody').append(RowContent);
      } else {
         $sOut.find('#LicenseInfoEditable tbody').append(RowContent);
      }
   }
   return $sOut[0].outerHTML;
}

// Code from http://www.performantdesign.com/2009/08/26/scrollintoview-but-only-if-out-of-view/
my_ifware.scrollIntoViewIfOutOfView = function(el) {
   var topOfPage = window.pageYOffset || document.documentElement.scrollTop || document.body.scrollTop;
   var heightOfPage = window.innerHeight || document.documentElement.clientHeight || document.body.clientHeight;
   var elY = 0;
   var elH = 0;
   if (document.layers) { // NS4
      elY = el.y;
      elH = el.height;
   }
   else {
      for(var p=el; p&&p.tagName!='BODY'; p=p.offsetParent){
         elY += p.offsetTop;
      }
      elH = el.offsetHeight;
   }
   if ((topOfPage + heightOfPage) < (elY + elH)) {
      el.scrollIntoView(false);
   }
   else if (elY < topOfPage) {
      el.scrollIntoView(true);
   }
}

my_ifware.sImageUrl = "images/";
my_ifware.anOpen = [];
my_ifware.handleDrilldown = function( oTable, nTr, NoAnimation) {
   var i = $.inArray( nTr, my_ifware.anOpen );
   if ( i === -1 ) {
      $('img', this).attr( 'src', my_ifware.sImageUrl+"details_close.png" );
      var nDetailsRow = oTable.fnOpen( nTr, my_ifware.fnFormatDetails(oTable, nTr), 'details' );
      if (!NoAnimation) {
         $(nDetailsRow).find('div.innerDetails').slideDown(400, function() {
            my_ifware.scrollIntoViewIfOutOfView(this);
         });
      } else {
         $(nDetailsRow).find('div.innerDetails').show();
      }

      my_ifware.withPermission('LicenseWrite', false, function() {
         $(nDetailsRow).find(".EditDetailsLink")
            .removeClass('disabled').addClass('clickable').removeAttr('title')
            .one('click', function() {
            $(this).parents('.LicenseOperationsTable').hide();
            my_ifware.handleEditLicense(false, oTable, $(this).parents('.LicenseOperationsTable').parents('tr').prev()[0],
                                        $(this).parents('.LicenseOperationsTable').parents('tr')[0]);
            return false;
         });

         $(nDetailsRow).find(".UpdateMaintenanceLink")
            .removeClass('disabled').addClass('clickable').attr('title', 'Update maintenance expiry in license.')
            .one('click', function() {
            my_ifware.handleUpdateMaintenance(oTable, $(this).parents('.innerDetails').parents('tr').prev()[0],
                                              $(this).parents('.innerDetails').parents('tr')[0]);
            return false;
         });

         my_ifware.withPermission('LicenseTransfer', false, function() {
            $(nDetailsRow).find(".TransferLicenseLink")
               .removeClass('disabled').addClass('clickable').removeAttr('title')
               .one('click', function() {
               var $OperationsTable = $(this).parents('.LicenseOperationsTable');

               var onTransferClick = function(InProductUse) {
                  $OperationsTable.hide();
                  my_ifware.handleEditLicense(true, oTable, $OperationsTable.parents('tr').prev()[0],
                                              $OperationsTable.parents('tr')[0], InProductUse);
               };

               if (PageParams.Product.toLowerCase() == 'iguana' && Storage.get('product') && Storage.get('product').toLowerCase() == 'iguana'
                   && Storage.get('productlocation') && Storage.get('instanceid')) {
                  var InProductUse = false;
                  $('<div>Do you want to transfer this activation to your current Iguana instance?</div>')
                     .appendTo('body:first').dialog({
                     title: "Use to Activate Instance?",
                     modal: true,
                     buttons: {
                        Yes: function() {
                           InProductUse = true;
                           $(this).dialog('close');
                        },
                        No: function(){$(this).dialog('close');}
                     },
                     close: function(){
                        onTransferClick(InProductUse);
                        $(this).dialog('destroy').remove();
                     }
                  });
               } else {
                  onTransferClick(false);
               }

               return false;
            });
         });
      });

      my_ifware.anOpen.push( nTr );
   }
   else {
      $('img', this).attr( 'src', my_ifware.sImageUrl+"details_open.png" );
      if (!NoAnimation) {
         $(nTr).next().find('div.innerDetails').slideUp( function() {
            oTable.fnClose( nTr );
            my_ifware.anOpen.splice( i, 1 );
         } );
      } else {
         oTable.fnClose( nTr );
         my_ifware.anOpen.splice( i, 1 );
      }
   }
};

my_ifware.formatLicenseDate = function(DateValue) {
   if (DateValue != '' &&
       DateValue != 'none') {
      return $.datepicker.formatDate(
         "yy/mm/dd",
         my_ifware.parseDate(DateValue)
      );
   } else {
      return 'N/A';
   }
};

my_ifware.handleEditLicense = function(isLicenseTransfer, oTable, nTr, DetailsRow, InProductUse) {
   var oData = oTable.fnGetData(nTr);

   var FieldSelector = '#LicenseInfoEditable td';

   $(DetailsRow).find('#LicenseInfoEditable td').each( function(i, Elem) {
      if ($(this).attr('id') != 'Field_instance_id' || isLicenseTransfer) {
         var FieldName = $(Elem).attr('id').match(/Field_(\w*)/)[1];
         var FieldSize = 20;
         if (my_ifware.needsWiderField[FieldName]) {
            FieldSize = 32;
         }
         if (my_ifware.isRequiredField[FieldName]) {
            $(Elem).siblings().find('.RequiredFieldMarker').show();
         }
         $(Elem).siblings().find('th').css('text-align', 'right');
         $(Elem).html(
            '<input type="text" value="' + $(Elem).text() +
            '" size=' + FieldSize +' />'
         );
      }
   });

   var ProductLocation, InstanceID;
   if (InProductUse) {
      ProductLocation = Storage.get('productlocation');
      InstanceID = Storage.get('instanceid');
      if (!ProductLocation || !InstanceID) {
         InProductUse = false;
      } else {
         $(DetailsRow).find('#Field_instance_id input').val(InstanceID)
            .attr('disabled', true);

         $(DetailsRow).find("#LicenseEditSave").val("Save and Activate Current Instance");
      }
   }

   $(DetailsRow).find('#LicenseEditCancel').click(function() {
      $(DetailsRow).find('#LicenseInfoEditable td').each( function(i, Elem) {
         if ($(this).attr('id') != 'Field_instance_id' || isLicenseTransfer) {
            $(Elem).siblings().find('.RequiredFieldMarker').hide();
            $(Elem).html(
               $(Elem).find('input').val()
            );
         }
      });
      $(DetailsRow).find('#LicenseEditButtons').hide();

      my_ifware.handleDrilldown(oTable, nTr, true); //close drill-down
      my_ifware.handleDrilldown(oTable, nTr, true); //re-open drill-down
   });
   $(DetailsRow).find('#LicenseEditSave').click(function() {
      var vals = {
         method: 'license.updateActivationInfo',
         token: Storage.get('Token'),
         product: PageParams.Product,
         activationid: oData['activation_id']
      };
      $(DetailsRow).find('#LicenseInfoEditable td').each( function(i, Elem) {
         var FieldName = $(Elem).attr('id').match(/Field_(\w*)/)[1];
         vals[FieldName] = $(Elem).find('input').val();
      });

      $(DetailsRow).find("#LicenseEditSave").hide();
      $(DetailsRow).find("#LicenseEditSpinner").show();

      $.ajax({
         type: "POST",
         url: LicensingAPI,
         data: vals,
         success: function( content ) {
            if (content.status == 'ok'){
               //Can't hurt to clear this even if we're not activating directly.
               Storage.remove('instanceid');
               Storage.remove('productlocation');
               Storage.remove('product');
               Storage.remove('version');

               if (InProductUse) {
                  var ActivationURL = my_ifware.getIguanaActivationURL(ProductLocation, content.data.code);
                  window.location.href = ActivationURL;
                  return;
               }

               var UpdatedRow = {};
               jQuery.extend(UpdatedRow,oData); //make a copy for editing
               for (var k in content.data) {
                  if (content.data.hasOwnProperty(k)) {
                     if (my_ifware.isDateField[k] == true) {
                        UpdatedRow[k] = my_ifware.formatLicenseDate(content.data[k]);
                     } else {
                        UpdatedRow[k] = content.data[k];
                     }
                  }
               }
               //Update, but don't redraw the table as that would cause sorting to potentially move the row.
               oTable.fnUpdate(UpdatedRow, nTr, undefined, false, false);

               my_ifware.handleDrilldown(oTable, nTr, true); //close drill-down
               my_ifware.handleDrilldown(oTable, nTr, true); //re-open drill-down
            } else {
               $(DetailsRow).find("#LicenseEditSave").show();
               my_ifware.errorDisplay('Could not update activation info.', content);
            } 
         }, 
         complete: function(jqXHR, textStatus) {
            if (textStatus != 'success') {
               $(DetailsRow).find("#LicenseEditSave").show();
               my_ifware.errorDisplay('Could not connect to license server.');
            }
            $(DetailsRow).find('#LicenseEditSpinner').hide();
         },
         dataType: "json"
      });

   });

   $(DetailsRow).find('#LicenseEditButtons').show();
};

my_ifware.handleUpdateMaintenance = function(oTable, nTr, DetailsRow) {
   var oData = oTable.fnGetData(nTr);

   $(DetailsRow).find(".UpdateMaintenanceLink").hide();
   $(DetailsRow).find(".UpdateMaintenanceSpinner").show();

   $.ajax({
      type: "POST",
      url: LicensingAPI,
      data: {
         method: 'license.updateMaintenance',
         token: Storage.get('Token'),
         product: PageParams.Product,
         activationid: oData['activation_id']
      },
      success: function( content ) {
         if (content.status == 'ok') {
            var UpdatedRow = {};
            jQuery.extend(UpdatedRow,oData); //make a copy for editing
            for (var k in content.data) {
               if (content.data.hasOwnProperty(k)) {
                  if (my_ifware.isDateField[k] == true) {
                     UpdatedRow[k] = my_ifware.formatLicenseDate(content.data[k]);
                  } else {
                     UpdatedRow[k] = content.data[k];
                  }
               }
            }
             //Update, but don't redraw the table as that would cause sorting to potentially move the row.
            oTable.fnUpdate(UpdatedRow, nTr, undefined, false, false);
            my_ifware.handleDrilldown(oTable, nTr, true); //close drill-down
            my_ifware.handleDrilldown(oTable, nTr, true); //re-open drill-down
         } else {
            $(DetailsRow).find(".UpdateMaintenanceLink").show();
            $(DetailsRow).find(".UpdateMaintenanceSpinner").hide();
            my_ifware.errorDisplay('Could not update maintenance expiry.', content);
         }
     },
     complete: function(jqXHR, textStatus) {
         if (textStatus != 'success') {
            $(DetailsRow).find(".UpdateMaintenanceLink").show();
            $(DetailsRow).find(".UpdateMaintenanceSpinner").hide();
            my_ifware.errorDisplay('Could not connect to license server.');
         }
     },
      dataType: "json"
   });
};

my_ifware.handleViewActivations = function(PageParams) {
    my_ifware.accessWithLogin( function() {
    my_ifware.InitializePage();
    $("#bread_crumb_specific").html(' &#8250; <a href="#Page=licenses">Licenses</a> &#8250; Activations <span class="ActivationTitle"></span>');

    $('#content').html(
        $('#EmptyViewActivationsPage').html()
    );

    var vals = {
        method: 'license.listActivations',
        token: Storage.get('Token'),
        product: PageParams.Product
    };

    if (PageParams.LicenseID) {
        vals.entitlementid = PageParams.LicenseID;
    }

    $.ajax({
        type: "POST",
        url: LicensingAPI,
        data: vals,
        success: function( content ) {
            if (content.status == 'ok') {
                var aoColumns = [{
                    "mData": null,
                    "sClass": "control center",
                    "sDefaultContent": '<img src="'+my_ifware.sImageUrl+'details_open.png'+'">',
                    "bSortable": false,
                    "sWidth": "5%"
                }];

                var ColumnsForExport = []; //Should be everything except the first "control" column
                var IndexOfDateCol = 1;

                var iStart = 0;
                if(PageParams.Product.toLowerCase() == "iguana") {
                  iStart = 1;
                }
                for (var i=iStart; i<my_ifware.LicenseInfoFields.length; i++) {
                //for (var i=1; i<my_ifware.LicenseInfoFields.length; i++) {
                    if (my_ifware.LicenseInfoFields[i].product &&
                        my_ifware.LicenseInfoFields[i].product.toLowerCase() != PageParams.Product.toLowerCase()) {
                        continue;
                    }
                    if (content.data.length > 0 && content.data[0][my_ifware.LicenseInfoFields[i].name] == undefined) {
                        continue;
                    }

                    if (my_ifware.LicenseInfoFields[i].isDate) {
                        for (var j=0; j<content.data.length; j++) {
                            content.data[j][my_ifware.LicenseInfoFields[i].name] =
                                my_ifware.formatLicenseDate(content.data[j][my_ifware.LicenseInfoFields[i].name]);
                        }
                    }

                    var CurColumn = {
                        'sTitle' : my_ifware.LicenseInfoFields[i].title,
                        'mData' : my_ifware.LicenseInfoFields[i].name
                    };

                    if (!my_ifware.LicenseInfoFields[i].showAsColumn) {
                        CurColumn.bVisible = false;
                    }

                    //ColumnsForExport.push(i);
                    aoColumns.push(CurColumn);
                    ColumnsForExport.push(aoColumns.length-1);

                    if (CurColumn.mData == 'date_modified') {
                        IndexOfDateCol = aoColumns.length - 1;
                    }
                }

                if (PageParams.LicenseID && content.data[0].license_type) {
                    $("#content").find("#ActivationListTitle").html(content.data[0].license_type + ' Licenses (' + content.data.length + ')');
                    $(".ActivationTitle").html('- ' + content.data[0].license_type);
                } else {
                    $("#content").find("#ActivationListTitle").html(PageParams.Product + ' Licenses (' + content.data.length + ')');
                    $(".ActivationTitle").html('- ' + PageParams.Product);
                }

                var oTableTools;
                if (my_ifware.hasFlash) {
                    oTableTools = {
                        "sSwfPath": 'copy_csv_xls.swf',
                        "aButtons": [
                            {
                                "sExtends": "csv",
                                "sButtonText": "Save to CSV",
                                "mColumns": ColumnsForExport,
                                "sTitle": 'activations'
                            }
                        ]
                    };
                } else {
                    oTableTools = {
                        "aButtons": [
                            {
                                "sExtends":    "text",
                                "sButtonText": "Save to CSV",
                                "sButtonClass": "disabled_save_csv",
                                "sToolTip": "This feature requires Adobe Flash to be installed and enabled for this site."
                            }
                        ]
                    };
                }

                var oTable = $("#content").find("#ActivationListTable").dataTable({
                    "aaData": content.data,
                    "bProcessing": true,
                    "aoColumns": aoColumns,
                    "bFilter": true, //false,
                    //"bAutoWidth": true,
                    "bAutoWidth": false,
                    "bPaginate": false,
                    "aaSorting": [[ IndexOfDateCol, "desc" ]],
                    "sDom": 'T<"clear">ifrt',
                    "oTableTools": oTableTools
                });

                $("#content").find('#ActivationListTable tbody tr').on( 'click', function() {
                    my_ifware.handleDrilldown(oTable, this);
                });
            } else {
                my_ifware.errorDisplay('Could not get activations.', content);
            }
        },
        complete: function(jqXHR, textStatus) {
            if (textStatus != 'success') {
                my_ifware.errorDisplay('Could not connect to license server.');
            }
            $("#content").find('#ActivationListSpinner').remove();
        },
        dataType: "json"
    });
    });
};

my_ifware.handleIguanaTrialActivation = function(PageParams) {
   $('#content').html(
      $('#StaticTrialActivationContent').html()
   );

   var $form = $("#content").find("#RegistrationForm");

   //If browser does support placeholder text, we can remove the labels.
   if (document.createElement('input').placeholder != undefined) {
      $form.find('label').hide();
   }

   var instanceid = Storage.get('instanceid'),
       productlocation = Storage.get('productlocation'),
       product = Storage.get('product'),
       version = Storage.get('version');

   if (!instanceid || !productlocation || !version
       || product.toLowerCase() != 'iguana') {
      my_ifware.errorDisplay("This page must be loaded via a link inside Iguana.");
      return;
   }

   $("#content").find('.IguanaID').text(instanceid);

   $("#content").find("#RegistrationSubmit").click(function(e) {
      $("#content").find("#RegistrationSubmit").attr("disabled", true).addClass("submit_button_disabled");
      $("#content").find("#TrialRegistrationSpinner").show();

      var $form = $("#content").find("#RegistrationForm");

      var vals = {
         method: 'account.createTrial',
         email: $form.find( 'input[name="Email"]' ).val(),
         firstname: $form.find( 'input[name="FirstName"]' ).val(),
         lastname: $form.find( 'input[name="LastName"]' ).val(),
         company: $form.find( 'input[name="Company"]' ).val(),
         product: 'Iguana',
         instanceid: instanceid,
         description: $form.find( 'input[name="Description"]' ).val()
      };

      if ($form.find( 'input[name="Email"]' ).val() != $form.find( 'input[name="Email2"]' ).val()) {
         PAGwarning('Email fields do not match.');
         $("#content").find("#RegistrationSubmit").attr("disabled", false).removeClass("submit_button_disabled");
         $("#content").find("#TrialRegistrationSpinner").hide();
         e.preventDefault();
         return;
      }

      if (!vals.instanceid || !vals.description || !vals.email || !vals.firstname || !vals.lastname) {
         $("#content").find("#RegistrationSubmit").attr("disabled", false).removeClass("submit_button_disabled");
         $("#content").find("#TrialRegistrationSpinner").hide();
         PAGwarning('Required field(s) missing.');
         e.preventDefault();
         return;
      }

      $.ajax({
         type: "POST",
         url: LicensingAPI,
         data: vals,
         success: function( content ) {
               if (content.status != 'ok') {
                  $("#content").find("#RegistrationSubmit").attr("disabled", false).removeClass("submit_button_disabled");
                  my_ifware.errorDisplay("Could not register.", content);
               } else {               
                  analytics.identify(vals.email, {
                  email: vals.email,
                  firstName: vals.firstname,
                  lastName: vals.lastname,
                  company: vals.company,
                  page:'account.createTrial'
               });
               analytics.track('Created trial account');
               $("#content").html(
                  $("#StaticTrialRegisteredPage").html()
               );

               $("#content").find("#summary_maintenance_expiry").html(
                  content.data.maintenance_expiry == "none" ? "N/A" : content.data.maintenance_expiry
               );
               $("#content").find("#summary_numchannels").html(content.data.num_channels);

               $("#content").find("#summary_instanceid").html(content.data.instanceid);
               $("#content").find("#summary_code").html(content.data.code);
               $("#content").find("#summary_license_expiry").html(
                  content.data.license_expiry == "none" ? "N/A" : content.data.license_expiry
               );
               $("#content").find("#summary_description").html(content.data.description);

               $("#content").find("#successful_registration_buttons").html(
                  '<form><input id="GoIntoProduct" class="submit_button" type="button" value="Activate inside Iguana" /></form>'
               );

               var ActivationURL = my_ifware.getIguanaActivationURL(productlocation, content.data.code);
               $("#content").find("#GoIntoProduct").click(function() {
                     window.location.href = ActivationURL;
                     return false;
                  }
               );

               Storage.remove('instanceid');
               Storage.remove('productlocation');
               Storage.remove('product');
               Storage.remove('version');
            }
         },
         // "#content").find("#TrialRegistrationSpinner").hide();
         complete: function(jqXHR, textStatus) {
            $("#content").find("#TrialRegistrationSpinner").hide();
            if (textStatus != 'success') {
               $("#content").find("#RegistrationSubmit").attr("disabled", false).removeClass("submit_button_disabled");
               PAGwarning('Could not connect to license server.');
            }
         },
         dataType: "json"
      });
   });
};
