/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%  Copyright 2014-2015 ImageMagick Studio LLC, a non-profit organization      %
%  dedicated to making software imaging solutions freely available.           %
%                                                                             %
%  You may not use this file except in compliance with the License.  You may  %
%  obtain a copy of the License at                                            %
%                                                                             %
%    http://www.imagemagick.org/script/license.php                            %
%                                                                             %
%  Unless required by applicable law or agreed to in writing, software        %
%  distributed under the License is distributed on an "AS IS" BASIS,          %
%  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   %
%  See the License for the specific language governing permissions and        %
%  limitations under the License.                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
#include "..\stdafx.h"
#include "..\ConfigureApp.h"
#include "SystemPage.h"
#include <Shldisp.h>

IMPLEMENT_DYNCREATE(SystemPage, CPropertyPage)

SystemPage::SystemPage() : CPropertyPage(IDD_SYSTEM_PAGE)
{
  _binDirectory="..\\bin\\";
  _intermediateDirectoryDebug = "..\\Debug\\";
  _intermediateDirectoryRelease = "..\\Release\\";
  _libDirectory="..\\lib\\";
}

SystemPage::~SystemPage()
{
}

string SystemPage::binDirectory() const
{
  return(addBackslash(_binDirectory));
}

string SystemPage::intermediateDirectoryDebug() const
{
  return(addBackslash(_intermediateDirectoryDebug));
}

string SystemPage::intermediateDirectoryRelease() const
{
  return(addBackslash(_intermediateDirectoryRelease));
}

string SystemPage::libDirectory() const
{
  return(addBackslash(_libDirectory));
}

BOOL SystemPage::OnSetActive()
{
  CPropertySheet
    *propertySheet;

  propertySheet=(CPropertySheet*) GetParent();
  propertySheet->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

  return(CPropertyPage::OnSetActive());
}

void SystemPage::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  DDX_Text(pDX,IDC_INTERMEDIATE_RELEASE,_intermediateDirectoryRelease);
  DDX_Text(pDX,IDC_INTERMEDIATE_DEBUG,_intermediateDirectoryDebug);
  DDX_Text(pDX,IDC_BIN_DIR,_binDirectory);
  DDX_Text(pDX,IDC_LIB_DIR,_libDirectory);
}

void SystemPage::OnBinDirBrowse()
{
  setDirectory("Select Bin directory",_binDirectory);
}

BOOL SystemPage::OnInitDialog() 
{
  CPropertyPage::OnInitDialog();

  UpdateData(FALSE);

  return(TRUE);
}

void SystemPage::OnIntermediateDirectoryDebugBrowse()
{
  setDirectory("Select Debug directory",_intermediateDirectoryDebug);
}

void SystemPage::OnIntermediateDirectoryReleaseBrowse()
{
  setDirectory("Select Release directory",_intermediateDirectoryRelease);
}

void SystemPage::OnLibDirBrowse()
{
  setDirectory("Select Lib directory",_libDirectory);
}

BEGIN_MESSAGE_MAP(SystemPage, CPropertyPage)
  ON_BN_CLICKED(IDC_INTERMEDIATE_RELEASE_BROWSE,OnIntermediateDirectoryReleaseBrowse)
  ON_BN_CLICKED(IDC_INTERMEDIATE_DEBUG_BROWSE, OnIntermediateDirectoryDebugBrowse)
  ON_BN_CLICKED(IDC_BIN_DIR, OnBinDirBrowse)
  ON_BN_CLICKED(IDC_BIN_DIR_BROWSE, OnLibDirBrowse)
END_MESSAGE_MAP()

string SystemPage::addBackslash(const CString &directory) const
{
  if (directory.GetAt(directory.GetLength()-1) != '\\')
    return(string(directory) + "\\");
  else
    return(string(directory));
}

bool SystemPage::browseForFolder(const char *title, char *retval)
{
  char
    directory[_MAX_PATH];

  BROWSEINFO
    info;

  LPITEMIDLIST
    itemidlist;

  LPMALLOC
    pMalloc;

  memset(directory,'\0',_MAX_PATH);

  if (::SHGetMalloc(&pMalloc) != NOERROR)
    return(false);

  info.hwndOwner = m_hWnd;
  info.pidlRoot = NULL;
  info.pszDisplayName = directory;
  info.lpszTitle = title;
  info.ulFlags = 0;
  info.lpfn = NULL;

  itemidlist = SHBrowseForFolder(&info);
  if (itemidlist == NULL) // User clicked Cancel
  {
    pMalloc->Release();
    return(false);
  }

  SHGetPathFromIDList(itemidlist, retval);
  pMalloc->Free(itemidlist);
  pMalloc->Release();
  return(true);
}

void SystemPage::setDirectory(const string &title,CString &directory)
{
  char
    path[_MAX_PATH];

  UpdateData(TRUE);
  if (browseForFolder(title.c_str(), path))
    directory=path;
  UpdateData(FALSE);
}

