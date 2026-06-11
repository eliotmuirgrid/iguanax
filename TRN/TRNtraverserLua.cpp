// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNtraverserLua
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 04 May 2023 - 08:54AM
// ---------------------------------------------------------------------------

#include <TRN/TRNtraverserLua.h>

#include <TRN/TRNintellisenseOptions.h>
#include <TRN/TRNconstant.h>
#include <THTM/THTMprinter.h>
#include <TRN/TRNhtmlEscapeString.h>
#include <TRN/TRNwriteLuaStringKey.h>
#include <TRN/TRNintellisenseDebug.h>  // For TRNfetchHelpJsonFromFile (why here?)

#include <LUA/LUAutils.h>
#include <LUA/LUAescape.h>

#include <LND/LNDutils.h>

#include <HELP/HELPresolve.h>
#include <LHLP/LHLPhelp.h>

#include <LUAL/LUALprint.h>

#include <SFI/SFIhtmlCrop.h>

#include <HELP/HELPmodel.h>

#include <REX/REXmatcher.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

TRNluaItem::TRNluaItem() : m_LuaType(LUA_TNONE), m_IsInteger(false), m_Integer(0), m_pNode(0) {
}

bool TRNluaItem::operator==(const TRNluaItem& That) const{
   return m_String    == That.m_String &&
            m_LuaType   == That.m_LuaType &&
            m_IsInteger == That.m_IsInteger &&
            m_Integer   == That.m_Integer &&
            m_pNode     == That.m_pNode &&
            m_pAddress  == That.m_pAddress;
}


TRNtraverserLua::TRNtraverserLua(lua_State* iL, int TableIndex, bool ForDisplay, char IndexChar)
   : L(iL),
   m_InitialStackSize(lua_gettop(iL)),
   m_TableIndex(TableIndex),
   m_KeyIndex(-1),
   m_ValueIndex(-1),
   m_ValueHasMetatable(false),
   m_ValueDisplayProtocolType(-1),
   m_ValueDisplayNodeType(-1),
   m_ForDisplay(ForDisplay),
   m_IndexChar(IndexChar),
   m_IterationState(NOT_STARTED)
{
   COL_METHOD(TRNtraverserLua::TRNtraverserLua);
   COL_VAR4(m_InitialStackSize, m_TableIndex, ForDisplay, IndexChar);
   COLPRECONDITION(TableIndex > 0);
   m_pTableStack = new COLarray<const void*>();
   initRoot();
}

TRNtraverserLua::TRNtraverserLua(lua_State* iL, int TableIndex, bool ForDisplay, char IndexChar, COLarray<const void*>* pTableStack)
   : L(iL),
   m_InitialStackSize(lua_gettop(iL)),
   m_TableIndex(TableIndex),
   m_KeyIndex(-1),
   m_ValueIndex(-1),
   m_ValueHasMetatable(false),
   m_ValueDisplayProtocolType(-1),
   m_ValueDisplayNodeType(-1),
   m_ForDisplay(ForDisplay),
   m_IndexChar(IndexChar),
   m_IterationState(NOT_STARTED)
{
   COL_METHOD(TRNtraverserLua::TRNtraverserLua);
   COL_VAR4(m_InitialStackSize, m_TableIndex, ForDisplay, IndexChar);
   COL_VAR(pTableStack);
   COLPRECONDITION(TableIndex > 0);
   m_pTableStack = pTableStack;
   m_pTableStack.setIsOwner(false);
}

TRNtraverserLua::~TRNtraverserLua(){
   COL_METHOD(TRNtraverserLua::~TRNtraverserLua);
   // Hint: commenting out the following assertion will allow some
   // "original exceptions" to get thrown beyond here so they
   // can be reported more easily.  Just in case this assertion is
   // failing and you can't tell why.
   COL_VAR2(lua_gettop(L), m_InitialStackSize);
   // COLASSERT(lua_gettop(L) == m_InitialStackSize);
}


static void TRNaddSearchMatchTableOptionsImpl(lua_State* L,
                                              const COLstring& RootVar,
                                              const COLstring& RootText,
                                              const REXmatcher& SearchRegex,
                                              int SearchIndex, // -1 means N/A
                                              int Level,
                                              COLarray<const void*>& TableStack,
                                              TRNsortedOptionsList& Options);


