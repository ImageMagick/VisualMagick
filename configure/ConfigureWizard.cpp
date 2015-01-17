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
#include "stdafx.h"
#include "resource.h"
#include "ConfigureWizard.h"
#include "CommandLineInfo.h"
#include "Shared.h"

IMPLEMENT_DYNAMIC(ConfigureWizard,CPropertySheet)

ConfigureWizard::ConfigureWizard(CWnd* pWndParent)
  : CPropertySheet(IDS_PROPSHT_CAPTION,pWndParent)
{
  AddPage(&_welcomePage);
  AddPage(&_targetPage);
  AddPage(&_systemPage);
  AddPage(&_finishedPage);

  SetWizardMode();
}

ConfigureWizard::~ConfigureWizard()
{
}

string ConfigureWizard::binDirectory() const
{
  return(_systemPage.binDirectory());
}

bool ConfigureWizard::build64bit() const
{
  return(_targetPage.build64bit());
}

bool ConfigureWizard::includeIncompatibleLicense() const
{
  return(_targetPage.includeIncompatibleLicense());
}

bool ConfigureWizard::includeOptional() const
{
  return(_targetPage.includeOptional());
}

string ConfigureWizard::intermediateDirectoryDebug() const
{
  return(_systemPage.intermediateDirectoryDebug());
}

string ConfigureWizard::intermediateDirectoryRelease() const
{
  return(_systemPage.intermediateDirectoryRelease());
}

string ConfigureWizard::libDirectory() const
{
  return(_systemPage.libDirectory() + solutionName() + "-" + platform() + "\\");
}

string ConfigureWizard::openCLIncludePath() const
{
  return(_targetPage.openCLIncludePath());
}

string ConfigureWizard::platform() const
{
  return(_targetPage.build64bit() ? "x64" : "Win32");
}

int ConfigureWizard::quantumDepth() const
{
  return(_targetPage.quantumDepth());
}

string ConfigureWizard::solutionName() const
{
  if (solutionType() == DYNAMIC_MT)
    return("DynamicMT");
  else if (solutionType() == STATIC_MTD)
    return("StaticMTD");
  else if (solutionType() == STATIC_MT)
    return("StaticMT");
  else
    return("ThisShouldNeverHappen");
}

int ConfigureWizard::solutionType() const
{
  return(_targetPage.solutionType());
}

bool ConfigureWizard::useHDRI() const
{
  return(_targetPage.useHDRI());
}

bool ConfigureWizard::useOpenCL() const
{
  return(_targetPage.useOpenCL());
}

bool ConfigureWizard::useOpenMP() const
{
  return(_targetPage.useOpenMP());
}

int ConfigureWizard::visualStudioVersion() const
{
  return(_targetPage.visualStudioVersion());
}

void ConfigureWizard::parseCommandLineInfo(const CommandLineInfo &info)
{
  _targetPage.build64bit(info.build64bit());
  _targetPage.quantumDepth(info.quantumDepth());
  _targetPage.solutionType(info.solutionType());
  _targetPage.useHDRI(info.useHDRI());
  _targetPage.useOpenCL(info.useOpenCL());
  _targetPage.visualStudioVersion(info.visualStudioVersion());
}

BEGIN_MESSAGE_MAP(ConfigureWizard,CPropertySheet)
END_MESSAGE_MAP()