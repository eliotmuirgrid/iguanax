

function BUTTONcontrol(T){
   let Target = T?.target;
   if(!Target) return;
   
   let Enabled = T?.enabled;

   let AddClass =  Enabled ? "BUTTONaction" : "BUTTONdisabled";
   let RemClass = !Enabled ? "BUTTONaction" : "BUTTONdisabled";

   Target.classList.add(AddClass);
   Target.classList.remove(RemClass);

   let EnabledTitle  = T?.enabledTitle  || "";
   let DisabledTitle = T?.disabledTitle || "";
   
   let title = Enabled ? EnabledTitle : DisabledTitle;
   if(Target.title != title) Target.title = title;

   let EnabledHref  = T?.enabledHref  || "";
   let DisabledHref = T?.disabledHref || "";

   let href = Enabled ? EnabledHref : DisabledHref;
   if(!href && Target.hasAttribute("href"))  Target.removeAttribute("href");
   if( href && !Target.hasAttribute("href")) Target.setAttribute   ("href", href);
   if( href && Target.href != href) Target.href = href;

}
