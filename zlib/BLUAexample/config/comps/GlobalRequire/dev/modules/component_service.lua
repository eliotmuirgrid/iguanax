local Config = require("modules.config")
local Validator = require("modules.validator")
local ComponentService = {}

function ComponentService.getComponentsByTags(compTag)
  return
end

function ComponentService.getComponentSourcePath(compGuid)
   return iguana.workingDir().."config"..Config.pathSeparator.."comps"..Config.pathSeparator..compGuid
end

function ComponentService.stopComponent(componentId)
   return false
end

function ComponentService.restartIguana()
   return
end

return ComponentService

