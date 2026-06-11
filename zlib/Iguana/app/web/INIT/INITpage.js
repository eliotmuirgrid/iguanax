function INITsetup(){
   document.body.innerHTML = '';
   COL_TRC("Setting up startup mode.");
   // Now that we come here immediately I should not need to do these clear operations.
   //clearInterval(SESSheartbeatId);  // TODO - I need to fix this architectural issues
   let PageTable = {
      ""         : function() { INITwelcome();  },
      "prepare"  : function() { INITprepare();  },
      "git"      : function() { INITgit();      },
      "location" : function() { INITlocation(); },
      "config"   : function() { INITconfig();   },
      "user"     : function() { INITuser();     },
      "server"   : function() { INITserver();   },
      "finish"   : function() { INITfinish();   },
      "crash"    : function() { INITcrash();    }
   }
   PAGEstart(PageTable);
   PAGEroute();
   INITheartbeatSetInterval();
}
