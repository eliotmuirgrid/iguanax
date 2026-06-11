#ifndef __COL_CLOSURE_H__
#define __COL_CLOSURE_H__
//-----------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  COLclosure
//
// Author:  Eliot Muir
//
// Date:    Fri  9 Jan 2015 08:52:35 EST
//
// Description:
//
// A closure takes arguments and it has the ability to capture additional
// variables.  This is an example of the intended usage:
//
// void ServeWebRequest(COLclosure2<int, int>* pClosure){
//    pClosure->run(2,3);
// }
//
// void MakeWebRequest(int ConnectionId, int SequenceId, int* pCount); 
//
// ServeWebRequest(COLnewClosure2<int, int>(&MakeWebRequest, &Count));
// See http://fogbugz.ifware.dynip.com/default.asp?W5842
// WARNING - this code is generated from a Lua script.  Scroll to the end.
//-----------------------------------------------------------------------------

class COLclosure0{
public:
	COLclosure0(){}
	virtual ~COLclosure0(){}
	virtual void run() const=0;
	void runAndDelete(){run();delete this;}
private:
	COLclosure0(const COLclosure0& Orig);
	COLclosure0& operator=(const COLclosure0& Orig);
};

template <typename A1>
class COLclosure1{
public:
	COLclosure1(){}
	virtual ~COLclosure1(){}
	virtual void run(A1 a1) const=0;
	void runAndDelete(A1 a1){run(a1);delete this;}
private:
	COLclosure1(const COLclosure1& Orig);
	COLclosure1& operator=(const COLclosure1& Orig);
};

template <typename A1,typename A2>
class COLclosure2{
public:
	COLclosure2(){}
	virtual ~COLclosure2(){}
	virtual void run(A1 a1,A2 a2) const=0;
	void runAndDelete(A1 a1,A2 a2){run(a1,a2);delete this;}
private:
	COLclosure2(const COLclosure2& Orig);
	COLclosure2& operator=(const COLclosure2& Orig);
};

template <typename A1,typename A2,typename A3>
class COLclosure3{
public:
	COLclosure3(){}
	virtual ~COLclosure3(){}
	virtual void run(A1 a1,A2 a2,A3 a3) const=0;
	void runAndDelete(A1 a1,A2 a2,A3 a3){run(a1,a2,a3);delete this;}
private:
	COLclosure3(const COLclosure3& Orig);
	COLclosure3& operator=(const COLclosure3& Orig);
};

template <typename A1,typename A2,typename A3,typename A4>
class COLclosure4{
public:
	COLclosure4(){}
	virtual ~COLclosure4(){}
	virtual void run(A1 a1,A2 a2,A3 a3,A4 a4) const=0;
	void runAndDelete(A1 a1,A2 a2,A3 a3,A4 a4){run(a1,a2,a3,a4);delete this;}
private:
	COLclosure4(const COLclosure4& Orig);
	COLclosure4& operator=(const COLclosure4& Orig);
};

template <typename A1,typename A2,typename A3,typename A4,typename A5>
class COLclosure5{
public:
	COLclosure5(){}
	virtual ~COLclosure5(){}
	virtual void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5) const=0;
	void runAndDelete(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5){run(a1,a2,a3,a4,a5);delete this;}
private:
	COLclosure5(const COLclosure5& Orig);
	COLclosure5& operator=(const COLclosure5& Orig);
};

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename A6>
class COLclosure6{
public:
	COLclosure6(){}
	virtual ~COLclosure6(){}
	virtual void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6) const=0;
	void runAndDelete(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6){run(a1,a2,a3,a4,a5,a6);delete this;}
private:
	COLclosure6(const COLclosure6& Orig);
	COLclosure6& operator=(const COLclosure6& Orig);
};

 class COLc0f0:public COLclosure0{
public:
	typedef void (*F)();
	COLc0f0():_pF(0){}
	COLc0f0(F pF):_pF(pF){}
	~COLc0f0(){}
	void run() const {(*_pF)();}
private:
	F _pF;
};


inline COLc0f0* COLnewClosure0(void (*fn)()){return new COLc0f0(fn);}

template <typename C1> class COLc0f1:public COLclosure0{
public:
	typedef void (*F)(C1 c1);
	COLc0f1():_pF(0){}
	COLc0f1(F pF,C1 c1):_pF(pF),_c1(c1){}
	~COLc0f1(){}
	void run() const {(*_pF)(_c1);}
private:
	F _pF;
	C1 _c1;
};

template <typename C1>
inline COLc0f1<C1>* COLnewClosure0(void (*fn)(C1),C1 c1){return new COLc0f1<C1>(fn,c1);}

