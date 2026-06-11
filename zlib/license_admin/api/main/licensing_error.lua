licensing_error = {
   InternalError = {code = 101, desc = 'Internal error.'},
   InvalidParams = {code = 102, desc = 'Invalid parameters.'},
   MethodNotFound = {code = 103, desc = 'Method does not exist.'},
   MethodDeprecated = {code = 104, desc = 'Method deprecated.'},
   ServiceUnavailable = {code = 105, desc = 'Service unavailable.'},
   InvalidRequest = {code = 106, desc = 'Invalid HTTP request.'},
   
   OtherError = {code = 200, desc = ''},
   TokenInvalid = {code = 201, desc = 'Session token invalid or expired.'},
   AuthFailed = {code = 202, desc = 'Authentication failed.'},
   AccountNotActivated = {code = 203, desc = 'Account has not been activated.'},
   AccountCantCreate = {code = 204, desc = 'Cannot create account.'},
   InsufficientPermissions = {code = 205, desc = 'Insufficient permissions.'},
   SupportExpired = {code = 206, desc = 'Support/maintenance payment not current.'},
   OutOfLicenses = {code = 207, desc = 'No such entitlement or insufficient licenses in entitlement.'},
   AccountCantActivate = {code = 208, desc = 'Cannot activate account.'},
   KeyInvalid = {code = 209, desc = 'Key invalid or expired.'}
}

function throwError(Err, Details)
   if not Err then Err = licensing_error.InternalError end      
   
   error{code = Err.code, desc = Err.desc, details = Details}   
end