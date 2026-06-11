function CORupdateHrefElem(Selector, href){
    let div = document.querySelector(Selector);
    if(!div) return;
    if(div.href !== href) div.href = href;
}

function CORupdateDashboardHref(){
    let href = DASHformatUrl();
    if(PAGEhash().includes("dashboard")) href = "#dashboard";
    CORupdateHrefElem(".CORdashboardIcon", href);
}

function CORupdateSettingsHref(){
    let href = SETformatUrl();
    if(PAGEhash().includes("settings")) href = "#settings";
    CORupdateHrefElem(".CORsettingsIcon", href);
}

function CORupdateTranslatorHref(){
    let Component = STORElocalRead("PRJlastSelectedComponent") || "";
    let StoredDev = STOREreadJsonVariable("TRANSpreviousDev")[Component] == "true";
    let href = TRANSformatUrl(Component, StoredDev);
    CORupdateHrefElem(".CORtranslatorIcon", href);
}
function CORupdateLogsHref(){
    let Component = STORElocalRead("PRJlastSelectedComponent") || "";
    let href = "#logs" + PAGEformatParams({component : Component});
    CORupdateHrefElem(".CORlogIcon", href);
}

function CORupdateCommandHref(){
    let hasCommand = CORcommandFeatureEnabled();
    let href = hasCommand ? "#command" : "#dashboard";
    if(hasCommand) href = PREformatCommandUrl();
    if(hasCommand && PAGEhash().includes("command")) href = "#command";
    CORupdateHrefElem(".CORcommandIcon", href);
}

function CORupdateHrefs(){
    CORupdateCommandHref();
    CORupdateDashboardHref();
    CORupdateSettingsHref();
    CORupdateTranslatorHref();
    CORupdateLogsHref();
}