static inline bool TRNluaIsSimple(int LuaType){
   switch (LuaType){
      case LUA_TNUMBER:
      case LUA_TSTRING:
      case LUA_TBOOLEAN:
         return true;
      default:
         return false;
   }
}

static inline bool TRNisInteger(lua_Number Num){
   return (lua_Number)(lua_Integer)Num == Num;
}

static void TRNprintLuaNumberKey(lua_State* L, int KeyIndex, COLostream& Stream){
   lua_Number NumValue = lua_tonumber(L, KeyIndex);
   if (TRNisInteger(NumValue)){
      Stream << '[' << lua_tointeger(L, KeyIndex) << ']';
   } else {
      Stream << '[' << NumValue << ']';
   }
}

static inline void TRNprintLuaBooleanKey(lua_State* L, int KeyIndex, COLostream& Stream){
   Stream << '[' << (lua_toboolean(L, KeyIndex) ? "true" : "false") << ']';
}

static bool TRNprintLuaTableNodeTreeKey(lua_State* L, int KeyIndex, int TableIndex, COLostream& Stream) {
   if (!lua_getmetatable(L, TableIndex)) {
      // table based node tree always have a meta-table.
      return false;
   }

   lua_getfield(L, -1, "_lookup");
   if (!lua_istable(L, -1)) {
      // _lookup table doesn't exist, this is probably not a table based node tree.
      lua_pop(L, 1); // pop _lookup
      lua_pop(L, 1); // pop metatable
      return false;
   }

   COL_TRC("We have a table-based node tree");
   lua_Number NumValue = lua_tonumber(L, KeyIndex);
   lua_rawgeti(L, -1, NumValue);
   COLstring KeyName = lua_tostring(L, -1);
   lua_pop(L, 1); // pop KeyName
   lua_pop(L, 1); // pop _lookup

   if (KeyName.is_null()) {
      // KeyIndex doesn't have a corresponding name in the _lookup table.
      lua_pop(L, 1); // pop metatable
      return false;
   }

   lua_pop(L, 1); // pop metatable

   // now we have the orginal stack

   COLstring Description;
   lua_rawgeti(L, TableIndex, NumValue);     // grab element at index <NumValue>
   if (lua_istable(L, -1)) {
      lua_getmetatable(L, -1);               // grab table element's meta-table
      lua_getfield(L, -1, "_rawdata");       // grab _rawdata from element's meta-table
      Description = lua_tostring(L, -1);
      COL_VAR(Description);
      lua_pop(L, 1);    // pop _rawdata
      lua_pop(L, 1);    // pop element's meta-table
   }
   else {
      Description = lua_tostring(L, -1);     // grab string element's value
   }
   lua_pop(L, 1);       // pop element

   Stream << (int) NumValue << ' ';
   if (KeyName.size() > TRN_INTELLISENSE_MAX_LENGTH){
      THTMprintEscaped(Stream, KeyName.c_str(), TRN_INTELLISENSE_MAX_LENGTH);
      Stream << TRN_INTELLISENSE_TRUNCATED;
   } else {
      THTMprintEscaped(Stream, KeyName.c_str(), KeyName.size());
   }

   Stream << " (" << Description << ")";
   return true;
}

static void TRNgetLuaItem(lua_State* L, int ItemIndex, TRNluaItem& LuaItem){
   COL_FUNCTION(TRNgetLuaItem);
   COL_VAR(ItemIndex);

   LuaItem.m_String.clear();
   LuaItem.m_IsInteger = false;
   LuaItem.m_Integer   = 0;
   LuaItem.m_LuaType   = lua_type(L, ItemIndex);
   LuaItem.m_pNode     = 0;
   LuaItem.m_pAddress  = 0;

   switch (LuaItem.m_LuaType){
      case LUA_TNUMBER:{
         if (TRNisInteger(lua_tonumber(L, ItemIndex))){
            lua_Integer IntValue = lua_tointeger(L, ItemIndex);
            LuaItem.m_String = COLint32ToString(IntValue);
            LuaItem.m_IsInteger = true;
            LuaItem.m_Integer = IntValue;
         } else {
            COLostream Stream(LuaItem.m_String);
            LUAformatNumber(L, ItemIndex, Stream);
         }
         break;
                       }
      case LUA_TBOOLEAN:
         LuaItem.m_String = (lua_toboolean(L, ItemIndex) ? "true" : "false");
         break;
      case LUA_TSTRING:
         LuaItem.m_String = LUAtoString(L, ItemIndex);
         break;
      default:
         LuaItem.m_pAddress = lua_topointer(L, ItemIndex);
         if (LNDisNode(L, ItemIndex)){
            LuaItem.m_pNode = LNDgetNode(L, ItemIndex);
            COLPOSTCONDITION(LuaItem.m_pNode);
         }
         break; // do nothing
   }
}