template <typename C1,typename C2> class COLc0f2:public COLclosure0{
public:
	typedef void (*F)(C1 c1,C2 c2);
	COLc0f2():_pF(0){}
	COLc0f2(F pF,C1 c1,C2 c2):_pF(pF),_c1(c1),_c2(c2){}
	~COLc0f2(){}
	void run() const {(*_pF)(_c1,_c2);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
};

template <typename C1,typename C2>
inline COLc0f2<C1,C2>* COLnewClosure0(void (*fn)(C1,C2),C1 c1,C2 c2){return new COLc0f2<C1,C2>(fn,c1,c2);}

template <typename C1,typename C2,typename C3> class COLc0f3:public COLclosure0{
public:
	typedef void (*F)(C1 c1,C2 c2,C3 c3);
	COLc0f3():_pF(0){}
	COLc0f3(F pF,C1 c1,C2 c2,C3 c3):_pF(pF),_c1(c1),_c2(c2),_c3(c3){}
	~COLc0f3(){}
	void run() const {(*_pF)(_c1,_c2,_c3);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
};

template <typename C1,typename C2,typename C3>
inline COLc0f3<C1,C2,C3>* COLnewClosure0(void (*fn)(C1,C2,C3),C1 c1,C2 c2,C3 c3){return new COLc0f3<C1,C2,C3>(fn,c1,c2,c3);}

template <typename C1,typename C2,typename C3,typename C4> class COLc0f4:public COLclosure0{
public:
	typedef void (*F)(C1 c1,C2 c2,C3 c3,C4 c4);
	COLc0f4():_pF(0){}
	COLc0f4(F pF,C1 c1,C2 c2,C3 c3,C4 c4):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4){}
	~COLc0f4(){}
	void run() const {(*_pF)(_c1,_c2,_c3,_c4);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
};

template <typename C1,typename C2,typename C3,typename C4>
inline COLc0f4<C1,C2,C3,C4>* COLnewClosure0(void (*fn)(C1,C2,C3,C4),C1 c1,C2 c2,C3 c3,C4 c4){return new COLc0f4<C1,C2,C3,C4>(fn,c1,c2,c3,c4);}

template <typename C1,typename C2,typename C3,typename C4,typename C5> class COLc0f5:public COLclosure0{
public:
	typedef void (*F)(C1 c1,C2 c2,C3 c3,C4 c4,C5 c5);
	COLc0f5():_pF(0){}
	COLc0f5(F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5){}
	~COLc0f5(){}
	void run() const {(*_pF)(_c1,_c2,_c3,_c4,_c5);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
};

template <typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc0f5<C1,C2,C3,C4,C5>* COLnewClosure0(void (*fn)(C1,C2,C3,C4,C5),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc0f5<C1,C2,C3,C4,C5>(fn,c1,c2,c3,c4,c5);}

template <typename C1,typename C2,typename C3,typename C4,typename C5,typename C6> class COLc0f6:public COLclosure0{
public:
	typedef void (*F)(C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6);
	COLc0f6():_pF(0){}
	COLc0f6(F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5),_c6(c6){}
	~COLc0f6(){}
	void run() const {(*_pF)(_c1,_c2,_c3,_c4,_c5,_c6);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
	C6 _c6;
};

template <typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc0f6<C1,C2,C3,C4,C5,C6>* COLnewClosure0(void (*fn)(C1,C2,C3,C4,C5,C6),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc0f6<C1,C2,C3,C4,C5,C6>(fn,c1,c2,c3,c4,c5,c6);}

template <typename A1> class COLc1f0:public COLclosure1<A1>{
public:
	typedef void (*F)(A1 a1);
	COLc1f0():_pF(0){}
	COLc1f0(F pF):_pF(pF){}
	~COLc1f0(){}
	void run(A1 a1) const {(*_pF)(a1);}
private:
	F _pF;
};

template <typename A1>
inline COLc1f0<A1>* COLnewClosure1(void (*fn)(A1)){return new COLc1f0<A1>(fn);}

template <typename A1,typename C1> class COLc1f1:public COLclosure1<A1>{
public:
	typedef void (*F)(A1 a1,C1 c1);
	COLc1f1():_pF(0){}
	COLc1f1(F pF,C1 c1):_pF(pF),_c1(c1){}
	~COLc1f1(){}
	void run(A1 a1) const {(*_pF)(a1,_c1);}
private:
	F _pF;
	C1 _c1;
};

template <typename A1,typename C1>
inline COLc1f1<A1,C1>* COLnewClosure1(void (*fn)(A1,C1),C1 c1){return new COLc1f1<A1,C1>(fn,c1);}

template <typename A1,typename C1,typename C2> class COLc1f2:public COLclosure1<A1>{
public:
	typedef void (*F)(A1 a1,C1 c1,C2 c2);
	COLc1f2():_pF(0){}
	COLc1f2(F pF,C1 c1,C2 c2):_pF(pF),_c1(c1),_c2(c2){}
	~COLc1f2(){}
	void run(A1 a1) const {(*_pF)(a1,_c1,_c2);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
};

template <typename A1,typename C1,typename C2>
inline COLc1f2<A1,C1,C2>* COLnewClosure1(void (*fn)(A1,C1,C2),C1 c1,C2 c2){return new COLc1f2<A1,C1,C2>(fn,c1,c2);}

template <typename A1,typename C1,typename C2,typename C3> class COLc1f3:public COLclosure1<A1>{
public:
	typedef void (*F)(A1 a1,C1 c1,C2 c2,C3 c3);
	COLc1f3():_pF(0){}
	COLc1f3(F pF,C1 c1,C2 c2,C3 c3):_pF(pF),_c1(c1),_c2(c2),_c3(c3){}
	~COLc1f3(){}
	void run(A1 a1) const {(*_pF)(a1,_c1,_c2,_c3);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
};

template <typename A1,typename C1,typename C2,typename C3>
inline COLc1f3<A1,C1,C2,C3>* COLnewClosure1(void (*fn)(A1,C1,C2,C3),C1 c1,C2 c2,C3 c3){return new COLc1f3<A1,C1,C2,C3>(fn,c1,c2,c3);}

template <typename A1,typename C1,typename C2,typename C3,typename C4> class COLc1f4:public COLclosure1<A1>{
public:
	typedef void (*F)(A1 a1,C1 c1,C2 c2,C3 c3,C4 c4);
	COLc1f4():_pF(0){}
	COLc1f4(F pF,C1 c1,C2 c2,C3 c3,C4 c4):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4){}
	~COLc1f4(){}
	void run(A1 a1) const {(*_pF)(a1,_c1,_c2,_c3,_c4);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
};

template <typename A1,typename C1,typename C2,typename C3,typename C4>
inline COLc1f4<A1,C1,C2,C3,C4>* COLnewClosure1(void (*fn)(A1,C1,C2,C3,C4),C1 c1,C2 c2,C3 c3,C4 c4){return new COLc1f4<A1,C1,C2,C3,C4>(fn,c1,c2,c3,c4);}

template <typename A1,typename C1,typename C2,typename C3,typename C4,typename C5> class COLc1f5:public COLclosure1<A1>{
public:
	typedef void (*F)(A1 a1,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5);
	COLc1f5():_pF(0){}
	COLc1f5(F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5){}
	~COLc1f5(){}
	void run(A1 a1) const {(*_pF)(a1,_c1,_c2,_c3,_c4,_c5);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
};

template <typename A1,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc1f5<A1,C1,C2,C3,C4,C5>* COLnewClosure1(void (*fn)(A1,C1,C2,C3,C4,C5),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc1f5<A1,C1,C2,C3,C4,C5>(fn,c1,c2,c3,c4,c5);}

template <typename A1,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6> class COLc1f6:public COLclosure1<A1>{
public:
	typedef void (*F)(A1 a1,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6);
	COLc1f6():_pF(0){}
	COLc1f6(F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5),_c6(c6){}
	~COLc1f6(){}
	void run(A1 a1) const {(*_pF)(a1,_c1,_c2,_c3,_c4,_c5,_c6);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
	C6 _c6;
};

template <typename A1,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc1f6<A1,C1,C2,C3,C4,C5,C6>* COLnewClosure1(void (*fn)(A1,C1,C2,C3,C4,C5,C6),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc1f6<A1,C1,C2,C3,C4,C5,C6>(fn,c1,c2,c3,c4,c5,c6);}

template <typename A1,typename A2> class COLc2f0:public COLclosure2<A1,A2>{
public:
	typedef void (*F)(A1 a1,A2 a2);
	COLc2f0():_pF(0){}
	COLc2f0(F pF):_pF(pF){}
	~COLc2f0(){}
	void run(A1 a1,A2 a2) const {(*_pF)(a1,a2);}
private:
	F _pF;
};

template <typename A1,typename A2>
inline COLc2f0<A1,A2>* COLnewClosure2(void (*fn)(A1,A2)){return new COLc2f0<A1,A2>(fn);}

template <typename A1,typename A2,typename C1> class COLc2f1:public COLclosure2<A1,A2>{
public:
	typedef void (*F)(A1 a1,A2 a2,C1 c1);
	COLc2f1():_pF(0){}
	COLc2f1(F pF,C1 c1):_pF(pF),_c1(c1){}
	~COLc2f1(){}
	void run(A1 a1,A2 a2) const {(*_pF)(a1,a2,_c1);}
private:
	F _pF;
	C1 _c1;
};

template <typename A1,typename A2,typename C1>
inline COLc2f1<A1,A2,C1>* COLnewClosure2(void (*fn)(A1,A2,C1),C1 c1){return new COLc2f1<A1,A2,C1>(fn,c1);}

template <typename A1,typename A2,typename C1,typename C2> class COLc2f2:public COLclosure2<A1,A2>{
public:
	typedef void (*F)(A1 a1,A2 a2,C1 c1,C2 c2);
	COLc2f2():_pF(0){}
	COLc2f2(F pF,C1 c1,C2 c2):_pF(pF),_c1(c1),_c2(c2){}
	~COLc2f2(){}
	void run(A1 a1,A2 a2) const {(*_pF)(a1,a2,_c1,_c2);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
};

template <typename A1,typename A2,typename C1,typename C2>
inline COLc2f2<A1,A2,C1,C2>* COLnewClosure2(void (*fn)(A1,A2,C1,C2),C1 c1,C2 c2){return new COLc2f2<A1,A2,C1,C2>(fn,c1,c2);}

template <typename A1,typename A2,typename C1,typename C2,typename C3> class COLc2f3:public COLclosure2<A1,A2>{
public:
	typedef void (*F)(A1 a1,A2 a2,C1 c1,C2 c2,C3 c3);
	COLc2f3():_pF(0){}
	COLc2f3(F pF,C1 c1,C2 c2,C3 c3):_pF(pF),_c1(c1),_c2(c2),_c3(c3){}
	~COLc2f3(){}
	void run(A1 a1,A2 a2) const {(*_pF)(a1,a2,_c1,_c2,_c3);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
};

template <typename A1,typename A2,typename C1,typename C2,typename C3>
inline COLc2f3<A1,A2,C1,C2,C3>* COLnewClosure2(void (*fn)(A1,A2,C1,C2,C3),C1 c1,C2 c2,C3 c3){return new COLc2f3<A1,A2,C1,C2,C3>(fn,c1,c2,c3);}

template <typename A1,typename A2,typename C1,typename C2,typename C3,typename C4> class COLc2f4:public COLclosure2<A1,A2>{
public:
	typedef void (*F)(A1 a1,A2 a2,C1 c1,C2 c2,C3 c3,C4 c4);
	COLc2f4():_pF(0){}
	COLc2f4(F pF,C1 c1,C2 c2,C3 c3,C4 c4):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4){}
	~COLc2f4(){}
	void run(A1 a1,A2 a2) const {(*_pF)(a1,a2,_c1,_c2,_c3,_c4);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
};

template <typename A1,typename A2,typename C1,typename C2,typename C3,typename C4>
inline COLc2f4<A1,A2,C1,C2,C3,C4>* COLnewClosure2(void (*fn)(A1,A2,C1,C2,C3,C4),C1 c1,C2 c2,C3 c3,C4 c4){return new COLc2f4<A1,A2,C1,C2,C3,C4>(fn,c1,c2,c3,c4);}

template <typename A1,typename A2,typename C1,typename C2,typename C3,typename C4,typename C5> class COLc2f5:public COLclosure2<A1,A2>{
public:
	typedef void (*F)(A1 a1,A2 a2,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5);
	COLc2f5():_pF(0){}
	COLc2f5(F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5){}
	~COLc2f5(){}
	void run(A1 a1,A2 a2) const {(*_pF)(a1,a2,_c1,_c2,_c3,_c4,_c5);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
};

template <typename A1,typename A2,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc2f5<A1,A2,C1,C2,C3,C4,C5>* COLnewClosure2(void (*fn)(A1,A2,C1,C2,C3,C4,C5),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc2f5<A1,A2,C1,C2,C3,C4,C5>(fn,c1,c2,c3,c4,c5);}

template <typename A1,typename A2,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6> class COLc2f6:public COLclosure2<A1,A2>{
public:
	typedef void (*F)(A1 a1,A2 a2,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6);
	COLc2f6():_pF(0){}
	COLc2f6(F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5),_c6(c6){}
	~COLc2f6(){}
	void run(A1 a1,A2 a2) const {(*_pF)(a1,a2,_c1,_c2,_c3,_c4,_c5,_c6);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
	C6 _c6;
};

template <typename A1,typename A2,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc2f6<A1,A2,C1,C2,C3,C4,C5,C6>* COLnewClosure2(void (*fn)(A1,A2,C1,C2,C3,C4,C5,C6),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc2f6<A1,A2,C1,C2,C3,C4,C5,C6>(fn,c1,c2,c3,c4,c5,c6);}

template <typename A1,typename A2,typename A3> class COLc3f0:public COLclosure3<A1,A2,A3>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3);
	COLc3f0():_pF(0){}
	COLc3f0(F pF):_pF(pF){}
	~COLc3f0(){}
	void run(A1 a1,A2 a2,A3 a3) const {(*_pF)(a1,a2,a3);}
private:
	F _pF;
};

template <typename A1,typename A2,typename A3>
inline COLc3f0<A1,A2,A3>* COLnewClosure3(void (*fn)(A1,A2,A3)){return new COLc3f0<A1,A2,A3>(fn);}

template <typename A1,typename A2,typename A3,typename C1> class COLc3f1:public COLclosure3<A1,A2,A3>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,C1 c1);
	COLc3f1():_pF(0){}
	COLc3f1(F pF,C1 c1):_pF(pF),_c1(c1){}
	~COLc3f1(){}
	void run(A1 a1,A2 a2,A3 a3) const {(*_pF)(a1,a2,a3,_c1);}
private:
	F _pF;
	C1 _c1;
};

template <typename A1,typename A2,typename A3,typename C1>
inline COLc3f1<A1,A2,A3,C1>* COLnewClosure3(void (*fn)(A1,A2,A3,C1),C1 c1){return new COLc3f1<A1,A2,A3,C1>(fn,c1);}

template <typename A1,typename A2,typename A3,typename C1,typename C2> class COLc3f2:public COLclosure3<A1,A2,A3>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,C1 c1,C2 c2);
	COLc3f2():_pF(0){}
	COLc3f2(F pF,C1 c1,C2 c2):_pF(pF),_c1(c1),_c2(c2){}
	~COLc3f2(){}
	void run(A1 a1,A2 a2,A3 a3) const {(*_pF)(a1,a2,a3,_c1,_c2);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
};

template <typename A1,typename A2,typename A3,typename C1,typename C2>
inline COLc3f2<A1,A2,A3,C1,C2>* COLnewClosure3(void (*fn)(A1,A2,A3,C1,C2),C1 c1,C2 c2){return new COLc3f2<A1,A2,A3,C1,C2>(fn,c1,c2);}

template <typename A1,typename A2,typename A3,typename C1,typename C2,typename C3> class COLc3f3:public COLclosure3<A1,A2,A3>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,C1 c1,C2 c2,C3 c3);
	COLc3f3():_pF(0){}
	COLc3f3(F pF,C1 c1,C2 c2,C3 c3):_pF(pF),_c1(c1),_c2(c2),_c3(c3){}
	~COLc3f3(){}
	void run(A1 a1,A2 a2,A3 a3) const {(*_pF)(a1,a2,a3,_c1,_c2,_c3);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
};

template <typename A1,typename A2,typename A3,typename C1,typename C2,typename C3>
inline COLc3f3<A1,A2,A3,C1,C2,C3>* COLnewClosure3(void (*fn)(A1,A2,A3,C1,C2,C3),C1 c1,C2 c2,C3 c3){return new COLc3f3<A1,A2,A3,C1,C2,C3>(fn,c1,c2,c3);}

template <typename A1,typename A2,typename A3,typename C1,typename C2,typename C3,typename C4> class COLc3f4:public COLclosure3<A1,A2,A3>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,C1 c1,C2 c2,C3 c3,C4 c4);
	COLc3f4():_pF(0){}
	COLc3f4(F pF,C1 c1,C2 c2,C3 c3,C4 c4):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4){}
	~COLc3f4(){}
	void run(A1 a1,A2 a2,A3 a3) const {(*_pF)(a1,a2,a3,_c1,_c2,_c3,_c4);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
};

template <typename A1,typename A2,typename A3,typename C1,typename C2,typename C3,typename C4>
inline COLc3f4<A1,A2,A3,C1,C2,C3,C4>* COLnewClosure3(void (*fn)(A1,A2,A3,C1,C2,C3,C4),C1 c1,C2 c2,C3 c3,C4 c4){return new COLc3f4<A1,A2,A3,C1,C2,C3,C4>(fn,c1,c2,c3,c4);}

template <typename A1,typename A2,typename A3,typename C1,typename C2,typename C3,typename C4,typename C5> class COLc3f5:public COLclosure3<A1,A2,A3>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5);
	COLc3f5():_pF(0){}
	COLc3f5(F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5){}
	~COLc3f5(){}
	void run(A1 a1,A2 a2,A3 a3) const {(*_pF)(a1,a2,a3,_c1,_c2,_c3,_c4,_c5);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
};

template <typename A1,typename A2,typename A3,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc3f5<A1,A2,A3,C1,C2,C3,C4,C5>* COLnewClosure3(void (*fn)(A1,A2,A3,C1,C2,C3,C4,C5),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc3f5<A1,A2,A3,C1,C2,C3,C4,C5>(fn,c1,c2,c3,c4,c5);}

template <typename A1,typename A2,typename A3,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6> class COLc3f6:public COLclosure3<A1,A2,A3>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6);
	COLc3f6():_pF(0){}
	COLc3f6(F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5),_c6(c6){}
	~COLc3f6(){}
	void run(A1 a1,A2 a2,A3 a3) const {(*_pF)(a1,a2,a3,_c1,_c2,_c3,_c4,_c5,_c6);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
	C6 _c6;
};

template <typename A1,typename A2,typename A3,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc3f6<A1,A2,A3,C1,C2,C3,C4,C5,C6>* COLnewClosure3(void (*fn)(A1,A2,A3,C1,C2,C3,C4,C5,C6),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc3f6<A1,A2,A3,C1,C2,C3,C4,C5,C6>(fn,c1,c2,c3,c4,c5,c6);}

template <typename A1,typename A2,typename A3,typename A4> class COLc4f0:public COLclosure4<A1,A2,A3,A4>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4);
	COLc4f0():_pF(0){}
	COLc4f0(F pF):_pF(pF){}
	~COLc4f0(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4) const {(*_pF)(a1,a2,a3,a4);}
private:
	F _pF;
};

template <typename A1,typename A2,typename A3,typename A4>
inline COLc4f0<A1,A2,A3,A4>* COLnewClosure4(void (*fn)(A1,A2,A3,A4)){return new COLc4f0<A1,A2,A3,A4>(fn);}

template <typename A1,typename A2,typename A3,typename A4,typename C1> class COLc4f1:public COLclosure4<A1,A2,A3,A4>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,C1 c1);
	COLc4f1():_pF(0){}
	COLc4f1(F pF,C1 c1):_pF(pF),_c1(c1){}
	~COLc4f1(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4) const {(*_pF)(a1,a2,a3,a4,_c1);}
private:
	F _pF;
	C1 _c1;
};

template <typename A1,typename A2,typename A3,typename A4,typename C1>
inline COLc4f1<A1,A2,A3,A4,C1>* COLnewClosure4(void (*fn)(A1,A2,A3,A4,C1),C1 c1){return new COLc4f1<A1,A2,A3,A4,C1>(fn,c1);}

template <typename A1,typename A2,typename A3,typename A4,typename C1,typename C2> class COLc4f2:public COLclosure4<A1,A2,A3,A4>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,C1 c1,C2 c2);
	COLc4f2():_pF(0){}
	COLc4f2(F pF,C1 c1,C2 c2):_pF(pF),_c1(c1),_c2(c2){}
	~COLc4f2(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4) const {(*_pF)(a1,a2,a3,a4,_c1,_c2);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
};

template <typename A1,typename A2,typename A3,typename A4,typename C1,typename C2>
inline COLc4f2<A1,A2,A3,A4,C1,C2>* COLnewClosure4(void (*fn)(A1,A2,A3,A4,C1,C2),C1 c1,C2 c2){return new COLc4f2<A1,A2,A3,A4,C1,C2>(fn,c1,c2);}

