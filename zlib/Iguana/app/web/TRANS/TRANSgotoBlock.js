function TRANScreateGotoDiv(SavedValue, Parent){
   const GotoDiv = `<div class='NISgotoDiv'>
                     <div class='NISgotoDivText'> Go to </div>
                     <input class='NISgotoInput' type='text' value='` + SavedValue + `'/>
                     <div   class='NISgotoGoButton'>OK</div>
                  </div>`;
   Parent.append(GotoDiv);
   return GotoDiv;
}

function TRANSremoveGotoDiv()      { $('.NISgotoDiv').remove(); TRANSsetCode();             }
function TRANSgotoDivOnEscape()    { setTimeout(function() { TRANSremoveGotoDiv(); }, 100); }

function TRANSinsertGotoDiv(Parent, ScopeInfo,FunctionScope){
   const Position = Parent.find('.position').get(0);
   COL_TRC("Parent =", Parent);
   const SavedValue = Position.innerHTML;
   const ExistingDiv = $('#' + ScopeInfo.s).find('.NISgotoDiv');
   let GotoDiv;
   if (ExistingDiv.length === 0) { GotoDiv = TRANScreateGotoDiv(SavedValue, Parent); }
   else                          { GotoDiv = ExistingDiv.get(0);                   }
   const InputField = $('.NISgotoInput');
   InputField.focus();
   InputField.select();
   const ChangeHandler = function() {
      let NewPos = parseInt(InputField.get(0).value, 10);
      if(!isNaN(NewPos)) {
         if(NewPos < 0) {
            NewPos = 1;
         }
         if(NewPos > ScopeInfo.l) {
            NewPos = ScopeInfo.l;
         }
         TRANSchangeCurrentIteration(FunctionScope, ScopeInfo, NewPos);
      }
      setTimeout(function() { TRANSremoveGotoDiv(); }, 100);
   };
   InputField.on('blur', ChangeHandler);
   InputField.on('keydown', function(event){
      if (event.keyCode == '13')      { ChangeHandler();      } 
      else if (event.keyCode == '27') { TRANSgotoDivOnEscape(); }
   });
}