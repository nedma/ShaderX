//------------------------------------------------------------------------
// I rewrote these classes from those made by David Poon (6 May 2001) to
// be included into Dimension3 Rendering framework. Also a little bit of 
// enhancements is done
// Originals are copyright (C)2001 David Poon and are public under GNU GPL
//------------------------------------------------------------------------
#pragma once

#include "baseclass.h"

///The critical section class
class CThreadCriticalSection : public CRootClass {
private:
	CRITICAL_SECTION CritSection;
public:
	CThreadCriticalSection();
	virtual ~CThreadCriticalSection();

    ///Enter critical section - wait until it is free free
	void Enter();
    ///Leave critical section
	void Leave();
    ///Try to enter critical section - return false if not inside
    bool TryEnter();
};

/// A CThreadLock provides a safe way to acquire and release a CThreadCriticalSection. 
/// The CThreadCriticalSection is acquired when the CThreadLock it created.
/// The CThreadCriticalSection is released when the CThreadLock goes out of scope.
class CThreadLock : public CRootClass {
public:
	CThreadLock(CThreadCriticalSection &Section);
	virtual ~CThreadLock();
private:
	CThreadCriticalSection *Section;
};

class CThread : public CBaseClass {
private:
	static friend DWORD WINAPI ThreadFunction(LPVOID args);
	
	HANDLE                      Thread;
	DWORD                       ThreadID;
	volatile bool               ShutdownRequest;
	volatile bool               Suspended;
	CThreadCriticalSection     *ThreadSection;
protected:
	bool GetShutdownRequest();
	virtual void Run() = 0;
public:
	CThread(char *ObjName, CEngine *OwnerEngine);
	virtual ~CThread();

	bool Create(DWORD ThreadStackSize);
	DWORD GetThreadId() const;
	void Start();
	void WaitTo();

	void Resume();
	void Suspend();
	void Shutdown();

    MAKE_CLSNAME("CThread");
};