template <typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3> class COLc4f3:public COLclosure4<A1,A2,A3,A4>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,C1 c1,C2 c2,C3 c3);
	COLc4f3():_pF(0){}
	COLc4f3(F pF,C1 c1,C2 c2,C3 c3):_pF(pF),_c1(c1),_c2(c2),_c3(c3){}
	~COLc4f3(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4) const {(*_pF)(a1,a2,a3,a4,_c1,_c2,_c3);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
};

template <typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3>
inline COLc4f3<A1,A2,A3,A4,C1,C2,C3>* COLnewClosure4(void (*fn)(A1,A2,A3,A4,C1,C2,C3),C1 c1,C2 c2,C3 c3){return new COLc4f3<A1,A2,A3,A4,C1,C2,C3>(fn,c1,c2,c3);}

template <typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3,typename C4> class COLc4f4:public COLclosure4<A1,A2,A3,A4>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,C1 c1,C2 c2,C3 c3,C4 c4);
	COLc4f4():_pF(0){}
	COLc4f4(F pF,C1 c1,C2 c2,C3 c3,C4 c4):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4){}
	~COLc4f4(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4) const {(*_pF)(a1,a2,a3,a4,_c1,_c2,_c3,_c4);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
};

template <typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3,typename C4>
inline COLc4f4<A1,A2,A3,A4,C1,C2,C3,C4>* COLnewClosure4(void (*fn)(A1,A2,A3,A4,C1,C2,C3,C4),C1 c1,C2 c2,C3 c3,C4 c4){return new COLc4f4<A1,A2,A3,A4,C1,C2,C3,C4>(fn,c1,c2,c3,c4);}

template <typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3,typename C4,typename C5> class COLc4f5:public COLclosure4<A1,A2,A3,A4>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5);
	COLc4f5():_pF(0){}
	COLc4f5(F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5){}
	~COLc4f5(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4) const {(*_pF)(a1,a2,a3,a4,_c1,_c2,_c3,_c4,_c5);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
};

template <typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc4f5<A1,A2,A3,A4,C1,C2,C3,C4,C5>* COLnewClosure4(void (*fn)(A1,A2,A3,A4,C1,C2,C3,C4,C5),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc4f5<A1,A2,A3,A4,C1,C2,C3,C4,C5>(fn,c1,c2,c3,c4,c5);}

template <typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6> class COLc4f6:public COLclosure4<A1,A2,A3,A4>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6);
	COLc4f6():_pF(0){}
	COLc4f6(F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5),_c6(c6){}
	~COLc4f6(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4) const {(*_pF)(a1,a2,a3,a4,_c1,_c2,_c3,_c4,_c5,_c6);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
	C6 _c6;
};

template <typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc4f6<A1,A2,A3,A4,C1,C2,C3,C4,C5,C6>* COLnewClosure4(void (*fn)(A1,A2,A3,A4,C1,C2,C3,C4,C5,C6),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc4f6<A1,A2,A3,A4,C1,C2,C3,C4,C5,C6>(fn,c1,c2,c3,c4,c5,c6);}

template <typename A1,typename A2,typename A3,typename A4,typename A5> class COLc5f0:public COLclosure5<A1,A2,A3,A4,A5>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5);
	COLc5f0():_pF(0){}
	COLc5f0(F pF):_pF(pF){}
	~COLc5f0(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5) const {(*_pF)(a1,a2,a3,a4,a5);}
private:
	F _pF;
};

template <typename A1,typename A2,typename A3,typename A4,typename A5>
inline COLc5f0<A1,A2,A3,A4,A5>* COLnewClosure5(void (*fn)(A1,A2,A3,A4,A5)){return new COLc5f0<A1,A2,A3,A4,A5>(fn);}

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename C1> class COLc5f1:public COLclosure5<A1,A2,A3,A4,A5>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,C1 c1);
	COLc5f1():_pF(0){}
	COLc5f1(F pF,C1 c1):_pF(pF),_c1(c1){}
	~COLc5f1(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5) const {(*_pF)(a1,a2,a3,a4,a5,_c1);}
private:
	F _pF;
	C1 _c1;
};

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename C1>
inline COLc5f1<A1,A2,A3,A4,A5,C1>* COLnewClosure5(void (*fn)(A1,A2,A3,A4,A5,C1),C1 c1){return new COLc5f1<A1,A2,A3,A4,A5,C1>(fn,c1);}

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2> class COLc5f2:public COLclosure5<A1,A2,A3,A4,A5>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,C1 c1,C2 c2);
	COLc5f2():_pF(0){}
	COLc5f2(F pF,C1 c1,C2 c2):_pF(pF),_c1(c1),_c2(c2){}
	~COLc5f2(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5) const {(*_pF)(a1,a2,a3,a4,a5,_c1,_c2);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
};

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2>
inline COLc5f2<A1,A2,A3,A4,A5,C1,C2>* COLnewClosure5(void (*fn)(A1,A2,A3,A4,A5,C1,C2),C1 c1,C2 c2){return new COLc5f2<A1,A2,A3,A4,A5,C1,C2>(fn,c1,c2);}

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3> class COLc5f3:public COLclosure5<A1,A2,A3,A4,A5>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,C1 c1,C2 c2,C3 c3);
	COLc5f3():_pF(0){}
	COLc5f3(F pF,C1 c1,C2 c2,C3 c3):_pF(pF),_c1(c1),_c2(c2),_c3(c3){}
	~COLc5f3(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5) const {(*_pF)(a1,a2,a3,a4,a5,_c1,_c2,_c3);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
};

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3>
inline COLc5f3<A1,A2,A3,A4,A5,C1,C2,C3>* COLnewClosure5(void (*fn)(A1,A2,A3,A4,A5,C1,C2,C3),C1 c1,C2 c2,C3 c3){return new COLc5f3<A1,A2,A3,A4,A5,C1,C2,C3>(fn,c1,c2,c3);}

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3,typename C4> class COLc5f4:public COLclosure5<A1,A2,A3,A4,A5>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,C1 c1,C2 c2,C3 c3,C4 c4);
	COLc5f4():_pF(0){}
	COLc5f4(F pF,C1 c1,C2 c2,C3 c3,C4 c4):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4){}
	~COLc5f4(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5) const {(*_pF)(a1,a2,a3,a4,a5,_c1,_c2,_c3,_c4);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
};

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3,typename C4>
inline COLc5f4<A1,A2,A3,A4,A5,C1,C2,C3,C4>* COLnewClosure5(void (*fn)(A1,A2,A3,A4,A5,C1,C2,C3,C4),C1 c1,C2 c2,C3 c3,C4 c4){return new COLc5f4<A1,A2,A3,A4,A5,C1,C2,C3,C4>(fn,c1,c2,c3,c4);}

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3,typename C4,typename C5> class COLc5f5:public COLclosure5<A1,A2,A3,A4,A5>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5);
	COLc5f5():_pF(0){}
	COLc5f5(F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5){}
	~COLc5f5(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5) const {(*_pF)(a1,a2,a3,a4,a5,_c1,_c2,_c3,_c4,_c5);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
};

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc5f5<A1,A2,A3,A4,A5,C1,C2,C3,C4,C5>* COLnewClosure5(void (*fn)(A1,A2,A3,A4,A5,C1,C2,C3,C4,C5),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc5f5<A1,A2,A3,A4,A5,C1,C2,C3,C4,C5>(fn,c1,c2,c3,c4,c5);}

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6> class COLc5f6:public COLclosure5<A1,A2,A3,A4,A5>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6);
	COLc5f6():_pF(0){}
	COLc5f6(F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5),_c6(c6){}
	~COLc5f6(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5) const {(*_pF)(a1,a2,a3,a4,a5,_c1,_c2,_c3,_c4,_c5,_c6);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
	C6 _c6;
};

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc5f6<A1,A2,A3,A4,A5,C1,C2,C3,C4,C5,C6>* COLnewClosure5(void (*fn)(A1,A2,A3,A4,A5,C1,C2,C3,C4,C5,C6),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc5f6<A1,A2,A3,A4,A5,C1,C2,C3,C4,C5,C6>(fn,c1,c2,c3,c4,c5,c6);}

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename A6> class COLc6f0:public COLclosure6<A1,A2,A3,A4,A5,A6>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6);
	COLc6f0():_pF(0){}
	COLc6f0(F pF):_pF(pF){}
	~COLc6f0(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6) const {(*_pF)(a1,a2,a3,a4,a5,a6);}
private:
	F _pF;
};

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename A6>
inline COLc6f0<A1,A2,A3,A4,A5,A6>* COLnewClosure6(void (*fn)(A1,A2,A3,A4,A5,A6)){return new COLc6f0<A1,A2,A3,A4,A5,A6>(fn);}

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1> class COLc6f1:public COLclosure6<A1,A2,A3,A4,A5,A6>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,C1 c1);
	COLc6f1():_pF(0){}
	COLc6f1(F pF,C1 c1):_pF(pF),_c1(c1){}
	~COLc6f1(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6) const {(*_pF)(a1,a2,a3,a4,a5,a6,_c1);}
private:
	F _pF;
	C1 _c1;
};

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1>
inline COLc6f1<A1,A2,A3,A4,A5,A6,C1>* COLnewClosure6(void (*fn)(A1,A2,A3,A4,A5,A6,C1),C1 c1){return new COLc6f1<A1,A2,A3,A4,A5,A6,C1>(fn,c1);}

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2> class COLc6f2:public COLclosure6<A1,A2,A3,A4,A5,A6>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,C1 c1,C2 c2);
	COLc6f2():_pF(0){}
	COLc6f2(F pF,C1 c1,C2 c2):_pF(pF),_c1(c1),_c2(c2){}
	~COLc6f2(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6) const {(*_pF)(a1,a2,a3,a4,a5,a6,_c1,_c2);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
};

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2>
inline COLc6f2<A1,A2,A3,A4,A5,A6,C1,C2>* COLnewClosure6(void (*fn)(A1,A2,A3,A4,A5,A6,C1,C2),C1 c1,C2 c2){return new COLc6f2<A1,A2,A3,A4,A5,A6,C1,C2>(fn,c1,c2);}

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3> class COLc6f3:public COLclosure6<A1,A2,A3,A4,A5,A6>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,C1 c1,C2 c2,C3 c3);
	COLc6f3():_pF(0){}
	COLc6f3(F pF,C1 c1,C2 c2,C3 c3):_pF(pF),_c1(c1),_c2(c2),_c3(c3){}
	~COLc6f3(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6) const {(*_pF)(a1,a2,a3,a4,a5,a6,_c1,_c2,_c3);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
};

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3>
inline COLc6f3<A1,A2,A3,A4,A5,A6,C1,C2,C3>* COLnewClosure6(void (*fn)(A1,A2,A3,A4,A5,A6,C1,C2,C3),C1 c1,C2 c2,C3 c3){return new COLc6f3<A1,A2,A3,A4,A5,A6,C1,C2,C3>(fn,c1,c2,c3);}

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3,typename C4> class COLc6f4:public COLclosure6<A1,A2,A3,A4,A5,A6>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,C1 c1,C2 c2,C3 c3,C4 c4);
	COLc6f4():_pF(0){}
	COLc6f4(F pF,C1 c1,C2 c2,C3 c3,C4 c4):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4){}
	~COLc6f4(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6) const {(*_pF)(a1,a2,a3,a4,a5,a6,_c1,_c2,_c3,_c4);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
};

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3,typename C4>
inline COLc6f4<A1,A2,A3,A4,A5,A6,C1,C2,C3,C4>* COLnewClosure6(void (*fn)(A1,A2,A3,A4,A5,A6,C1,C2,C3,C4),C1 c1,C2 c2,C3 c3,C4 c4){return new COLc6f4<A1,A2,A3,A4,A5,A6,C1,C2,C3,C4>(fn,c1,c2,c3,c4);}

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3,typename C4,typename C5> class COLc6f5:public COLclosure6<A1,A2,A3,A4,A5,A6>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5);
	COLc6f5():_pF(0){}
	COLc6f5(F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5){}
	~COLc6f5(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6) const {(*_pF)(a1,a2,a3,a4,a5,a6,_c1,_c2,_c3,_c4,_c5);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
};

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc6f5<A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5>* COLnewClosure6(void (*fn)(A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc6f5<A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5>(fn,c1,c2,c3,c4,c5);}

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6> class COLc6f6:public COLclosure6<A1,A2,A3,A4,A5,A6>{
public:
	typedef void (*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6);
	COLc6f6():_pF(0){}
	COLc6f6(F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6):_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5),_c6(c6){}
	~COLc6f6(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6) const {(*_pF)(a1,a2,a3,a4,a5,a6,_c1,_c2,_c3,_c4,_c5,_c6);}
private:
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
	C6 _c6;
};

template <typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc6f6<A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5,C6>* COLnewClosure6(void (*fn)(A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5,C6),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc6f6<A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5,C6>(fn,c1,c2,c3,c4,c5,c6);}