static bool TRNisTableInStack(const void* pTable, const COLarray<const void*>& TableStack){
   //COL_FUNCTION(TRNisTableInStack);
   int MostRecentTable = TableStack.size()-1;
   for (int i = MostRecentTable; i >= 0; i--){
      if (TableStack[i] == pTable){
         return true;
      }
   }
   return false;
}

void TRNtraverserLua::initRoot(){
   COL_METHOD(TRNtraverserLua::initRoot);
   m_ValueHasMetatable = LUALdisplayComplexItem(
      L, m_TableIndex, &m_ValueShortLabel, &m_ValueLongLabel, m_ValueDisplayProtocolType,
      m_ValueDisplayNodeType, m_pTableStack.get(), true);
   COL_VAR4(m_ValueLongLabel, m_ValueShortLabel, m_ValueDisplayProtocolType, m_ValueDisplayNodeType);

   TRNgetLuaItem(L, m_TableIndex, m_Value);
}

void TRNtraverserLua::initIteration(){
   //COL_METHOD(TRNtraverserLua::initIteration);
   const void* pThisTable = lua_topointer(L, m_TableIndex);
   if (TRNisTableInStack(pThisTable, *m_pTableStack)){
      // If table is already in the stack, we just don't step in.
      m_IterationState = DONE;
      return;
   }
   m_pTableStack->push_back(pThisTable);

   // Check for _keys or _displayKeys (see #19668).
   if (initKeysTableIteration()){
      // initKeysTableIteration() already did everything we need.
   } else if (lua_istable(L, m_TableIndex)){
      m_IterationState = ROOT_TABLE;
      initStackForIteration();
   } else if (!lua_isuserdata(L, m_TableIndex) || !initIndexTableIteration()){
      m_IterationState = DONE;
   }

   if (m_IterationState == DONE){
      m_pTableStack->pop_back();
   }
}

void TRNtraverserLua::initStackForIteration(){
   //COL_METHOD(TRNtraverserLua::initStackForIteration);
   lua_pushnil(L); // first key
   lua_pushnil(L); // fake value
   m_ValueIndex = lua_gettop(L);
   m_KeyIndex   = m_ValueIndex - 1;
}

// If a keys table is found, leave it on top of the stack and return 1.
// Returns 0 otherwise.
static int TRNgetKeysTable(lua_State* L, int i, bool ForDisplay){
   COL_FUNCTION(TRNgetKeysTable);
   COL_VAR2(i, ForDisplay);
   const char* pKeysMetafield = ForDisplay ? "_displayKeys" : "_keys";
   if (luaL_getmetafield(L, i, pKeysMetafield)){
      if (lua_istable(L, -1)){
         return 1;
      } else if (lua_isfunction(L, -1)){
         lua_pushvalue(L, i); // push copy of item to top of stack, for pcall.
         if (lua_pcall(L, 1, 1, 0) == 0){
            // Success; return value is on top of the stack.
            if (lua_istable(L, -1)){
               return 1;
            }
         }
      } else {
         lua_pop(L, 1); // pop metafield
      }
   }
   return 0;
}

bool TRNtraverserLua::initKeysTableIteration(){
   //COL_METHOD(TRNtraverserLua::initKeysTableIteration);
   if (TRNgetKeysTable(L, m_TableIndex, m_ForDisplay)){
      m_IterationState = KEYS_TABLE;
      m_OriginalTableIndex = m_TableIndex;
      m_TableIndex = lua_gettop(L);
      lua_pushnil(L); // first integer index
      lua_pushnil(L); // fake key
      lua_pushnil(L); // fake value
      m_ValueIndex = lua_gettop(L);
      m_KeyIndex   = m_ValueIndex - 1;
      return true;
   } else {
      return false;
   }
}

