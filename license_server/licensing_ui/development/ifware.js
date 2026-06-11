ifware = window.ifware || {};

(function(ifware) {
    $lambda =  function(x) { return function() { return x; }; };  // intentional global
    $empty = function() { };  // intentional global

    /** Create a state object and attach it to $this.data,
     * and also support inheritance by merging states with the current state.
     * @param $this
     * @param defaults
     * @param userOptions
     * @param parentClass
     * @return {state}
     */
    ifware.extendFrom = function init($this, defaults, userOptions, parentClass){
        if (parentClass) parentClass($this, userOptions);
        $.extend(defaults, userOptions);
        if ($this.data('state')) {
            $.extend($this.data('state'), defaults);
        } else {
            $this.data('state', defaults);
        }
        return $this.data('state');
    };
})(ifware);