template <class T> class COLc0m0 : public COLclosure0{
public:
	typedef void (T::*F)();
	COLc0m0():_pF(0),_pObj(0){}
	COLc0m0(T* pObj,F pF):_pObj(pObj),_pF(pF){}
	~COLc0m0(){}
	void run() const {(_pObj->*_pF)();}
private:
	T* _pObj;
	F _pF;
};

template <class T>
inline COLc0m0<T>* COLnewClosure0(T* pObj, void (T::*fn)()){return new COLc0m0<T>(pObj,fn);}

template <class T>
inline COLc0m0<T>* COLnewClosure0(T* pObj, void (T::*fn)() const){return new COLc0m0<T>(pObj,(void (T::*)())fn);}

template <class T,typename C1> class COLc0m1 : public COLclosure0{
public:
	typedef void (T::*F)(C1 c1);
	COLc0m1():_pF(0),_pObj(0){}
	COLc0m1(T* pObj,F pF,C1 c1):_pObj(pObj),_pF(pF),_c1(c1){}
	~COLc0m1(){}
	void run() const {(_pObj->*_pF)(_c1);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
};

template <class T,typename C1>
inline COLc0m1<T,C1>* COLnewClosure0(T* pObj, void (T::*fn)(C1),C1 c1){return new COLc0m1<T,C1>(pObj,fn,c1);}

template <class T,typename C1>
inline COLc0m1<T,C1>* COLnewClosure0(T* pObj, void (T::*fn)(C1) const,C1 c1){return new COLc0m1<T,C1>(pObj,(void (T::*)(C1))fn,c1);}

template <class T,typename C1,typename C2> class COLc0m2 : public COLclosure0{
public:
	typedef void (T::*F)(C1 c1,C2 c2);
	COLc0m2():_pF(0),_pObj(0){}
	COLc0m2(T* pObj,F pF,C1 c1,C2 c2):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2){}
	~COLc0m2(){}
	void run() const {(_pObj->*_pF)(_c1,_c2);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
};

template <class T,typename C1,typename C2>
inline COLc0m2<T,C1,C2>* COLnewClosure0(T* pObj, void (T::*fn)(C1,C2),C1 c1,C2 c2){return new COLc0m2<T,C1,C2>(pObj,fn,c1,c2);}

template <class T,typename C1,typename C2>
inline COLc0m2<T,C1,C2>* COLnewClosure0(T* pObj, void (T::*fn)(C1,C2) const,C1 c1,C2 c2){return new COLc0m2<T,C1,C2>(pObj,(void (T::*)(C1,C2))fn,c1,c2);}

template <class T,typename C1,typename C2,typename C3> class COLc0m3 : public COLclosure0{
public:
	typedef void (T::*F)(C1 c1,C2 c2,C3 c3);
	COLc0m3():_pF(0),_pObj(0){}
	COLc0m3(T* pObj,F pF,C1 c1,C2 c2,C3 c3):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3){}
	~COLc0m3(){}
	void run() const {(_pObj->*_pF)(_c1,_c2,_c3);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
};

template <class T,typename C1,typename C2,typename C3>
inline COLc0m3<T,C1,C2,C3>* COLnewClosure0(T* pObj, void (T::*fn)(C1,C2,C3),C1 c1,C2 c2,C3 c3){return new COLc0m3<T,C1,C2,C3>(pObj,fn,c1,c2,c3);}

template <class T,typename C1,typename C2,typename C3>
inline COLc0m3<T,C1,C2,C3>* COLnewClosure0(T* pObj, void (T::*fn)(C1,C2,C3) const,C1 c1,C2 c2,C3 c3){return new COLc0m3<T,C1,C2,C3>(pObj,(void (T::*)(C1,C2,C3))fn,c1,c2,c3);}

template <class T,typename C1,typename C2,typename C3,typename C4> class COLc0m4 : public COLclosure0{
public:
	typedef void (T::*F)(C1 c1,C2 c2,C3 c3,C4 c4);
	COLc0m4():_pF(0),_pObj(0){}
	COLc0m4(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4){}
	~COLc0m4(){}
	void run() const {(_pObj->*_pF)(_c1,_c2,_c3,_c4);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
};

template <class T,typename C1,typename C2,typename C3,typename C4>
inline COLc0m4<T,C1,C2,C3,C4>* COLnewClosure0(T* pObj, void (T::*fn)(C1,C2,C3,C4),C1 c1,C2 c2,C3 c3,C4 c4){return new COLc0m4<T,C1,C2,C3,C4>(pObj,fn,c1,c2,c3,c4);}

template <class T,typename C1,typename C2,typename C3,typename C4>
inline COLc0m4<T,C1,C2,C3,C4>* COLnewClosure0(T* pObj, void (T::*fn)(C1,C2,C3,C4) const,C1 c1,C2 c2,C3 c3,C4 c4){return new COLc0m4<T,C1,C2,C3,C4>(pObj,(void (T::*)(C1,C2,C3,C4))fn,c1,c2,c3,c4);}

template <class T,typename C1,typename C2,typename C3,typename C4,typename C5> class COLc0m5 : public COLclosure0{
public:
	typedef void (T::*F)(C1 c1,C2 c2,C3 c3,C4 c4,C5 c5);
	COLc0m5():_pF(0),_pObj(0){}
	COLc0m5(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5){}
	~COLc0m5(){}
	void run() const {(_pObj->*_pF)(_c1,_c2,_c3,_c4,_c5);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
};

template <class T,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc0m5<T,C1,C2,C3,C4,C5>* COLnewClosure0(T* pObj, void (T::*fn)(C1,C2,C3,C4,C5),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc0m5<T,C1,C2,C3,C4,C5>(pObj,fn,c1,c2,c3,c4,c5);}

template <class T,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc0m5<T,C1,C2,C3,C4,C5>* COLnewClosure0(T* pObj, void (T::*fn)(C1,C2,C3,C4,C5) const,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc0m5<T,C1,C2,C3,C4,C5>(pObj,(void (T::*)(C1,C2,C3,C4,C5))fn,c1,c2,c3,c4,c5);}

template <class T,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6> class COLc0m6 : public COLclosure0{
public:
	typedef void (T::*F)(C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6);
	COLc0m6():_pF(0),_pObj(0){}
	COLc0m6(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5),_c6(c6){}
	~COLc0m6(){}
	void run() const {(_pObj->*_pF)(_c1,_c2,_c3,_c4,_c5,_c6);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
	C6 _c6;
};

template <class T,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc0m6<T,C1,C2,C3,C4,C5,C6>* COLnewClosure0(T* pObj, void (T::*fn)(C1,C2,C3,C4,C5,C6),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc0m6<T,C1,C2,C3,C4,C5,C6>(pObj,fn,c1,c2,c3,c4,c5,c6);}

template <class T,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc0m6<T,C1,C2,C3,C4,C5,C6>* COLnewClosure0(T* pObj, void (T::*fn)(C1,C2,C3,C4,C5,C6) const,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc0m6<T,C1,C2,C3,C4,C5,C6>(pObj,(void (T::*)(C1,C2,C3,C4,C5,C6))fn,c1,c2,c3,c4,c5,c6);}

template <class T,typename A1> class COLc1m0 : public COLclosure1<A1>{
public:
	typedef void (T::*F)(A1 a1);
	COLc1m0():_pF(0),_pObj(0){}
	COLc1m0(T* pObj,F pF):_pObj(pObj),_pF(pF){}
	~COLc1m0(){}
	void run(A1 a1) const {(_pObj->*_pF)(a1);}
private:
	T* _pObj;
	F _pF;
};

template <class T,typename A1>
inline COLc1m0<T,A1>* COLnewClosure1(T* pObj, void (T::*fn)(A1)){return new COLc1m0<T,A1>(pObj,fn);}

template <class T,typename A1>
inline COLc1m0<T,A1>* COLnewClosure1(T* pObj, void (T::*fn)(A1) const){return new COLc1m0<T,A1>(pObj,(void (T::*)(A1))fn);}

template <class T,typename A1,typename C1> class COLc1m1 : public COLclosure1<A1>{
public:
	typedef void (T::*F)(A1 a1,C1 c1);
	COLc1m1():_pF(0),_pObj(0){}
	COLc1m1(T* pObj,F pF,C1 c1):_pObj(pObj),_pF(pF),_c1(c1){}
	~COLc1m1(){}
	void run(A1 a1) const {(_pObj->*_pF)(a1,_c1);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
};

template <class T,typename A1,typename C1>
inline COLc1m1<T,A1,C1>* COLnewClosure1(T* pObj, void (T::*fn)(A1,C1),C1 c1){return new COLc1m1<T,A1,C1>(pObj,fn,c1);}

template <class T,typename A1,typename C1>
inline COLc1m1<T,A1,C1>* COLnewClosure1(T* pObj, void (T::*fn)(A1,C1) const,C1 c1){return new COLc1m1<T,A1,C1>(pObj,(void (T::*)(A1,C1))fn,c1);}

template <class T,typename A1,typename C1,typename C2> class COLc1m2 : public COLclosure1<A1>{
public:
	typedef void (T::*F)(A1 a1,C1 c1,C2 c2);
	COLc1m2():_pF(0),_pObj(0){}
	COLc1m2(T* pObj,F pF,C1 c1,C2 c2):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2){}
	~COLc1m2(){}
	void run(A1 a1) const {(_pObj->*_pF)(a1,_c1,_c2);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
};

template <class T,typename A1,typename C1,typename C2>
inline COLc1m2<T,A1,C1,C2>* COLnewClosure1(T* pObj, void (T::*fn)(A1,C1,C2),C1 c1,C2 c2){return new COLc1m2<T,A1,C1,C2>(pObj,fn,c1,c2);}

template <class T,typename A1,typename C1,typename C2>
inline COLc1m2<T,A1,C1,C2>* COLnewClosure1(T* pObj, void (T::*fn)(A1,C1,C2) const,C1 c1,C2 c2){return new COLc1m2<T,A1,C1,C2>(pObj,(void (T::*)(A1,C1,C2))fn,c1,c2);}

template <class T,typename A1,typename C1,typename C2,typename C3> class COLc1m3 : public COLclosure1<A1>{
public:
	typedef void (T::*F)(A1 a1,C1 c1,C2 c2,C3 c3);
	COLc1m3():_pF(0),_pObj(0){}
	COLc1m3(T* pObj,F pF,C1 c1,C2 c2,C3 c3):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3){}
	~COLc1m3(){}
	void run(A1 a1) const {(_pObj->*_pF)(a1,_c1,_c2,_c3);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
};

template <class T,typename A1,typename C1,typename C2,typename C3>
inline COLc1m3<T,A1,C1,C2,C3>* COLnewClosure1(T* pObj, void (T::*fn)(A1,C1,C2,C3),C1 c1,C2 c2,C3 c3){return new COLc1m3<T,A1,C1,C2,C3>(pObj,fn,c1,c2,c3);}

template <class T,typename A1,typename C1,typename C2,typename C3>
inline COLc1m3<T,A1,C1,C2,C3>* COLnewClosure1(T* pObj, void (T::*fn)(A1,C1,C2,C3) const,C1 c1,C2 c2,C3 c3){return new COLc1m3<T,A1,C1,C2,C3>(pObj,(void (T::*)(A1,C1,C2,C3))fn,c1,c2,c3);}

template <class T,typename A1,typename C1,typename C2,typename C3,typename C4> class COLc1m4 : public COLclosure1<A1>{
public:
	typedef void (T::*F)(A1 a1,C1 c1,C2 c2,C3 c3,C4 c4);
	COLc1m4():_pF(0),_pObj(0){}
	COLc1m4(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4){}
	~COLc1m4(){}
	void run(A1 a1) const {(_pObj->*_pF)(a1,_c1,_c2,_c3,_c4);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
};

template <class T,typename A1,typename C1,typename C2,typename C3,typename C4>
inline COLc1m4<T,A1,C1,C2,C3,C4>* COLnewClosure1(T* pObj, void (T::*fn)(A1,C1,C2,C3,C4),C1 c1,C2 c2,C3 c3,C4 c4){return new COLc1m4<T,A1,C1,C2,C3,C4>(pObj,fn,c1,c2,c3,c4);}

template <class T,typename A1,typename C1,typename C2,typename C3,typename C4>
inline COLc1m4<T,A1,C1,C2,C3,C4>* COLnewClosure1(T* pObj, void (T::*fn)(A1,C1,C2,C3,C4) const,C1 c1,C2 c2,C3 c3,C4 c4){return new COLc1m4<T,A1,C1,C2,C3,C4>(pObj,(void (T::*)(A1,C1,C2,C3,C4))fn,c1,c2,c3,c4);}

template <class T,typename A1,typename C1,typename C2,typename C3,typename C4,typename C5> class COLc1m5 : public COLclosure1<A1>{
public:
	typedef void (T::*F)(A1 a1,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5);
	COLc1m5():_pF(0),_pObj(0){}
	COLc1m5(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5){}
	~COLc1m5(){}
	void run(A1 a1) const {(_pObj->*_pF)(a1,_c1,_c2,_c3,_c4,_c5);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
};

template <class T,typename A1,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc1m5<T,A1,C1,C2,C3,C4,C5>* COLnewClosure1(T* pObj, void (T::*fn)(A1,C1,C2,C3,C4,C5),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc1m5<T,A1,C1,C2,C3,C4,C5>(pObj,fn,c1,c2,c3,c4,c5);}

template <class T,typename A1,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc1m5<T,A1,C1,C2,C3,C4,C5>* COLnewClosure1(T* pObj, void (T::*fn)(A1,C1,C2,C3,C4,C5) const,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc1m5<T,A1,C1,C2,C3,C4,C5>(pObj,(void (T::*)(A1,C1,C2,C3,C4,C5))fn,c1,c2,c3,c4,c5);}

template <class T,typename A1,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6> class COLc1m6 : public COLclosure1<A1>{
public:
	typedef void (T::*F)(A1 a1,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6);
	COLc1m6():_pF(0),_pObj(0){}
	COLc1m6(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5),_c6(c6){}
	~COLc1m6(){}
	void run(A1 a1) const {(_pObj->*_pF)(a1,_c1,_c2,_c3,_c4,_c5,_c6);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
	C6 _c6;
};

template <class T,typename A1,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc1m6<T,A1,C1,C2,C3,C4,C5,C6>* COLnewClosure1(T* pObj, void (T::*fn)(A1,C1,C2,C3,C4,C5,C6),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc1m6<T,A1,C1,C2,C3,C4,C5,C6>(pObj,fn,c1,c2,c3,c4,c5,c6);}

template <class T,typename A1,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc1m6<T,A1,C1,C2,C3,C4,C5,C6>* COLnewClosure1(T* pObj, void (T::*fn)(A1,C1,C2,C3,C4,C5,C6) const,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc1m6<T,A1,C1,C2,C3,C4,C5,C6>(pObj,(void (T::*)(A1,C1,C2,C3,C4,C5,C6))fn,c1,c2,c3,c4,c5,c6);}

template <class T,typename A1,typename A2> class COLc2m0 : public COLclosure2<A1,A2>{
public:
	typedef void (T::*F)(A1 a1,A2 a2);
	COLc2m0():_pF(0),_pObj(0){}
	COLc2m0(T* pObj,F pF):_pObj(pObj),_pF(pF){}
	~COLc2m0(){}
	void run(A1 a1,A2 a2) const {(_pObj->*_pF)(a1,a2);}
private:
	T* _pObj;
	F _pF;
};

template <class T,typename A1,typename A2>
inline COLc2m0<T,A1,A2>* COLnewClosure2(T* pObj, void (T::*fn)(A1,A2)){return new COLc2m0<T,A1,A2>(pObj,fn);}

template <class T,typename A1,typename A2>
inline COLc2m0<T,A1,A2>* COLnewClosure2(T* pObj, void (T::*fn)(A1,A2) const){return new COLc2m0<T,A1,A2>(pObj,(void (T::*)(A1,A2))fn);}

template <class T,typename A1,typename A2,typename C1> class COLc2m1 : public COLclosure2<A1,A2>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,C1 c1);
	COLc2m1():_pF(0),_pObj(0){}
	COLc2m1(T* pObj,F pF,C1 c1):_pObj(pObj),_pF(pF),_c1(c1){}
	~COLc2m1(){}
	void run(A1 a1,A2 a2) const {(_pObj->*_pF)(a1,a2,_c1);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
};

template <class T,typename A1,typename A2,typename C1>
inline COLc2m1<T,A1,A2,C1>* COLnewClosure2(T* pObj, void (T::*fn)(A1,A2,C1),C1 c1){return new COLc2m1<T,A1,A2,C1>(pObj,fn,c1);}

template <class T,typename A1,typename A2,typename C1>
inline COLc2m1<T,A1,A2,C1>* COLnewClosure2(T* pObj, void (T::*fn)(A1,A2,C1) const,C1 c1){return new COLc2m1<T,A1,A2,C1>(pObj,(void (T::*)(A1,A2,C1))fn,c1);}

template <class T,typename A1,typename A2,typename C1,typename C2> class COLc2m2 : public COLclosure2<A1,A2>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,C1 c1,C2 c2);
	COLc2m2():_pF(0),_pObj(0){}
	COLc2m2(T* pObj,F pF,C1 c1,C2 c2):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2){}
	~COLc2m2(){}
	void run(A1 a1,A2 a2) const {(_pObj->*_pF)(a1,a2,_c1,_c2);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
};

template <class T,typename A1,typename A2,typename C1,typename C2>
inline COLc2m2<T,A1,A2,C1,C2>* COLnewClosure2(T* pObj, void (T::*fn)(A1,A2,C1,C2),C1 c1,C2 c2){return new COLc2m2<T,A1,A2,C1,C2>(pObj,fn,c1,c2);}

template <class T,typename A1,typename A2,typename C1,typename C2>
inline COLc2m2<T,A1,A2,C1,C2>* COLnewClosure2(T* pObj, void (T::*fn)(A1,A2,C1,C2) const,C1 c1,C2 c2){return new COLc2m2<T,A1,A2,C1,C2>(pObj,(void (T::*)(A1,A2,C1,C2))fn,c1,c2);}

template <class T,typename A1,typename A2,typename C1,typename C2,typename C3> class COLc2m3 : public COLclosure2<A1,A2>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,C1 c1,C2 c2,C3 c3);
	COLc2m3():_pF(0),_pObj(0){}
	COLc2m3(T* pObj,F pF,C1 c1,C2 c2,C3 c3):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3){}
	~COLc2m3(){}
	void run(A1 a1,A2 a2) const {(_pObj->*_pF)(a1,a2,_c1,_c2,_c3);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
};

template <class T,typename A1,typename A2,typename C1,typename C2,typename C3>
inline COLc2m3<T,A1,A2,C1,C2,C3>* COLnewClosure2(T* pObj, void (T::*fn)(A1,A2,C1,C2,C3),C1 c1,C2 c2,C3 c3){return new COLc2m3<T,A1,A2,C1,C2,C3>(pObj,fn,c1,c2,c3);}

template <class T,typename A1,typename A2,typename C1,typename C2,typename C3>
inline COLc2m3<T,A1,A2,C1,C2,C3>* COLnewClosure2(T* pObj, void (T::*fn)(A1,A2,C1,C2,C3) const,C1 c1,C2 c2,C3 c3){return new COLc2m3<T,A1,A2,C1,C2,C3>(pObj,(void (T::*)(A1,A2,C1,C2,C3))fn,c1,c2,c3);}

template <class T,typename A1,typename A2,typename C1,typename C2,typename C3,typename C4> class COLc2m4 : public COLclosure2<A1,A2>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,C1 c1,C2 c2,C3 c3,C4 c4);
	COLc2m4():_pF(0),_pObj(0){}
	COLc2m4(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4){}
	~COLc2m4(){}
	void run(A1 a1,A2 a2) const {(_pObj->*_pF)(a1,a2,_c1,_c2,_c3,_c4);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
};

template <class T,typename A1,typename A2,typename C1,typename C2,typename C3,typename C4>
inline COLc2m4<T,A1,A2,C1,C2,C3,C4>* COLnewClosure2(T* pObj, void (T::*fn)(A1,A2,C1,C2,C3,C4),C1 c1,C2 c2,C3 c3,C4 c4){return new COLc2m4<T,A1,A2,C1,C2,C3,C4>(pObj,fn,c1,c2,c3,c4);}

template <class T,typename A1,typename A2,typename C1,typename C2,typename C3,typename C4>
inline COLc2m4<T,A1,A2,C1,C2,C3,C4>* COLnewClosure2(T* pObj, void (T::*fn)(A1,A2,C1,C2,C3,C4) const,C1 c1,C2 c2,C3 c3,C4 c4){return new COLc2m4<T,A1,A2,C1,C2,C3,C4>(pObj,(void (T::*)(A1,A2,C1,C2,C3,C4))fn,c1,c2,c3,c4);}

template <class T,typename A1,typename A2,typename C1,typename C2,typename C3,typename C4,typename C5> class COLc2m5 : public COLclosure2<A1,A2>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5);
	COLc2m5():_pF(0),_pObj(0){}
	COLc2m5(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5){}
	~COLc2m5(){}
	void run(A1 a1,A2 a2) const {(_pObj->*_pF)(a1,a2,_c1,_c2,_c3,_c4,_c5);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
};

template <class T,typename A1,typename A2,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc2m5<T,A1,A2,C1,C2,C3,C4,C5>* COLnewClosure2(T* pObj, void (T::*fn)(A1,A2,C1,C2,C3,C4,C5),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc2m5<T,A1,A2,C1,C2,C3,C4,C5>(pObj,fn,c1,c2,c3,c4,c5);}

template <class T,typename A1,typename A2,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc2m5<T,A1,A2,C1,C2,C3,C4,C5>* COLnewClosure2(T* pObj, void (T::*fn)(A1,A2,C1,C2,C3,C4,C5) const,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc2m5<T,A1,A2,C1,C2,C3,C4,C5>(pObj,(void (T::*)(A1,A2,C1,C2,C3,C4,C5))fn,c1,c2,c3,c4,c5);}

template <class T,typename A1,typename A2,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6> class COLc2m6 : public COLclosure2<A1,A2>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6);
	COLc2m6():_pF(0),_pObj(0){}
	COLc2m6(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5),_c6(c6){}
	~COLc2m6(){}
	void run(A1 a1,A2 a2) const {(_pObj->*_pF)(a1,a2,_c1,_c2,_c3,_c4,_c5,_c6);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
	C6 _c6;
};

