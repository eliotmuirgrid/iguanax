function Ack(message)
	local M = hl7.parse{vmd="ack.vmd", data=message}
   -- A is the outbound ACKnowledgement message
   local A = hl7.message{vmd="ack.vmd", name="ACK"}
   A.MSH[3][1] = M.MSH[5][1]
   A.MSH[4][1] = M.MSH[6][1]
   A.MSH[5][1] = M.MSH[3][1]
   A.MSH[6][1] = M.MSH[4][1]
   A.MSH[7]    = M.MSH[7]
   A.MSH[8]    = M.MSH[8]
   A.MSH[9][1] = "ACK"
   A.MSH[10]   = "A"..M.MSH[10]
   A.MSH[11]   = M.MSH[11]
   A.MSH[12]   = M.MSH[12]
   A.MSA[1]    = "AA"
   A.MSA[2]    = M.MSH[10] 
   local Ack = A:S()
   return Ack
end


function main()
   local message = [[MSH|^~\&|AcmeMed|Lab|Main HIS|St. Micheals|20110213144932||ADT^A03|9B38584D9903051F0D2B52CC0148965775D2D23FE4C51BE060B33B6ED27DA820|P|2.6|
EVN||20110213144532||||20110213145902|
PID|||4525285^^^ADT1||Smith^Tracy||19980210|F||Martian|86 Yonge St.^^ST. LOUIS^MO^51460|||||||10-346-6|284-517-569|
NK1|1|Smith^Gary|Second Cousin|
PV1||E||||||5101^Garland^Mary^F^^DR|||||||||||1318095^^^ADT1|||||||||||||||||||||||||20110213144956|
OBX|||WT^WEIGHT||102|pounds|
OBX|||HT^HEIGHT||32|cm|
]]

   Ack(message)

end
