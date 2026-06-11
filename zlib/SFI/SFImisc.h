#ifndef __SFI_MISC_H__
#define __SFI_MISC_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFImisc
//
// Description:
//
// Miscellaneous utility functions used by various SFI modules.
//
// Copied from CRYmisc.h.
//
// Date: November 29, 2004 
//
//
//---------------------------------------------------------------------------
#include <COL/COLerror.h>
#include <COL/COLminimumInclude.h>

template <class T> inline T rotlFixed(T x, unsigned int y)
{
   COLPRECONDITION(y < sizeof(T)*8);
   return (x<<y) | (x>>(sizeof(T)*8-y));
}

inline COLuint32 byteReverse(COLuint32 value)
{
#ifdef PPC_INTRINSICS
   // PPC: load reverse indexed instruction
   return (word32)__lwbrx(&value,0);
#elif defined(FAST_ROTATE)
   // 5 instructions with rotate instruction, 9 without
   return (rotrFixed(value, 8U) & 0xff00ff00) | (rotlFixed(value, 8U) & 0x00ff00ff);
#else
   // 6 instructions with rotate instruction, 8 without
   value = ((value & 0xFF00FF00) >> 8) | ((value & 0x00FF00FF) << 8);
   return rotlFixed(value, 16U);
#endif
}

template <class T>
void byteReverse(T *out, const T *in, unsigned int byteCount)
{
   unsigned int count = (byteCount+sizeof(T)-1)/sizeof(T);
   for (unsigned int i=0; i<count; i++)
      out[i] = byteReverse(in[i]);
}

template <class T>
inline void GetUserKeyBigEndian
(
   T *out, 
   unsigned int outlen, 
   const COLuint8 *in, 
   unsigned int inlen
)
{
   const unsigned int U = sizeof(T);
   COLPRECONDITION(inlen <= outlen*U);
   memcpy(out, in, inlen);
   memset((COLuint8 *)out+inlen, 0, outlen*U-inlen);
#ifndef WORDS_BIGENDIAN
   byteReverse(out, out, inlen);
#endif
}

// Fetch 4 words from user's buffer into "a", "b", "c", "d" in BIG-endian order
template <class T>
inline void GetBlockBigEndian(const COLuint8 *block, T &a, T &b, T &c, T &d)
{
#ifdef WORDS_BIGENDIAN
   a = ((T *)block)[0];
   b = ((T *)block)[1];
   c = ((T *)block)[2];
   d = ((T *)block)[3];
#else
   a = byteReverse(((T *)block)[0]);
   b = byteReverse(((T *)block)[1]);
   c = byteReverse(((T *)block)[2]);
   d = byteReverse(((T *)block)[3]);
#endif
}

// Put 4 words back into user's buffer in BIG-endian order
template <class T>
inline void PutBlockBigEndian(COLuint8 *block, T a, T b, T c, T d)
{
#ifdef WORDS_BIGENDIAN
   ((T *)block)[0] = a;
   ((T *)block)[1] = b;
   ((T *)block)[2] = c;
   ((T *)block)[3] = d;
#else
   ((T *)block)[0] = byteReverse(a);
   ((T *)block)[1] = byteReverse(b);
   ((T *)block)[2] = byteReverse(c);
   ((T *)block)[3] = byteReverse(d);
#endif
}

#define SecAlloc(type, number) (new type[(number)])
#define SecFree(ptr, number) (delete [] (ptr))

//! a block of memory allocated using SecAlloc
template <class T> struct SecBlock
{
   explicit SecBlock(unsigned int size=0)
      : size(size) {ptr = SecAlloc(T, size);}
   SecBlock(const SecBlock<T> &t)
      : size(t.size) {ptr = SecAlloc(T, size); memcpy(ptr, t.ptr, size*sizeof(T));}
   SecBlock(const T *t, unsigned int len)
      : size(len) {ptr = SecAlloc(T, len); memcpy(ptr, t, len*sizeof(T));}
   ~SecBlock()
      {SecFree(ptr, size);}

#if defined(__GNUC__) || defined(__BCPLUSPLUS__)
   operator const void *() const
      {return ptr;}
   operator void *()
      {return ptr;}
#endif
#if defined(__GNUC__)   // reduce warnings
   operator const void *()
      {return ptr;}
#endif

   operator const T *() const
      {return ptr;}
   operator T *()
      {return ptr;}
#if defined(__GNUC__)   // reduce warnings
   operator const T *()
      {return ptr;}
#endif

// CodeWarrior defines _MSC_VER
#if !defined(_MSC_VER) || defined(__MWERKS__)
   template <typename I>
   T *operator +(I offset)
      {return ptr+offset;}

   template <typename I>
   const T *operator +(I offset) const
      {return ptr+offset;}

   template <typename I>
   T& operator[](I index)
      {assert(index<size); return ptr[index];}

   template <typename I>
   const T& operator[](I index) const
      {assert(index<size); return ptr[index];}
#endif

   const T* Begin() const
      {return ptr;}
   T* Begin()
      {return ptr;}
   const T* End() const
      {return ptr+size;}
   T* End()
      {return ptr+size;}

   unsigned int Size() const {return size;}

   void Assign(const T *t, unsigned int len)
   {
      New(len);
      memcpy(ptr, t, len*sizeof(T));
   }

   void Assign(const SecBlock<T> &t)
   {
      New(t.size);
      memcpy(ptr, t.ptr, size*sizeof(T));
   }

   SecBlock& operator=(const SecBlock<T> &t)
   {
      Assign(t);
      return *this;
   }

   bool operator==(const SecBlock<T> &t) const
   {
      return size == t.size && memcmp(ptr, t.ptr, size*sizeof(T)) == 0;
   }

   bool operator!=(const SecBlock<T> &t) const
   {
      return !operator==(t);
   }

   void New(unsigned int newSize)
   {
      if (newSize != size)
      {
         T *newPtr = SecAlloc(T, newSize);
         SecFree(ptr, size);
         ptr = newPtr;
         size = newSize;
      }
   }

   void CleanNew(unsigned int newSize)
   {
      if (newSize != size)
      {
         T *newPtr = SecAlloc(T, newSize);
         SecFree(ptr, size);
         ptr = newPtr;
         size = newSize;
      }
      memset(ptr, 0, size*sizeof(T));
   }

   void Grow(unsigned int newSize)
   {
      if (newSize > size)
      {
         T *newPtr = SecAlloc(T, newSize);
         memcpy(newPtr, ptr, size*sizeof(T));
         SecFree(ptr, size);
         ptr = newPtr;
         size = newSize;
      }
   }

   void CleanGrow(unsigned int newSize)
   {
      if (newSize > size)
      {
         T *newPtr = SecAlloc(T, newSize);
         memcpy(newPtr, ptr, size*sizeof(T));
         memset(newPtr+size, 0, (newSize-size)*sizeof(T));
         SecFree(ptr, size);
         ptr = newPtr;
         size = newSize;
      }
   }

   template <class _Tp>
   inline const _Tp& STDMIN(const _Tp& __a, const _Tp& __b) {
     return __b < __a ? __b : __a;
   }

   void Resize(unsigned int newSize)
   {
      if (newSize != size)
      {
         T *newPtr = SecAlloc(T, newSize);
         memcpy(newPtr, ptr, STDMIN(newSize, size)*sizeof(T));
         SecFree(ptr, size);
         ptr = newPtr;
         size = newSize;
      }
   }

   void swap(SecBlock<T> &b);

   unsigned int size;
   T *ptr;
};

#endif
