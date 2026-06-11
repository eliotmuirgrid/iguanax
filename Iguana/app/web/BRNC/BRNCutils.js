function BRNCcomponentId()           { return PAGEhashParameter("component");         }
function BRNCfilterId()              { return PAGEhashParameter("filter");            }
function BRNCapiData()               { return { component: BRNCcomponentId(), development: true }; }

function BRNCbranchName()            { return BRNCdata?.branch             || "";     }
function BRNCdefaultBranch()         { return BRNCdata?.default_branch     || "";     }
function BRNCrepoWeb()               { return BRNCdata?.urls?.web          || "";     }
function BRNCrepoSsh()               { return BRNCdata?.urls?.ssh          || "";     }
function BRNCrepoWebBranchUrl()      { return BRNCdata?.urls?.web_branch   || "";     }