let STCeditMode;
let STCcollectionRendered = undefined;

function SETcollectionModelInit(){
   STCresetGlobals();
   MODELpollerAdd('STCmodel', 'SETcollection', 2, 'git/catalogue', {});
   MODELpollerAdd('STCmodel', 'STCeditPermission', 10, 'component/permission', {});
   MODELviewAdd  ('STCmodel', 'SETcollectionRenderView');
   MODELviewAdd  ('STCmodel', 'STCeditButtonUpdate');
   MODELforcePoll('STCmodel');
   MODELstart    ('STCmodel');
}

function STCeditPermission(){
   return MODELdata?.STCeditPermission?.data?.edit_collections || false;
}

function STCresetGlobals(){
   STCeditMode = false;
   STCcollectionRendered = undefined;
}