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
#include "..\Shared.h"
#include "TargetPage.h"

IMPLEMENT_DYNCREATE(TargetPage, CPropertyPage)

TargetPage::TargetPage() : CPropertyPage(IDD_TARGET_PAGE)
{
  setOpenCLIncludePath();
  setVisualStudioVersion();

  _build64bit=FALSE;
  _includeIncompatibleLicense=FALSE;
  _quantumDepth=Q16;
  _solutionType=DYNAMIC_MT;
  _useHDRI=PathFileExists(L"..\\MagickCore") ? TRUE : FALSE;
  _useOpenCL=!_openCLIncludePath.empty();
  _useOpenMP=TRUE;
}

TargetPage::~TargetPage()
{
}

bool TargetPage::includeIncompatibleLicense() const
{
  return(_includeIncompatibleLicense == 1);
}

void TargetPage::includeIncompatibleLicense(bool value)
{
  _includeIncompatibleLicense=value;
}

bool TargetPage::includeOptional() const
{
  return(_includeOptional == 1);
}

void TargetPage::includeOptional(bool value)
{
  _includeOptional=value;
}

bool TargetPage::build64bit() const
{
  return(_build64bit == 1);
}

void TargetPage::build64bit(bool value)
{
  _build64bit=value;
}

wstring TargetPage::openCLIncludePath() const
{
  return(_openCLIncludePath);
}

int TargetPage::quantumDepth() const
{
  return(_quantumDepth);
}

void TargetPage::quantumDepth(int value)
{
  _quantumDepth=value;
}

int TargetPage::solutionType() const
{
  return(_solutionType);
}

void TargetPage::solutionType(int value)
{
  _solutionType=value;
}

bool TargetPage::useHDRI() const
{
  return(_useHDRI == 1);
}

void TargetPage::useHDRI(bool value)
{
  _useHDRI=value;
}

bool TargetPage::useOpenCL() const
{
  return(_useOpenCL == 1);
}

void TargetPage::useOpenCL(bool value)
{
  if (!_openCLIncludePath.empty())
    _useOpenCL=value;
}

bool TargetPage::useOpenMP() const
{
  return(_useOpenMP == 1);
}

void TargetPage::useOpenMP(bool value)
{
  _useOpenMP=value;
}

int TargetPage::visualStudioVersion() const
{
  return(_visualStudioVersion);
}

void TargetPage::visualStudioVersion(int value)
{
  _visualStudioVersion=value;
}

void TargetPage::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);

  DDX_Check(pDX,IDC_BUILD_64_BIT,_build64bit);
  DDX_CBIndex(pDX,IDC_QUANTUM_DEPTH,_quantumDepth);
  DDX_Radio(pDX,IDC_PROJECT_DYNAMIC_MT,_solutionType);
  DDX_Check(pDX,IDC_HDRI,_useHDRI);
  DDX_Check(pDX,IDC_OPEN_CL,_useOpenCL);
  DDX_Check(pDX,IDC_OPEN_MP,_useOpenMP);
  DDX_CBIndex(pDX,IDC_VISUALSTUDIO,_visualStudioVersion);
  DDX_Check(pDX,IDC_INCLUDE_INCOMPATIBLE,_includeIncompatibleLicense);
  DDX_Check(pDX,IDC_INCLUDE_OPTIONAL,_includeOptional);
}

BOOL TargetPage::OnInitDialog()
{
  CButton
    *openCL;

  CPropertyPage::OnInitDialog();

  if (_openCLIncludePath.empty())
  {
    openCL=(CButton*) GetDlgItem(IDC_OPEN_CL);
    openCL->EnableWindow(FALSE);
  }

  UpdateData(FALSE);

  return TRUE;
}

BEGIN_MESSAGE_MAP(TargetPage, CPropertyPage)
END_MESSAGE_MAP()

wstring TargetPage::getEnvironmentVariable(const wchar_t *name)
{
  wchar_t
    *buffer;

  size_t
    length;

  wstring
    value;

  if (_wdupenv_s(&buffer,&length,name) == 0)
  {
    if (length > 0)
    {
      value=wstring(buffer);
      free(buffer);
      return(value);
    }
  }

  return(value);
}

bool TargetPage::openCLIncludePathExists(const wchar_t *name)
{
  _openCLIncludePath=getEnvironmentVariable(name) + L"\\include";
  return(PathFileExists(_openCLIncludePath.c_str()) ? true : false);
}

void TargetPage::setOpenCLIncludePath()
{
  if (openCLIncludePathExists(L"AMDAPPSDKROOT"))
    return;

  if (openCLIncludePathExists(L"CUDA_PATH"))
    return;

  if (openCLIncludePathExists(L"INTELOCLSDKROOT"))
    return;

  _openCLIncludePath=L"";
}

void TargetPage::setVisualStudioVersion()
{
  if (!getEnvironmentVariable(L"VS110COMNTOOLS").empty())
    _visualStudioVersion=VS2012;
  else if (!getEnvironmentVariable(L"VS100COMNTOOLS").empty())
    _visualStudioVersion=VS2010;
  else
    _visualStudioVersion=VS2002;
}