template <class T,typename A1,typename A2,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc2m6<T,A1,A2,C1,C2,C3,C4,C5,C6>* COLnewClosure2(T* pObj, void (T::*fn)(A1,A2,C1,C2,C3,C4,C5,C6),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc2m6<T,A1,A2,C1,C2,C3,C4,C5,C6>(pObj,fn,c1,c2,c3,c4,c5,c6);}

template <class T,typename A1,typename A2,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc2m6<T,A1,A2,C1,C2,C3,C4,C5,C6>* COLnewClosure2(T* pObj, void (T::*fn)(A1,A2,C1,C2,C3,C4,C5,C6) const,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc2m6<T,A1,A2,C1,C2,C3,C4,C5,C6>(pObj,(void (T::*)(A1,A2,C1,C2,C3,C4,C5,C6))fn,c1,c2,c3,c4,c5,c6);}

template <class T,typename A1,typename A2,typename A3> class COLc3m0 : public COLclosure3<A1,A2,A3>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3);
	COLc3m0():_pF(0),_pObj(0){}
	COLc3m0(T* pObj,F pF):_pObj(pObj),_pF(pF){}
	~COLc3m0(){}
	void run(A1 a1,A2 a2,A3 a3) const {(_pObj->*_pF)(a1,a2,a3);}
private:
	T* _pObj;
	F _pF;
};

template <class T,typename A1,typename A2,typename A3>
inline COLc3m0<T,A1,A2,A3>* COLnewClosure3(T* pObj, void (T::*fn)(A1,A2,A3)){return new COLc3m0<T,A1,A2,A3>(pObj,fn);}

template <class T,typename A1,typename A2,typename A3>
inline COLc3m0<T,A1,A2,A3>* COLnewClosure3(T* pObj, void (T::*fn)(A1,A2,A3) const){return new COLc3m0<T,A1,A2,A3>(pObj,(void (T::*)(A1,A2,A3))fn);}

template <class T,typename A1,typename A2,typename A3,typename C1> class COLc3m1 : public COLclosure3<A1,A2,A3>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,C1 c1);
	COLc3m1():_pF(0),_pObj(0){}
	COLc3m1(T* pObj,F pF,C1 c1):_pObj(pObj),_pF(pF),_c1(c1){}
	~COLc3m1(){}
	void run(A1 a1,A2 a2,A3 a3) const {(_pObj->*_pF)(a1,a2,a3,_c1);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
};

template <class T,typename A1,typename A2,typename A3,typename C1>
inline COLc3m1<T,A1,A2,A3,C1>* COLnewClosure3(T* pObj, void (T::*fn)(A1,A2,A3,C1),C1 c1){return new COLc3m1<T,A1,A2,A3,C1>(pObj,fn,c1);}

template <class T,typename A1,typename A2,typename A3,typename C1>
inline COLc3m1<T,A1,A2,A3,C1>* COLnewClosure3(T* pObj, void (T::*fn)(A1,A2,A3,C1) const,C1 c1){return new COLc3m1<T,A1,A2,A3,C1>(pObj,(void (T::*)(A1,A2,A3,C1))fn,c1);}

template <class T,typename A1,typename A2,typename A3,typename C1,typename C2> class COLc3m2 : public COLclosure3<A1,A2,A3>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,C1 c1,C2 c2);
	COLc3m2():_pF(0),_pObj(0){}
	COLc3m2(T* pObj,F pF,C1 c1,C2 c2):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2){}
	~COLc3m2(){}
	void run(A1 a1,A2 a2,A3 a3) const {(_pObj->*_pF)(a1,a2,a3,_c1,_c2);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
};

