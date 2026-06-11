if (!ifware_admin) var ifware_admin = {};

if (!LicenseAdminAPI) { //Assume a standard API location of '/api-admin'.
    var url = $.url(window.location.href);
    var LicenseAdminAPI = url.attr('protocol') + "://" + url.attr('host') +
            (url.attr('port') ? ":" + url.attr('port') : "") + "/api-admin";
}
ifware_admin.errorDisplay = function(message, content, OnClose) {
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
}

ifware_admin.withPermission = function(PermissionName, DisplayErrors, onSuccess, onFail) {
    var Permissions = Storage.getObject('Permissions');
    if (Permissions == undefined) {
        ifware_admin.errorDisplay('Error in determining permissions.', content);
        if (onFail) { onFail(); }
    } else if (Permissions[PermissionName] == true) {
        if (onSuccess) { onSuccess(); }
    } else {
        if (DisplayErrors) {
            PAGwarning('Need to have ' + PermissionName + ' permission to proceed.');
        }
        if (onFail) { onFail(); }
    }
}

ifware_admin.InitializePage = function() {
    $(".OnlyBeforeLoad").hide();
    $(".HiddenOnStart").show();
    if (Storage.get('Token')) {
        $("#HeaderInfo").show();
    }
    ifware_admin.withPermission('FinancialRead', false, function() {
        $(".AccountHistorySections").show();
    }, function(){
        $(".AccountHistorySections").hide();
    });
}

// http://stackoverflow.com/a/2587398
// parse a date in yyyy-mm-dd format
ifware_admin.parseDate = function(input) {
    var parts = input.match(/(\d+)/g);
    // new Date(year, month [, date [, hours[, minutes[, seconds[, ms]]]]])
    return new Date(parts[0], parts[1]-1, parts[2]); // months are 0-based
}

ifware_admin.doLogin = function(afterLogin) {
    $(".OnlyBeforeLoad").hide();
    $(".HiddenOnStart").show();
    $('#HeaderInfo').hide();

    $("#content").html($("#StaticLoginWarning").html() + $("#StaticLoginPage").html());

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

        password = CryptoJS.MD5(password).toString(CryptoJS.enc.Hex);

        var posting = $.ajax({
            type: "POST",
            url: LicenseAdminAPI,
            data: {   method: 'session.login',
                username: username,
                password: password},
            success: function( content ) {
                console.log(content);
                if (content.status == 'ok') {
                    Storage.set('Token',content.data.Token, 1);
                    Storage.setObject('Permissions', content.data.Info.Permissions, 1);
                    afterLogin();
                } else {
                    ifware_admin.errorDisplay('Could not log in.', content);
                }
            },
            error: function( jqXHR, textStatus, errorThrown) {
                PAGwarning('Could not connect to server.');
            },
            dataType: "json"
        });
    });
}

//checks whether current session token is valid
//if not, presents log-in
//if it logs in, executes afterLogin(); else redirects to main page
ifware_admin.accessWithLogin = function(afterLogin, IsSplashPage) {
    if (!Storage.get('Token')) {
        ifware_admin.doLogin(afterLogin, IsSplashPage);
    } else {
        $.ajax({
            type: "POST",
            url: LicenseAdminAPI,
            data: {
                method: 'session.checktoken',
                token: Storage.get('Token')
            },
            success: function( content ) {
                console.log(content);
                if (content.status == 'ok') {
                    afterLogin();
                } else {
                    ifware_admin.doLogin(afterLogin, IsSplashPage);
                }
            },
            complete: function(jqXHR, textStatus) {
                if (textStatus != "success") {
                    PAGwarning("Could not connect to server.",function() {
                        window.location.hash="";
                    });
                }
            },
            dataType: "json"
        });
    }
}

ifware_admin.handleLogout = function(PageParams) {
    $.ajax({
        type: "POST",
        url: LicenseAdminAPI,
        data: {
            method: 'session.logout',
            token: Storage.get('Token')
        },
        success: function( content ) {
            console.log(content);
        },
        complete: function(jqXHR, textStatus) {
            console.log(textStatus);
            Storage.remove('Token');
            window.location.hash = "";
        },
        dataType: "json"
    });
}

