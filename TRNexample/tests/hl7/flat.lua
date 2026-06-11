-- This code does a flat auto completion call.
function main(Data)
   local H = hl7.message{vmd="tests/hl7/demo.vmd", name="ADTA01"}
   H.MSH[3][1] = "Epic"
   H.😊
end
   