template <class T,typename A1,typename A2,typename A3,typename C1,typename C2>
inline COLc3m2<T,A1,A2,A3,C1,C2>* COLnewClosure3(T* pObj, void (T::*fn)(A1,A2,A3,C1,C2),C1 c1,C2 c2){return new COLc3m2<T,A1,A2,A3,C1,C2>(pObj,fn,c1,c2);}

template <class T,typename A1,typename A2,typename A3,typename C1,typename C2>
inline COLc3m2<T,A1,A2,A3,C1,C2>* COLnewClosure3(T* pObj, void (T::*fn)(A1,A2,A3,C1,C2) const,C1 c1,C2 c2){return new COLc3m2<T,A1,A2,A3,C1,C2>(pObj,(void (T::*)(A1,A2,A3,C1,C2))fn,c1,c2);}

template <class T,typename A1,typename A2,typename A3,typename C1,typename C2,typename C3> class COLc3m3 : public COLclosure3<A1,A2,A3>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,C1 c1,C2 c2,C3 c3);
	COLc3m3():_pF(0),_pObj(0){}
	COLc3m3(T* pObj,F pF,C1 c1,C2 c2,C3 c3):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3){}
	~COLc3m3(){}
	void run(A1 a1,A2 a2,A3 a3) const {(_pObj->*_pF)(a1,a2,a3,_c1,_c2,_c3);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
};

template <class T,typename A1,typename A2,typename A3,typename C1,typename C2,typename C3>
inline COLc3m3<T,A1,A2,A3,C1,C2,C3>* COLnewClosure3(T* pObj, void (T::*fn)(A1,A2,A3,C1,C2,C3),C1 c1,C2 c2,C3 c3){return new COLc3m3<T,A1,A2,A3,C1,C2,C3>(pObj,fn,c1,c2,c3);}

template <class T,typename A1,typename A2,typename A3,typename C1,typename C2,typename C3>
inline COLc3m3<T,A1,A2,A3,C1,C2,C3>* COLnewClosure3(T* pObj, void (T::*fn)(A1,A2,A3,C1,C2,C3) const,C1 c1,C2 c2,C3 c3){return new COLc3m3<T,A1,A2,A3,C1,C2,C3>(pObj,(void (T::*)(A1,A2,A3,C1,C2,C3))fn,c1,c2,c3);}

template <class T,typename A1,typename A2,typename A3,typename C1,typename C2,typename C3,typename C4> class COLc3m4 : public COLclosure3<A1,A2,A3>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,C1 c1,C2 c2,C3 c3,C4 c4);
	COLc3m4():_pF(0),_pObj(0){}
	COLc3m4(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4){}
	~COLc3m4(){}
	void run(A1 a1,A2 a2,A3 a3) const {(_pObj->*_pF)(a1,a2,a3,_c1,_c2,_c3,_c4);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
};

template <class T,typename A1,typename A2,typename A3,typename C1,typename C2,typename C3,typename C4>
inline COLc3m4<T,A1,A2,A3,C1,C2,C3,C4>* COLnewClosure3(T* pObj, void (T::*fn)(A1,A2,A3,C1,C2,C3,C4),C1 c1,C2 c2,C3 c3,C4 c4){return new COLc3m4<T,A1,A2,A3,C1,C2,C3,C4>(pObj,fn,c1,c2,c3,c4);}

template <class T,typename A1,typename A2,typename A3,typename C1,typename C2,typename C3,typename C4>
inline COLc3m4<T,A1,A2,A3,C1,C2,C3,C4>* COLnewClosure3(T* pObj, void (T::*fn)(A1,A2,A3,C1,C2,C3,C4) const,C1 c1,C2 c2,C3 c3,C4 c4){return new COLc3m4<T,A1,A2,A3,C1,C2,C3,C4>(pObj,(void (T::*)(A1,A2,A3,C1,C2,C3,C4))fn,c1,c2,c3,c4);}

template <class T,typename A1,typename A2,typename A3,typename C1,typename C2,typename C3,typename C4,typename C5> class COLc3m5 : public COLclosure3<A1,A2,A3>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5);
	COLc3m5():_pF(0),_pObj(0){}
	COLc3m5(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5){}
	~COLc3m5(){}
	void run(A1 a1,A2 a2,A3 a3) const {(_pObj->*_pF)(a1,a2,a3,_c1,_c2,_c3,_c4,_c5);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
};

template <class T,typename A1,typename A2,typename A3,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc3m5<T,A1,A2,A3,C1,C2,C3,C4,C5>* COLnewClosure3(T* pObj, void (T::*fn)(A1,A2,A3,C1,C2,C3,C4,C5),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc3m5<T,A1,A2,A3,C1,C2,C3,C4,C5>(pObj,fn,c1,c2,c3,c4,c5);}

template <class T,typename A1,typename A2,typename A3,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc3m5<T,A1,A2,A3,C1,C2,C3,C4,C5>* COLnewClosure3(T* pObj, void (T::*fn)(A1,A2,A3,C1,C2,C3,C4,C5) const,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc3m5<T,A1,A2,A3,C1,C2,C3,C4,C5>(pObj,(void (T::*)(A1,A2,A3,C1,C2,C3,C4,C5))fn,c1,c2,c3,c4,c5);}

template <class T,typename A1,typename A2,typename A3,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6> class COLc3m6 : public COLclosure3<A1,A2,A3>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6);
	COLc3m6():_pF(0),_pObj(0){}
	COLc3m6(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5),_c6(c6){}
	~COLc3m6(){}
	void run(A1 a1,A2 a2,A3 a3) const {(_pObj->*_pF)(a1,a2,a3,_c1,_c2,_c3,_c4,_c5,_c6);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
	C6 _c6;
};

template <class T,typename A1,typename A2,typename A3,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc3m6<T,A1,A2,A3,C1,C2,C3,C4,C5,C6>* COLnewClosure3(T* pObj, void (T::*fn)(A1,A2,A3,C1,C2,C3,C4,C5,C6),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc3m6<T,A1,A2,A3,C1,C2,C3,C4,C5,C6>(pObj,fn,c1,c2,c3,c4,c5,c6);}

template <class T,typename A1,typename A2,typename A3,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc3m6<T,A1,A2,A3,C1,C2,C3,C4,C5,C6>* COLnewClosure3(T* pObj, void (T::*fn)(A1,A2,A3,C1,C2,C3,C4,C5,C6) const,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc3m6<T,A1,A2,A3,C1,C2,C3,C4,C5,C6>(pObj,(void (T::*)(A1,A2,A3,C1,C2,C3,C4,C5,C6))fn,c1,c2,c3,c4,c5,c6);}

template <class T,typename A1,typename A2,typename A3,typename A4> class COLc4m0 : public COLclosure4<A1,A2,A3,A4>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4);
	COLc4m0():_pF(0),_pObj(0){}
	COLc4m0(T* pObj,F pF):_pObj(pObj),_pF(pF){}
	~COLc4m0(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4) const {(_pObj->*_pF)(a1,a2,a3,a4);}
private:
	T* _pObj;
	F _pF;
};

template <class T,typename A1,typename A2,typename A3,typename A4>
inline COLc4m0<T,A1,A2,A3,A4>* COLnewClosure4(T* pObj, void (T::*fn)(A1,A2,A3,A4)){return new COLc4m0<T,A1,A2,A3,A4>(pObj,fn);}

