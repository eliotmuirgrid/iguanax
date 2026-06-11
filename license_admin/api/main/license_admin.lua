license_admin = {}

admin_config = loadfile('license_admin_config.lua')()

require 'ifware_license'
require 'license_admin_utils'
require 'licensing_error'

function license_admin.getSession(params)
   if not params.Token then
      throwError(licensing_error.InvalidParams, 'Missing required parameter: Token.')
   end
   
   local ok, res = pcall(session.getSession, params.Token)      
   if not ok then
      throwError(licensing_error.TokenInvalid)
   else
      return res
   end   
end

function license_admin.login(params)
   if params.Username == '' then
      throwError(licensing_error.AuthFailed, 'Invalid username or password.')
   end
   
   local UserInfo = {}   
   
   local conn = initDb()
   conn:pTransaction(function()         
         local restable = conn:query(
            "SELECT * FROM Users U " ..
            "WHERE U.LoginID = "
            .. conn:quote(params.Username)
         )
         
         if #restable == 0
            or restable[1].Password:nodeValue() ~= params.Password then
            throwError(licensing_error.AuthFailed, 'Invalid username or password.')
         elseif #restable > 1 then
            logIssue('Error', 'Database error - email associated with multiple accounts/individuals.', {params = params})
            throwError(licensing_error.InternalError, 'Operation could not be completed due to a database error')  
         end
         
         conn:execute(SQLupdateFromTable('Users',
               {LastLogin = os.date('%Y-%m-%d %H:%M:%S'),
               SecondLastLogin = restable[1].LastLogin:nodeValue()},
               {LoginID = restable[1].LoginID:nodeValue()}
            ))                  
         
         UserInfo.UserID = restable[1].UserID:nodeValue()
         UserInfo.LoginID = restable[1].LoginID:nodeValue()
         UserInfo.LastName = restable[1].LastName:nodeValue()
      end
   )   
       
   local Session = session.newSession(UserInfo)         
   return {Token = Session.Token, Info = license_admin.getAccountInfo({Token = Session.Token}, Session)}
end

function license_admin.logout(params)
   session.clearSession(params.Token)
   return {}
end

--The token-checking has already happened before the method being called.
function license_admin.checkToken(params)   
   return {}
end