ifware_admin.doCompanySearch = function(SearchArgs) {
    var vals = {
        method: 'companies.getinfo',
        token: Storage.get('Token')
    };

    if (SearchArgs.Name != '') {
        vals.name = SearchArgs.Name;
    }
    if (SearchArgs.Keyword != '') {
        vals.keyword = SearchArgs.Keyword;
    }
    if (SearchArgs.CompanyID != '') {
        vals.companyid = SearchArgs.CompanyID;
    }

    if (!(vals.name || vals.keyword || vals.companyid)) {
        return;
    }

    $.ajax({
        type: "POST",
        url: LicenseAdminAPI,
        data: vals,
        success: function( content ) {
            console.log(content);
            if (content.status != 'ok') {
                ifware_admin.errorDisplay("", content);
            } else {
                $("#content").find("#Companies").html('');

                if (content.data.length == 0) {
                    PAGwarning('No results found.');
                    return;
                }

                var ColumnNames = ['CompanyID','Name','MaintenanceInvoiceDate','SupportType','TemporaryLicensePermission','LicenseTransferPermission'];
                var aoColumns = [];

                for (var i=0; i<ColumnNames.length; i++) {
                    aoColumns.push({
                        'sTitle' : ColumnNames[i],
                        'mData' : ColumnNames[i]
                    });
                }

                var oTable = $("#content").find("#Companies").dataTable({
                    "aaData": content.data,
                    "aoColumns": aoColumns,
                    "bProcessing": true,
                    "bFilter": true, //false,
                    "bAutoWidth": true,
                    "bPaginate": false,
                    "sDom": 'ifrt'
                });

                $("#content").find('#Companies tbody tr').on( 'click', function() {
                    var oData = oTable.fnGetData( this );

                    window.location.hash = "#Page=company&CompanyID=" + oData.CompanyID;
                });
            }

        },
        complete: function(jqXHR, textStatus) {
            console.log(textStatus);
            if (textStatus != 'success') {
                PAGwarning('Could not connect to license server.');
            }
        },
        dataType: "json"
    });
}

ifware_admin.handleIndex = function(PageParams) {
    ifware_admin.accessWithLogin( function() {
        ifware_admin.InitializePage();

        $("#content").html(
            $("#StaticIndexPage").html()
        );

        $('#content').find( 'input[name="name"]' ).val(PageParams.Name);
        $('#content').find( 'input[name="keyword"]' ).val(PageParams.Keyword);
        $('#content').find( 'input[name="companyid"]' ).val(PageParams.CompanyID);

        ifware_admin.doCompanySearch(PageParams);

        $('#content').find('#CompanySearchSubmit').click(function() {
            window.location.hash = PAGhref("",{
                Name: $('#content').find( 'input[name="name"]' ).val(),
                Keyword: $('#content').find( 'input[name="keyword"]' ).val(),
                CompanyID: $('#content').find( 'input[name="companyid"]' ).val()
            });
        });
    });
}

ifware_admin.handleUpdatePermissions = function(PageParams) {
    var vals = {
        method: 'companies.updatepermissions',
        token: Storage.get('Token'),
        companyid: PageParams.CompanyID
    };
    vals.TemporaryLicense = $("#content")
        .find("#TemporaryLicensePermission").prop("checked");
    vals.LicenseTransfer = $("#content")
        .find("#LicenseTransferPermission").prop("checked");

    $.ajax({
        type: "POST",
        url: LicenseAdminAPI,
        data: {
            method: 'companies.updatepermissions',
            token: Storage.get('Token'),
            companyid: PageParams.CompanyID
        },
        success: function( content ) {
            console.log(content);
            if (content.status != 'ok') {
                ifware_admin.errorDisplay("Failed to update permissions.", content);
            } else {
                PAGwarning('Successfully updated permissions');
            }

        },
        complete: function(jqXHR, textStatus) {
            console.log(textStatus);
            if (textStatus != 'success') {
                PAGwarning('Could not connect to license server.');
            }
        },
        dataType: "json"
    });
}

