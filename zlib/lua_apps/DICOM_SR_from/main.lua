require "sr"
require "mapping"

function main()
   for filename, info in os.fs.glob(sr.DATA_DIR..'inbound\\*.dcm') do
      local name = filename:sub(string.len(sr.DATA_DIR..'\\inbound\\'), filename:len()-4)
      local err  = sr.DCMtoXML("inbound\\"..name..".dcm", "processed\\"..name..".xml")
      if err=='' then
         local SR_in  = sr.loadXML("processed\\"..name..".xml")
         local SR_out = transform(SR_in)
         sr.saveXML("processed\\"..name.."_out.xml", SR_out)
         err = sr.XMLtoDCM("processed\\"..name.."_out.xml", "outbound\\"..name.."_out.dcm")
       end
      if err=='' then
         print ("Processed DICOM file "..filename)
         sr.move_file("inbound\\"..name..".dcm", "processed\\"..name..".dcm")
      else
         print ("Failed to process DICOM file "..filename)
         sr.move_file("inbound\\"..name..".dcm", "error\\"..name..".dcm")        
      end
   end
end