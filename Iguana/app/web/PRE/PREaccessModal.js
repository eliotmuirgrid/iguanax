/* ==========================================================================
   PRE Access Modal
   - Shared modal wiring and submit flow for peer access controls
========================================================================== */

var PRE_ACC_SAVING = false;
var PRE_ACC_INIT_LEVEL = "none";
var PRE_ACC_AUTH_INIT = "";

function PREaccessSavingGet() {
   return PRE_ACC_SAVING === true;
}

function PREaccessSavingSet(Value) {
   PRE_ACC_SAVING = (Value === true);
}

function PREaccessInitialLevelGet() {
   return PRE_ACC_INIT_LEVEL;
}

function PREaccessInitialLevelSet(Level) {
   PRE_ACC_INIT_LEVEL = String(Level || "none");
}

function PREaccessAuthorizedInitialStateGet() {
   return PRE_ACC_AUTH_INIT;
}

function PREaccessAuthorizedInitialStateSet(State) {
   PRE_ACC_AUTH_INIT = String(State || "");
}

function PREaccessModalNavigate(RouteBackFn) {
   location.hash = RouteBackFn();
}

function PREaccessModalOpen(Title, ContentHtml, ClassName, Width, RouteBackFn) {
   POPpopoutOpen({
      title: Title,
      content: ContentHtml,
      className: ClassName,
      width: Width,
      esc_close: true,
      callback: function() {
         PREaccessModalNavigate(RouteBackFn);
      }
   });
}

function PREaccessModalSetInitialState(Root, Peer, NormalizeLevelFn) {
   PREaccessInitialLevelSet(NormalizeLevelFn(Peer.access_level));
   PREaccessSavingSet(false);
}

function PREaccessModalSelectedRadio(Root, RadioCheckedSelector) {
   return Root.querySelector(RadioCheckedSelector);
}

function PREaccessModalBindSave(
   Root,
   Peer,
   SaveButtonSelector,
   RadioCheckedSelector,
   HasPendingChangeFn,
   SubmitFn
) {
   var SaveBtn = Root.querySelector(SaveButtonSelector);
   if (!SaveBtn) return;
   SaveBtn.onclick = function() {
      if (PREaccessSavingGet()) return;
      var selected = PREaccessModalSelectedRadio(Root, RadioCheckedSelector);
      if (!selected) return;
      if (!HasPendingChangeFn(Root)) return;
      SubmitFn(Root, Peer, selected.value);
   };
}

function PREaccessModalBindCancel(Root, CancelButtonSelector, RouteBackFn) {
   var CancelBtn = Root.querySelector(CancelButtonSelector);
   if (!CancelBtn) return;
   CancelBtn.onclick = function() {
      PREaccessModalNavigate(RouteBackFn);
   };
}

function PREaccessModalBindRadios(Root, RadioSelector, UpdateHelperFn, UpdateSaveButtonFn) {
   var Radios = Root.querySelectorAll(RadioSelector);
   for (var i = 0; i < Radios.length; i++) {
      Radios[i].onchange = function() {
         UpdateHelperFn(Root);
         UpdateSaveButtonFn(Root);
      };
   }
}

function PREshowAccessModal(Peer, Config) {
   var cfg = Config;
   var peer = Peer || {};

   PREaccessModalOpen(
      cfg.title,
      cfg.modalHtml(peer),
      cfg.className,
      cfg.width,
      cfg.routeBack
   );

   var Root = document.querySelector(cfg.rootSelector);
   if (!Root) return;

   PREaccessModalSetInitialState(Root, peer, cfg.normalizeLevel);
   PREaccessModalBindSave(
      Root,
      peer,
      cfg.saveButtonSelector,
      cfg.radioCheckedSelector,
      cfg.hasPendingChange,
      cfg.submit
   );
   PREaccessModalBindCancel(Root, cfg.cancelButtonSelector, cfg.routeBack);
   PREaccessModalBindRadios(Root, cfg.radioSelector, cfg.updateHelper, cfg.updateSaveButton);

   cfg.updateHelper(Root);
   cfg.updateSaveButton(Root);
}

function PREaccessPollModels(ModelNames) {
   for (var i = 0; i < ModelNames.length; i++) {
      MODELforcePoll(ModelNames[i]);
   }
}

function PREaccessSubmitFinish(Root, Level, Config) {
   PREaccessInitialLevelSet(Config.normalizeLevel(Level));
   Config.setSaveState(Root, "saved");
   PREaccessPollModels(Config.pollModels);
   setTimeout(function() {
      PREaccessModalNavigate(Config.routeBack);
   }, Config.saveFeedbackMs);
}

function PREaccessSubmitFail(Root, Message, Config) {
   Config.onError(Message);
   PREaccessSavingSet(false);
   Config.updateSaveButton(Root);
}

function PREaccessSubmitApi(Root, Peer, Level, Config) {
   APIcall(Config.apiPath, { id: Peer.id, access_level: Level }, function(R) {
      if (R && R.success) {
         PREaccessSubmitFinish(Root, Level, Config);
         return;
      }
      PREaccessSubmitFail(
         Root,
         (R && R.error) ? R.error : "Failed to set access level.",
         Config
      );
   });
}

function PREaccessSubmit(Root, Peer, Level, Config) {
   var cfg = Config;
   PREaccessSavingSet(true);
   cfg.setSaveState(Root, "saving");
   PREaccessSubmitApi(Root, Peer, Level, cfg);
}
