#include <stdio.h>

#include <FIL/FILutils.h>

#include <SDM/SDMlogEvent.h>

#include <CMD/CMDlineParser.h>

#include <DBG/DBGinit.h>
#include <COL/COLerror.h>
#include <COL/COLthreadPool.h>
#include <COL/COLclosure.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void APPstack(int i){
   COL_FUNCTION(APPstack);
   COL_VAR(i);
   if (i > 0){
      return APPstack(i-1);
   }
   COLcout << "Backtrace:" << COLbacktrace();
}

int main(int argc, const char** argv){
   FILinitExecutableName(argv[0]);
   CMDlineParser Parser;
   Parser.parseArgs(argc, argv);
   
   COLstring Error;
   DBGinitDebug(&Error);

   COLthreadPool Pool;
   Pool.start();

   APPstack(10);
   //for (int i = 0; i < 10; i++){
   //   Pool.scheduleTask(COLnewClosure0(&APPstack, 10));
   //}

   Pool.shutdown(COLthreadPool::shutdown_after_pending_tasks);
   return 0;
}
