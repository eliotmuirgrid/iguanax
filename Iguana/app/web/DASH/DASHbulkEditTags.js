function DASHbulkEditModal(ComponentGuids) {
   let Card = POPpopoutOpen({title: 'Bulk Edit Tags', content: DASHbulkEditHtml(ComponentGuids), className: 'DASHbulkActionPopup',callback: DASHresetBulkMenu, esc_close : true});
   document.querySelector('.DASHcancelTags').addEventListener('click', function() { POPpopoutClose(Card); })
   document.querySelector('.DASHsaveTags').addEventListener('click', function() { DASHbulkEditComponents(ComponentGuids); })
   let div = document.querySelector(".DASHaddTag");
   FORMfocus(div);
   FORMenterListener(div, function() { DASHbulkEditComponents(ComponentGuids); });
   FORMenterListener(document.querySelector(".DASHremoveTag"), function() { DASHbulkEditComponents(ComponentGuids); });
}

function DASHbulkEditHtml(ComponentGuids) {
   return /*html*/`
   <div class="DASHbulkDialogClass">
      ${DASHcomponentTable('EDITING TAGS', ComponentGuids)}
      <div class='FORMlabel DASHaddTagLabel'>Add tags:</div><input class='DASHaddTag FORMinput' placeholder=''/>
      <div class="DASHbulkEditAddFeedback"></div>
      <br>
      <div class='FORMlabel'>Remove tags:</div><input class='DASHremoveTag FORMinput' placeholder=''/>
      <div class="DASHbulkEditRemoveFeedback"></div>
   </div>
   <div class="DASHbulkEditSubmitFeedback"></div>
   <div class='BUTTONgroup DASHbuttonGroup'>
      <div class='DASHcancelTags BUTTONstandard BUTTONcancel'>CANCEL</div>
      <div class='DASHsaveTags BUTTONstandard BUTTONaction'>SAVE</div>
   </div>`;
}

function DASHbulkEditComponents(ComponentGuids = DASHbulkGetCheckedList()) {
   let Data = DASHbulkEditGetVals();
   let components = { 'components':  ComponentGuids}
   if(!DASHbulkEditCheck()) { return; }
   APIcall('component/edit/tags', {...components, ...Data}, function(R) {
      if(!R.success) { return DASHbulkdEditFeedback(document.querySelector('.DASHbulkEditSubmitFeedback'), R.error); }
      let Message = GINcommitMessage('Edited tags for', components.components);
      let Added = "";
      let Removed = "";
      if(Data.add.trim())    { Added = ` Added: "${Data.add.trim()}".`; }
      if(Data.remove.trim()) { Removed = ` Removed: "${Data.remove.trim()}".`; }
      GINaddAndCommit({message: Message + Added + Removed, components: components.components});
      POPpopoutRemoveAll();
   })
}

function DASHbulkEditCheck() {
   let check            = DASHbulkEditValidateFields();
   const addSelector    = document.querySelector('.DASHbulkEditAddFeedback');
   const removeSelector = document.querySelector('.DASHbulkEditRemoveFeedback');
   let Err              = 'Can only accept valid hashtags. Please check the above field';
   check.add ? DASHbulkdEditFeedback(addSelector, '') : DASHbulkdEditFeedback(addSelector, Err);
   check.remove ? DASHbulkdEditFeedback(removeSelector, '') : DASHbulkdEditFeedback(removeSelector, Err);
   return check.add && check.remove;
}

function DASHbulkEditValidateFields() {
   const data         = DASHbulkEditGetVals();
   const validateTags = (tags, action) => {
      tags.split(/\s+/).forEach(tag => {
         if(tag.trim() && !EDITisHashtag(tag)) { result[action] = false; }
      });
   };
   let result = {add: true, remove: true};
   if(data.add.trim())    { validateTags(data.add, 'add'); }
   if(data.remove.trim()) { validateTags(data.remove, 'remove'); }
   return result;
}

function DASHbulkEditGetVals() {
   const addVals    = document.querySelector('.DASHaddTag')?.value;
   const removeVals = document.querySelector('.DASHremoveTag')?.value;
   return {add: addVals, remove: removeVals};
}

function DASHbulkdEditFeedback(selector, feedback) { if(selector) { selector.innerHTML = feedback; } }
