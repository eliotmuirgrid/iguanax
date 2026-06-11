#include <ctype.h>
#include <stdlib.h>
#include <COL/COLlog.h>
#include <COL/COLmap.h>
#include <CHL/CHLutils.h>
#include <COL/COLostream.h>
#include <CMD/CMDlineParser.h>
#include <DLL/CHMdataType.h>
#include <DLL/CHMidentifier.h>
#include <DLL/CHMengineInternal.h>
#include <DLL/CHMsegmentSubField.h>
#include <DLL/CHMsegmentGrammar.h>
#include <DLL/CHMmessageGrammar.h>
#include <DLL/CHMdateTimeGrammar.h>
#include <DLL/CHMcolumnDefinition.h>
#include <DLL/CHMmessageNodeAddress.h>
#include <DLL/CHMtableGrammarInternal.h>
#include <DLL/CHMtableDefinitionInternal.h>
#include <DLL/CHMmessageDefinitionInternal.h>
#include <DLL/CHMenumerationGrammar.h>
#include <DLL/CHMdataType.h>
extern "C" {
#include <DLL/CHMpythonConfig.h>
}
COL_LOG_MODULE;

#define RNLEN 10
#define FNLEN 67

class VMDexport {
private:
   COLmap<COLstring,int>         namReg;
   COLmap<const void*,COLstring> objReg;
   COLostreamString              errost;
   COLostreamString              outost;

