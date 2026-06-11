--This module requires session_config to be globally defined.
session = {}

local Sessions = {}

local LastCleared = nil
   
local function clearExpired()
   for k,v in pairs(Sessions) do
      if os.difftime(os.time(), Sessions[k].FirstAccess) > session_config.SessionDuration or
         os.difftime(os.time(), Sessions[k].LastAccess) > session_config.InactiveTimeAllowed then
         Sessions[k] = nil
      end
   end      
end

function session.newSession(data)   
   data = data or {}
   local token = util.guid(128)
   
   Sessions[token] = {
      Token = token,
      FirstAccess = os.time(),
      LastAccess = os.time(),
      data = data,
   }
   
   return Sessions[token]
end

function session.newSessionPredefined(token, data)
   Sessions[token] = {
      Token = token,
      FirstAccess = os.time(),
      LastAccess = os.time(),
      data = data,
   }
   
   return Sessions[token]
end

function session.getSessionShell(data)
   return {
      Token = util.guid(128),
      FirstAccess = os.time(),
      LastAccess = os.time(),
      data = data,
   }
end

function session.getSession(token)
   if not LastCleared or os.difftime(os.time(), LastCleared) > session_config.TimeBetweenClearing then
      clearExpired()
      LastCleared = os.time()
   end
   
   if not Sessions[token] then
      error('Invalid or expired token.')
   end
   
   Sessions[token].LastAccess = os.time()
   return Sessions[token]
end

function session.clearSession(token)
   Sessions[token] = nil
end