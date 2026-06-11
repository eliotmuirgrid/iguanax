function ENVverify(dataNode) {
   if(!dataNode) { return false; }
   if(dataNode.value) {
      dataNode.classList.remove('ENVinvalidInput');
      return true;
   }
   dataNode.classList.add('ENVinvalidInput');
   dataNode.nextElementSibling.textContent = "Field cannot be empty";
   return false;
}

function ENVhasRecursion(key, value, dataNode) {
   if (value.includes(`\${${key}}`)) {
      dataNode.classList.add('ENVinvalidInput');
      dataNode.nextElementSibling.textContent = "Value cannot contain itself";
      return true;
   }
   return false;
}

function ENVsetLocal(envCard, originalKey) {
   const [keyInput, valueInput] = document.querySelectorAll('.FORMinput');
   const newKey     = keyInput.value;
   const keyCheck   = ENVverify(keyInput);
   const valueCheck = ENVverify(valueInput);
   if (!keyCheck || !valueCheck || ENVhasRecursion(newKey, valueInput.value, valueInput)) { return; }
   APIcall('environment/set_local', { original_key: originalKey, new_key: newKey, value: valueInput.value }, function(R) {
      if (!R.success) { return ENVsetCardApiFeedback(R.error); }
      let Message = GINsetEnv(newKey);
      GINaddAndCommit({message: Message, settings: true});
      POPpopoutClose(envCard);
      ENVscreen();
   });
}

function ENVdeleteLocal(envCard, originalKey) {
   if(!originalKey) { return ENVsetCardApiFeedback("Name required for deletion"); }
   if(ENVvariables.system.hasOwnProperty(originalKey)) { return ENVsetCardApiFeedback("Cannot delete system variable"); }
   APIcall('environment/delete', {key: originalKey}, function(R) {
      if(!R.success) { return ENVsetCardApiFeedback(R.error); }
      let Message = GINdeleteEnv(originalKey);
      GINaddAndCommit({message: Message, settings: true});
      POPpopoutClose(envCard);
      ENVscreen();
   });
}