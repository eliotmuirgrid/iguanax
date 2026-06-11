UNITaddTest("COLglobMatch_0", () => {
   UNITassertEquals(true, COLglobMatch("fred barney", "fred"));
});

UNITaddTest("COLglobMatch_1", () => {
   UNITassertEquals(true, COLglobMatch("fred", "fred"));
});

UNITaddTest("COLglobMatch_2", () => {
   UNITassertEquals(true, COLglobMatch("fred barney", "fred"));
});

UNITaddTest("COLglobMatch_3", () => {
   UNITassertEquals(true, COLglobMatch("fred barney", "barney"));
});

UNITaddTest("COLglobMatch_4", () => {
   UNITassertEquals(true, COLglobMatch("fred* barney*", "barney"));
});

UNITaddTest("COLglobMatch_5", () => {
   UNITassertEquals(true, COLglobMatch("fred* barney*", "fred"));
});

UNITaddTest("COLglobMatch_6", () => {
   UNITassertEquals(true, COLglobMatch("*", "fred"));
});

UNITaddTest("COLglobMatch_7", () => {
   UNITassertEquals(true, COLglobMatch("**", "fred"));
});

UNITaddTest("COLglobMatch_8", () => {
   UNITassertEquals(true, COLglobMatch("abc *** def", "fred"));
});

UNITaddTest("COLglobMatch_9", () => {
   UNITassertEquals(true, COLglobMatch("abc ***d def", "fred"));
});

UNITaddTest("COLglobMatch_10", () => {
   UNITassertEquals(false, COLglobMatch("abc ***d def -fred", "fred"));
});

UNITaddTest("COLglobMatch_11", () => {
   UNITassertEquals(false, COLglobMatch("abc ***d def -f*d xyz", "fred"));
});

UNITaddTest("COLglobMatch_12", () => {
   UNITassertEquals(false, COLglobMatch("abc **d def -f*d xyz", "fred"));
});

UNITaddTest("COLglobMatch_13", () => {
   UNITassertEquals(true, COLglobMatch("abc ***d def -f*d xyz *d", "fred"));
});

UNITaddTest("COLglobMatch_14", () => {
   UNITassertEquals(true, COLglobMatch("f* b*", "barney"));
});

UNITaddTest("COLglobMatch_15", () => {
   UNITassertEquals(true, COLglobMatch("fred*", "fred"));
});

UNITaddTest("COLglobMatch_16", () => {
   UNITassertEquals(true, COLglobMatch("f*", "fred"));
});

UNITaddTest("COLglobMatch_17", () => {
   UNITassertEquals(false, COLglobMatch("* -fred", "fred"));
});

UNITaddTest("COLglobMatch_18", () => {
   UNITassertEquals(true, COLglobMatch("* -barney", "fred"));
});

UNITaddTest("COLglobMatch_19", () => {
   UNITassertEquals(true, COLglobMatch("abc *dy -def xyz -fredd", "freddy"));
});

UNITaddTest("COLglobMatch_20", () => {
   UNITassertEquals(true, COLglobMatch("abc **dy -def xyz -fredd", "freddy"));
});

UNITaddTest("COLglobMatch_21", () => {
   UNITassertEquals(true, COLglobMatch("abc ***dy -def xyz -fredd", "freddy"));
});

UNITaddTest("COLglobMatch_22", () => {
   UNITassertEquals(false, COLglobMatch("abc ***dy -def xyz -freddy", "freddy"));
});

UNITaddTest("COLglobMatch_23", () => {
   UNITassertEquals(false, COLglobMatch("abc ***dy -def xyz -freddy fredd", "freddy"));
});

UNITaddTest("COLglobMatch_24", () => {
   UNITassertEquals(true, COLglobMatch("abc **dy -def xyz -fredd", "freddy"));
});

UNITaddTest("COLglobMatch_25", () => {
   UNITassertEquals(true, COLglobMatch("abc ***dy -def xyz -fredd", "freddy"));
});

UNITaddTest("COLglobMatch_26", () => {
   UNITassertEquals(true, COLglobMatch("abc *ddy -def xyz -fredd", "freddy"));
});

