//-----------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  COLthreadPool
//
// Author:  Andrew Vajoczki
//
// Description
//
// Dynamic thread pool to schedule COLclosure0* instances.
//-----------------------------------------------------------------------------
#include "COLthreadPool.h"
#include "COLlocker.h"
#include "COLerror.h"
#include "COLclosure.h"
#include "COLlog.h"
COL_LOG_MODULE;

// All scheduled tasks are wrapped and executed in the controller thread.
// Once in the controller thread this wrapper's run method then
// schedules execution of the task on a worker thread.
class COLdeferredClosure : public COLclosure0 {
public:
   COLdeferredClosure(COLthreadPool& ThreadPool, COLclosure0* pClosure) : m_ThreadPool(ThreadPool) {
      m_pClosure = pClosure;
   }
   virtual ~COLdeferredClosure(){
      delete m_pClosure;
   }
   void run() const {
      // Note: it is perfectly valid to push a null _runnable here. A null _runnable instructs a worker thread to quit.
      m_ThreadPool._taskQueue.put(m_pClosure);
      m_pClosure = NULL;  // consumed by _taskQueue, set to NULL
   }
   COLdeferredClosure* clone() const{
      return 0; // not supported
   }
private:
   COLdeferredClosure(const COLdeferredClosure& Orig);
   COLdeferredClosure& operator=(const COLdeferredClosure& Orig);
   COLthreadPool& m_ThreadPool;
   mutable COLclosure0* m_pClosure;  // TODO - ELIOT - kind of ugly hack.  I don't really think inheriting from COLclosure0 was a good design decision - looking at yours truly?
};

COLthreadPool::COLthreadPool(int minThreads, int maxThreads){
   COL_METHOD(COLthreadPool::COLthreadPool);
   COL_VAR2(minThreads, maxThreads);
   _wasStarted = 0;
   _immediateShutdownRequested = 0;
   _numThreadsAvailable = 0;
   _minThreads = minThreads;
   _maxThreads = maxThreads;
   _threadWhereCreated = COLthreadGetCurrentThreadID();
   if (_minThreads <= 0)
      _minThreads = 1;
   if (_maxThreads < _minThreads)
      _maxThreads = _minThreads;
   COL_VAR2(_minThreads, _maxThreads);
}

COLthreadPool::~COLthreadPool(){
   COL_METHOD(COLthreadPool::~COLthreadPool);
   COLASSERT(_threadWhereCreated == COLthreadGetCurrentThreadID());
   shutdown(COLthreadPool::shutdown_after_pending_tasks);
   if (_wasStarted) {
      COL_TRC("joining with controller thread");
      _controllerThread.join();
      COL_TRC("joined with controller thread");
   }
   COL_TRC("deleting pending tasks");
   // all threads are finished reading the task queue at this point so we may clean up any possible pending tasks.
   while (!_taskQueue.empty())
      delete _taskQueue.take();
   while (!_controllerThreadQueue.empty())
      delete _controllerThreadQueue.take();
}

void COLthreadPool::scheduleTask(COLclosure0* pTask){
   if (!_immediateShutdownRequested){
      _controllerThreadQueue.put(new COLdeferredClosure(*this, pTask));
   } else {
      delete pTask;
   }
}

void COLthreadPool::shutdown(COLthreadPool::ShutdownType shutdownType){
   COL_METHOD(COLthreadPool::shutdown);
   COL_VAR(shutdownType);
   if (shutdownType == shutdown_immediate) {
      COL_TRC("immediate shutdown requested");
      _immediateShutdownRequested = 1;
   } else {
      COL_TRC("shutdown when tasks completed requested");
   }
   // Instruct threads to quit. The order of events here is significant.
   _controllerThreadQueue.put(new COLdeferredClosure(*this, 0));
   _controllerThreadQueue.put(0);
}

// This function is only called (directly or indirectly)
// from within the COLthreadPool::_controllerThreadRunner thread.
void COLthreadPool::_addThreads(int numberOfNewThreads){
   COL_METHOD(COLthreadPool::_addThreads);
   COL_VAR(numberOfNewThreads);
   for (int i = 0; i < numberOfNewThreads; i++) {
      if (_immediateShutdownRequested) {
         COL_INF("_addThreads " << (i+1) << " of " << numberOfNewThreads
            << " aborted due to shutdown request");
         return;
      }
      COLthread* pThread = new COLthread();
      pThread->start(&COLthreadPool::_workerThreadRunner, this);
      _threadJoinQueue.put(pThread);
   }
   COL_TRC("added " << numberOfNewThreads << " worker thread(s) for a total of " << getTotalNumberOfThreads());
}