   COLstring rpad(COLstring s, int n) { if (s.size()<n) s.append(n-s.size(),' '); return s; }
   COLstring ascii(const COLstring& s) { // Fix-up the string so that it is ASCII
      COLstring buf;
      for (int i=0, N=s.size() ; i<N ; i++) {
         if      (isprint(s[i])                                      ) { buf+=s[i];       }
         else if (i+0<N && s[i]==-110                                ) { buf+='\'';       }
         else if (i+1<N && s[i]== -62 && s[i+1]== -76                ) { buf+='\''; i+=1; }
         else if (i+2<N && s[i]== -30 && s[i+1]==-128 && s[i+2]==-103) { buf+='\''; i+=2; }
         else if (i+2<N && s[i]== -30 && s[i+1]==-128 && s[i+2]==-109) { buf+='-' ; i+=2; }
         else if (i+2<N && s[i]== -30 && s[i+1]==-128 && s[i+2]==-100) { buf+='\"'; i+=2; }
         else if (i+2<N && s[i]== -30 && s[i+1]==-128 && s[i+2]== -99) { buf+='\"'; i+=2; }
         else if (i+2<N && s[i]== -30 && s[i+1]==  63 && s[i+2]==  63) { buf+='\"'; i+=2; }
         else if (i+2<N && s[i]== -30 && s[i+1]==  63 && s[i+2]== -99) { buf+='\"'; i+=2; }
      }
      if (buf!=s) { errost << "\nWARN: modified text content\n\tOld:" << s << "\n\tNew:" << buf << "\n"; }
      return buf;
   }
   COLstring quote(const COLstring& str, bool force) { // quote styles ABC "ABC" 'ABC' [ABC]
      for (int i=0, N=str.size() ; i<N && !force ; i++) { force = (str[i]!='_' && !isalnum(str[i])); }
      if (!force && str.size()) { return str; }
      const char opn[] = "\"\'[";
      const char cls[] = "\"\']";
      for (int i=0, j=0 ; i<3 ; i++) {
         if (!str.find(&j,cls[i],0)) { return opn[i] + str + cls[i]; }
      }
      errost << "\nERROR: unable to quote string" << str << "\n";
      return opn[0] + str + cls[0];
   }
   COLstring qname(const COLstring& str) { return quote(ascii(str),false); }
   COLstring qtext(const COLstring& str) { return quote(ascii(str),true ); }
   COLstring comment(const COLstring txt) {
      COLstring buf = " -- ";
      if (txt.is_null()) { return "\n"; }
      for (const char* p=txt.c_str() ; *p ; p++)
         { buf += (*p=='\n' || *p=='\r' || !isprint(*p)) ? ' ' : (*p); }
      return buf+"\n";
   }
   COLstring nameof(const COLstring& pfx, const void* hdl, const COLstring& sfx, int width) {
      if (objReg.count(hdl)==0) {
         objReg[hdl] = pfx;
         const COLstring key = pfx+sfx;
         if (namReg.count(key)==0) { namReg[key] = 0; }
         else {
             objReg[hdl].append("_").append(COLint32ToString(++namReg[key])); 
             errost << "\nWARN: duplicate name " << key << "\n";
         }
         objReg[hdl].append(sfx);
      }
      return rpad(qname(objReg[hdl]),width);
   }
   COLstring format(const CHMdateTimeGrammar& grm) {
      COLstring fmt;
      for (size_t i=0, N=grm.countOfMaskItem() ; i<N ; i++) {
         switch (grm.maskItem(i)) {
            case CHMdateTimeGrammar::miYear4     : fmt.append("Y"); break;
            case CHMdateTimeGrammar::miMonth     : fmt.append("m"); break;
            case CHMdateTimeGrammar::miDay       : fmt.append("d"); break;
            case CHMdateTimeGrammar::miHour      : fmt.append("H"); break;
            case CHMdateTimeGrammar::miMinute    : fmt.append("M"); break;
            case CHMdateTimeGrammar::miSecond    : fmt.append("S"); break;
            case CHMdateTimeGrammar::miSecondPart: fmt.append("f"); break;
            case CHMdateTimeGrammar::miZone      : fmt.append("z"); break;
         }
      }
      if (!grm.fieldsRequired()) { fmt.append("?"); }
      return fmt;
   }
   void schExportComposite(const CHMcompositeGrammar& grm) {
      COL_FUNCTION(schExportComposite);
      const COLstring name = grm.name();
      const COLstring desc = grm.description();
      const COLstring padd = rpad("\n",14);
      const COLstring warn = "\nWARN: Composite Type "+name;
      outost << nameof(name,&grm,"_c",RNLEN) << " = notation { " << qtext(name+":"+desc) << " }";
      if (grm.countOfField()==0) {
         errost << warn << " has empty contents.  Forcing it to be an alias for string type.\n";
         outost  << " string";
      }
      for (COLuint32 i=0, N=grm.countOfField() ; i<N ; i++) {
         const CHMdataType type = grm.fieldDataType(i);
         const bool        reqd = grm.fieldIsRequired(i);
         const COLstring   fild = grm.fieldName(i);
         const bool        comp = (type==CHMcompositeType);
         if (N>1 || comp) {
            if (N<=1 ) { errost << warn << " is aliasing composite\n"; }
            if (!comp) { errost << warn << " has mixed content.\n";    }
            outost  << padd << "element " << rpad(qname(fild),FNLEN) << " { "
                    << (reqd?"options { non-empty } ":"                      ");
         } else {
            if (fild!="Value") { errost << warn << " - Expected field name of Value instead of " << fild << "\n"; }
            outost << (reqd?"options { non-empty } ":" ") ;
         }
         if (comp) {
            const CHMcompositeGrammar* ref = grm.fieldCompositeType(i);
            if (grm.fieldIsLengthRestricted(i)) { errost << warn << " ignoring length restriction.\n"; }
            outost << nameof(ref->name(),ref,"_c",RNLEN) ;
         } else {
            if      (type==CHMstringType   ) { outost << "string" ; }
            else if (type==CHMintegerType  ) { outost << "integer"; }
            else if (type==CHMdoubleType   ) { outost << "double" ; }
            else if (type==CHMdateTimeType ) { outost << "datetime(" << qtext(format(*(grm.fieldDateTimeGrammar(i)))); }
            if (grm.fieldIsLengthRestricted(i) && grm.fieldMaxLength(i)>0) {
               if (type==CHMdateTimeType)    { outost << "," << grm.fieldMaxLength(i); }
               else                          { outost << "(" << grm.fieldMaxLength(i); }
            }
            if (type==CHMdateTimeType)       { outost << ")"; }
         }
         if (N>1 || comp) { outost << " }"; }
      }
      outost << " ; \n\n";
   }
   void schExportSegment(const CHMsegmentGrammar& grm) {
      COL_FUNCTION(schExportSegment);
      const COLstring name = grm.name();
      const COLstring padd = rpad("\n",RNLEN+7);
      if (grm.countOfIdentifier()!=0) {
        errost << "\nERROR: Segment Type " << name << " has identity constraints which cannot be exported\n";
      }
      outost << nameof(name,&grm,"_s",RNLEN) << " = element " << qname(name) << " {"
             << padd << "options  { segment }"
             << padd << "notation { " << qtext(name+":"+grm.description()) << " }";
      for (COLuint32 i=0, N=grm.countOfField() ; i<N ; i++) {
         const CHMsegmentSubField&  fld = grm.field(i);
         const CHMcompositeGrammar* ref = fld.fieldType();
         const bool                 req = fld.isRequired();
         const bool                 rep = (fld.maxRepeats()!=0);
         outost << padd << "element " << rpad(qname(fld.name()),FNLEN) << " { "
                << (req?"options { non-empty } ":"                      ")
                << nameof(ref->name(),ref,"_c",RNLEN) << " }" << (rep?"*":"") ;
      }
      outost << rpad("\n",RNLEN+3) << " } ;\n\n";
   }
   void schExportGrouping(const CHMmessageGrammar& grm, const COLstring& padd) {
      COL_FUNCTION(schExportGrouping);
      COLstring sfx;
      if (grm.isRepeating()) { sfx = (grm.isOptional())?"* ":"+ "; }
      else                   { sfx = (grm.isOptional())?"? ":" " ; }
      if (grm.isNode()) { // Leaf node - a segment
         const CHMsegmentGrammar* ref = grm.segment();
         outost << padd << nameof(ref->name(),ref,"_s",0);
      } else {
         outost << padd << "element " << qname(grm.grammarName()) << " {";
         if (grm.ignoreSegmentOrder()) { outost << padd << "   options  { unordered }"; }
         for (int i=0 ; i<grm.countOfSubGrammar() ; i++) { schExportGrouping(*(grm.subGrammar(i)),padd+"  "); }
         outost << padd << "}"<<sfx;
      }
   }
   void schExportMessage(const CHMmessageDefinitionInternal& msg) {
      COL_FUNCTION(schExportMessage);
      const CHMmessageGrammar& grm = *(msg.messageGrammar());
      if (grm.countOfSubGrammar()==0) { return; }
      const COLstring mname = msg.name();
      const COLstring mdesc = msg.description();
      const COLstring padd  = rpad("\n",RNLEN+7);
      outost << nameof(mname,&msg,"_m",RNLEN) << " = element " << qname(mname) << " {"
             << padd << "options  { "
             << (msg.ignoreUnknownSegments() ? " ignore_unknown_seg" : "")
             << ((msg.ignoreSegmentOrder() || grm.ignoreSegmentOrder()) ? " unordered" : "") << " }"
             << padd << "notation { " << qtext(mname+":"+mdesc) << " }"
             << padd << "identity { ";
      bool brac = false;
      COLstring mtype, mtrig;
      for (COLuint32 i=0, N=msg.countOfIdentifier() ; i<N ; i++) {
         const CHMidentifier&         idn = msg.identifier(i);
         const CHMmessageNodeAddress& adr = *(idn.nodeAddress());
         const COLstring              seg = idn.segment();
         if      (seg=="MSH" && adr.depth()==2 && adr.nodeIndex(0)==8 && adr.nodeIndex(1)==0) { mtype=idn.value(); }
         else if (seg=="MSH" && adr.depth()==2 && adr.nodeIndex(0)==8 && adr.nodeIndex(1)==1) { mtrig=idn.value(); }
         else {
            if (!brac) { brac = true; outost << "("; }
            outost << padd << "   */(1/1/1/1=<<" << (idn.segment()) << ">> ";
            for (COLuint32 j=0, M=adr.depth() ; j<M ; j++) {
               if (j==0) { // Segment level
                  const int rep = adr.repeatIndex(j);
                  outost << adr.nodeIndex(j)+2 << "/" << (rep>0?rep:1);
               } else {
                  outost << "/" << adr.nodeIndex(j)+1;
               }
            }
            for (int j=adr.depth()+1 ; j<4 ; j++) { outost << "/1"; }
            outost << "=<<" << idn.value() << ">>)";
         }
      }
      if (mtype.size() || mtrig.size()) {
         if (brac) { outost << padd << "   "; }
         outost << "*/(1/1/1/1=<<MSH>>";
         if (mtype.size()) { outost << " 10/1/1/1=<<" << mtype << ">>"; }
         if (mtrig.size()) { outost << " 10/1/2/1=<<" << mtrig << ">>"; }
         outost << ") ";
      }
      if (brac) { outost << padd << ") }"; }
      else      { outost << " }";          }
      outost << padd << "populate { " << qname("MSH") << "/9/1=" << qtext(mtype) << " "
                                      << qname("MSH") << "/9/2=" << qtext(mtrig) << " }";
      COLASSERT(!grm.isNode());
      for (int i=0, N=grm.countOfSubGrammar() ; i<N ; i++) { schExportGrouping(*(grm.subGrammar(i)),padd); }
      outost << rpad("\n",RNLEN+4) << "} ;\n\n";
   }
public:
   void schExport(const CHMengineInternal& eng) {
      COL_FUNCTION(schExport);
      for (COLuint32 i=0 ; i<eng.countOfComposite() ; i++) { schExportComposite(*(eng.composite(i))); }
      for (COLuint32 i=0 ; i<eng.countOfSegment()   ; i++) { schExportSegment  (*(eng.segment  (i))); }
      for (COLuint32 i=0 ; i<eng.countOfMessage()   ; i++) { schExportMessage  (*(eng.message  (i))); }
      outost << rpad("start",RNLEN) << " = ";
      for (COLindex i=eng.firstMessage(), N=eng.lastMessage(), x=0 ; true ; i=eng.nextMessage(i)) {
         const COLuint32 j = eng.matchIndexToMessageIndex(i);
         const CHMmessageDefinitionInternal* ref = eng.message(j);
         outost << nameof(ref->name(),ref,"_m",RNLEN);
         if (i==N)     { break;                        }
         if (++x%7==0) { outost << rpad("\n",RNLEN+1); }
         outost << " | ";
      }
      outost << ";\n";
   }
   void dbsExport(const CHMengineInternal& eng) {
      COL_FUNCTION(dbsExport);
      for (COLuint32 i=0, N=eng.countOfTable() ; i<N ; i++) {
         const CHMtableDefinitionInternal& tbl = *(eng.table(i));
         outost << "create table [" << (tbl.tableName()) << "]" << comment(tbl.description()) << "(\n";
         COLstring key;
         for (COLuint32 j=0, M=tbl.countOfColumn() ; j<M ; j++) {
            const CHMcolumnDefinition& col = tbl.column(j);
            const COLstring& cname = "[" + (col.name()) + "]";
            COLstring        ctype = "unknown";
            switch (col.dataType()) {
               case CHMstringType  : ctype = "string"  ; break;
               case CHMintegerType : ctype = "integer" ; break;
               case CHMdoubleType  : ctype = "double"  ; break;
               case CHMdateTimeType: ctype = "datetime"; break;
            }
            if (col.isKey()) { key.append(key.size()?",":"").append(cname); }
            outost << "   " << cname << " " << ctype << ((j+1<M || key.size())?",":"") << comment(col.description());
         }
         if (key.size()) { outost << "      primary key (" << key << ")\n"; }
         outost << ");\n\n";
      }
   }
   COLstring errors() const { return errost.str(); }
   COLstring output() const { return outost.str(); }
};