UNITaddTest("COLglobMatch_27", () => {
   UNITassertEquals(true, COLglobMatch("ab*abc*def -def xyz", "abcabcdef"));
});

UNITaddTest("COLglobMatch_28", () => {
   UNITassertEquals(true, COLglobMatch("ab*abc*def -def xyz", "abcabc2def"));
});

UNITaddTest("COLglobMatch_29", () => {
   UNITassertEquals(true, COLglobMatch("ab*abc*def abc def", "abdefabcdedededef"));
});

UNITaddTest("COLglobMatch_30", () => {
   UNITassertEquals(true, COLglobMatch("ab*abc*def* abc def", "abdefabcdedededefk"));
});

UNITaddTest("COLglobMatch_31", () => {
   UNITassertEquals(true, COLglobMatch("*Abc", "AbxAbc"));
});

UNITaddTest("COLglobMatch_32", () => {
   UNITassertEquals(true, COLglobMatch("*Abc*b*", "AbcAbc"));
});

UNITaddTest("COLglobMatch_33", () => {
   UNITassertEquals(true, COLglobMatch("*Abc", "AbcAbc"));
});

UNITaddTest("COLglobMatch_34", () => {
   UNITassertEquals(true, COLglobMatch(" *Abc ", "AbcAbc"));
});

UNITaddTest("COLglobMatch_35", () => {
   UNITassertEquals(true, COLglobMatch("abc **d def", "fred"));
});

UNITaddTest("COLglobMatch_36", () => {
   UNITassertEquals(true, COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "ABc"));
});

UNITaddTest("COLglobMatch_37", () => {
   UNITassertEquals(true, COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "CAB"));
});

UNITaddTest("COLglobMatch_38", () => {
   UNITassertEquals(false, COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "ABCdef"));
});

UNITaddTest("COLglobMatch_39", () => {
   UNITassertEquals(true, COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "ABCde"));
});

UNITaddTest("COLglobMatch_40", () => {
   UNITassertEquals(false, COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "CARP"));
});

UNITaddTest("COLglobMatch_41", () => {
   UNITassertEquals(false, COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "CAT"));
});

UNITaddTest("COLglobMatch_42", () => {
   UNITassertEquals(true, COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "CATS"));
});

UNITaddTest("COLglobMatch_43", () => {
   UNITassertEquals(true, COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "BIRD"));
});

UNITaddTest("COLglobMatch_44", () => {
   UNITassertEquals(false, COLglobMatch("AB* CA* -ABCdef -CAR* -CAT* BIRD CATS", "CART"));
});

UNITaddTest("COLglobMatch_45", () => {
   UNITassertEquals(true, COLglobMatch(" * -* *", "abc"));
});

UNITaddTest("COLglobMatch_46", () => {
   UNITassertEquals(true, COLglobMatch("*", "abc"));
});

UNITaddTest("COLglobMatch_47", () => {
   UNITassertEquals(true, COLglobMatch("*", ""));
});

UNITaddTest("COLglobMatch_48", () => {
   UNITassertEquals(true, COLglobMatch("* ", ""));
});

UNITaddTest("COLglobMatch_49", () => {
   UNITassertEquals(true, COLglobMatch("*c", "c"));
});

UNITaddTest("COLglobMatch_50", () => {
   UNITassertEquals(true, COLglobMatch("* ", "abc"));
});

UNITaddTest("COLglobMatch_51", () => {
   UNITassertEquals(true, COLglobMatch("*c ", "c"));
});

UNITaddTest("COLglobMatch_52", () => {
   UNITassertEquals(true, COLglobMatch("test/pattern?with?spaces", "test/pattern with spaces"));
});

UNITaddTest("COLglobMatch_53", () => {
   UNITassertEquals(true, COLglobMatch("?* ", "abc"));
});

UNITaddTest("COLglobMatch_54", () => {
   UNITassertEquals(true, COLglobMatch("abc *r*??* -def xyz -fredd", "freddy"));
});

UNITaddTest("COLglobMatch_55", () => {
   UNITassertEquals(true, COLglobMatch("abc *r*?? -def xyz -fredd", "freddy"));
});

