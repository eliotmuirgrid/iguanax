Hl7message = [[MSH|^~\&|MESA_ADT|ABC_ADMITTING|iFW|ZYX_HOSPITAL|||ADT^A04|103102|P|2.4||||||||
EVN||200007010800||||200007010800
PID|||583295^^^ADT1||DOE^JOHN^middle||19610615|M-||2106-3|123 CAIN. STREET^^ORANGEBORO^NC^27847-1020|GL|
]]
function main(Data)
	local out = hl7.parse{vmd="hl7nodeName.vmd", data=Data}
   --  out.PID[5][1][2] is "Given Name"
   out.PID[5][1][2]:nodeName()
   out.PID[5][1][2]:nodeType()
   assert(out.PID[5][1][2]:nodeValue() == "JOHN")
   assert(out.PID[5][1]["Given Name"]:nodeValue() == "JOHN")
   assert(out["PID"]["Patient Name"][1][2]:nodeValue() == "JOHN")
   assert(out["PID"]["Patient Name"][1]["Given Name"]:nodeValue() == "JOHN")
   
   out.PID[3][1][1]:nodeName()
   assert(out.PID[3][1][1]:nodeValue() == "583295")
   assert(out.PID[3][1]["ID Number"]:nodeValue() == "583295")
   
   assert(out["PID"]["Patient Name"][1]["ST"]:nodeValue() == "JOHN")
   
end
