#ifndef _ASYNC_JOB_QUEUE_
#define _ASYNC_JOB_QUEUE_

#define ASYNC_JOB_ADD		   20901
#define ASYNC_JOB_PROCESSED    20902
#define ASYNC_JOB_DONE		   20903
#define ASYNC_JOB_REMOVE	   20904

#define ASYNC_JOB_MAX_JOBS	1000

#include "AsyncJob.h"

CString GetCwd();
DWORD WINAPI DoAsyncJob(LPVOID lpParams);


class AsyncJobQueue
{
private:
	AsyncJob *first,*last;
	int max_id;
	int max_jobs;

public:
	HANDLE h_WakeUpEvent,h_TerminateEvent;
	HANDLE wait_events[2];
	HWND m_hEventReceiver;
	HANDLE m_hAsyncJobThread;
	CRITICAL_SECTION m_csAsync;

	AsyncJobQueue(HWND hEventReceiver);
	~AsyncJobQueue();

	//boss methods
	int Add(AsyncJob* new_job);
	AsyncJob* Remove(int id);

	//worker methods
	AsyncJob* GetJob();
	void Done(int id,CString result_data);

	void Internal(AsyncJob* job);
};

#endif //_ASYNC_JOB_QUEUE_
