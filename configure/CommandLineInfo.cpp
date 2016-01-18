/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%  Copyright 2014-2016 ImageMagick Studio LLC, a non-profit organization      %
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
#include "CommandLineInfo.h"
#include "Shared.h"

CommandLineInfo::CommandLineInfo(const ConfigureWizard &wizard)
{
  _build64bit=wizard.build64bit();
  _excludeDeprecated=wizard.excludeDeprecated();
  _includeIncompatibleLicense=wizard.includeIncompatibleLicense();
  _includeOptional=wizard.includeOptional();
  _installedSupport=wizard.installedSupport();
  _noWizard=false;
  _quantumDepth=wizard.quantumDepth();
  _solutionType=wizard.solutionType();
  _useHDRI=wizard.useHDRI();
  _useOpenCL=wizard.useOpenCL();
  _useOpenMP=wizard.useOpenMP();
  _visualStudioVersion=wizard.visualStudioVersion();
}

CommandLineInfo::CommandLineInfo(const CommandLineInfo& obj)
{
  *this = obj;
}

CommandLineInfo& CommandLineInfo::operator=(const CommandLineInfo& obj)
{
  return *this;
}

bool CommandLineInfo::build64bit() const
{
  return(_build64bit);
}

bool CommandLineInfo::excludeDeprecated() const
{
  return(_excludeDeprecated);
}

bool CommandLineInfo::includeIncompatibleLicense() const
{
  return(_includeIncompatibleLicense);
}

bool CommandLineInfo::includeOptional() const
{
  return(_includeOptional);
}

bool CommandLineInfo::installedSupport() const
{
  return(_installedSupport);
}

bool CommandLineInfo::noWizard() const
{
  return(_noWizard);
}

int CommandLineInfo::quantumDepth() const
{
  return(_quantumDepth);
}

int CommandLineInfo::solutionType() const
{
  return(_solutionType);
}

bool CommandLineInfo::useHDRI() const
{
  return(_useHDRI);
}

bool CommandLineInfo::useOpenCL() const
{
  return(_useOpenCL);
}

bool CommandLineInfo::useOpenMP() const
{
  return(_useOpenMP);
}

int CommandLineInfo::visualStudioVersion() const
{
  return(_visualStudioVersion);
}

void CommandLineInfo::ParseParam(const wchar_t* pszParam, BOOL bFlag, BOOL bLast)
{
  if (!bFlag)
    return;

  if (_wcsicmp(pszParam, L"dmt") == 0)
    _solutionType=DYNAMIC_MT;
  else if (_wcsicmp(pszParam, L"deprecated") == 0)
    _excludeDeprecated=FALSE;
  else if (_wcsicmp(pszParam, L"smt") == 0)
    _solutionType=STATIC_MT;
  else if (_wcsicmp(pszParam, L"smtd") == 0)
    _solutionType=STATIC_MTD;
  else if (_wcsicmp(pszParam, L"hdri") == 0)
    _useHDRI=true;
  else if (_wcsicmp(pszParam, L"incompatibleLicense") == 0)
     _includeIncompatibleLicense=true;
  else if (_wcsicmp(pszParam, L"includeOptional") == 0)
    _includeOptional=true;
  else if (_wcsicmp(pszParam, L"installedSupport") == 0)
    _installedSupport=true;
  else if (_wcsicmp(pszParam, L"noHdri") == 0)
    _useHDRI=false;
  else if (_wcsicmp(pszParam, L"noOpenMP") == 0)
    _useOpenMP=false;
  else if (_wcsicmp(pszParam, L"noWizard") == 0)
    _noWizard=true;
  else if (_wcsicmp(pszParam, L"openCL") == 0)
    _useOpenCL=true;
  else if (_wcsicmp(pszParam, L"Q8") == 0)
    _quantumDepth=Q8;
  else if (_wcsicmp(pszParam, L"Q16") == 0)
    _quantumDepth=Q16;
  else if (_wcsicmp(pszParam, L"Q32") == 0)
    _quantumDepth=Q32;
  else if (_wcsicmp(pszParam, L"Q64") == 0)
    _quantumDepth=Q64;
  else if (_wcsicmp(pszParam, L"x64") == 0)
    _build64bit=true;
  else if (_wcsicmp(pszParam, L"VS2002") == 0)
    _visualStudioVersion=VS2002;
  else if (_wcsicmp(pszParam, L"VS2010") == 0)
    _visualStudioVersion=VS2010;
  else if (_wcsicmp(pszParam, L"VS2012") == 0)
    _visualStudioVersion=VS2012;
  else if (_wcsicmp(pszParam, L"VS2013") == 0)
    _visualStudioVersion=VS2013;
  else if (_wcsicmp(pszParam, L"VS2015") == 0)
    _visualStudioVersion=VS2015;
}
