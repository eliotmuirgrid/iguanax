#ifndef __COL_THREAD_QUEUE_H__
#define __COL_THREAD_QUEUE_H__
//-----------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  COLthreadQueue
//
// Author:  Andrew Vajoczki
//
// Description
//
// A threadsafe producer/consumer queue (FIFO).
//
// Suitable for use with one or more producers and one or more consumers
// in different threads.
//
// Note: if a pointer type is used with this class, it will NOT
// delete the entries upon destruction of this instance.
//
// COLthreadsafeProducerConsumerQueue might have been a more descriptive
// name for this class, but it's too long.
//-----------------------------------------------------------------------------

#include <COL/COLqueue.h>
#include <COL/COLthread.h>
#include <COL/COLlocker.h>

template <class Type> class COLthreadQueue{
public:
   COLthreadQueue() {}
   ~COLthreadQueue() {}

   // Put an item at the end the queue without blocking.
   // Typically, producer(s) will pass this function a pointer to
   // a heap allocated object which the consumer is expected to delete
   // after take().
   void put(Type v) {
      COLlocker guard(mutex_);
      queue_.push(v);
      condition_.signal();
   }

   // Grab an item from the queue.
   // Block until an element is available and then return the element.
   // Typically, the consumer will process the element and delete it
   // when done.
   Type take() {
      COLlocker guard(mutex_);
      for (;;) {
         if (!queue_.empty()) {
            Type popped = queue_.front();
            queue_.pop(); // remove item from front of queue
            return popped;
         }
         condition_.wait(&mutex_);
      }
   }

   // Get the number of elements pending the queue.
   // Note: just a snapshot at a point in time for observational purposes.
   int size() const {
      COLthreadQueue* This = const_cast<COLthreadQueue*>(this);
      COLlocker guard(This->mutex_);
      int len = This->queue_.size();
      return len;
   }

   // Is the queue empty?
   // Note: just a snapshot at a point in time for observational purposes.
   bool empty() const { return size() == 0; }

private:
   COLthreadQueue(const COLthreadQueue&);             // disallow
   COLthreadQueue& operator=(const COLthreadQueue&);  // disallow

private:
   COLqueue<Type> queue_;
   COLmutex       mutex_;
   COLcondition   condition_;
};

#endif // end of defensive include

