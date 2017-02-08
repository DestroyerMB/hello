#include "stdafx.h"
#include <afxinet.h>
#include "AsyncJobQueue.h"

const wchar_t *GetWC(const char *c)
{
    const size_t cSize = strlen(c)+1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs (wc, c, cSize);

    return wc;
}

CString GetCwd()
{
	TCHAR szPath[MAX_PATH];
	GetModuleFileName( NULL, szPath, MAX_PATH );
	CString path(szPath);
	int pos;
	for(int i=path.GetLength()-1;i>=0;i--)
	if(path[i]=='\\') 
	{
		pos=i;
		break;
	}
	path=path.Left(pos);
	
	return path;
}

size_t GetSizeOfFile(const CString& path)
{
    struct _stat fileinfo;
    _wstat(path, &fileinfo);
    return fileinfo.st_size;
}

CString LoadUtf8FileToString(const CString& filename)
{
    std::wstring buffer;            // stores file contents
    FILE* f = _wfopen(filename, L"rtS, ccs=UTF-8");

    // Failed to open file
    if (f == NULL)
    {
        // ...handle some error...
		return buffer.c_str();
    }

    size_t filesize = GetSizeOfFile(filename);

    // Read entire file contents in to memory
    if (filesize > 0)
    {
        buffer.resize(filesize);
        size_t wchars_read = fread(&(buffer.front()), sizeof(wchar_t), filesize, f);
        buffer.resize(wchars_read);
        buffer.shrink_to_fit();
    }

    fclose(f);

    return buffer.c_str();
}

int StartWithShift(CString data,int start_pos,CString shift,CString catalog_page_num)
{
	if(shift.GetLength()>0)
	{
		shift.Replace(_T("%catalog_page"),catalog_page_num);
		if(shift[0]=='!') //relative, find
		{
			shift=shift.Right(shift.GetLength()-1);
			int pos=data.Find(shift,start_pos);
			if(pos>=0)
				start_pos=pos+shift.GetLength();
		}
		else start_pos+=_wtoi(shift);
	}
	return start_pos;
}

CString FullAddress(CString address,CString mandatory_url_part)
{
	if(address.Find(mandatory_url_part)<0)
		address=mandatory_url_part+address;
	return address;
}

void GetPage(CString address,CString data,CString &result_data)
{	
	SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;       

	CString path=GetCwd();
	CString file_name=path+_T("\\")+data;
	//DeleteFile(file_name);
	result_data=file_name;

    HANDLE h = CreateFile(file_name,
        FILE_APPEND_DATA,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        &sa,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL );

    PROCESS_INFORMATION pi; 
    STARTUPINFO si;
    BOOL ret = FALSE; 
    DWORD flags = CREATE_NO_WINDOW;

    ZeroMemory( &pi, sizeof(PROCESS_INFORMATION) );
    ZeroMemory( &si, sizeof(STARTUPINFO) );
    si.cb = sizeof(STARTUPINFO); 
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput = NULL;
    si.hStdError = h;
    si.hStdOutput = h;

    path=path+_T("\\curl.exe");
	CString cmd_line=path+_T(" \"")+address+_T("\"");
    ret = CreateProcess(path, (LPWSTR)(LPCWSTR)cmd_line, NULL, NULL, TRUE, flags, NULL, NULL, &si, &pi);
	WaitForSingleObject( pi.hProcess, INFINITE );
	DWORD exit_code;
	GetExitCodeProcess(pi.hProcess, &exit_code);
	// Close process and thread handles. 
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	CloseHandle( h );
}

