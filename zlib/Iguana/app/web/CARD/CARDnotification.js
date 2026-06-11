function CARDnotificationViewHtml() {
    return /*html*/ `<div class='CARDnotificationMessage'></div>`;
}

// TODO - does this get called too much - like when we are in edit mode
// TODO - also kind of gross that we have markup that is always there for this purpose
// why not insert it as needed.
function CARDupdateNotificationView() {
   let Message = document.querySelector(".CARDnotificationMessage");
   let NotificationBox = Message.closest('.CARDnotificationTarget');
   if (!Message) { return; }
   let Html;
   let OperationalData =  MODELdata.CARDoperationalNotiData.data;
   let NotificationData = MODELdata.CARDnotificationData.data;
   if (NotificationData.component_id === CARDcomponentId() && OperationalData.enabled) {
      Html = /*html*/ `This component has been selected in <a href="#settings/notifications">Settings &gt; Notifications</a> to receive notification calls.<p>If it is turned off, these alerts will be disabled.</p>`;
      if(NotificationBox.classList.contains('CARDhidden')) NotificationBox.classList.remove('CARDhidden');
   }
   else if (NotificationData.component_id === CARDcomponentId() && !OperationalData.enabled) {
      Html =  /*html*/ `This component has been selected in <a href="#settings/notifications">Settings &gt; Notifications</a> to receive notification calls.<p>These alerts are disabled because the component is not running.</p>`;
      if(NotificationBox.classList.contains('CARDhidden')) NotificationBox.classList.remove('CARDhidden');
   } else {
      Html = "";
      if(!NotificationBox.classList.contains('CARDhidden')) NotificationBox.classList.add('CARDhidden');
   }
   if(Message.innerHTML !== Html) Message.innerHTML = Html;
}