session_config = loadfile('license_admin_session_config.lua')()

require 'license_admin'
require 'session'
require 'licensing_error'

-- Global settings
iguana.stopOnError(false)

local methodInfo = {   
   ['session.login'] = {
      method = license_admin.login,
      required = {'Username','Password'}
   },
   ['session.logout'] = {
      method = license_admin.logout,
      required = {'Token'}
   },
   ['session.checktoken'] = {
      method = license_admin.checkToken,
      needsession = true,
      required = {'Token'}
   },
   ['companies.getinfo'] = {
      method = license_admin.getCompanyInfo,
      needsession = true,
      required = {'Token'},      
      optional = {'CompanyID','Name','Keyword'},
      
      checks = function(params)
         params.CompanyID = tonumber(params.CompanyID)
         
         local numfields = 0
         if params.CompanyID then
            numfields = numfields + 1
         end
         if params.Name then numfields = numfields + 1 end
         if params.Keyword then numfields = numfields + 1 end

         if numfields > 1 then
            throwError(licensing_error.InvalidParams, 'Must be only one search parameter.')
         end
         if numfields == 0 then
            throwError(licensing_error.InvalidParams, 'Must be at least one search parameter.')
         end
      end
   },
   ['companies.updatepermissions'] = {
      method = license_admin.updateCompanyPermissions,
      needsession = true,
      required = {'Token', 'CompanyID'},
      optional = {'LicenseTransfer','TemporaryLicense'}
   },
   ['usage.temporarylicense'] = {
      method = license_admin.usageTemporaryLicense,
      needsession = true,
      required = {'Token', 'CompanyID', 'Product'},
   },
   ['usage.licensetransfer'] = {
      method = license_admin.usageLicenseTransfer,
      needsession = true,
      required = {'Token', 'CompanyID', 'Product'}
   },
}

local function handleRequest(req)
   local params = {}   
   
   for k,v in pairs(req.params) do
      k = k:lower()      
      if k == 'method' then
         v = v:lower()
      end
      params[k] = v
   end   

   if iguana.isTest() then
      session.newSessionPredefined("foo",{
            LoginID = 'michael.druker',            
            UserID = 57,
            LastName = 'Druker',  
            ['Info']={['Permissions']={['FinancialRead']=true, ['FinancialWrite']=true}}
         }) --for testing!      
   end

   if methodInfo[params.method] then
      local CapitalizedParams = {}      
      
      local ParamsErrorText
      
      for i,v in ipairs(methodInfo[params.method].required) do
         if not params[v:lower()] then
            ParamsErrorText = 'Missing required parameter: ' .. v .. '.'
         elseif #params[v:lower()] == 0 then
            ParamsErrorText = 'Required parameter cannot be empty: ' .. v .. '.'
         end
         CapitalizedParams[v] = params[v:lower()]
      end
      if methodInfo[params.method].optional then
         for i,v in ipairs(methodInfo[params.method].optional) do
            if params[v:lower()] then
               CapitalizedParams[v] = params[v:lower()]
            end
         end
      end     
      
      local Session      
      if methodInfo[params.method].needsession then
         Session = license_admin.getSession(CapitalizedParams)
      end
      
      --Waiting to throw invalid parameter errors until after checking the session.
      if ParamsErrorText then
         throwError(licensing_error.InvalidParams, ParamsErrorText)            
      end
      
      if methodInfo[params.method].checks then
         methodInfo[params.method].checks(CapitalizedParams)
      end

      response = methodInfo[params.method].method(CapitalizedParams, Session)
   else
      throwError(licensing_error.MethodNotFound)      
   end  

   return response
end

function processRequest(Data)
   local success, result = pcall(net.http.parseRequest, {data = Data})
   
   if not success then
      throwError(licensing_error.InvalidRequest)
   end
   
   return handleRequest(result)
end

function main(Data)
   local success, result = pcall(processRequest, Data)
   
   if success then
      net.http.respond{
         entity_type='text/json; charset=utf-8',
         body = json.serialize{data = {
               status = 'ok',
               data = result
            }
         }
      }
   else
      local ErrorBody
      if type(result) == 'table' and result.code and result.desc then
         result.status = 'error'
         ErrorBody = json.serialize{data=result}
      else
         ErrorBody = json.serialize{data= {
               status = 'error',           
               code = 100,
               desc = 'Unknown error.',               
            }
         }
         iguana.logError(tostring(result))
      end
         
      net.http.respond{
         entity_type='text/json; charset=utf-8',
         body=ErrorBody      
      }
   end
end
