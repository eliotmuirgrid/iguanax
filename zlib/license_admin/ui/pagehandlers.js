/** @license
 * Copyright (c) 2010-2013 iNTERFACEWARE Inc.  All rights reserved.
 */
var PageHandler = {}

PageHandler.pageLookup = {
    "": ifware_admin.handleIndex, //default
    login: ifware_admin.handleIndex,
    company: ifware_admin.handleCompany,
    logout: ifware_admin.handleLogout
};

PageHandler.defaultRequest = function(ErrorMessage){
    PAGwarning(ErrorMessage);
}

PageHandler.renderError = function(ErrorMessage, Params){
    PAGwarning(ErrorMessage);
}
