/* ==========================================================================
   PRE Notice HTML
   - Shared parameterized HTML builders for notices and simple shells
========================================================================== */

function PREsearchInputHtml(ClassName, Placeholder, Autocomplete) {
   var klass = ESChtmlEscape(String(ClassName || ""));
   var placeholder = ESChtmlEscape(String(Placeholder || ""));
   var autocomplete = ESChtmlEscape(String(Autocomplete || "off"));
   return /*html*/`
      <input
         type="search"
         class="${klass}"
         placeholder="${placeholder}"
         autocomplete="${autocomplete}">`;
}

function PREactionLinksHtml(Actions, ContainerClass, ButtonClass, PrimaryClass, SecondaryClass) {
   if (!Array.isArray(Actions) || Actions.length === 0) return "";
   var containerClass = String(ContainerClass || "");
   var buttonClass = String(ButtonClass || "");
   var primaryClass = String(PrimaryClass || "");
   var secondaryClass = String(SecondaryClass || "");
   var html = "";
   for (var i = 0; i < Actions.length; i++) {
      var action = Actions[i] || {};
      if (!action.href || !action.label) continue;
      var variantClass = action.primary ? primaryClass : secondaryClass;
      html += `<a href="${ESChtmlEscape(action.href)}" class="${ESChtmlEscape(buttonClass + " " + variantClass).trim()}">${ESChtmlEscape(action.label)}</a>`;
   }
   if (!html) return "";
   return `<div class="${ESChtmlEscape(containerClass)}">${html}</div>`;
}

function PREnoticeHtml(ContainerClass, TitleClass, TextClass, Title, Text, ActionsHtml) {
   return /*html*/`
      <div class="${ESChtmlEscape(String(ContainerClass || ""))}">
         <div class="${ESChtmlEscape(String(TitleClass || ""))}">${ESChtmlEscape(String(Title || ""))}</div>
         <div class="${ESChtmlEscape(String(TextClass || ""))}">${ESChtmlEscape(String(Text || ""))}</div>
         ${String(ActionsHtml || "")}
      </div>`;
}

function PREfeatureNoticeHtml(Title, Text, Actions) {
   var actionsHtml = PREactionLinksHtml(
      Actions,
      "PREemptyActions",
      "BUTTONstandard",
      "BUTTONaction",
      "BUTTONdefault"
   );
   return PREnoticeHtml(
      "PREemptyState PREfeatureNotice",
      "PREemptyTitle",
      "PREemptyText PREfeatureNoticeText",
      Title,
      Text,
      actionsHtml
   );
}

function PREgridScreenHtml(ContainerClass, ContentClass, TargetClass, HeaderHtml) {
   var containerClass = ESChtmlEscape(String(ContainerClass || ""));
   var contentClass = ESChtmlEscape(String(ContentClass || ""));
   var targetClass = ESChtmlEscape(String(TargetClass || ""));
   var headerHtml = String(HeaderHtml || "");
   return /*html*/`
   <div class="${containerClass}">
      <div class="${contentClass}">
         ${headerHtml}
         <div class="${targetClass}"></div>
      </div>
   </div>`;
}
