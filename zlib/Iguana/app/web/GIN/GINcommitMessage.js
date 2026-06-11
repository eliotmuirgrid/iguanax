function GINaddConn(SourceName, ComponentName){ 
   return `Connected "${SourceName}" to component "${ComponentName}".`; 
}
function GINremoveConn(SourceName, ComponentName){ 
   return `Disconnected "${SourceName}" from component "${ComponentName}".`; 
}
function GINaddFromUrl(ComponentName, GitUrl){ 
   return `Added component "${ComponentName}" from Git URL "${GitUrl}".`; 
}
function GINchangeField(Key, Name){ 
   return `Changed the value of "${Key}" in component "${Name}".`; 
}
function GINkeepOriginalMakeEditable(ComponentName){ 
   return `Made component "${ComponentName}" editable, remaining connected to the original Git URL.`;
}
function GINcreateCopyMakeEditable(ComponentName){ 
   return `Made component "${ComponentName}" editable by creating a copy, disconnecting from the original Git URL.`;
}
function GINsetCommit(CommitHash, ComponentName){ 
   return `Set the commit to "${CommitHash}" on component "${ComponentName}".`;
}
function GINsetUpstream(ComponentName, GitUrl){ 
   return `Set the upstream of component "${ComponentName}" to: "${GitUrl}".`;
}
function GINsetBranch(ComponentName, OldBranch, Branch, Created){
   if (Created === false){
     return `Switched component "${ComponentName}" from "${OldBranch}" to existing branch "${Branch}" (contents replaced).`;
   }
   return `Created and set component "${ComponentName}" from "${OldBranch}" to new branch "${Branch}".`;
}
function GINdeleteFile(FileName){ 
   return `Deleted the file: "${FileName}".`;
}
function GINdeleteAllFiles(){ 
   return `Deleted all debug crash files.`;
}
function GINsavePins(){ 
   return `Saved pins.`;
}
function GINdeleteEnv(Key){
   return `Deleted the "${Key}" environment variable.`;
}
function GINsaveDirectoryAccess(Data, DirEnabled, OsExRestriction){
   let Msg = `Os execute restriction set to ${OsExRestriction}`;
   if(!DirEnabled) return `Turned off Directory Access Restrictions.`;
   if(Data.length === 0) return `Cleared the list of Directories allowed to be Accessed.`;
   return `Saved the list of Directories allowed to be Accessed.`;
}
function GINsetEnv(Key){
   return `Set the "${Key}" environment variable.`;
}
function GINgenerateKeyPair(){
   return `Generated a new public and private ssh key pair.`;
}
function GINresetKeyPath(){
   return `Reset the location of the public and private ssh key pair.`;
}
function GINsaveServers(Data){
   if(Object.keys(Data).length === 0) return `Cleared the list of known Git servers.`;
   return `Saved the list of known Git servers.`;
}
function GINsetCredentials(){
   return `Set the username and email address credentials used for Git commits.`;
}
function GINsetSsoProvider(){
   return `Set the SSO provider and client id for user authentication.`;
}
function GINsetToken(){
   return `Set the Git server access token.`;
}
function GINsetUsername(){
   return `Set the Git username.`;
}
function GINaddCatalogue(GitUrl){
   return `Added the collection "${GitUrl}" to the catalogue.`;
}
function GINsaveCatalogue(Data){
   if(Object.keys(Data).length === 0) return `Cleared the list of collections in the catalogue.`;
   return `Saved the list of collections in the catalogue.`;
}
function GINsaveDatabase(){
   return `Saved the database settings.`;
}
function GINsetAutoUpdate(){
   return `Set the automatic update time.`;
}
function GINsetAutoUpdateOn(){
   return `Enabled automatic updates.`;
}
function GINsetAutoUpdateOff(){
   return `Disabled automatic updates.`;
}
function GINsaveWebServer(){
   return `Saved the web server settings.`;
}
function GINsaveNewLogFolder(Path){
   return `Saved the new log folder location to: "${Path}".`;
}
function GINsaveNewBackupLogFolder(Path){
   return `Saved the new backup log folder location to: "${Path}".`;
}
function GINsaveNotifications(){
   return `Saved the notification settings.`;
}
function GINaddRole(RoleName){
   return `Added the role: "${RoleName}".`;
}
function GINremoveRole(RoleName){
   return `Deleted the role: "${RoleName}".`;
}
function GINsaveRole(RoleName){
   return `Saved the role: "${RoleName}".`;
}
function GINsavePurgeRules(){
   return `Saved the log purge rules.`;
}
function GINsaveComponentDescription(ComponentName){
   return `Saved the description for component "${ComponentName}".`;
}
function GINsaveComponentName(OldName, NewName){
   return `Renamed component "${OldName}" to "${NewName}".`;
}
function GINaddUser(Username){
   return `Added the user: "${Username}".`;
}
function GINdeleteUser(Username){
   return `Deleted the user: "${Username}".`;
}
function GINsaveUserPassword(Username){
   return `Updated the password for the user: "${Username}".`;
}
function GINsaveUserRoles(Username){
   return `Updated the roles for the user: "${Username}".`;
}
function GINupdateLicense(){
   return `Updated the license.`;
}
function GINupdateInstanceUser(Username){
   if(Username == "") return `Cleared the "Instance Management" user.`;
   return `Updated the "Instance Management" user to: "${Username}".`;
}
function GINupdateAutoInterval(Interval){
   if(Interval == 0) return `Disabled automatic pushes.`;
   if(Interval == 1) return `Set the automatic push interval to be every minute.`;  
   return `Set the automatic push interval to be every "${Interval}" minutes.`;
}
function GINaddPeer(Name){
   return `Added "${Name}" to the cluster.`;
}
function GINdeletePeer(PeerId){
   return `Deleted the Peer with ID: "${PeerId}".`;
}
function GINremoveComponentUpstream(ComponentName){
   return `Removed the upstream repository for component "${ComponentName}".`;
}
function GINremoveLibraryUpstream(LibraryName){
   return `Removed the upstream repository for library "${LibraryName}".`;
}
function GINremoveInstanceUpstream(){
   return `Removed the upstream repository for the instance.`;
}

function GINsetOrggs(orgs){
   if(!orgs || orgs.length == 0) return "Removed the organization.json file and defaulting to auto-generated organizations list.";
   return "Saved a custom git organizations list to organization.json file.";
}