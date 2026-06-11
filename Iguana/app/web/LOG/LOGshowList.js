const MAX_LOG_ENTRIES = 1800;
let LOGshowListData = [];

function LOGcleanContent(Content){
    const tempDiv = document.createElement('div');
    tempDiv.innerHTML = Content;
    return tempDiv.innerHTML;
}

function LOGneedsDateRow(Idx, Data) {
    if (Idx === 0) return false;
    const CurrentDate = LOGdayFormat(Data[Idx]?.time || 0);
    const OldDate     = LOGdayFormat(Data[Idx-1]?.time || 0);
    return CurrentDate !== OldDate;
}

function LOGrowDateHtml(CurrentDate) {
    return `<div class="LOGdateRow LOGdateRowList">${CurrentDate}</div>`;
}

function LOGargs(I) {
    return {
        ...(I.comp !== "Iguana-Service" && { component: I.comp }),
        time: I.time,
        id: I.id,
        filter: PAGEhashParameter("filter"),
        search: PAGEhashParameter("search"),
        exact:  PAGEhashParameter("exact"),
        tags:   PAGEhashParameter("tags"),
        from:   PAGEhashParameter("from"),
        to:     PAGEhashParameter("to")
    };
}

function LOGhasRelated(I) {
    return I.rel_comp !== "";
}

const LOGlistItemIdx = (Idx, Data, Buttons) => {
    const I = Data[Idx];
    const Args = PAGEformatParams(LOGargs(I));
    const CurrentDate = LOGyearMonthDateTime({
        epoch: I?.time || 0,
        includeTime: false,
        simpleTime: false,
        shortDayLabel: false
    });
    const DateRow = LOGneedsDateRow(Idx, LOGshowListData) ? LOGrowDateHtml(CurrentDate) : "";
    const GapRow  = LOGneedsGapRow (Idx, LOGshowListData) ? LOGrowGapHtml (Idx, Data) : "";
    return LOGrowHtml(I, Args, DateRow, GapRow, Buttons);
};

function LOGtypeButton(type){
    if(type == "message") return `<div class="LOGmessage" title="This is a message log.">M</div>`;
    if(type == "error")   return `<div class="LOGerror" title="This is an error log.">!</div>`;
    return `<div class="LOGemptyGap"></div>`;
}

function LOGrowTagsHtml(tags){
    let tagsHtml = "";
    if(tags.length == 0) return tagsHtml;
    const roleData = Object.fromEntries(tags.map(tag => [tag, true]));
    tagsHtml = HASHTAGrenderTags(tags, "LOGtag", roleData);
    return `<div class="LOGtags">${tagsHtml}</div>`;
}

function LOGrowHtml(I, Args, DateRow, GapRow, Buttons){
    const Content = LOGcleanContent(I.content);
    const Matches = I?.match ? "" : "LOGblurEntry";
    const href1 = `#logs/details${Args}`;
    const href2 = LOGhasRelated(I) && Buttons ? 
        `<div class="LOGgrey">[<a class="LOGdetails" href="#logs/related${Args}"> Context </a>]</div>` : 
        "<div></div>";
    let typeHtml = LOGtypeButton(I.type);
    let Tags = I?.tags || [];
    let tagsHtml = LOGrowTagsHtml(Tags);
    let Selected = "";
    let Time = PAGEhashParameter("time");
    let Id = PAGEhashParameter("id");
    if(Time && Id && Time == I.time && Id == I.id) Selected = " LOGentrySelected";
    return /*html*/`
        ${GapRow}
        ${DateRow}
        <div class="LOGlogEntry ${Matches}${Selected}">
            <div class="LOGtimeColumn">
                <div class="LOGselect">
                    <a class="BUTTONstandard BUTTONdefault LOGselectButton">SELECT</a>
                </div>
                <div class="LOGtimeTypeColumn">
                    <div class="LOGcellTime" title="${LOGyearMonthDateTime({ epoch: I.time, includeTime: true, simpleTime: false, shortDayLabel: false })}">${LOGtimeOfDayFormat(I.time)}</div>
                </div>
            </div>
            ${typeHtml}
            <div class="LOGentryBody">
                <div class="LOGcellPreview">${Content}</div>
            </div>
            ${tagsHtml}
            ${href2}
            ${Buttons ? `<div class="LOGgrey">[<a class="LOGdetails" href="${href1}" title="View details. You can also double-click this log entry."> Details </a>]</div>` : ""}
        </div>`;
}