int main(int argc, const char** argv) {
   CMDlineParser psr;
   COL_LOG_INIT(argc, argv);
   psr.setDescription("Export VMD file to dbs or sch format");
   psr.addFlagWithArgument("vmd" , "<vmd_file>", "Input VMD file");
   psr.addFlagWithArgument("mode", "dbs|sch"   , "Export format" );
   psr.addFlagWithArgument("cfg" , "<cfg_idx>" , "Select sepcific config index");
   psr.parseArgs(argc, argv);
   if (psr.parsingErrorsPresent(COLcout) ||
      !psr.isFlagPresent("vmd")          ||
      !psr.isFlagPresent("mode")          ) { psr.showUsage(COLcout); return 1; }
   CHMengineInternal eng;
   eng.clear();
   CHLloadEngine(eng, psr.flagArgument("vmd"));
   if (psr.isFlagPresent("cfg")) {
      const int i = ::atoi(psr.flagArgument("cfg").c_str());
      if (0<=i && i<eng.countOfConfig()) { eng.setCurrentConfig(i); }
   }
   const int i = eng.currentConfig();
   COLcerr << "Exporting Configuration " << i << " : " << eng.configName(i) << "\n" << flush;
   VMDexport vmde;
   if      (psr.flagArgument("mode")=="dbs") { vmde.dbsExport(eng); }
   else if (psr.flagArgument("mode")=="sch") { vmde.schExport(eng); }
   else                                      { psr.showUsage(COLcout); return 1; }
   COLcerr << vmde.errors() << flush;
   COLcout << vmde.output() << flush;
   return 0;
}
