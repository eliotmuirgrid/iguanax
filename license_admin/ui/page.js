/** @license
 * Copyright (c) 2010-2011 iNTERFACEWARE Inc.  All rights reserved.
 */

// The page.js file should be kept very small.  Please note there is a deliberate
// structure here to keep it to just the map of pages to their handler functions
// and the associated utility functions.  
//
// In general the pattern is that each screen or group of screens should have their
// own associated implementation file to make it easier to navigate the file structure
// and know where to find stuff.

var PAGlocation = '';
var PAGcurrentPage = '';
var PAGbaseTitle = '';

// returns value of a href in <a href="...">  with the right escaping to make a link to a page
// Link = "<a href='" + PAGhref("OpenEditor", { Script : C.script, Data : C.data} ) + "'>MyLink</a>";
function PAGhref(Page, PageParams){
    var Anchor = "#Page=" + Page;
    if (PageParams){
        for (i in PageParams){
            if("Page" == i) continue;
            Anchor += "&" + i + "=" + PAGescapeQuotes(encodeURIComponent(PageParams[i]));
        }
    }
    return Anchor;
}

// returns value of a href in <a href="...">  with the right escaping to make a link to a page
// Link = PAGlink("OpenEditor", { Script : C.script, Data : C.data} ) + "'>MyLink</a>";
function PAGlink(Page, PageParams, Text){
    return "<a href='" + PAGhref(Page, PageParams) + "'>" + Text + "</a>"
}

// return string with single quotes escaped
function PAGescapeQuotes(Input) {
    return Input.replace(/'/g, "&#39;").replace(/"/, "&quot;");
}

// NOTE: This can only be called on an encoded string.  With decoded strings, pluses will be incorrectly converted 
// to the general encoding.
function PAGconvertSpaceEncoding(EncodedInput){
    return EncodedInput.replace(/\+/g, "%20");
}

// Basically an alternative to alert().
function PAGwarning(Message, OnClose){
    $('<div>' + PAGhtmlEscape(Message) + '</div>').appendTo('body:first').dialog({
        title: 'Warning',
        modal: true,
        buttons: {OK:function(){$(this).dialog('close');}},
        close: function(){
            $(this).dialog('destroy').remove();
            if (OnClose) { OnClose(); }
        }
    });
}

function PAGhtmlEscape(Text){
    return Text.replace(/;/g, '&#59;')
        .replace(/&(?!#59;)/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;')
        .replace(/"/g, '&quot;');
}

function PAGgetField(PageParams, Name){
    if (typeof (PageParams[Name]) === 'undefined'){
        throw new Error('Need to have ' + Name + ' defined.');
    }
    return PageParams[Name];
}

function PAGgetOptionalField(PageParams, Name){
    if (typeof (PageParams[Name]) === 'undefined'){
        return "";
    }
    return PageParams[Name];
}

function PAGparseAnchorString(AnchorString){
    var PageParams = {};
    var Vars = AnchorString.split('&');
    for (var i=0; i < Vars.length; i++){
        var Parts = Vars[i].split('=');
        if(undefined != Parts[1]){
            Parts[1] = PAGconvertSpaceEncoding(Parts[1]);
        }
        PageParams[Parts[0]] = decodeURIComponent(Parts[1]);
    }
    return PageParams;
}

function PAGcallPage(AnchorString){
    console.log(AnchorString);
    var AnchorParts = AnchorString.split('#');
    console.log(AnchorParts);

    var JustAnchor;
    if (AnchorParts.length < 2){
        //PageHandler.defaultRequest("No page specified.");
        //return;
        JustAnchor = "";
    } else {
        JustAnchor = AnchorParts[1];
    }
    //if (JustAnchor == 'error') return;
    PageParams = PAGparseAnchorString(JustAnchor);

    PAGcurrentPage = PageParams.Page;

    window.onresize = null; // Reset the page's onresize handler (see #16696)
    $(window).unbind('keydown'); // Reset page's keydown handler (see #18564)

    if (typeof(PageParams.Page) === 'undefined' || "" == PageParams.Page){
        //PageHandler.defaultRequest("No page specified.");
        //return;
        PageParams.Page = "";
    }
    if (typeof(PageHandler.pageLookup[PageParams.Page]) === 'undefined'){
        PageHandler.renderError("Page " + PageParams.Page + " not found.", PageParams);
        //return;

        PageParams.Page = "";
    }
    PageHandler.pageLookup[PageParams.Page](PageParams);
}

$(document).ready(function(){
    function PAGcheckForHashChange(){
        if(PAGlocation != location.href){
            PAGlocation = location.href;
            console.log("Hash change= ", PAGlocation);
            PAGcallPage(location.href);
        }
    }

    function PAGonHashChange(){
        console.log("Hash change= ", location.href);
        PAGcallPage(location.href);
    }
    // document.documentMode considered because IE 8 in compatibility mode reports true for this event.
    if('onhashchange' in window && ( document.documentMode === undefined || document.documentMode > 7 )){
        window.onhashchange = PAGonHashChange;
    }
    else{
        // We're going to polling location.href.
        setInterval(PAGcheckForHashChange, 100);
    }
    PAGbaseTitle = $(document).get(0).title;

    //Don't reload when a link is to a hash-based page
    $('noreload').click(function(event) {
        event.preventDefault();
        window.location.hash = this.hash;
    });

    PAGonHashChange();
});





