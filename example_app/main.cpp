#include <stdio.h>

#include <COL/COLstring.h>
#include <COL/COLostream.h>

#include <CMD/CMDlineParser.h>

void APPtest(int i);

int main(int argc, const char** argv){
   CMDlineParser Parser;

   Parser.parseArgs(argc, argv);
   if (Parser.parsingErrorsPresent(COLcout)){
      Parser.showUsage(COLcout);
   }

   COLstring MyString;
   MyString = "Harold";
   MyString += " is a nice guy!";
   COLcout << MyString << newline;
   for (int i=0; i < 8; i++){
      APPtest(i);
   }
}
