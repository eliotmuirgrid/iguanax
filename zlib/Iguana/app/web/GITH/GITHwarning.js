function GITHupdateWarning(){
   STAupdateTargetClass(".GITHgitCredTitle",           "GITHwarning", GITHcredWarning());
   STAupdateTargetClass(".GITHgitCloudTitle",          "GITHwarning", GITHcloudWarning());
   STAupdateTargetClass(".GITHserviceUser .GITHtitle", "GITHwarning", GITHusernameWarning());
   STAupdateTargetClass(".GITHkeyFiles .GITHtitle",    "GITHwarning", GITHkeyWarning());
   STAupdateTargetClass(".GITHorganization .GITHtitle","GITHwarning", GITHorgsWarning());
}

function GITHupdateSlowWarning(){
   STAupdateTargetClass(".GITHappPassword .GITHtitle", "GITHwarning", GITHtokenWarning());
   STAupdateTargetClass(".GITHsshModal .GITHtitle",    "GITHwarning", GITHsshWarning());
}