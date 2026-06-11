#include <COL/COLlog.h>
COL_LOG_MODULE;

class APPfoo{
public:
   APPfoo() {
      COL_METHOD(APPfoo::APPfoo);
   }
   ~APPfoo() {
      COL_METHOD(APPfoo::~APPfoo);
   }  
};

void APPtest(int i){
   COL_FUNCTION(APPtest);
   COL_VAR(i);
   if (i == 7){
      COL_TRC("Bang!");
   }
   COLstring A = "sklfjas;dkfhjasp;fjikjIOHPIUGHuygweufghrwiefubweiufhwioe w eufhioweu wiefuwio ufwoieu";
   COL_DUMP("Some Data:", A.c_str(), A.size());
   APPfoo C;
   APPfoo D;
}