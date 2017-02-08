#include "stdafx.h"
#include "AsyncJob.h"

AsyncJob::AsyncJob()
{
	id=0;
	next=NULL;
	job_type=GET_PAGE;
	status=JOB_QUEUED;
	callback_function=NULL;
}

AsyncJob::~AsyncJob()
{
}