bool TRNtraverserLua::initIndexTableIteration(){
   COL_METHOD(TRNtraverserLua::initIndexTableIteration);
   if (lua_getmetatable(L, m_TableIndex)){
      lua_getfield(L, -1, "__index");
      if (lua_istable(L, -1)){
         lua_replace(L, -2); // replace metatable with its __index table
         m_TableIndex = lua_gettop(L);
         m_IterationState = INDEX_TABLE;
         initStackForIteration();
         // note that the index table is left on the stack - we pop it later.
         return true;
      }
      lua_pop(L, 2); // pop __index and metatable
   }
   return false;
}

void TRNtraverserLua::iterateTable(){
   COL_METHOD(TRNtraverserLua::iterateTable);
   while (true) {
      if (m_IterationState == KEYS_TABLE){
         lua_pop(L, 2); // removes 'key' and 'value'; keeps 'integer index' for next iteration
      } else {
         lua_pop(L, 1); // removes 'value'; keeps 'key' for next iteration
      }
      if (lua_next(L, m_TableIndex) != 0){
         if (m_IterationState == KEYS_TABLE){
            lua_pushvalue(L, m_KeyIndex); // pushes a copy of the key
            lua_gettable(L, m_OriginalTableIndex); // pops the copy of the key, and pushes originalTable[key] on top of the stack (into m_ValueIndex).
         }
         if (initKeyAndValue()){
            return; // iteration successful
         } else {
            continue; // skip item
         }
      } else if (m_IterationState == ROOT_TABLE) {
         // If there was nothing left in the root table, check the __index table.
         if (initIndexTableIteration()){
            continue; // try loop again
         }
      }
      // If we reach here, there's nothing left to look through.
      if (m_IterationState == KEYS_TABLE || m_IterationState == INDEX_TABLE){
         lua_pop(L, 1); // pop the index table.
      }
      m_IterationState = DONE;
      m_pTableStack->pop_back();
      return;
   }
}

bool TRNtraverserLua::stepNext(){
   //COL_METHOD(TRNtraverserLua::stepNext);
   if (m_IterationState == NOT_STARTED){
      initIteration();
   }
   if (m_IterationState != DONE){
      iterateTable();
   }
   return m_IterationState != DONE;
}

TRNtraverser* TRNtraverserLua::traverser() const {
   COL_METHOD(TRNtraverserLua::traverser);
	if (m_Value.m_pNode){
      return TRNcreateTraverser(*m_Value.m_pNode);
   } else if (lua_istable(L, m_ValueIndex) || lua_isuserdata(L, m_ValueIndex)){
      return new TRNtraverserLua(L, m_ValueIndex, m_ForDisplay, m_IndexChar, m_pTableStack.get());
   } else {
      COLPRECONDITION(!isLeaf());
      return 0;
   }
}

bool TRNtraverserLua::isLeaf() const {
   COL_METHOD(TRNtraverserLua::isLeaf);
   COL_VAR(m_Value.m_LuaType);
   // TODO for now we will just say anything that is a table or userdata
   // is not a leaf.  Userdata with no entries in an __index metatable
   // could be considered leaves, but some work would need to be done to
   // get them displayed as such when they are the root of the tree.
   return m_Value.m_LuaType != LUA_TTABLE && m_Value.m_LuaType != LUA_TUSERDATA;
}

void TRNtraverserLua::getTypeInfo(int& ProtocolType, int& NodeType) const {
   COL_METHOD(TRNtraverserLua::getTypeInfo);
   if (m_IterationState != NOT_STARTED) {
      ProtocolType = m_Value.m_pNode ? m_Value.m_pNode->protocolType() : m_ValueDisplayProtocolType;
      NodeType     = m_Value.m_pNode ? m_Value.m_pNode->nodeType()     : m_ValueDisplayNodeType;
   } else {
      COL_TRC("Iteration state has started");
      ProtocolType = m_ValueDisplayProtocolType;
      NodeType     = m_ValueDisplayNodeType;
   }
   COL_VAR2(ProtocolType, NodeType);
}

