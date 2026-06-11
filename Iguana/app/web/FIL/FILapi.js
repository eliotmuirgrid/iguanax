let FILsepString = "";

function FILsep() { return FILsepString; }

function FILsepUpdate() {
   if(FILsepString === "") { APIcall("file_browser/sep", {}, (r) => { FILsepString = r.data.file_sep; }); }
}

// {"data":{"volumes":["C:\\","D:\\","Y:\\","Z:\\"]},"success":true}
function FILgetVolumes(callback) {
   APIcall('file_browser/volumes', {component: "local"}, (r) => { if(callback) { callback(r); } });
}

// {"data":{"component.json":{"file":true,"type":"json"},"dev":{"file":false,"type":""}},"success":true}
function FILlist(path, showHidden, filter, callback) {
   const data = {dir: FILpathSepAdd(path), show_shidden: showHidden, type: '*.*', filter: filter};
   APIcall('file_browser/list', data, (r) => { if(callback) { callback(r); } });
}

// {"data":{"path":"/Users/void/Library/IguanaX/"},"success":true}
function FILextractPath(path, callback) {
   APIcall('file_browser/extract_path', {path: path}, (r) => { if(callback) { callback(r); } });
}

function FILresolvePath(userPath, partialPath, callback) {
   const data = {user_path: userPath, partial_path: partialPath};
   APIcall('file_browser/resolve_path', data, (r) => { if(callback) { callback(r); } });
}

function FILmakeNewFolder(path, folderName, callback) {
   const data = {path: path, folder: folderName};
   APIcall('file_browser/new_folder', data, (r) => { if(callback) { callback(r); } });
}
