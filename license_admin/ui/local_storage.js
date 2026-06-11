/** @license
 * A thin wrapper around storage, with one key difference:
 * this allows the specification of the data's expiry (like cookies).
 * Each time storage is accessed (through set(), get(), or remove()),
 * all expired data will be removed.
 * Only strings may be stored, but if the string 'true' or 'false' is stored, then it will return Boolean version.
 * Copyright (c) 2010-2011 iNTERFACEWARE Inc.  All rights reserved.
 */

// require 'ifware.js'
(function(ifware) {
    ifware.LocalStorage = function(storeName) {
        var ONE_DAY_IN_MS = 24*60*60*1000,
        LastExpiryCheck, // number of days since 1970/01/01.
        store = window.localStorage, // declare reference so it will be easy to shim later if necessary.
        expiryDelimiter = '^',
        delimiter = '_';

        // current time, in ms, since 1970/01/01.
        function now(){
            return (new Date()).getTime();
        };

        // time in a specified number of days, in ms, since 1970/01/01.
        function nowPlusDays(Days){
            var days = now() + (Days * ONE_DAY_IN_MS);
            return days;
        };

        function splitEntry(Entry){
            var i = Entry.indexOf(expiryDelimiter);
            return [Entry.substring(0,i), Entry.substring(i+1)];
        };

        function clearExpired(){
            // For now, since we only support 'day' granularity, we
            // might as well just check for expired entries once a day.
            $(this).trigger(this.events.clearExpired);
            var Today = Math.floor(now() / ONE_DAY_IN_MS);
            if (Today === LastExpiryCheck) return;
            var Now = now();
            for (k in store){
                try {
                    var Entry = splitEntry(store[k]);
                    var Expiry = parseInt(Entry[0]);
                    if (Expiry != NaN && Expiry < Now){
                        store.removeItem(k);
                    }
                } catch (e) {
                    // do nothing - format of storage key likely just
                    // didn't match what we expected.
                }
            }
            LastExpiryCheck = Today;
        };

        ////////////////
        // Public API //
        ////////////////
        if (store) {
            return {
                events: { clearExpired: 'clearExpired' },
                set: function(key, value, days){
                    clearExpired.call(this);
                    // Write out the expiry in ms.  We could write out in
                    // days, but we may want local_storage.js to be able
                    // to support smaller granularity than days.
                    if (storeName) {
                        key = storeName.concat(delimiter, key);
                    }
                    store.setItem(key, nowPlusDays(days).toString().concat(expiryDelimiter, value));
                },

                setObject: function(key, value, days, jsonArg1, jsonArg2) {
                    /*
                     (key:String, value: Object, [*args]) -> null

                     Store a JSON-compatible object.
                     */
                    var json = JSON.stringify(value, jsonArg1, jsonArg2);
                    this.set(key, json, days);
                },

                get: function(key) {
                    clearExpired.call(this);
                    try {
                        if (storeName) {
                            key = storeName.concat(delimiter, key);
                        }
                        var Entry = splitEntry(store.getItem(key));
                        if (Entry[1] === 'true' || Entry[1] === 'false') {  // return a Boolean if the string is true or false.
                            return Entry[1] === 'true';
                        }
                        return Entry[1];
                    } catch (e) {
                        return null;
                    }
                },

                getObject: function (key, jsonArg) {
                    /*
                     (key:String, [*args]) -> Object

                     Return a parsed JSON object stored under the specified key.
                     */
                    var json = this.get(key);
                    return json ? JSON.parse(json, jsonArg) : null;
                },

                remove: function(key) {
                    clearExpired.call(this);
                    if (storeName) {
                        key = storeName.concat(delimiter, key);
                        store.removeItem(key);
                    }
                }

//                clear: function() {
//                    store.clear();
//                }
            }
        } else {  // until we get a shim that uses cookies or sessionStorage we will just define a storage that does nothing...
            return {
                set:$empty,
                get:$lambda(null),
                remove:$empty,
                clear:$empty
            };
        }
    };

    ifware.LocalStorage.setOrGetState = function(state, stateVarLabel, toState, storage) {
        if (typeof toState === 'undefined') {
            if (!state[stateVarLabel].loaded) {
                state[stateVarLabel].loaded = true;
                var cachedVal = storage.get(stateVarLabel);
                if (cachedVal) {  // if there is no cached val, it will end up being the default value (the value passed originally)
                    state[stateVarLabel] = cachedVal;
                }
            }
        }
        else if (toState !== state[stateVarLabel]) {
            if (state[stateVarLabel].constructor === toState.constructor) {
                state[stateVarLabel] = toState;
                storage.set(stateVarLabel, toState);
            }
            else {
                throw 'Cannot set state of '.concat(stateVarLabel, ' to a value to a value of a different type.');
            }
        }
        return state[stateVarLabel];
    };

    // Create a general-purpose instance of local storage, separate from per-project settings.
    ifware.storage = ifware.LocalStorage();
})(ifware || {});