int ProcessCatalog(AsyncJobQueue* job_queue,CString input_file_name,CString link_start,CString link_start_shift,CString link_stop,
					CString next_page_start,CString next_page_shift,CString next_page_stop,CString mandatory_url_part)
{
	int return_code=0;
	CString data = LoadUtf8FileToString(input_file_name);

	CString page_num;
	for(int i=input_file_name.GetLength()-1;i>=0;i--)
	{
		if(input_file_name[i]=='\\') 
		{
			page_num=input_file_name.Right(input_file_name.GetLength()-i-1);
			break;
		}
	}
  
	//find next page
	int len=next_page_start.GetLength();
	int start_pos=data.Find(next_page_start);
	int stop_pos=data.Find(next_page_stop,start_pos+len);
	if(start_pos>=0 && stop_pos>=0 && stop_pos>start_pos)
	{
		//process shift
		start_pos=StartWithShift(data,start_pos+len,next_page_shift,page_num);
		
		CString address=data.Mid(start_pos,stop_pos-start_pos);
		address=FullAddress(address,mandatory_url_part);
    
		//start job to get next page
		CString next_page;
		next_page.Format(_T("%d"),_wtoi(input_file_name)+1);
		AsyncJob* job=new AsyncJob;
		job->job_type=GET_PAGE;
		job->address=address;
		job->data=next_page;
		job_queue->Add(job);
	}
	else return_code=1;
  
	//find all links to details
	bool find=true;
	int link_num=0;
	while(find)
	{
		int start_len=link_start.GetLength();
		start_pos=data.Find(link_start);
		stop_pos=data.Find(link_stop,start_pos+start_len);
		if(start_pos>=0 && stop_pos>=0 && stop_pos>start_pos)
		{
			//shift processing
			start_pos=StartWithShift(data,start_pos+len,link_start_shift,page_num);
			
			CString address=data.Mid(start_pos,stop_pos-start_pos);
			address=FullAddress(address,mandatory_url_part);
			data=data.Right(data.GetLength()-stop_pos);
      
			//start job to get details page
			CString details_page;
			details_page.Format(_T("_%d!%d"),_wtoi(page_num),++link_num);
			AsyncJob* job=new AsyncJob;
			job->job_type=GET_PAGE;
			job->address=address;
			job->data=details_page;
			job_queue->Add(job);
		}
		else find=false;
	}
  
	DeleteFile(input_file_name);
	
	return return_code;
}

CString ProcessDetails(CString input_file_name,std::vector<CString> names,std::vector<CString> starts,std::vector<CString> stops)
{
	CString data = LoadUtf8FileToString(input_file_name);
	CString result;
  
	std::vector<CString>::size_type sz=names.size();
	for(int prop=0;prop<sz;prop++)
	{
		CString start_phrase=starts[prop];
		CString stop_phrase=stops[prop];
		int start_len=starts[prop].GetLength();
		int start_pos=data.Find(start_phrase);
		int stop_pos=data.Find(stop_phrase,start_pos+start_len);
		if(start_pos>=0 && stop_pos>=0 && stop_pos>start_pos)
		{
			if(!result.IsEmpty()) result+=_T(";");
			start_pos+=start_len;
			CString txt=data.Mid(start_pos,stop_pos-start_pos);
			txt.Replace(_T("&nbsp;"),_T(" "));
			txt.Replace(_T("&#64;"),_T("@"));
			txt.Replace(';',',');
			result+=names[prop]+txt;
		}
	}
	result+=_T("\n");

	DeleteFile(input_file_name);

	return result;
}

AsyncJobQueue::AsyncJobQueue(HWND hEventReceiver)
{
	first=last=NULL;
	max_id=0;
	max_jobs=ASYNC_JOB_MAX_JOBS;
	m_hEventReceiver=hEventReceiver;
	job_count=0;

	h_WakeUpEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
	h_TerminateEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
	wait_events[0]=h_WakeUpEvent;
	wait_events[1]=h_TerminateEvent;

	::InitializeCriticalSection(&m_csAsync);
	m_hAsyncJobThread = ::CreateThread(NULL,0,DoAsyncJob,this,0,NULL);
}

AsyncJobQueue::~AsyncJobQueue()
{
	if(m_hAsyncJobThread)
	{
		::CloseHandle(m_hAsyncJobThread);
		m_hAsyncJobThread = NULL;
	}
	::DeleteCriticalSection(&m_csAsync);
		
	while(first)
	{
		AsyncJob *next=first->next;
		delete first;
		first=next;
	}
}

