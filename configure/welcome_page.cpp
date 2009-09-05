// welcome_page.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "welcome_page.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CWelcomePage, CPropertyPage)


/////////////////////////////////////////////////////////////////////////////
// CWelcomePage property page

CWelcomePage::CWelcomePage() : CPropertyPage(CWelcomePage::IDD)
{
	//{{AFX_DATA_INIT(CWelcomePage)
	m_welcomeText = _T("");
	//}}AFX_DATA_INIT
}

CWelcomePage::~CWelcomePage()
{
}

void CWelcomePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWelcomePage)
	DDX_Text(pDX, IDC_WELCOME_TEXT, m_welcomeText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWelcomePage, CPropertyPage)
	//{{AFX_MSG_MAP(CWelcomePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CWelcomePage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_welcomeText  = "Welcome!\n\n";
	
	m_welcomeText += "This is the VisualMagick configuration wizard. It will help you set up ";
	m_welcomeText += "your build environment and customize the build process for your needs. ";
	m_welcomeText += "Its purpose is to create Visual Studio 6.0 project (DSP) and ";
 	m_welcomeText += "workspace (DSW) files, or Visual Studio 7.0 solution (SLN) and ";
 	m_welcomeText += "project files (VCPROJ).\n\n";

	m_welcomeText += "When the build environment has been created you can build the system in ";
	m_welcomeText += "the Visual Studio IDE.\n\n";

	m_welcomeText += "NOTE: Please be advised that support for Visual Studio 6.0 will not be ";
	m_welcomeText += "actively supported very soon. VS7 has been out for years now and all ";
	m_welcomeText += "current work takes place in this environment.\n\n";

	m_welcomeText += "Good Luck - and thanks for all the fish!";

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWelcomePage::OnSetActive() 
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();
	psheet->SetWizardButtons(PSWIZB_NEXT);
	
	return CPropertyPage::OnSetActive();
}
