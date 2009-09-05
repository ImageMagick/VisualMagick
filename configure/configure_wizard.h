// configure_wizard.h : header file
//
// This class defines custom modal property sheet 
// CConfigureWizard.
 
#ifndef __CONFIGURE5F_WIZARD_H__
#define __CONFIGURE5F_WIZARD_H__

#include "welcome_page.h"
#include "target_page.h"
#include "system_page.h"
#include "finished_page.h"

/////////////////////////////////////////////////////////////////////////////
// CConfigureWizard

class CConfigureWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(CConfigureWizard)

// Construction
public:
	CConfigureWizard(CWnd* pWndParent = NULL);

// Attributes
public:
	CWelcomePage m_Page1;
	CTargetPage m_Page2;
	CSystemPage m_Page3;
	CFinishedPage m_Page4;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigureWizard)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CConfigureWizard();

// Generated message map functions
protected:
	//{{AFX_MSG(CConfigureWizard)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __CONFIGURE5F_WIZARD_H__
