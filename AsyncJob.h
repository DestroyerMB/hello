#ifndef _ASYNC_JOB_
#define _ASYNC_JOB_

#include <vector>

//job types
#define GET_PAGE        0
#define PROCESS_CATALOG 1
#define PROCESS_DETAILS 2

//states
#define JOB_QUEUED    0
#define JOB_PROCESSED 1
#define JOB_DONE      2

class AsyncJob;

typedef void (*callback)(AsyncJob* job);

/*-------------------*/
/*   shifts format   */
/*-------------------*/
// number:         just +num to start position
// !string:        after find start phrase we also find this string
// %catalog_page:  macros to shift for page number (i.e. +4 if catalog page is 4)
// !%catalog_page: macros to find catalog page number after phrase

class AsyncJob
{	
public:
	int id,job_type,status;
	CString name,address,data,result_data,user,password;
	callback callback_function;
	CString details_link_start,details_link_stop,next_page_start,next_page_stop;
	CString details_shift,next_page_shift;
	//properties in details page
	std::vector<CString> names,starts,stops;
	std::vector<CString> shifts;
	AsyncJob* next;

	AsyncJob();
	~AsyncJob();
};

#endif //_ASYNC_JOB_