int AsyncJobQueue::Add(AsyncJob* new_job)
{
	EnterCriticalSection(&m_csAsync);
	
	++job_count;
	
	if(last) 
	{
		last->next=new_job;
		last=new_job;
	}
	else first=last=new_job;

	if(++max_id>99999) max_id=1;
	new_job->id=max_id;

	LeaveCriticalSection(&m_csAsync);

	//send event
	PostMessage(m_hEventReceiver,WM_COMMAND,ASYNC_JOB_ADD,new_job->id);
	SetEvent(h_WakeUpEvent);
	
	return max_id;
}

AsyncJob* AsyncJobQueue::Remove(int id)
{
	--job_count;
	
	AsyncJob *pointer=first,*prev_pointer=NULL;
	while(pointer)
	{
		if(pointer->id==id) break;
		prev_pointer=pointer;
		pointer=pointer->next;
	}
	if(!pointer) 
	{
		LeaveCriticalSection(&m_csAsync);
		return NULL;
	}

	if(pointer==first)
		first=pointer->next;
	if(prev_pointer!=NULL) prev_pointer->next=pointer->next;
	if(!pointer->next) last=prev_pointer;
	
	//send event
	PostMessage(m_hEventReceiver,WM_COMMAND,ASYNC_JOB_REMOVE,id);
	
	return pointer;
}

AsyncJob* AsyncJobQueue::GetJob()
{
	EnterCriticalSection(&m_csAsync);
	
	//get first job in queue which is not processed or done
	AsyncJob *pointer=first;
	while(pointer)
	{
		if(pointer->status==JOB_QUEUED) 
		{	
			//send event
			PostMessage(m_hEventReceiver,WM_COMMAND,ASYNC_JOB_PROCESSED,pointer->id);
			
			pointer->status=JOB_PROCESSED;
			break;
		}
		pointer=pointer->next;
	}
	
	LeaveCriticalSection(&m_csAsync);
	return pointer;
}

void AsyncJobQueue::Done(int id,CString result_data)
{
	EnterCriticalSection(&m_csAsync);
	
	AsyncJob *pointer=first;
	while(pointer)
	{
		if(pointer->id==id) break;
		pointer=pointer->next;
	}
	if(pointer) 
	{
		pointer->status=JOB_DONE;
		pointer->result_data=result_data;
	}

	LeaveCriticalSection(&m_csAsync);

	//send event
	PostMessage(m_hEventReceiver,WM_COMMAND,ASYNC_JOB_DONE,id);
}

DWORD WINAPI DoAsyncJob(LPVOID lpParams)
{
	AsyncJobQueue* job_queue=(AsyncJobQueue*)lpParams;
	
	bool terminated=false;
	while(!terminated)
	{
		//get all jobs and process
		AsyncJob *job=job_queue->GetJob();
		while(job)
		{
			//check if we are terminated
			DWORD event_ind=WaitForSingleObject(job_queue->h_TerminateEvent,10);
			if(event_ind!=WAIT_TIMEOUT) 
			{
				terminated=true;
				break;
			}

			//process
			CString result_data;
			if(job->job_type==GET_PAGE)
				GetPage(job->address,job->data,result_data);
			else if(job->job_type==PROCESS_CATALOG)
			{
				job->return_code=ProcessCatalog(job_queue,job->data,job->details_link_start,job->details_shift,job->details_link_stop,
												job->next_page_start,job->next_page_shift,job->next_page_stop,job->mandatory_url_part);
			}
			else result_data=ProcessDetails(job->data,job->names,job->starts,job->stops); 
			
			//set result and get next job
			job_queue->Done(job->id,result_data);
			job=job_queue->GetJob();
		}

		//in nothing to do then wait
		if(!terminated)
		{
			DWORD event_ind=WaitForMultipleObjects(2,job_queue->wait_events,FALSE,INFINITE);
			if(event_ind==1) 
			{
				terminated=true;
			}
		}
	}

	return 0;
}