void TRNtraverserLua::getUniqueKey(COLstring& Key) const{
   COL_METHOD(TRNtraverserLua::getUniqueKey);
   if (m_IterationState != NOT_STARTED){
      const TRNluaItem& CurrentKey = currentKey();
      switch (CurrentKey.m_LuaType){
      case LUA_TNUMBER:
         Key = TRN_NUMBER_KEY_PREFIX + CurrentKey.m_String;
         break;
      case LUA_TBOOLEAN:
         Key = TRN_BOOL_KEY_PREFIX + CurrentKey.m_String;
         break;
      case LUA_TSTRING:
         Key = TRN_STRING_KEY_PREFIX + CurrentKey.m_String;
         break;
      default:
         // Nothing we can do with other weird key types.
         Key = "";
         break;
      }
   } else {
      Key = TRN_ROOT_KEY;
   }
   COL_VAR(Key);
}

void TRNtraverserLua::getLuaKey(COLstring& LuaKey) const {
   COL_METHOD(TRNtraverserLua::getLuaKey);
   COL_VAR(m_IterationState);
   if (m_IterationState != NOT_STARTED){
      LuaKey = currentKey().m_String;
      COL_TRC("Set LuaKey = " << LuaKey);
   } else {
      COL_TRC("Not setting LuaKey");
   }
}

bool TRNtraverserLua::isComplexLuaItem() const {
   return m_ValueHasMetatable;
}

TRNtraverser::eSearchResult TRNtraverserLua::findMatch(const REXmatcher& SearchRegex, int SearchIndex) const {
   COL_METHOD(TRNtraverserLua::findMatch);
   const TRNluaItem& Key = currentKey();

   // First, search for a match in the key
   if (Key.m_IsInteger && Key.m_Integer == SearchIndex){
      return TRNtraverser::INDEX_MATCH;
   }
   COLindex DummyIndex = 0;
   if (SearchRegex.findMatchWithLength(Key.m_String.c_str(), Key.m_String.size(), 0, DummyIndex, DummyIndex)){
      return TRNtraverser::STRING_MATCH;
   }

   // Then, check the value, if it is a simple type
   if (SearchRegex.findMatchWithLength(m_Value.m_String.c_str(), m_Value.m_String.size(), 0, DummyIndex, DummyIndex)){
      return TRNtraverser::STRING_MATCH;
   }

   return TRNtraverser::NO_MATCH;
}

COLstring TRNtraverserLua::optionDisplayText(COLstring* pFuncName) const{
   COL_METHOD(TRNtraverserLua::optionDisplayText);
   COLPRECONDITION(m_KeyIndex > 0);
   COLstring DisplayText;
   COLostream DisplayStream(DisplayText);

   switch (currentKey().m_LuaType){
      case LUA_TNUMBER:
         if (TRNprintLuaTableNodeTreeKey(L, m_KeyIndex, m_TableIndex, DisplayStream)) {
            COL_TRC("Printed node name instead of index for table-based node tree");
            return DisplayText;
         }
         TRNprintLuaNumberKey(L, m_KeyIndex, DisplayStream);
         break;
      case LUA_TSTRING:{
         size_t Size = 0;
         const char* pData = lua_tolstring(L, m_KeyIndex, &Size);
         if (Size > TRN_INTELLISENSE_MAX_LENGTH){
            THTMprintEscaped(DisplayStream, pData, TRN_INTELLISENSE_MAX_LENGTH);
            DisplayStream << TRN_INTELLISENSE_TRUNCATED;
         } else {
            THTMprintEscaped(DisplayStream, pData, Size);
         }
      } break;
      case LUA_TBOOLEAN:
         TRNprintLuaBooleanKey(L, m_KeyIndex, DisplayStream);
         break;
      default:
         COLPRECONDITION(false);
   }
   writeTreeValue(DisplayText, pFuncName);

   return DisplayText;
}

