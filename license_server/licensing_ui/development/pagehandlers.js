/**
 * Copyright (c) 2010-2013 iNTERFACEWARE Inc.  All rights reserved.
 */
var PageHandler = {}

PageHandler.pageLookup = {
    "": my_ifware.handleSplash, //default
    login: my_ifware.handleIndex,
    createaccount: my_ifware.handleCreateAccount,
    activateaccount: my_ifware.handleActivateAccount,
    licenses: my_ifware.handleLicenses,
    getlicense: my_ifware.handleGetLicense,
    activate: my_ifware.handleActivate,
    logout: my_ifware.handleLogout,
    invoices: my_ifware.handleInvoices,
    profile: my_ifware.handleProfile,
    passwordreset: my_ifware.handlePasswordReset,
    setpassword: my_ifware.handlePasswordChange,
    viewactivations: my_ifware.handleViewActivations,
    trialactivation: my_ifware.handleIguanaTrialActivation
};

PageHandler.defaultRequest = function(ErrorMessage){
    PAGwarning(ErrorMessage);
}

PageHandler.renderError = function(ErrorMessage, Params){
    PAGwarning(ErrorMessage);
}