ifware_admin.getUsageTable = function(AjaxArgs, ColumnNames, $Elem) {
    $.ajax({
        type: "POST",
        url: LicenseAdminAPI,
        data: AjaxArgs,
        success: function( content ) {
            console.log(content);
            if (content.status != 'ok') {
                ifware_admin.errorDisplay("", content);
            } else {
                var aoColumns = [];

                for (var i=0; i<ColumnNames.length; i++) {
                    aoColumns.push({
                        'sTitle' : ColumnNames[i],
                        'mData' : ColumnNames[i]
                    });
                }

                var oTable = $Elem.dataTable({
                    "aaData": content.data,
                    "aoColumns": aoColumns,
                    "bProcessing": true,
                    "bFilter": true, //false,
                    "bAutoWidth": true,
                    "bPaginate": false,
                    "sDom": 'ifrt',
                    "aaSorting": [[ 0, "desc" ]],
                });
            }
        },
        complete: function(jqXHR, textStatus) {
            console.log(textStatus);
            if (textStatus != 'success') {
                PAGwarning('Could not connect to license server.');
            }
        },
        dataType: "json"
    });
}

ifware_admin.handleCompany = function(PageParams) {
    ifware_admin.accessWithLogin( function() {
        ifware_admin.InitializePage();

        $("#content").html(
            $("#StaticCompanyPage").html()
        );

        $.ajax({
            type: "POST",
            url: LicenseAdminAPI,
            data: {
                method: 'companies.getinfo',
                token: Storage.get('Token'),
                companyid: PageParams.CompanyID
            },
            success: function( content ) {
                console.log(content);
                if (content.status != 'ok') {
                    ifware_admin.errorDisplay("", content);
                } else {
                    $("#content").find("#CompanyNameSection").html(
                        "<h3>" + content.data[0].Name + "</h3>"
                    );

                    $("#content").find('#TemporaryLicensePermission')
                        .prop("checked",content.data[0].TemporaryLicensePermission);
                    $("#content").find('#LicenseTransferPermission')
                        .prop("checked",content.data[0].LicenseTransferPermission);

                    $("#content").find("#CompanyPermissions").show();

                    $("#content").find("#CompanyPermissionsSubmit").click(function() {
                        var vals = {
                            method: 'companies.updatepermissions',
                            token: Storage.get('Token'),
                            companyid: PageParams.CompanyID
                        };
                        vals.TemporaryLicense = $("#content")
                            .find("#TemporaryLicensePermission").prop("checked");
                        vals.LicenseTransfer = $("#content")
                            .find("#LicenseTransferPermission").prop("checked");

                        $.ajax({
                            type: "POST",
                            url: LicenseAdminAPI,
                            data: vals,
                            success: function( content ) {
                                console.log(content);
                                if (content.status != 'ok') {
                                    ifware_admin.errorDisplay("Failed to update permissions.", content);
                                } else {
                                    PAGwarning('Successfully updated permissions');
                                }

                            },
                            complete: function(jqXHR, textStatus) {
                                console.log(textStatus);
                                if (textStatus != 'success') {
                                    PAGwarning('Could not connect to license server.');
                                }
                            },
                            dataType: "json"
                        });
                    });
                }

            },
            complete: function(jqXHR, textStatus) {
                console.log(textStatus);
                if (textStatus != 'success') {
                    PAGwarning('Could not connect to license server.');
                }

                $("#content").find("#CompanyPermissionsSpinner").hide();
            },
            dataType: "json"
        });

        ifware_admin.getUsageTable({
            method: 'usage.temporarylicense',
            token: Storage.get('Token'),
            companyid: PageParams.CompanyID,
            product: 'Iguana'
        }, ['Timestamp','IndividualLastName','InstanceID','Description'],
            $('#content').find('#UsageTemporaryLicensesIguana'));

        ifware_admin.getUsageTable({
                method: 'usage.temporarylicense',
                token: Storage.get('Token'),
                companyid: PageParams.CompanyID,
                product: 'Chameleon'
            }, ['Timestamp','IndividualLastName','InstanceID','Description'],
            $('#content').find('#UsageTemporaryLicensesChameleon'));

        ifware_admin.getUsageTable({
                method: 'usage.licensetransfer',
                token: Storage.get('Token'),
                companyid: PageParams.CompanyID,
                product: 'Iguana'
            }, ['Timestamp','IndividualLastName','OldInstanceID','NewInstanceID'],
            $('#content').find('#UsageLicenseTransfersIguana'));

        ifware_admin.getUsageTable({
                method: 'usage.licensetransfer',
                token: Storage.get('Token'),
                companyid: PageParams.CompanyID,
                product: 'Chameleon'
            }, ['Timestamp','IndividualLastName','OldInstanceID','NewInstanceID'],
            $('#content').find('#UsageLicenseTransfersChameleon'));
    });
}