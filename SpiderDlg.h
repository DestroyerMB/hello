
// SpiderDlg.h : header file
//
#pragma once

#include "AsyncJobQueue.h"
#include "afxwin.h"

// CSpiderDlg dialog
class CSpiderDlg : public CDialogEx
{
// Construction
public:
	CSpiderDlg(CWnd* pParent = NULL);	// standard constructor
	CSpiderDlg::~CSpiderDlg();

// Dialog Data
	enum { IDD = IDD_SPIDER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	AsyncJobQueue* job_queue;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnCommand(WPARAM wParam,LPARAM lParam);
	CComboBox m_Template;
	CEdit m_StartPage;
	CEdit m_Pause;
	CEdit m_DetailsStart;
	CEdit m_DetailsStop;
	CEdit m_NextPageStart;
	CEdit m_NextPageStop;
	CEdit m_NameStart;
	CEdit m_NameStop;
	CEdit m_PhoneStart;
	CEdit m_PhoneStop;
	CEdit m_EmailStart;
	CEdit m_EmailStop;
	CEdit m_DetailsStartShift;
	CEdit m_NextPageStartShift;
	CEdit m_NameStartShift;
	CEdit m_PhoneStartShift;
	CEdit m_EmailStartShift;
	CEdit m_MandatoryUrlPart;
	CString start_page,mandatory_url_part,details_start,details_stop,next_page_start,next_page_stop,name_start,name_stop,phone_start,phone_stop,email_start,email_stop;
	CString details_shift,next_page_shift,name_shift,phone_shift,email_shift;
	CStatic m_StaticFirms;
	long firms_processed,catalog_pages_processed,details_pages_processed;
	CStatic m_StaticCatalog;
	CStatic m_StaticDetails;
};
