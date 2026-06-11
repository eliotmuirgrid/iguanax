local Importer = require("modules.importer")
local Req = require("modules.component_service")
local Exporter = require("modules.exporter")

function main(Data)
   -- The other files are left mostly blank, only adding a few functions to some 
   -- global tables. What we care about in this test is that the global state
   -- isn't corrupted in any way - no excess pops. 
   local count = 0
   for _ in pairs(Req) do 
      count = count + 1
   end
   
	if count~=4 then 
      error("component_service table does not have the right number of entries") 
   end
end
	