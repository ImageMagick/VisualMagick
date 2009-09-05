// finished_page.cpp : implementation file
//

#include "stdafx.h"
#include "configure.h"
#include "finished_page.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFinishedPage property page

IMPLEMENT_DYNCREATE(CFinishedPage, CPropertyPage)

CFinishedPage::CFinishedPage() : CPropertyPage(CFinishedPage::IDD)
{
	//{{AFX_DATA_INIT(CFinishedPage)
	m_text = _T("");
	//}}AFX_DATA_INIT
}

CFinishedPage::~CFinishedPage()
{
}

void CFinishedPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFinishedPage)
	DDX_Text(pDX, IDC_FINISHED_TEXT, m_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFinishedPage, CPropertyPage)
	//{{AFX_MSG_MAP(CFinishedPage)
	ON_BN_CLICKED(IDC_BuildIt, OnBuildIt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFinishedPage message handlers

BOOL CFinishedPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_text  = "Congratulations!\n\n";

	m_text += "You have finished the VisualMagick configuration. Now press finish to finally";
	m_text += " produce the Visual Studio compatible files.\n\n";
  m_text += "You will find different files in the VisualMagick folder based on the build";
  m_text += " options you selected. The dynamic (DLL) build is named VisualDynamicMT";
  m_text += " while the static build configurations are named VisualStaticXX, were the XX";
  m_text += " indicates the type of threading allowed: (ST: single-threaded, MT; multi-threaded,";
  m_text += " and MTD: multi-threaded DLL).\n\n";
  m_text += "NOTE: MT is the most common and the recommended static build option\n\n";
  m_text += "Locate the item in the workspace or solution called \"All\". Set this as the active";
  m_text += " project, set the configuration to either Debug or Release and then just build this.";
  m_text += "\n\nGood luck!";

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CFinishedPage::OnSetActive() 
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);

	return CPropertyPage::OnSetActive();
}

void CFinishedPage::OnBuildIt() 
{
  CString strOptions = "ALL /RECURSE=1";
  CString strWorkspace = "VisualDynamicMT.dsw";

  CString strCommandLine;
  strCommandLine.Format("D:\\DevStudio\\Common\\MSDev98\\Bin\\MSDEV \"%s\" /MAKE %s",
    strWorkspace, strOptions);

  if (WinExec(strCommandLine, SW_SHOW) <= 31)
	  MessageBox(_T("Unable to run MSDEV"));
}