UNITaddTest("COLglobMatch_56", () => {
   UNITassertEquals(true, COLglobMatch("?*Abc", "AbcAbc"));
});

UNITaddTest("COLglobMatch_57", () => {
   UNITassertEquals(true, COLglobMatch("abc f*r*?? -def xyz -fredd", "freddy"));
});

UNITaddTest("COLglobMatch_58", () => {
   UNITassertEquals(true, COLglobMatch("abc ?r*d?? -def xyz -fredd", "freddy"));
});

UNITaddTest("COLglobMatch_59", () => {
   UNITassertEquals(true, COLglobMatch("abc ?r*d? -def xyz -fredd", "freddy"));
});

UNITaddTest("COLglobMatch_60", () => {
   UNITassertEquals(true, COLglobMatch("*???", "123"));
});

UNITaddTest("COLglobMatch_61", () => {
   UNITassertEquals(true, COLglobMatch(" *??? ", "123"));
});

UNITaddTest("COLglobMatch_62", () => {
   UNITassertEquals(true, COLglobMatch("*???", "1234"));
});

UNITaddTest("COLglobMatch_63", () => {
   UNITassertEquals(true, COLglobMatch(" *??? ", "1234"));
});

UNITaddTest("COLglobMatch_64", () => {
   UNITassertEquals(true, COLglobMatch("?*3??", "1333"));
});

UNITaddTest("COLglobMatch_65", () => {
   UNITassertEquals(true, COLglobMatch("?*?3?", "1333"));
});

UNITaddTest("COLglobMatch_66", () => {
   UNITassertEquals(true, COLglobMatch("?*??3", "1333"));
});

UNITaddTest("COLglobMatch_67", () => {
   UNITassertEquals(false, COLglobMatch("?*??3", "333"));
});

UNITaddTest("COLglobMatch_68", () => {
   UNITassertEquals(true, COLglobMatch("?*", "abc"));
});

UNITaddTest("COLglobMatch_69", () => {
   UNITassertEquals(true, COLglobMatch("??*", "abc"));
});

UNITaddTest("COLglobMatch_70", () => {
   UNITassertEquals(true, COLglobMatch("???*", "abc"));
});

UNITaddTest("COLglobMatch_71", () => {
   UNITassertEquals(true, COLglobMatch("*???*", "abc"));
});

UNITaddTest("COLglobMatch_72", () => {
   UNITassertEquals(true, COLglobMatch("???", "abc"));
});

UNITaddTest("COLglobMatch_73", () => {
   UNITassertEquals(true, COLglobMatch("??? ", "abc"));
});

UNITaddTest("COLglobMatch_74", () => {
   UNITassertEquals(true, COLglobMatch("??* ", "abc"));
});

UNITaddTest("COLglobMatch_75", () => {
   UNITassertEquals(true, COLglobMatch("???* ", "abc"));
});

UNITaddTest("COLglobMatch_76", () => {
   UNITassertEquals(true, COLglobMatch("*???* ", "abc"));
});

UNITaddTest("COLglobMatch_77", () => {
   UNITassertEquals(true, COLglobMatch("??? ", "abc"));
});

UNITaddTest("COLglobMatch_78", () => {
   UNITassertEquals(true, COLglobMatch("???", "abc"));
});

UNITaddTest("COLglobMatch_79", () => {
   UNITassertEquals(true, COLglobMatch("*Test?with?space* *regular*", "test/[Test with spaces]"));
});

UNITaddTest("COLglobMatch_80", () => {
   UNITassertEquals(true, COLglobMatch("*Test?with?space* *regular*", "test/[some Test with spaces 1]"));
});

UNITaddTest("COLglobMatch_81", () => {
   UNITassertEquals(true, COLglobMatch("*Test?with?space* *regular*", "test/[another Test with spaces 2]"));
});

UNITaddTest("COLglobMatch_82", () => {
   UNITassertEquals(false, COLglobMatch("*Test?with?space* *regular*", "test/[No match with spaces 2]"));
});

UNITaddTest("COLglobMatch_83", () => {
   UNITassertEquals(true, COLglobMatch("*Test?with?space* *regular*", "regular/test"));
});