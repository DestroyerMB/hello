
// SpiderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Spider.h"
#include "SpiderDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSpiderDlg dialog
CSpiderDlg::CSpiderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSpiderDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	catalog_pages_processed=0;
	details_pages_processed=0;
	firms_processed=0;
}

CSpiderDlg::~CSpiderDlg()
{
	if(job_queue)
	{
		SetEvent(job_queue->h_TerminateEvent);
		Sleep(200);
		delete job_queue;
		job_queue = NULL;
	}
}	

void CSpiderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEMPLATE, m_Template);
	DDX_Control(pDX, IDC_START_PAGE, m_StartPage);
	DDX_Control(pDX, IDC_PAUSE, m_Pause);
	DDX_Control(pDX, IDC_DETAILS_START, m_DetailsStart);
	DDX_Control(pDX, IDC_DETAILS_STOP, m_DetailsStop);
	DDX_Control(pDX, IDC_NEXT_PAGE_START, m_NextPageStart);
	DDX_Control(pDX, IDC_NEXT_PAGE_STOP, m_NextPageStop);
	DDX_Control(pDX, IDC_NAME_START, m_NameStart);
	DDX_Control(pDX, IDC_NAME_STOP, m_NameStop);
	DDX_Control(pDX, IDC_PHONE_START, m_PhoneStart);
	DDX_Control(pDX, IDC_PHONE_STOP, m_PhoneStop);
	DDX_Control(pDX, IDC_EMAIL_START, m_EmailStart);
	DDX_Control(pDX, IDC_EMAIL_STOP, m_EmailStop);
	DDX_Control(pDX, IDC_DETAILS_START_SHIFT, m_DetailsStartShift);
	DDX_Control(pDX, IDC_NEXT_PAGE_START_SHIFT, m_NextPageStartShift);
	DDX_Control(pDX, IDC_NAME_START_SHIFT, m_NameStartShift);
	DDX_Control(pDX, IDC_PHONE_START_SHIFT, m_PhoneStartShift);
	DDX_Control(pDX, IDC_EMAIL_START_SHIFT, m_EmailStartShift);
	DDX_Control(pDX, IDC_STATIC_FIRMS, m_StaticFirms);
	DDX_Control(pDX, IDC_STATIC_CATALOG, m_StaticCatalog);
	DDX_Control(pDX, IDC_STATIC_DETAILS, m_StaticDetails);
}

