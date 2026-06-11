const {addEventListener, localStorage} = window;

const TERMkey = 'TERMterminal';

const TERMhelp = {
   clear: 'clear the terminal screen',
   wipe : 'Remove all your history of command lines',
   exit : 'Closes the terminal',
};

const TERMhtml = ({shell: {prompt, separator}}) => (
   /*html*/`
  <div class="TERMcontainer">
    <output></output>
    <div class="TERMcommand">
      <div class="TERMprompt">${prompt}${separator}</div>
      <input class="input" spellcheck="false" autofocus />
    </table>
  </div>
`);

function TERMencloseInDivHtml(html){
   return /*html*/`<div class="TERMpreserveWhitespace">${html}</div>`;
}

const TERMcloneCommandHtml = (el) => {
   const line      = el.cloneNode(true);
   const input     = line.querySelector('.input');
   input.autofocus = false;
   input.readOnly  = true;
   input.insertAdjacentHTML('beforebegin', TERMencloseInDivHtml(ESChtmlEscape(input.value)));
   input.parentNode.removeChild(input);
   line.classList.add('line');
   return line;
};

const TERMcommands = {
   clear  : function(terminal) { terminal.clear();},
   help   : function(terminal, [command]) {
      if(command) {terminal.output(`help: ${TERMhelp[command] || `no help topics match <u>${ESChtmlEscape(command)}</u>`}`);}
      else {
         terminal.output('Type <u>help {commandName}</u> to find out more about the function <u>name</u>.');
         terminal.output(`Available commands are: ${Object.keys(terminal.commands).filter(c => c !== 'execute').join(', ')}`);
         terminal.output('All <u>git commands</u> will also be run as if in an actual terminal in the working directory of the <u>current component</u>.');
         terminal.output('Only <u>single</u> git commands will be executed. Commands with <u>&&</u> or <u>pipes</u> will not be executed.');
      }
   },
   wipe   : function(terminal) {
      terminal.prompt('Are you sure remove all your commands history? Y/N', (value) => {
         if(value.trim().toUpperCase() === 'Y') {
            localStorage.removeItem(TERMkey);
            terminal.history       = [];
            terminal.historyCursor = 0;
            terminal.output('History of commands wiped.');
         }
      });
   },
   execute: function(terminal, component_id, command, parameters) {
      parameters.unshift(command);
      let Command = parameters.join(' ');
      // console.log("Command: " + Command)
      // TRANSunmarkCurrentLineAndSave(function(){
         let ApiData = {};
         ApiData.component = component_id;
         ApiData.commands = [ Command ];
         ApiData.development = TRANSdevelopment();
         APIcall('git/custom_command', ApiData, function(Response) {
            // console.log(Response);
            if (!Response.success){
               terminal.output(Response.error);
            }
            let Lines = Response.data[0].split('\n');
            for (let i=0; i < Lines.length; i++){
               terminal.output(Lines[i]);   
            }
            if (!Response['success']) {
               terminal.output(Response['error']);
               TERMcommandChainArray = []; // clear rest of commands and callback because one command failed
               TERMcommandChainCallback = undefined;
            }
            TRANSfileCurrentReload();
            TERMexecuteCommandChainImpl();
         });
      // });

   },
   exit   : function() {
      document.querySelector('.CARDgitTerm').innerHTML = `Open Git Terminal`;
      // reset anchor innerHtml to empty string
   },
};

class TERMterminal {
   constructor(props = {}) {
      const {
               component_id = '',
               container = 'GITterminal',
               anchor    = '.CORmain',
               welcome   = 'Git terminal. Type help for documentation or exit to close the terminal.',
               prompt    = '',
               separator = '&gt;',
            }            = props;
      this.commands      = TERMcommands;
      this.history       = localStorage[TERMkey] ? JSON.parse(localStorage[TERMkey]) : [];
      this.historyCursor = this.history.length;
      this.welcome       = welcome;
      this.shell         = {prompt, separator};
      this.component_id  = component_id;
      this.anchor        = anchor;

      document.querySelector(this.anchor).innerHTML = `<div id="GITterminal"></div>`;
      const el                                      = document.getElementById(container);
      if(el) {
         this.cacheDOM(el);
         this.addListeners();
         if(welcome) {this.output(welcome);}
      }
      else {throw Error(`Container #${container} doesn't exists.`);}
   }

   state = {prompt: undefined, idle: undefined};

   cacheDOM = (el) => {
      el.classList.add(TERMkey);
      el.insertAdjacentHTML('beforeEnd', TERMhtml(this));

      // Cache DOM nodes
      const container = el.querySelector('.TERMcontainer');
      this.DOM        = {
         container,
         output : container.querySelector('.TERMcontainer output'),
         command: container.querySelector('.TERMcommand'),
         input  : container.querySelector('.TERMcommand .input'),
         prompt : container.querySelector('.TERMcommand .TERMprompt'),
      };
   };

