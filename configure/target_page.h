#if !defined(AFX_TARGET_PAGE_H__3814798F_A99A_11D3_8F4C_002018383984__INCLUDED_)
#define AFX_TARGET_PAGE_H__3814798F_A99A_11D3_8F4C_002018383984__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// target_page.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTargetPage dialog

class CTargetPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTargetPage)

// Construction
public:
	CTargetPage();
	~CTargetPage();

// Dialog Data
	//{{AFX_DATA(CTargetPage)
	enum { IDD = IDD_TARGET_PAGE };
	int		m_projectType;
	BOOL	m_useX11Stubs;
	BOOL	m_decorateFiles;
	BOOL	m_optionalFiles;
	BOOL	m_standalone;
	BOOL	m_visualStudio7;
	BOOL	m_build64Bit;
	BOOL	m_bigCoderDLL;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTargetPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTargetPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnEditConfig();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TARGET_PAGE_H__3814798F_A99A_11D3_8F4C_002018383984__INCLUDED_)