const LOGshowListRender = () => {
    const Div = document.querySelector('.LOGlistDiv');
    if (!Div) return;

    const H = LOGshowListData.map((_, i) => LOGlistItemIdx(i, LOGshowListData, true)).join('');
    
    if (Div.innerHTML !== H) {
        Div.innerHTML = H;
        LOGselectTopAndBottom();
        MODELupdate("LOGmodel");
        MODELupdate("LOGscrollbarModel");
    }
};

const LOGshowListAppend = (Data) => {
    const Div = document.querySelector('.LOGlistDiv');
    if (!Div) return;

    LOGremoveTopRows(LOGnumberOfLogsToRemove(Data, LOGshowListData));
    LOGremoveWastedDateRows();

    LOGshowListData = LOGcombineListsBackward(Data, LOGshowListData);
    const StartIdx = LOGshowListData.length - Data.length;
    const H = LOGshowListData.slice(StartIdx).map((_, i) => LOGlistItemIdx(i + StartIdx, LOGshowListData, true)).join('');
    
    Div.insertAdjacentHTML('beforeend', H);
    MODELupdate("LOGmodel");
};

const LOGshowListPrepend = (Data) => {
    const Div = document.querySelector('.LOGlistDiv');
    if (!Div) return;

    LOGremoveBottomRows(LOGnumberOfLogsToRemove(Data, LOGshowListData));
    LOGremoveWastedDateRows();

    LOGshowListData = LOGcombineListsForward(Data.reverse(), LOGshowListData);
    const H = Data.map((_, i) => LOGlistItemIdx(i, LOGshowListData, true)).join('');
    
    const Idx = Data.length; 
    const Time = LOGshowListData[Idx]?.time || 0;
    const CurrentDate = LOGyearMonthDateTime({
        epoch: Time,
        includeTime: false,
        simpleTime: false,
        shortDayLabel: false
     });
    const GapRow  = LOGneedsGapRow (Idx, LOGshowListData) ? LOGrowGapHtml (Idx, LOGshowListData) : "";
    const DateRow = LOGneedsDateRow(Idx, LOGshowListData) ? LOGrowDateHtml(CurrentDate) : "";
    
    Div.insertAdjacentHTML('afterbegin', H + GapRow + DateRow);
    MODELupdate("LOGmodel");
};

const LOGremoveWastedDateRows = () => {
    document.querySelectorAll('.LOGdateRowList').forEach(dateRowList => {
        const nextSibling = dateRowList.nextElementSibling;
        const previousSibling = dateRowList.previousElementSibling;
        if (!previousSibling || !nextSibling || !nextSibling.classList.contains('LOGlogEntry')) {
            dateRowList.remove();
        }
    });
};

const LOGremoveTopRows = (n) => {
    const logs = document.querySelectorAll('.LOGlogEntry');
    for (let i = n - 1; i >= 0; i--) {
        logs[i]?.remove();
    }
};

const LOGremoveBottomRows = (n) => {
    const logs = document.querySelectorAll('.LOGlogEntry');
    const logsLength = logs.length - 1;
    for (let i = logsLength; i > logsLength - n; i--) {
        logs[i]?.remove();
    }
};

function LOGnewestItem() {
    return LOGshowListData[0] || {};
}

function LOGoldestItem() {
    return LOGshowListData[LOGshowListData.length - 1] || {};
}

function LOGcombineListsForward(NewArray, OldArray) {
    const CombinedArray = NewArray.concat(OldArray);
    return CombinedArray.slice(0, MAX_LOG_ENTRIES);
}

function LOGcombineListsBackward(NewArray, OldArray) {
    const CombinedArray = OldArray.concat(NewArray);
    return CombinedArray.slice(-MAX_LOG_ENTRIES);
}

function LOGnumberOfLogsToRemove(arr1, arr2) {
    return Math.max(arr1.length + arr2.length - MAX_LOG_ENTRIES, 0);
}