template <class T,typename A1,typename A2,typename A3,typename A4>
inline COLc4m0<T,A1,A2,A3,A4>* COLnewClosure4(T* pObj, void (T::*fn)(A1,A2,A3,A4) const){return new COLc4m0<T,A1,A2,A3,A4>(pObj,(void (T::*)(A1,A2,A3,A4))fn);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename C1> class COLc4m1 : public COLclosure4<A1,A2,A3,A4>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,C1 c1);
	COLc4m1():_pF(0),_pObj(0){}
	COLc4m1(T* pObj,F pF,C1 c1):_pObj(pObj),_pF(pF),_c1(c1){}
	~COLc4m1(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4) const {(_pObj->*_pF)(a1,a2,a3,a4,_c1);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename C1>
inline COLc4m1<T,A1,A2,A3,A4,C1>* COLnewClosure4(T* pObj, void (T::*fn)(A1,A2,A3,A4,C1),C1 c1){return new COLc4m1<T,A1,A2,A3,A4,C1>(pObj,fn,c1);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename C1>
inline COLc4m1<T,A1,A2,A3,A4,C1>* COLnewClosure4(T* pObj, void (T::*fn)(A1,A2,A3,A4,C1) const,C1 c1){return new COLc4m1<T,A1,A2,A3,A4,C1>(pObj,(void (T::*)(A1,A2,A3,A4,C1))fn,c1);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename C1,typename C2> class COLc4m2 : public COLclosure4<A1,A2,A3,A4>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,C1 c1,C2 c2);
	COLc4m2():_pF(0),_pObj(0){}
	COLc4m2(T* pObj,F pF,C1 c1,C2 c2):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2){}
	~COLc4m2(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4) const {(_pObj->*_pF)(a1,a2,a3,a4,_c1,_c2);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename C1,typename C2>
inline COLc4m2<T,A1,A2,A3,A4,C1,C2>* COLnewClosure4(T* pObj, void (T::*fn)(A1,A2,A3,A4,C1,C2),C1 c1,C2 c2){return new COLc4m2<T,A1,A2,A3,A4,C1,C2>(pObj,fn,c1,c2);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename C1,typename C2>
inline COLc4m2<T,A1,A2,A3,A4,C1,C2>* COLnewClosure4(T* pObj, void (T::*fn)(A1,A2,A3,A4,C1,C2) const,C1 c1,C2 c2){return new COLc4m2<T,A1,A2,A3,A4,C1,C2>(pObj,(void (T::*)(A1,A2,A3,A4,C1,C2))fn,c1,c2);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3> class COLc4m3 : public COLclosure4<A1,A2,A3,A4>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,C1 c1,C2 c2,C3 c3);
	COLc4m3():_pF(0),_pObj(0){}
	COLc4m3(T* pObj,F pF,C1 c1,C2 c2,C3 c3):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3){}
	~COLc4m3(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4) const {(_pObj->*_pF)(a1,a2,a3,a4,_c1,_c2,_c3);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3>
inline COLc4m3<T,A1,A2,A3,A4,C1,C2,C3>* COLnewClosure4(T* pObj, void (T::*fn)(A1,A2,A3,A4,C1,C2,C3),C1 c1,C2 c2,C3 c3){return new COLc4m3<T,A1,A2,A3,A4,C1,C2,C3>(pObj,fn,c1,c2,c3);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3>
inline COLc4m3<T,A1,A2,A3,A4,C1,C2,C3>* COLnewClosure4(T* pObj, void (T::*fn)(A1,A2,A3,A4,C1,C2,C3) const,C1 c1,C2 c2,C3 c3){return new COLc4m3<T,A1,A2,A3,A4,C1,C2,C3>(pObj,(void (T::*)(A1,A2,A3,A4,C1,C2,C3))fn,c1,c2,c3);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3,typename C4> class COLc4m4 : public COLclosure4<A1,A2,A3,A4>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,C1 c1,C2 c2,C3 c3,C4 c4);
	COLc4m4():_pF(0),_pObj(0){}
	COLc4m4(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4){}
	~COLc4m4(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4) const {(_pObj->*_pF)(a1,a2,a3,a4,_c1,_c2,_c3,_c4);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3,typename C4>
inline COLc4m4<T,A1,A2,A3,A4,C1,C2,C3,C4>* COLnewClosure4(T* pObj, void (T::*fn)(A1,A2,A3,A4,C1,C2,C3,C4),C1 c1,C2 c2,C3 c3,C4 c4){return new COLc4m4<T,A1,A2,A3,A4,C1,C2,C3,C4>(pObj,fn,c1,c2,c3,c4);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3,typename C4>
inline COLc4m4<T,A1,A2,A3,A4,C1,C2,C3,C4>* COLnewClosure4(T* pObj, void (T::*fn)(A1,A2,A3,A4,C1,C2,C3,C4) const,C1 c1,C2 c2,C3 c3,C4 c4){return new COLc4m4<T,A1,A2,A3,A4,C1,C2,C3,C4>(pObj,(void (T::*)(A1,A2,A3,A4,C1,C2,C3,C4))fn,c1,c2,c3,c4);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3,typename C4,typename C5> class COLc4m5 : public COLclosure4<A1,A2,A3,A4>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5);
	COLc4m5():_pF(0),_pObj(0){}
	COLc4m5(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5){}
	~COLc4m5(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4) const {(_pObj->*_pF)(a1,a2,a3,a4,_c1,_c2,_c3,_c4,_c5);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc4m5<T,A1,A2,A3,A4,C1,C2,C3,C4,C5>* COLnewClosure4(T* pObj, void (T::*fn)(A1,A2,A3,A4,C1,C2,C3,C4,C5),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc4m5<T,A1,A2,A3,A4,C1,C2,C3,C4,C5>(pObj,fn,c1,c2,c3,c4,c5);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc4m5<T,A1,A2,A3,A4,C1,C2,C3,C4,C5>* COLnewClosure4(T* pObj, void (T::*fn)(A1,A2,A3,A4,C1,C2,C3,C4,C5) const,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc4m5<T,A1,A2,A3,A4,C1,C2,C3,C4,C5>(pObj,(void (T::*)(A1,A2,A3,A4,C1,C2,C3,C4,C5))fn,c1,c2,c3,c4,c5);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6> class COLc4m6 : public COLclosure4<A1,A2,A3,A4>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6);
	COLc4m6():_pF(0),_pObj(0){}
	COLc4m6(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5),_c6(c6){}
	~COLc4m6(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4) const {(_pObj->*_pF)(a1,a2,a3,a4,_c1,_c2,_c3,_c4,_c5,_c6);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
	C6 _c6;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc4m6<T,A1,A2,A3,A4,C1,C2,C3,C4,C5,C6>* COLnewClosure4(T* pObj, void (T::*fn)(A1,A2,A3,A4,C1,C2,C3,C4,C5,C6),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc4m6<T,A1,A2,A3,A4,C1,C2,C3,C4,C5,C6>(pObj,fn,c1,c2,c3,c4,c5,c6);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc4m6<T,A1,A2,A3,A4,C1,C2,C3,C4,C5,C6>* COLnewClosure4(T* pObj, void (T::*fn)(A1,A2,A3,A4,C1,C2,C3,C4,C5,C6) const,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc4m6<T,A1,A2,A3,A4,C1,C2,C3,C4,C5,C6>(pObj,(void (T::*)(A1,A2,A3,A4,C1,C2,C3,C4,C5,C6))fn,c1,c2,c3,c4,c5,c6);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5> class COLc5m0 : public COLclosure5<A1,A2,A3,A4,A5>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5);
	COLc5m0():_pF(0),_pObj(0){}
	COLc5m0(T* pObj,F pF):_pObj(pObj),_pF(pF){}
	~COLc5m0(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5) const {(_pObj->*_pF)(a1,a2,a3,a4,a5);}
private:
	T* _pObj;
	F _pF;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5>
inline COLc5m0<T,A1,A2,A3,A4,A5>* COLnewClosure5(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5)){return new COLc5m0<T,A1,A2,A3,A4,A5>(pObj,fn);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5>
inline COLc5m0<T,A1,A2,A3,A4,A5>* COLnewClosure5(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5) const){return new COLc5m0<T,A1,A2,A3,A4,A5>(pObj,(void (T::*)(A1,A2,A3,A4,A5))fn);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename C1> class COLc5m1 : public COLclosure5<A1,A2,A3,A4,A5>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,C1 c1);
	COLc5m1():_pF(0),_pObj(0){}
	COLc5m1(T* pObj,F pF,C1 c1):_pObj(pObj),_pF(pF),_c1(c1){}
	~COLc5m1(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5) const {(_pObj->*_pF)(a1,a2,a3,a4,a5,_c1);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename C1>
inline COLc5m1<T,A1,A2,A3,A4,A5,C1>* COLnewClosure5(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,C1),C1 c1){return new COLc5m1<T,A1,A2,A3,A4,A5,C1>(pObj,fn,c1);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename C1>
inline COLc5m1<T,A1,A2,A3,A4,A5,C1>* COLnewClosure5(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,C1) const,C1 c1){return new COLc5m1<T,A1,A2,A3,A4,A5,C1>(pObj,(void (T::*)(A1,A2,A3,A4,A5,C1))fn,c1);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2> class COLc5m2 : public COLclosure5<A1,A2,A3,A4,A5>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,C1 c1,C2 c2);
	COLc5m2():_pF(0),_pObj(0){}
	COLc5m2(T* pObj,F pF,C1 c1,C2 c2):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2){}
	~COLc5m2(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5) const {(_pObj->*_pF)(a1,a2,a3,a4,a5,_c1,_c2);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2>
inline COLc5m2<T,A1,A2,A3,A4,A5,C1,C2>* COLnewClosure5(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,C1,C2),C1 c1,C2 c2){return new COLc5m2<T,A1,A2,A3,A4,A5,C1,C2>(pObj,fn,c1,c2);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2>
inline COLc5m2<T,A1,A2,A3,A4,A5,C1,C2>* COLnewClosure5(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,C1,C2) const,C1 c1,C2 c2){return new COLc5m2<T,A1,A2,A3,A4,A5,C1,C2>(pObj,(void (T::*)(A1,A2,A3,A4,A5,C1,C2))fn,c1,c2);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3> class COLc5m3 : public COLclosure5<A1,A2,A3,A4,A5>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,C1 c1,C2 c2,C3 c3);
	COLc5m3():_pF(0),_pObj(0){}
	COLc5m3(T* pObj,F pF,C1 c1,C2 c2,C3 c3):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3){}
	~COLc5m3(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5) const {(_pObj->*_pF)(a1,a2,a3,a4,a5,_c1,_c2,_c3);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3>
inline COLc5m3<T,A1,A2,A3,A4,A5,C1,C2,C3>* COLnewClosure5(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,C1,C2,C3),C1 c1,C2 c2,C3 c3){return new COLc5m3<T,A1,A2,A3,A4,A5,C1,C2,C3>(pObj,fn,c1,c2,c3);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3>
inline COLc5m3<T,A1,A2,A3,A4,A5,C1,C2,C3>* COLnewClosure5(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,C1,C2,C3) const,C1 c1,C2 c2,C3 c3){return new COLc5m3<T,A1,A2,A3,A4,A5,C1,C2,C3>(pObj,(void (T::*)(A1,A2,A3,A4,A5,C1,C2,C3))fn,c1,c2,c3);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3,typename C4> class COLc5m4 : public COLclosure5<A1,A2,A3,A4,A5>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,C1 c1,C2 c2,C3 c3,C4 c4);
	COLc5m4():_pF(0),_pObj(0){}
	COLc5m4(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4){}
	~COLc5m4(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5) const {(_pObj->*_pF)(a1,a2,a3,a4,a5,_c1,_c2,_c3,_c4);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3,typename C4>
inline COLc5m4<T,A1,A2,A3,A4,A5,C1,C2,C3,C4>* COLnewClosure5(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,C1,C2,C3,C4),C1 c1,C2 c2,C3 c3,C4 c4){return new COLc5m4<T,A1,A2,A3,A4,A5,C1,C2,C3,C4>(pObj,fn,c1,c2,c3,c4);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3,typename C4>
inline COLc5m4<T,A1,A2,A3,A4,A5,C1,C2,C3,C4>* COLnewClosure5(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,C1,C2,C3,C4) const,C1 c1,C2 c2,C3 c3,C4 c4){return new COLc5m4<T,A1,A2,A3,A4,A5,C1,C2,C3,C4>(pObj,(void (T::*)(A1,A2,A3,A4,A5,C1,C2,C3,C4))fn,c1,c2,c3,c4);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3,typename C4,typename C5> class COLc5m5 : public COLclosure5<A1,A2,A3,A4,A5>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5);
	COLc5m5():_pF(0),_pObj(0){}
	COLc5m5(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5){}
	~COLc5m5(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5) const {(_pObj->*_pF)(a1,a2,a3,a4,a5,_c1,_c2,_c3,_c4,_c5);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc5m5<T,A1,A2,A3,A4,A5,C1,C2,C3,C4,C5>* COLnewClosure5(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,C1,C2,C3,C4,C5),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc5m5<T,A1,A2,A3,A4,A5,C1,C2,C3,C4,C5>(pObj,fn,c1,c2,c3,c4,c5);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc5m5<T,A1,A2,A3,A4,A5,C1,C2,C3,C4,C5>* COLnewClosure5(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,C1,C2,C3,C4,C5) const,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc5m5<T,A1,A2,A3,A4,A5,C1,C2,C3,C4,C5>(pObj,(void (T::*)(A1,A2,A3,A4,A5,C1,C2,C3,C4,C5))fn,c1,c2,c3,c4,c5);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6> class COLc5m6 : public COLclosure5<A1,A2,A3,A4,A5>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6);
	COLc5m6():_pF(0),_pObj(0){}
	COLc5m6(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5),_c6(c6){}
	~COLc5m6(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5) const {(_pObj->*_pF)(a1,a2,a3,a4,a5,_c1,_c2,_c3,_c4,_c5,_c6);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
	C6 _c6;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc5m6<T,A1,A2,A3,A4,A5,C1,C2,C3,C4,C5,C6>* COLnewClosure5(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,C1,C2,C3,C4,C5,C6),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc5m6<T,A1,A2,A3,A4,A5,C1,C2,C3,C4,C5,C6>(pObj,fn,c1,c2,c3,c4,c5,c6);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc5m6<T,A1,A2,A3,A4,A5,C1,C2,C3,C4,C5,C6>* COLnewClosure5(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,C1,C2,C3,C4,C5,C6) const,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc5m6<T,A1,A2,A3,A4,A5,C1,C2,C3,C4,C5,C6>(pObj,(void (T::*)(A1,A2,A3,A4,A5,C1,C2,C3,C4,C5,C6))fn,c1,c2,c3,c4,c5,c6);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6> class COLc6m0 : public COLclosure6<A1,A2,A3,A4,A5,A6>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6);
	COLc6m0():_pF(0),_pObj(0){}
	COLc6m0(T* pObj,F pF):_pObj(pObj),_pF(pF){}
	~COLc6m0(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6) const {(_pObj->*_pF)(a1,a2,a3,a4,a5,a6);}
private:
	T* _pObj;
	F _pF;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6>
inline COLc6m0<T,A1,A2,A3,A4,A5,A6>* COLnewClosure6(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,A6)){return new COLc6m0<T,A1,A2,A3,A4,A5,A6>(pObj,fn);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6>
inline COLc6m0<T,A1,A2,A3,A4,A5,A6>* COLnewClosure6(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,A6) const){return new COLc6m0<T,A1,A2,A3,A4,A5,A6>(pObj,(void (T::*)(A1,A2,A3,A4,A5,A6))fn);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1> class COLc6m1 : public COLclosure6<A1,A2,A3,A4,A5,A6>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,C1 c1);
	COLc6m1():_pF(0),_pObj(0){}
	COLc6m1(T* pObj,F pF,C1 c1):_pObj(pObj),_pF(pF),_c1(c1){}
	~COLc6m1(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6) const {(_pObj->*_pF)(a1,a2,a3,a4,a5,a6,_c1);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1>
inline COLc6m1<T,A1,A2,A3,A4,A5,A6,C1>* COLnewClosure6(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,A6,C1),C1 c1){return new COLc6m1<T,A1,A2,A3,A4,A5,A6,C1>(pObj,fn,c1);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1>
inline COLc6m1<T,A1,A2,A3,A4,A5,A6,C1>* COLnewClosure6(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,A6,C1) const,C1 c1){return new COLc6m1<T,A1,A2,A3,A4,A5,A6,C1>(pObj,(void (T::*)(A1,A2,A3,A4,A5,A6,C1))fn,c1);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2> class COLc6m2 : public COLclosure6<A1,A2,A3,A4,A5,A6>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,C1 c1,C2 c2);
	COLc6m2():_pF(0),_pObj(0){}
	COLc6m2(T* pObj,F pF,C1 c1,C2 c2):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2){}
	~COLc6m2(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6) const {(_pObj->*_pF)(a1,a2,a3,a4,a5,a6,_c1,_c2);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2>
inline COLc6m2<T,A1,A2,A3,A4,A5,A6,C1,C2>* COLnewClosure6(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,A6,C1,C2),C1 c1,C2 c2){return new COLc6m2<T,A1,A2,A3,A4,A5,A6,C1,C2>(pObj,fn,c1,c2);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2>
inline COLc6m2<T,A1,A2,A3,A4,A5,A6,C1,C2>* COLnewClosure6(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,A6,C1,C2) const,C1 c1,C2 c2){return new COLc6m2<T,A1,A2,A3,A4,A5,A6,C1,C2>(pObj,(void (T::*)(A1,A2,A3,A4,A5,A6,C1,C2))fn,c1,c2);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3> class COLc6m3 : public COLclosure6<A1,A2,A3,A4,A5,A6>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,C1 c1,C2 c2,C3 c3);
	COLc6m3():_pF(0),_pObj(0){}
	COLc6m3(T* pObj,F pF,C1 c1,C2 c2,C3 c3):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3){}
	~COLc6m3(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6) const {(_pObj->*_pF)(a1,a2,a3,a4,a5,a6,_c1,_c2,_c3);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3>
inline COLc6m3<T,A1,A2,A3,A4,A5,A6,C1,C2,C3>* COLnewClosure6(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,A6,C1,C2,C3),C1 c1,C2 c2,C3 c3){return new COLc6m3<T,A1,A2,A3,A4,A5,A6,C1,C2,C3>(pObj,fn,c1,c2,c3);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3>
inline COLc6m3<T,A1,A2,A3,A4,A5,A6,C1,C2,C3>* COLnewClosure6(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,A6,C1,C2,C3) const,C1 c1,C2 c2,C3 c3){return new COLc6m3<T,A1,A2,A3,A4,A5,A6,C1,C2,C3>(pObj,(void (T::*)(A1,A2,A3,A4,A5,A6,C1,C2,C3))fn,c1,c2,c3);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3,typename C4> class COLc6m4 : public COLclosure6<A1,A2,A3,A4,A5,A6>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,C1 c1,C2 c2,C3 c3,C4 c4);
	COLc6m4():_pF(0),_pObj(0){}
	COLc6m4(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4){}
	~COLc6m4(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6) const {(_pObj->*_pF)(a1,a2,a3,a4,a5,a6,_c1,_c2,_c3,_c4);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3,typename C4>
inline COLc6m4<T,A1,A2,A3,A4,A5,A6,C1,C2,C3,C4>* COLnewClosure6(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,A6,C1,C2,C3,C4),C1 c1,C2 c2,C3 c3,C4 c4){return new COLc6m4<T,A1,A2,A3,A4,A5,A6,C1,C2,C3,C4>(pObj,fn,c1,c2,c3,c4);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3,typename C4>
inline COLc6m4<T,A1,A2,A3,A4,A5,A6,C1,C2,C3,C4>* COLnewClosure6(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,A6,C1,C2,C3,C4) const,C1 c1,C2 c2,C3 c3,C4 c4){return new COLc6m4<T,A1,A2,A3,A4,A5,A6,C1,C2,C3,C4>(pObj,(void (T::*)(A1,A2,A3,A4,A5,A6,C1,C2,C3,C4))fn,c1,c2,c3,c4);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3,typename C4,typename C5> class COLc6m5 : public COLclosure6<A1,A2,A3,A4,A5,A6>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5);
	COLc6m5():_pF(0),_pObj(0){}
	COLc6m5(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5){}
	~COLc6m5(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6) const {(_pObj->*_pF)(a1,a2,a3,a4,a5,a6,_c1,_c2,_c3,_c4,_c5);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc6m5<T,A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5>* COLnewClosure6(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc6m5<T,A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5>(pObj,fn,c1,c2,c3,c4,c5);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3,typename C4,typename C5>
inline COLc6m5<T,A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5>* COLnewClosure6(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5) const,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5){return new COLc6m5<T,A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5>(pObj,(void (T::*)(A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5))fn,c1,c2,c3,c4,c5);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6> class COLc6m6 : public COLclosure6<A1,A2,A3,A4,A5,A6>{
public:
	typedef void (T::*F)(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6);
	COLc6m6():_pF(0),_pObj(0){}
	COLc6m6(T* pObj,F pF,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6):_pObj(pObj),_pF(pF),_c1(c1),_c2(c2),_c3(c3),_c4(c4),_c5(c5),_c6(c6){}
	~COLc6m6(){}
	void run(A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6) const {(_pObj->*_pF)(a1,a2,a3,a4,a5,a6,_c1,_c2,_c3,_c4,_c5,_c6);}
private:
	T* _pObj;
	F _pF;
	C1 _c1;
	C2 _c2;
	C3 _c3;
	C4 _c4;
	C5 _c5;
	C6 _c6;
};

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc6m6<T,A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5,C6>* COLnewClosure6(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5,C6),C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc6m6<T,A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5,C6>(pObj,fn,c1,c2,c3,c4,c5,c6);}

template <class T,typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename C1,typename C2,typename C3,typename C4,typename C5,typename C6>
inline COLc6m6<T,A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5,C6>* COLnewClosure6(T* pObj, void (T::*fn)(A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5,C6) const,C1 c1,C2 c2,C3 c3,C4 c4,C5 c5,C6 c6){return new COLc6m6<T,A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5,C6>(pObj,(void (T::*)(A1,A2,A3,A4,A5,A6,C1,C2,C3,C4,C5,C6))fn,c1,c2,c3,c4,c5,c6);}

/*function ArgList(ArgCount, CapCount)
   local R = ''
   for i=1, ArgCount do
      R=R..'A'..i..' '..'a'..i
      if i<ArgCount then R=R..',' end
   end
   if (ArgCount > 0 and CapCount > 0) then R=R..',' end
   for i=1, CapCount do
      R=R..'C'..i..' '..'c'..i
      if i<CapCount then R=R..',' end
   end
   
   return R
end

function CallList(ArgCount, CapCount)
   local R = ''
   for i=1, ArgCount do
      R=R..'a'..i
      if i<ArgCount then R=R..',' end
   end
   if (ArgCount > 0 and CapCount > 0) then R=R..',' end
   for i=1, CapCount do
      R=R..'c'..i
      if i<CapCount then R=R..',' end
   end
   return R
end

function RunList(ArgCount, CapCount)
   local R = ''
   for i=1, ArgCount do
      R=R..'a'..i
      if i<ArgCount then R=R..',' end
   end
   if (ArgCount > 0 and CapCount > 0) then R=R..',' end
   for i=1, CapCount do
      R=R..'_c'..i
      if i<CapCount then R=R..',' end
   end
   return R
end

function TemplateList(ArgCount,CapCount)
   local R = ''
   for i=1, ArgCount do
      R=R..'A'..i
      if i<ArgCount then R=R..',' end
   end
   if (ArgCount > 0 and CapCount > 0) then R=R..',' end
   for i=1, CapCount do
      R=R..'C'..i
      if i<CapCount then R=R..',' end
   end

   return R   
end

function FunctionCapGen(Arg, CapCount)
   local R = ''
   local Alist = TemplateList(Arg,0)
   for i=0, CapCount do
      local ClassName = "COLc"..Arg..'f'..i
      local TempExpress = TemplateExpression(Arg,i)
      R=R..TempExpress.." class "..ClassName..':public COLclosure'..Arg
      if (Arg > 0) then
         R=R..'<'..Alist..'>'
      end
      R=R..'{\n'  
      R=R..'public:\n'
      R=R..'\ttypedef void (*F)('..ArgList(Arg, i)..');\n'
      R=R..'\t'..ClassName..'():_pF(0){}\n'
      local CList = ArgList(0, i)
      local ExtraArgs = ''
      if (i > 0) then ExtraArgs = ','..CList end
      local ArgInit =''
      for j =1, i do
         ArgInit=ArgInit..',_c'..j..'(c'..j..')'
      end
      R=R..'\t'..ClassName..'(F pF'..ExtraArgs..'):_pF(pF)'..ArgInit..'{}\n'
      R=R..'\t~'..ClassName..'(){}\n'
      R=R..'\tvoid run('..ArgList(Arg,0)..') const {(*_pF)('..RunList(Arg,i)..');}\n';
      R=R..'private:\n'
      R=R..'\tF _pF;\n'
      for j=1, i do
         R=R..'\tC'..j..' _c'..j..';\n'
      end
      R=R..'};\n\n'
      local TList = TemplateList(Arg,i)
      local TListD =''
      if #TList > 0 then TListD = '<'..TList..'>' end
      local DecClist = ArgList(0,i)
      if #DecClist > 0 then DecClist=','..DecClist end
      local Clist = CallList(0,i)
      if #Clist > 0 then Clist = ','..Clist end
      R=R..TempExpress..'\ninline '..ClassName..TListD..'* COLnewClosure'..Arg..
        '(void (*fn)('..TList..')'..DecClist..'){return new '..ClassName..TListD..'(fn'..Clist..');}\n\n'
   end
   return R
end

function MethodCapGen(Arg, CapCount)
   local R = ''
   local Alist = TemplateList(Arg,0)
   for i=0, CapCount do
      local ClassName = "COLc"..Arg..'m'..i
      local TempExpress = TemplateExpression(Arg,i, true)
      R=R..TempExpress.." class "..ClassName..' : public COLclosure'..Arg
      if (Arg > 0) then
         R=R..'<'..Alist..'>'
      end
      R=R..'{\n'  
      R=R..'public:\n'
      R=R..'\ttypedef void (T::*F)('..ArgList(Arg, i)..');\n'
      R=R..'\t'..ClassName..'():_pF(0),_pObj(0){}\n'
      local CList = ArgList(0, i)
      local ExtraArgs = ''
      if (i > 0) then ExtraArgs = ','..CList end
      local ArgInit =''
      for j =1, i do
         ArgInit=ArgInit..',_c'..j..'(c'..j..')'
      end
      R=R..'\t'..ClassName..'(T* pObj,F pF'..ExtraArgs..'):_pObj(pObj),_pF(pF)'..ArgInit..'{}\n'
      R=R..'\t~'..ClassName..'(){}\n'
      R=R..'\tvoid run('..ArgList(Arg,0)..') const {(_pObj->*_pF)('..RunList(Arg,i)..');}\n';
      R=R..'private:\n'
      R=R..'\tT* _pObj;\n'
      R=R..'\tF _pF;\n'
      for j=1, i do
         R=R..'\tC'..j..' _c'..j..';\n'
      end
      R=R..'};\n\n'
      local TList = TemplateList(Arg,i)
      local TListD ='<T>'
      if #TList > 0 then TListD = '<T,'..TList..'>' end
      local DecClist = ArgList(0,i)
      if #DecClist > 0 then DecClist=','..DecClist end
      local Clist = CallList(0,i)
      if #Clist > 0 then Clist = ','..Clist end
      R=R..TempExpress..'\ninline '..ClassName..TListD..'* COLnewClosure'..Arg..
        '(T* pObj, void (T::*fn)('..TList..')'..DecClist..'){return new '..ClassName..TListD..'(pObj,fn'..Clist..');}\n\n'
      R=R..TempExpress..'\ninline '..ClassName..TListD..'* COLnewClosure'..Arg..
        '(T* pObj, void (T::*fn)('..TList..') const'..DecClist..'){return new '..ClassName..TListD..'(pObj,(void (T::*)('..TList..'))fn'..Clist..');}\n\n'
   end
   return R
end

function TemplateExpression(ArgCount, CapCount, Class)
   local R=''
   if (ArgCount+CapCount > 0 or Class) then 
      R=R..'template <'
      if (Class) then 
         R = R..'class T'
         if (ArgCount + CapCount > 0) then R=R..',' end
      end
      for j=1,ArgCount do
         R=R..'typename A'..j
         if j<ArgCount then R=R..',' end
      end
      if (ArgCount > 0 and CapCount >0) then R=R..',' end
      for j=1,CapCount do
         R=R..'typename C'..j
         if j<CapCount then R=R..',' end
      end
      R=R..'>'
   end
   return R;
end


function ClosureGenerator(ArgCount, CapCount)   
   local R = ''
   for i=0, ArgCount do
      local Args = ArgList(i,0)
      local Alist = CallList(i,0)
      R=R..TemplateExpression(i,0)..'\nclass COLclosure' .. i .."{\n" 
      R=R..'public:\n'
      R=R..'\tCOLclosure'..i..'(){}\n'
      R=R..'\tvirtual ~COLclosure'..i..'(){}\n'
      R=R..'\tvirtual void run('..Args..') const=0;\n'
      R=R..'\tvoid runAndDelete('..Args..'){run('..Alist..');delete this;}\n'
      R=R..'private:\n'
      R=R..'\tCOLclosure'..i..'(const COLclosure'..i..'& Orig);\n'
      R=R..'\tCOLclosure'..i..'& operator=(const COLclosure'..i..'& Orig);\n'
      R=R..'};\n\n'
   end
   for i=0,ArgCount do
      R=R..FunctionCapGen(i, CapCount)
   end
   for i=0,ArgCount do
      R=R..MethodCapGen(i, CapCount)
   end
  
   return R
end

Header=[[#ifndef __COL_CLOSURE_H__
#define __COL_CLOSURE_H__
//-----------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  COLclosure
//
// Author:  Eliot Muir
//
// Date:    Fri  9 Jan 2015 08:52:35 EST
//
// Description:
//
// A closure takes arguments and it has the ability to capture additional
// variables.  This is an example of the intended usage:
//
// void ServeWebRequest(COLclosure2<int, int>* pClosure){
//    pClosure->run(2,3);
// }
//
// void MakeWebRequest(int ConnectionId, int SequenceId, int* pCount); 
//
// ServeWebRequest(COLnewClosure2<int, int>(&MakeWebRequest, &Count));
// See http://fogbugz.ifware.dynip.com/default.asp?W5842
// WARNING - this code is generated from a Lua script.  Scroll to the end.
//-----------------------------------------------------------------------------
]]

Footer=[[

#endif // end of defensive include
]]


function main(Data)
   local SelfFName = _G.component.dir()
   local S = io.open(SelfFName.."dev/main.lua", "r");
   local Script = S:read('*a')
   S:close()
   local G = ClosureGenerator(6,6)
   local C = Header..G..'/'..'*'..Script..'*'..'/'..Footer
   trace(C)
   local F = io.open('/Users/eliotmuir/main2/COL/COLclosureGen.h', 'w')
   F:write(C)
   
end*/
#endif // end of defensive include