COLstring TRNtraverserLua::deepOptionDisplayText(const REXmatcher& SearchRegex, const COLstring& RootDisplayText, bool IsIndexMatch) const {
   COL_METHOD(TRNtraverserLua::deepOptionDisplayText);
   // IsIndexMatch - is this option a result of searching for a specific index
   COLstring Result = RootDisplayText;
   COLostream ResultStream(Result);
   if (IsIndexMatch){
      COLPRECONDITION(currentKey().m_IsInteger);
      ResultStream << '[' << TRN_BEGIN_HIGHLIGHT_SEARCH_MATCH << currentKey().m_Integer << TRN_END_HIGHLIGHT_SEARCH_MATCH << ']';
   } else {
      if (!RootDisplayText.is_null()) ResultStream << TRN_LEVEL_DIVIDER;
      COLstring HtmlName;
      bool Truncate = (currentKey().m_String.size() > TRN_INTELLISENSE_MAX_LENGTH);
      if (Truncate) {
         HtmlName = LUAstringEscape(currentKey().m_String.c_str(), TRN_INTELLISENSE_MAX_LENGTH);
      } else {
         HtmlName = LUAstringEscape(currentKey().m_String.c_str(), currentKey().m_String.size());
      }
      SearchRegex.regexReplace(HtmlName, TRN_HIGHLIGHT_SEARCH_MATCH, TRNhtmlEscapeString);
      ResultStream << HtmlName;
      if (Truncate) ResultStream << TRN_INTELLISENSE_TRUNCATED;
   }
   return Result;
}

//Dummy function, only really used in TRNtraverserNode
//Ticket #27741 for more info.
COLstring TRNtraverserLua::childValue() const{
   return "";
}

COLstring TRNtraverserLua::optionFullText(const COLstring& RootVar) const {
   COL_METHOD(TRNtraverserLua::optionFullText);
   COLstring FullText = RootVar;
   COLostream FullStream(FullText);

   switch (lua_type(L, m_KeyIndex)){
      case LUA_TNUMBER:
         TRNprintLuaNumberKey(L, m_KeyIndex, FullStream);
         break;
      case LUA_TSTRING:{
         COLstring KeyName = LUAtoString(L, m_KeyIndex);
         if (m_Value.m_LuaType == LUA_TFUNCTION){
            lua_pushvalue(L,m_ValueIndex);
            HELPobject FunctionHelp;
            COLstring HelpFile;
            if (TRNfetchHelpJsonFromFile(L, &FunctionHelp, &HelpFile)){
               COL_VAR(FunctionHelp.ParameterTable);
               TRNwriteLuaStringKey(KeyName, FullStream, FunctionHelp.Name.find(":") != npos);
               FullStream << (FunctionHelp.ParameterTable ? '{' : '(');
            } else {
               COL_TRC("Could not find parameter table, defaulting to '('.");
               TRNwriteLuaStringKey(KeyName, FullStream, m_IndexChar == ':');
               FullStream << '(';
            };
            lua_pop(L,1);
         } else {
            COL_TRC("Not a function -- just output keyname -- i.e net.ftp");
            TRNwriteLuaStringKey(KeyName, FullStream, m_IndexChar == ':');
         }
         break;
      }
      case LUA_TBOOLEAN:
         TRNprintLuaBooleanKey(L, m_KeyIndex, FullStream);
         break;
      default:
         COLPRECONDITION(false);
   }

   return FullText;
}

COLstring TRNtraverserLua::optionFullText(const COLstring& RootVar, lua_State* L) const{
   //This is a dummy definition of optionFullText because it is used in TRNtraverserNode
   //and must be defined.
   //Will just call the original version of TRNtraverserLua::optionFullText
   //For more info check out ticket/branch #27741
   return optionFullText(RootVar);
}