BEGIN_MESSAGE_MAP(CSpiderDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CSpiderDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSpiderDlg message handlers

BOOL CSpiderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_StartPage.SetWindowText(_T("https://firmy.abc.cz/potraviny-a-napoje/"));
	m_DetailsStart.SetWindowText(_T("<div class=\"item_content clearfix\">"));
	m_DetailsStartShift.SetWindowText(_T("21"));
	m_DetailsStop.SetWindowText(_T("\">"));
	m_NextPageStart.SetWindowText(_T("class=\"pagination clearfix\">"));
	m_NextPageStartShift.SetWindowText(_T("!%catalog_page</strong></span><a href=\""));
	m_NextPageStop.SetWindowText(_T("\">"));
	m_NameStart.SetWindowText(_T("<h1>"));
	m_NameStop.SetWindowText(_T("</h1>"));
	m_PhoneStart.SetWindowText(_T("contact_telefon\"></span>"));
	m_PhoneStop.SetWindowText(_T("</td>"));
	m_EmailStart.SetWindowText(_T("href=\"mai&#108;&#116;&#111;&#58;"));
	m_EmailStop.SetWindowText(_T("?body="));

	job_queue=new AsyncJobQueue(this->m_hWnd);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSpiderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSpiderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSpiderDlg::OnBnClickedOk()
{
	m_StartPage.GetWindowText(start_page);
	m_MandatoryUrlPart.GetWindowText(mandatory_url_part);
	m_DetailsStart.GetWindowText(details_start);
	m_DetailsStop.GetWindowText(details_stop);
	m_NextPageStart.GetWindowText(next_page_start);
	m_NextPageStop.GetWindowText(next_page_stop);
	m_NameStart.GetWindowText(name_start);
	m_NameStop.GetWindowText(name_stop);
	m_PhoneStart.GetWindowText(phone_start);
	m_PhoneStop.GetWindowText(phone_stop);
	m_EmailStart.GetWindowText(email_start);
	m_EmailStop.GetWindowText(email_stop);

	m_DetailsStartShift.GetWindowText(details_shift);
	m_NextPageStartShift.GetWindowText(next_page_shift);
	m_NameStartShift.GetWindowText(name_shift);
	m_PhoneStartShift.GetWindowText(phone_shift);
	m_EmailStartShift.GetWindowText(email_shift);
	
	catalog_pages_processed=0;
	details_pages_processed=0;
	firms_processed=0;

	AsyncJob* job=new AsyncJob;
	job->job_type=GET_PAGE;
	job->address=start_page;
	job->data="1";
	job_queue->Add(job);
}

BOOL CSpiderDlg::OnCommand(WPARAM wParam,LPARAM lParam)
{	
	if(wParam==ASYNC_JOB_DONE)
	{
		AsyncJob* job=job_queue->Remove(lParam);
		if(job)
		{
			int job_type=job->job_type;
			CString name=job->name;
			CString address=job->address;
			CString data=job->data;
			CString result_data=job->result_data;
			delete job; job=NULL;

			if(job_type==GET_PAGE)
			{
				if(data.Find('_')<0)
				{
					//process catalog
					AsyncJob* new_job=new AsyncJob;
					new_job->job_type=PROCESS_CATALOG;
					new_job->data=result_data;
					job->mandatory_url_part=mandatory_url_part;
					new_job->details_link_start=details_start;
					new_job->details_shift=details_shift;
					new_job->details_link_stop=details_stop;
					new_job->next_page_start=next_page_start;
					new_job->next_page_shift=next_page_shift;
					new_job->next_page_stop=next_page_stop;
					job_queue->Add(new_job);
					
					//update status
					CString s;
					s.Format(_T("%d"),++catalog_pages_processed);
					m_StaticCatalog.SetWindowText(_T("Catalog pages: ")+s);
					m_StaticCatalog.RedrawWindow();
				}
				else 
				{
					//process details
					AsyncJob* new_job=new AsyncJob;
					new_job->job_type=PROCESS_DETAILS;
					new_job->data=result_data;
					new_job->names.push_back(_T("Firma: "));
					new_job->starts.push_back(name_start);
					new_job->stops.push_back(name_stop);
					new_job->names.push_back(_T("Telefon: "));
					new_job->starts.push_back(phone_start);
					new_job->stops.push_back(phone_stop);
					new_job->names.push_back(_T("Email: "));
					new_job->starts.push_back(email_start);
					new_job->stops.push_back(email_stop);
					job_queue->Add(new_job);

					//update status
					CString s;
					s.Format(_T("%d"),++details_pages_processed);
					m_StaticDetails.SetWindowText(_T("Details pages: ")+s);
					m_StaticDetails.RedrawWindow();
				}
			}
			else if(job_type==PROCESS_DETAILS)
			{
				CString output_file_name=GetCwd()+_T("\\results.csv");
				FILE *output_file=_wfopen(output_file_name, L"a, ccs=UTF-8");
				if(!output_file) output_file=_wfopen(output_file_name, L"w, ccs=UTF-8");
				if(output_file)
				{
					fputws(result_data,output_file);
					fclose(output_file);

					//update status
					CString s;
					s.Format(_T("%d"),++firms_processed);
					m_StaticFirms.SetWindowText(_T("Firms: ")+s);
					m_StaticFirms.RedrawWindow();
					
					SetWindowText(_T("Spider: ")+s);
				}
			}
		}

	}

	if(wParam==ASYNC_JOB_ADD || wParam==ASYNC_JOB_PROCESSED || wParam==ASYNC_JOB_DONE || wParam==ASYNC_JOB_REMOVE) return true;

	return CDialog::OnCommand(wParam,lParam);
}
