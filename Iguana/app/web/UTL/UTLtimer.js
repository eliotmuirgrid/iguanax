const UTLinterval = {
   intervals: {},

   new: function(func, delay) {
      const newInterval = setInterval(func, delay);
      this.intervals[newInterval] = true;
      return newInterval;
   },

   clear: function(id) {
      clearInterval(id);
      delete this.intervals[id];
   },

   clearAll: function() {
      for(let id in this.intervals) {
         this.clear(id);
      }
   },
};

function Timer(Callback, Period, DefaultState) {
   const THIS = this;
   this._Interval = UTLinterval;
   this._RefreshTime = Period || 2*1000;
   this._State = DefaultState || false;

   this.pause = function(force) {
      if (THIS._State || force) {
         THIS._Interval.clear(THIS._Id)
         THIS._State = false;
         return true;
      } else {
         return false;
      }
   }

   this.resume = function(force) {
      if (!THIS._State || force) {
         THIS._Id = THIS._Interval.new(Callback, THIS._RefreshTime);
         THIS._State = true;
      }
   }

   this.clear = function() {
      THIS._Interval.clear(THIS._Id)
   }
}