--Must be done inside a pTransaction call.
local function valueInTable(TableName, ColumnName, Value)
   local conn = initDb()
      
   local restable = conn:query(
      'SELECT * FROM '
      .. TableName ..
      ' T WHERE T.' .. ColumnName .. 
      ' = ' .. conn:quote(Value)
   )
   
   return (#restable > 0)
end

function license_admin.getPermissions(params, Session)     
   local Permissions = {
      --here as a demo - not the full list
      FinancialRead = hasPermission(Session, 'FinancialRead'),
      FinancialWrite = hasPermission(Session, 'FinancialWrite')
   }   
   
   return Permissions
end

function license_admin.getAccountInfo(params, Session)       
   return {Permissions = license_admin.getPermissions(params, Session)}
end

--Only getting one of CompanyID, Keyword, or Name
function license_admin.getCompanyInfo(params, Session)       
   local conn = initDb()
   params.CompanyID = tonumber(params.CompanyID)
   
   local sql = [[SELECT
   CompanyID,
   Name,
   MaintenanceInvoiceDate,
   SupportType,
   LicenseTransferPermission,
   TemporaryLicensePermission
   FROM Companies C ]]
   if params.CompanyID or params.Keyword or params.Name then
      sql = sql .. [[WHERE ]]
   end
   
   if params.CompanyID then
      sql = sql .. 'CompanyID = ' .. params.CompanyID
   elseif params.Keyword then      
      sql = sql .. 'Name LIKE ' ..
      conn:quote('%' .. params.Keyword .. '%')
   elseif params.Name then
      sql = sql .. 'Name = ' .. conn:quote(params.Name)
   end
   
   local restable = conn:pQuery(sql)
   
   local ResultTable = resultSetToTable(restable)
   
   for i=1,#restable do
      if ResultTable[i].MaintenanceInvoiceDate == '0000-00-00 00:00:00' then
         ResultTable[i].MaintenanceInvoiceDate = ''
      end
      ResultTable[i].TemporaryLicensePermission = (ResultTable[i].TemporaryLicensePermission == '1')
      ResultTable[i].LicenseTransferPermission = (ResultTable[i].LicenseTransferPermission == '1')
   end
     
   return ResultTable   
end

function license_admin.updateCompanyPermissions(params, Session)   
   local conn = initDb()
   params.CompanyID = tonumber(params.CompanyID)
   
   local NewValues = {}
   if params.LicenseTransfer == 'false' then
      NewValues.LicenseTransferPermission = 0
   elseif params.LicenseTransfer == 'true' then
      NewValues.LicenseTransferPermission = 1
   end
   if params.TemporaryLicense == 'false' then
      NewValues.TemporaryLicensePermission = 0
   elseif params.TemporaryLicense == 'true' then
      NewValues.TemporaryLicensePermission = 1
   end   
   
   if NewValues == {} then return {} end
   
   conn:pTransaction(function() 
         conn:execute(
            SQLupdateFromTable('Companies', NewValues,
               {CompanyID = params.CompanyID})
         )
      end
   )
         
   return {}
end

function license_admin.usageTemporaryLicense(params, Session)   
   local conn = initDb()
   params.CompanyID = tonumber(params.CompanyID)
   
   local TableName
   if params.Product:lower() == 'iguana' then
      TableName = 'CompanyTemporaryNewRegistrations'
   elseif params.Product:lower() == 'chameleon' then
      TableName = 'CompanyTemporaryRegistrations'
   else
      throwError(licensing_error.InvalidParams, "Product must be 'Iguana' or 'Chameleon'.")
   end   
   
   local res = conn:pQuery([[SELECT
      CompanyID,
      IndividualLastName as IndividualLastName,
      MachineID as InstanceID,
      Created as Timestamp,
      Description
      FROM ]] .. TableName .. ' WHERE CompanyID = '
      .. params.CompanyID .. ' ORDER BY Created DESC')
   
   return resultSetToTable(res)   
end

function license_admin.usageLicenseTransfer(params, Session)   
   local conn = initDb()
   params.CompanyID = tonumber(params.CompanyID)
   
   local res
   if params.Product:lower() == 'iguana' then
      res = conn:pQuery([[
         SELECT         
         H.OldMachineID as OldInstanceID,
         H.NewMachineID as NewInstanceID,
         H.OldEntitlement as OldLicenseType,
         H.NewEntitlement as NewLicenseType,
         H.Modified as Timestamp,
         H.IndividualLastName as IndividualLastName
         FROM CompanyNewRegistrationHistory H
         JOIN CompanyNewRegistration R
         ON H.NewRegistrationID = R.NewRegistrationID
         WHERE R.CompanyID = ]]
         .. params.CompanyID .. [[ AND 
         H.IndividualLastName IS NOT NULL
         ORDER BY Modified
         DESC]])      
   elseif params.Product:lower() == 'chameleon' then
      res = conn:pQuery([[
         SELECT
         H.OldMachineID as OldInstanceID,
         H.NewMachineID as NewInstanceID,
         H.OldRegistrationType as OldLicenseType,
         H.NewRegistrationType as NewLicenseType,
         H.Modified as Timestamp,
         H.IndividualLastName as IndividualLastName
         FROM CompanyRegistrationHistory H
         JOIN CompanyRegistration R
         ON H.RegistrationID = R.RegistrationID
         WHERE R.CompanyID = ]]
         .. params.CompanyID .. [[ AND 
         H.IndividualLastName IS NOT NULL
         ORDER BY Modified
         DESC]])      
   else
      throwError(licensing_error.InvalidParams, "Product must be 'Iguana' or 'Chameleon'.")
   end   
   
   return resultSetToTable(res)   
end












