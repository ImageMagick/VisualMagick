// welcome_page.h : header file
//

#ifndef __WELCOME5F_PAGE_H__
#define __WELCOME5F_PAGE_H__

/////////////////////////////////////////////////////////////////////////////
// CWelcomePage dialog

class CWelcomePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CWelcomePage)

// Construction
public:
	CWelcomePage();
	~CWelcomePage();

// Dialog Data
	//{{AFX_DATA(CWelcomePage)
	enum { IDD = IDD_WELCOME_PAGE };
	CString	m_welcomeText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWelcomePage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWelcomePage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};



#endif // __WELCOME5F_PAGE_H__
