// system_page.cpp : implementation file
//

#include "stdafx.h"
#include "configure.h"
#include "system_page.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSystemPage property page

IMPLEMENT_DYNCREATE(CSystemPage, CPropertyPage)

CSystemPage::CSystemPage() : CPropertyPage(CSystemPage::IDD)
{
	//{{AFX_DATA_INIT(CSystemPage)
	m_tempDebug = _T("");
	m_tempRelease = _T("");
	m_outputBin = _T("");
	m_outputLib = _T("");
	//}}AFX_DATA_INIT
}

CSystemPage::~CSystemPage()
{
}

void CSystemPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSystemPage)
	DDX_Text(pDX, IDC_TEMP_DEBUG, m_tempDebug);
	DDX_Text(pDX, IDC_TEMP_RELEASE, m_tempRelease);
	DDX_Text(pDX, IDC_OUTPUT_BIN, m_outputBin);
	DDX_Text(pDX, IDC_OUTPUT_LIB, m_outputLib);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSystemPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSystemPage)
	ON_BN_CLICKED(IDC_TEMP_RELEASE_BROWSE, OnTempReleaseBrowse)
	ON_BN_CLICKED(IDC_TEMP_DEBUG_BROWSE, OnTempDebugBrowse)
	ON_BN_CLICKED(IDC_OUTPUT_BIN_BROWSE, OnOutputBinBrowse)
	ON_BN_CLICKED(IDC_OUTPUT_LIB_BROWSE, OnOutputLibBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystemPage message handlers

BOOL CSystemPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSystemPage::OnTempReleaseBrowse() 
{
	char str[1024];

	UpdateData(TRUE);
	if (BrowseForFolder(m_hWnd, "Select Release directory", str))
  {
    lstrcat(str, "\\");
	  m_tempRelease = str;
  }
	UpdateData(FALSE);
}

void CSystemPage::OnTempDebugBrowse() 
{
	char str[1024];

	UpdateData(TRUE);
	if (BrowseForFolder(m_hWnd, "Select Debug directory", str))
  {
    lstrcat(str, "\\");
	  m_tempDebug = str;
  }
	UpdateData(FALSE);
}

void CSystemPage::OnOutputBinBrowse() 
{
	char str[1024];

	UpdateData(TRUE);
	if (BrowseForFolder(m_hWnd, "Select Bin directory", str))
  {
    lstrcat(str, "\\");
	  m_outputBin = str;
  }
	UpdateData(FALSE);
}

void CSystemPage::OnOutputLibBrowse() 
{
	char str[1024];

	UpdateData(TRUE);
	if (BrowseForFolder(m_hWnd, "Select Lib directory", str))
  {
    lstrcat(str, "\\");
	  m_outputLib = str;
  }
	UpdateData(FALSE);
}

BOOL CSystemPage::OnSetActive() 
{
	CPropertySheet* psheet = (CPropertySheet*) GetParent();
	psheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	return CPropertyPage::OnSetActive();
}