   addListeners = () => {
      const {DOM} = this;
      const observer = new MutationObserver(() => {       // Replaced DOMSubtreeModified with MutationObserver to resolve console deprecation errors.
         setTimeout(() => DOM.input.scrollIntoView(), 10);
      });

      observer.observe(DOM.output, { childList: true, subtree: true });

      DOM.output.addEventListener('click', event => event.stopPropagation(), false);
      DOM.input.addEventListener('keyup', this.onKeyUp, false);
      DOM.input.addEventListener('keydown', this.onKeyDown, false);
      DOM.command.addEventListener('click', () => DOM.input.focus(), false);
   };

   onKeyUp = (event) => {
      const {keyCode}                          = event;
      const {DOM, history = [], historyCursor} = this;

      if(keyCode === 27) { // ESC key
         DOM.input.value = '';
         event.stopPropagation();
         event.preventDefault();
      }
      else if([38, 40].includes(keyCode)) {
         if(keyCode === 38 && historyCursor > 0) {this.historyCursor -= 1;} // {38} UP key
         if(keyCode === 40 && historyCursor < history.length - 1) {this.historyCursor += 1;} // {40} DOWN key
         if(history[this.historyCursor]) {DOM.input.value = history[this.historyCursor];}
      }
   };

   onKeyDown = ({keyCode}) => {
      const {commands = {}, DOM, history, onInputCallback, state} = this;
      let commandLine                                             = DOM.input.value.trim();
      if(keyCode !== 13 || !commandLine) {return;}
      const [command, ...parameters] = commandLine.split(' ');

      if(state.prompt) {
         state.prompt = false;
         this.onAskCallback(command);
         this.setPrompt();
         this.resetCommand();
         return;
      }

      // Save command line in history
      history.push(commandLine);
      localStorage[TERMkey] = JSON.stringify(history);
      this.historyCursor    = history.length;

      // Clone command as a new output line
      DOM.output.appendChild(TERMcloneCommandHtml(DOM.command));

      // Clean command line
      DOM.command.classList.add('hidden');
      DOM.input.value = '';

      // Dispatch command
      if(Object.keys(commands).includes(command)) {
         const callback = commands[command];
         if(callback) {callback(this, parameters);}
         if(onInputCallback) {onInputCallback(command, parameters);}
      }
      else {
         const callback = commands['execute'];
         callback(this, this.component_id, command, parameters);
         if(onInputCallback) {onInputCallback(command, parameters);}
      }
   };

   resetCommand = () => {
      const {DOM}     = this;
      DOM.input.value = '';
      DOM.command.classList.remove('input');
      DOM.command.classList.remove('hidden');
      if(DOM.input.scrollIntoView) {DOM.input.scrollIntoView();}
   };

   clear() {
      this.DOM.output.innerHTML = '';
      this.resetCommand();
   }

   idle() {
      const {DOM} = this;
      DOM.command.classList.add('idle');
      DOM.prompt.innerHTML = '<div class="spinner"></div>';
   }

   prompt(prompt, callback = () => {}) {
      this.state.prompt         = true;
      this.onAskCallback        = callback;
      this.DOM.prompt.innerHTML = `${prompt}:`;
      this.resetCommand();
      this.DOM.command.classList.add('input');
   }

   onInput(callback) {this.onInputCallback = callback;}

   output(html = '&nbsp;') {
      if(html !== '&nbsp;') html = ESChtmlEscape(html)
      this.DOM.output.insertAdjacentHTML('beforeEnd', `<span>${html}</span>`);
      this.resetCommand();
   }

   setPrompt(prompt = this.shell.prompt) {
      const {DOM, shell: {separator}} = this;
      this.shell                      = {prompt, separator};
      DOM.command.classList.remove('idle');
      DOM.prompt.innerHTML = `${prompt}${separator}`;
      DOM.input.focus();
   }

   executeCustom(CustomCommand) {
      const $selector = document.querySelectorAll('.TERMcommand');
      let input = $selector[$selector.length - 1].children[1];
      input.value = CustomCommand;
      this.onKeyDown({keyCode:13});
   }
}

let TERMcommandChainArray = [];
let TERMcommandChainCallback;

function TERMexecuteCommandChainImpl() {
   if (TERMcommandChainArray.length) {
      let Command = TERMcommandChainArray.shift(); 
      setTimeout(function(){TGITterminal.executeCustom(Command); }, 1000);
   } else {
      console.log("Git Terminal: All commands are finished");
      if (TERMcommandChainCallback) {
         TERMcommandChainCallback();
         TERMcommandChainCallback = undefined;
      }
      MODELforcePoll('TRANSmodel');
      TRANSfileCurrentReload();
      TERMdelayedSetFocus();
   }
}

function TERMdelayedSetFocus(){
   setTimeout(TERMsetFocus, 300);
}

function TERMsetFocus(){
   let Div = document.querySelector(".TERMcommand input")
   if(!Div ) return;
   Div.focus();
}
