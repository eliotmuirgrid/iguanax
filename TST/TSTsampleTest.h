
class TSTsampleTest
{
public:
   TSTsampleTest();


   void testFunction()
   {

   }

   static TSTtestable* constructTest()
   {
      TST_START_COLLECTION(TSTsampleTest)
         TST_ADD_TESTFUNCTION(TSTsampleTest,testFunction)
      TST_END_COLLECTION
   }
}

#define TST_START_COLLECTION(_NAME) \
   TSTtestCollection* pCollection = new TSTtestCollection(_NAME)

#define TST_ADD_TEST_FUNCTION(_CLASS,_FUNCTION) \
   pCollection->addTest(new TSTtestFunction<_CLASS>(_CLASS::_FUNCTION));

#define TST_END_COLLECTION() \
   return pCollection;

#define TST_ASSERT(_EXPRESSION) \
   if (!(_EXPRESSION)) \ 
   {
      COLstring ErrorString;
      COLostream Stream(ErrorString);
      ErrorString << "Assertion Failure: _EXPRESSION";
      throw TSTexception(ErrorString.c_str(),__LINE__,__FILE__,TSTexception::eAssertionFailure);
   }

