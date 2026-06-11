function NTRhandleChildrenRequest(NodeChildrenContainer, TreeRoot, NodeTextSpan, Id, Response) {
   NodeTextSpan.classList.remove('waiting');
   Response = Response.data;
   if(Response.ErrorMessage) {
      NodeChildrenContainer.classList.add('error');
      NodeChildrenContainer.textContent = Response.ErrorMessage;
   } else if(Response.ScriptError) {
      NTRdisplayScriptError(NodeChildrenContainer, Response.ScriptError);
   } else {
      const Children     = Response.Root.c;
      const CountOfChild = Children.length;
      for(let ChildIndex = 0; ChildIndex < CountOfChild; ++ChildIndex) {
         NodeChildrenContainer.appendChild(NTRcreateTreeNode(Children[ChildIndex], Id));
      }
      NTRshowOrHideEmptyNodes(TreeRoot);
      NTRscrollToChildren(NodeTextSpan, NodeChildrenContainer);
   }
}

function NTRtranslatorRequestChildren(NodeChildrenContainer, TreeRoot, NodeTextSpan, Id) {
   NodeTextSpan.classList.add('waiting');
   const NodeLi      = NodeTextSpan.closest('li');
   const NodeAddress = NTRgetNodeAddress($(NodeLi));  // has to be jQuery object
   COL_TRC("NodeAddress=", NodeAddress);
   const NodeAddressString = JSON.stringify(NodeAddress);
   const Vars              = {};
   Vars.component          = TRANScomponentGuid();
   Vars.development        = TRANSdevelopment();
   Vars.filename           = TRANSfile();
   Vars.id                 = Id;
   Vars.sample_data_index  = SMPLindex();
   Vars.root_address       = NodeAddressString;
   COL_TRC("Vars=", Vars);
   APIcall('ide/annotation/window', Vars, function(Response) {
      NTRhandleChildrenRequest(NodeChildrenContainer, TreeRoot, NodeTextSpan, Id, Response)
   });
}

function NTRlogsRequestChildren(NodeChildrenContainer, TreeRoot, NodeTextSpan) {
   NodeTextSpan.classList.add('waiting');
   const NodeLi            = NodeTextSpan.closest('li');
   const NodeAddress       = NTRgetNodeAddress($(NodeLi));
   const NodeAddressString = JSON.stringify(NodeAddress);
   const Vars              = {
      use_default           : DETuseDefault(),
      vmd                   : DETgetVmd(),
      root_address          : NodeAddressString,
      data                  : DETrawData().content,
      full_argument_requests: []
   };
   APIcall('log/viewer/annotation/window', Vars, function(Response) {
      NTRhandleChildrenRequest(NodeChildrenContainer, TreeRoot, NodeTextSpan, Vars.id, Response);
   });
}