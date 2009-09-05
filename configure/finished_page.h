#if !defined(AFX_FINISHED_PAGE_H__75F5C8C2_A9F5_11D3_8F4C_002018383984__INCLUDED_)
#define AFX_FINISHED_PAGE_H__75F5C8C2_A9F5_11D3_8F4C_002018383984__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// finished_page.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFinishedPage dialog

class CFinishedPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CFinishedPage)

// Construction
public:
	CFinishedPage();
	~CFinishedPage();

// Dialog Data
	//{{AFX_DATA(CFinishedPage)
	enum { IDD = IDD_FINISHED_PAGE };
	CString	m_text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFinishedPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFinishedPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBuildIt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINISHED_PAGE_H__75F5C8C2_A9F5_11D3_8F4C_002018383984__INCLUDED_)
