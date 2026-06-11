
#include "AVTclear.h"
#include "AVTformat.h"

#include <COL/COLlog.h>
#include <COL/COLostream.h>
#include <COL/COLstring.h>
COL_LOG_MODULE;


COLostream& operator<<(COLostream& Stream, const AVTclear& Clear) {
   Stream << Clear.Clear();
   return Stream;
}