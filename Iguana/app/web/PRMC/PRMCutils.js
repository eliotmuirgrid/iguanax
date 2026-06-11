function PRMCcomponentId()           { return PAGEhashParameter("component");         }
function PRMCfilterId()              { return PAGEhashParameter("filter");            }
function PRMCdevelopment()           { return (typeof TRANSdevelopment === "function") && TRANSdevelopment(); }
function PRMCtranslatorContext()     { return PAGEhash() === "translator" && PRMCcomponentId() && PRMCdevelopment(); }
function PRMCapiData()               { return { component: PRMCcomponentId(), development: PRMCdevelopment() }; }

function PRMCbranchName()            { return PRMCdata?.branch || PRMCdata?.default_branch || ""; }
function PRMCdefaultBranch()         { return PRMCdata?.default_branch     || "";     }
function PRMCrepoWeb()               { return PRMCdata?.urls?.web          || "";     }
function PRMCrepoSsh()               { return PRMCdata?.urls?.ssh          || "";     }
function PRMCrepoWebBranchUrl()      { return PRMCdata?.urls?.web_branch   || "";     }
