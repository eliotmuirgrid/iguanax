function BRIDGEcreate(channelNames) {
   const channels = {};
   channelNames.forEach(name => { channels[name] = null; });
   return {
      channels,
      bind(handlers) { Object.assign(this.channels, handlers); },
      emit(name, ...args) { this.channels[name]?.(...args); }
   };
}

function BRIDGEinit(name, channelKeys) {
   if(!window[name]) {
      window[name] = {};
      channelKeys.forEach(key => { window[name][key] = BRIDGEcreate([]); });
   }
   return window[name];
}