void TRNtraverserLua::writeTreeValue(COLstring& Sink, COLstring* pFuncName, const REXmatcher* pSearchRegex) const {
   COL_METHOD(TRNtraverserLua::writeTreeValue);
   COL_VAR(pSearchRegex);
   COLostream ResultStream(Sink);
   ResultStream << TRN_BEGIN_TREE_VALUE;
   if (!pSearchRegex || !TRNluaIsSimple(m_Value.m_LuaType)){
      COL_TRC("Writing out: " << m_ValueLongLabel);
      SFIhtmlCrop(m_ValueLongLabel.c_str(), m_ValueLongLabel.size(), TRN_INTELLISENSE_TRUNCATED, TRN_INTELLISENSE_MAX_LENGTH, Sink);
   } else {
      COLstring EscapedAndHighlightedStr = m_Value.m_String;
      pSearchRegex->regexReplace(EscapedAndHighlightedStr, TRN_HIGHLIGHT_SEARCH_MATCH, TRNsearchMatchEscaper);
      if (m_Value.m_LuaType == LUA_TSTRING) ResultStream << '\"';
      SFIhtmlCrop(EscapedAndHighlightedStr.c_str(), EscapedAndHighlightedStr.size(), TRN_INTELLISENSE_TRUNCATED, TRN_INTELLISENSE_MAX_LENGTH, Sink);
      if (m_Value.m_LuaType == LUA_TSTRING) ResultStream << '\"';
   }
   ResultStream << TRN_END_TREE_VALUE;

   if (m_Value.m_LuaType == LUA_TFUNCTION){
      lua_pushvalue(L,m_ValueIndex);
      HELPobject FunctionHelp;
      COLstring DummyFile;
      bool HaveHelp = TRNfetchHelpJsonFromFile(L, &FunctionHelp, &DummyFile);
      lua_pop(L,1);
      // const COLstring* pShortDesc = (FunctionHelp.SummaryLine.size() > 0 ? &FunctionHelp.SummaryLine : &FunctionHelp.Description);
      const COLstring* pShortDesc = &FunctionHelp.SummaryLine;
      COL_VAR2(*pShortDesc, pShortDesc->size());
      if (pFuncName) { *pFuncName = FunctionHelp.Name; } // Only set it if it was a valid pointer from the caller
      if (HaveHelp && pShortDesc->size() ){
         //note we assume that this can contain html chars
         COLstring ShortDesc = LHLPhelpTruncateText(*pShortDesc,120);
         ResultStream << ' ';
         LHLPhelpAddStyledValue(ResultStream,ShortDesc);
      }
   }
}

void TRNtraverserLua::getTreeviewText(COLstring& ShortLabel, COLstring& LongLabel) const {
   COL_METHOD(TRNtraverserLua::getTreeviewText);
   ShortLabel = m_ValueShortLabel;
   LongLabel  = m_ValueLongLabel;
}

void TRNtraverserLua::getTreeviewKey(COLstring& Key) const{
   COL_METHOD(TRNtraverserLua::getTreeviewKey);
   if (m_IterationState == NOT_STARTED){
      Key = m_ValueShortLabel.is_null() ? m_ValueLongLabel : m_ValueShortLabel;
   } else {
      int ProtocolType, NodeType; // not used
      LUALdisplayComplexItem(L, m_KeyIndex, &Key, NULL, ProtocolType, NodeType, m_pTableStack.get(), true);
   }
}

void TRNtraverserLua::getTreeviewValue(COLstring& Value) const{
   COL_METHOD(TRNtraverserLua::getTreeviewValue);
   Value = m_ValueLongLabel;
}

const NODplace* TRNtraverserLua::nodPlace() const{
   return m_Value.m_pNode;
}

bool TRNtraverserLua::initKeyAndValue(){
   COL_METHOD(TRNtraverserLua::initKeyAndValue);
   TRNluaItem& Key = m_Keys.push_back();
   TRNgetLuaItem(L, m_KeyIndex, Key);
   bool SkipItem = (!m_ForDisplay && !TRNluaIsSimple(Key.m_LuaType)) ||
                   (m_IndexChar == ':' && !lua_isfunction(L, m_ValueIndex)) ||
                   (m_Keys.indexOf(Key) < m_Keys.size()-1) ||
                   (LUALhtmlFilterTableItem(L, m_TableIndex, m_KeyIndex, m_ValueIndex));
   if (SkipItem){
      // We're skipping the key due to m_ForDisplay, m_IndexChar, key was found before, or it's been filtered out by HtmlPrinter.
      m_Keys.pop_back();
      return false;
   } else {
      m_ValueHasMetatable = false;
      m_ValueShortLabel.clear();
      m_ValueLongLabel.clear();
      TRNgetLuaItem(L, m_ValueIndex, m_Value);
      m_ValueHasMetatable = LUALdisplayComplexItem(L, m_ValueIndex,
         &m_ValueShortLabel, &m_ValueLongLabel, m_ValueDisplayProtocolType, m_ValueDisplayNodeType,
         m_pTableStack.get(), true);
      return true;
   }
}
