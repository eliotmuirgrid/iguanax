function EDIchangeListeners(e){
   let WindowId = WNDeventId(e);
   let FileName = EDInodeWindows[WindowId]?.file;
   if(!FileName) FileName = EDImatchingWindows[WindowId];
   let Value = e.target.value;
   if(Value === "{}[newmessage321")      { EDIaddNew(EDIgenerateUniqueName("Message"     , FileName), "message",       FileName); }
   if(Value === "{}[newsegment321")      { EDIaddNew(EDIgenerateUniqueName("Segment"     , FileName), "segment",       FileName); }
   if(Value === "{}[newsegmentgroup321") { EDIaddNew(EDIgenerateUniqueName("SegmentGroup", FileName), "segment_group", FileName); }
   if(Value === "{}[newcomposite321")    { EDIaddNew(EDIgenerateUniqueName("Composite"   , FileName), "composite",     FileName); } 
}