void COLthreadPool::addThreads(int numNewThreads){
   COL_METHOD(COLthreadPool::addThreads);
   // schedule the threads create operation to take place on the 
   // controller thread (COLthreadPool::_controllerThreadRunner)
   if (!_immediateShutdownRequested) {
      _controllerThreadQueue.put(COLnewClosure0(this, &COLthreadPool::_addThreads, numNewThreads));
   }
}

int COLthreadPool::getTotalNumberOfThreads() const{    // this is only a snapshot!
   return _threadJoinQueue.size();
}

int COLthreadPool::getPendingTaskCount() const{    // this is only a snapshot!
   return _taskQueue.size();
}

// static
void* COLthreadPool::_controllerThreadRunner(void* arg){
   COL_FUNCTION(COLthreadPool::_controllerThreadRunner);
   COLthreadPool* This = (COLthreadPool*)arg;
   COLASSERT(This);
   COL_VAR(This);
   This->_addThreads(This->_minThreads);
   COL_TRC("before thread creation loop...");
   for (;;) {
      // Note: this loop MUST NOT break upon _immediateShutdownRequested or it will not shut down its worker threads properly.
      // block until we have a task to run
      COLclosure0* task = This->_controllerThreadQueue.take();
      // if the task is null we exit the loop
      if (task == 0) break;
      // execute the task and delete it
      task->run();
      delete task;
      // add another worker thread if the number of pending tasks increase
      if (This->getTotalNumberOfThreads() < This->_maxThreads) {
         int numPendingTasks = This->getPendingTaskCount();
         int numAvailableWorkers = This->getAvailableWorkerThreadCount();
         if (numPendingTasks > numAvailableWorkers) {
            This->_addThreads(1);
         }
      }
   }
   COL_TRC("after thread creation loop...");
   while (!This->_threadJoinQueue.empty()) {
      COL_TRC("waiting for " << This->_threadJoinQueue.size()
         << " threads to be joined");
      COLthread* thr = This->_threadJoinQueue.take();
      thr->join();
      delete thr;
   }
   return 0;
}

void COLthreadPool::start(){
   COL_METHOD(COLthreadPool::start);
   COLASSERT(_threadWhereCreated == COLthreadGetCurrentThreadID());
   if (!_immediateShutdownRequested) {
      if (!_wasStarted) {
         _controllerThread.start(&COLthreadPool::_controllerThreadRunner, this);
         _wasStarted = 1;
      } else {
         COL_WRN("start() was already called - ignored.");
      }
   }
}

void COLthreadPool::increaseAvailableWorkerThreadCount(int n){
   COL_METHOD(COLthreadPool::increaseAvailableWorkerThreadCount);
   COL_VAR(n);
   int available = 0;
   {
      COLlocker guard(_mutex);
      COLASSERT(_numThreadsAvailable >= 0);
      available = (_numThreadsAvailable += n);
   }
   COL_VAR(available);
}

void COLthreadPool::decreaseAvailableWorkerThreadCount(int n){
   COL_METHOD(COLthreadPool::decreaseAvailableWorkerThreadCount);
   COL_VAR(n);
   int available = 0;
   {
      COLlocker guard(_mutex);
      available = (_numThreadsAvailable -= n);
      COLASSERT(_numThreadsAvailable >= 0);
   }
   COL_VAR(available);
}

int COLthreadPool::getAvailableWorkerThreadCount() const{
   COLlocker guard(const_cast<COLmutex&>(_mutex));
   COLASSERT(_numThreadsAvailable >= 0);
   return _numThreadsAvailable;
}

// static
void* COLthreadPool::_workerThreadRunner(void* arg){
   COL_FUNCTION(COLthreadPool::_workerThreadRunner);
   COLthreadPool* This = (COLthreadPool*)arg;
   COL_VAR(This);
   COLASSERT(This);
   // this worker thread is open for business
   This->increaseAvailableWorkerThreadCount();
   // Loop until immediate shutdown is requested or a null task is read from the task queue.
   // An immediate shutdown may leave tasks in queue unexecuted.
   while (!This->_immediateShutdownRequested){
      // block until we have a task to run
      COLclosure0* task = This->_taskQueue.take();
      // if the task is null we exit the loop
      if (task == 0) break;
      // just prior to running the task we decrease the available count
      This->decreaseAvailableWorkerThreadCount();
      // execute the task and delete it
      try {
         task->run();
      } catch(const COLerror& Error){
         COLabortWithMessage(COLstring("COLerror thrown during thread:") + Error.description() + " Code: " + COLintToString(Error.code()));
      } catch (...) {
         COLabortWithMessage("Unhandled exception during thread operation.");
      }
      delete task;
      // just after running the task we increase the available count
      This->increaseAvailableWorkerThreadCount();
   }
   // push a null task for another _workerThreadRunner to quit
   This->_taskQueue.put(0);
   // this thread is over, subtract one from available count
   This->decreaseAvailableWorkerThreadCount();
   return 0;
}

