/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%  Copyright 2014-2021 ImageMagick Studio LLC, a non-profit organization      %
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

CommandLineInfo::CommandLineInfo(const ConfigureWizard &wizard)
{
  _platform=wizard.platform();
  _excludeDeprecated=wizard.excludeDeprecated();
  _includeIncompatibleLicense=wizard.includeIncompatibleLicense();
  _includeOptional=wizard.includeOptional();
  _installedSupport=wizard.installedSupport();
  _noWizard=false;
  _quantumDepth=wizard.quantumDepth();
  _solutionType=wizard.solutionType();
  _useHDRI=wizard.useHDRI();
  _useOpenCL=true;
  _useOpenMP=wizard.useOpenMP();
  _visualStudioVersion=wizard.visualStudioVersion();
  _zeroConfigurationSupport=wizard.zeroConfigurationSupport();
}

CommandLineInfo::CommandLineInfo(const CommandLineInfo& obj)
{
  *this = obj;
}

CommandLineInfo& CommandLineInfo::operator=(const CommandLineInfo& obj)
{
  return *this;
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

Platform CommandLineInfo::platform() const
{
  return(_platform);
}

QuantumDepth CommandLineInfo::quantumDepth() const
{
  return(_quantumDepth);
}

SolutionType CommandLineInfo::solutionType() const
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

VisualStudioVersion CommandLineInfo::visualStudioVersion() const
{
  return(_visualStudioVersion);
}

bool CommandLineInfo::zeroConfigurationSupport() const
{
  return(_zeroConfigurationSupport);
}

void CommandLineInfo::ParseParam(const wchar_t* pszParam, BOOL bFlag, BOOL bLast)
{
  if (!bFlag)
    return;

  if (_wcsicmp(pszParam, L"arm64") == 0)
    _platform=Platform::ARM64;
  else if (_wcsicmp(pszParam, L"dmt") == 0)
    _solutionType=SolutionType::DYNAMIC_MT;
  else if (_wcsicmp(pszParam, L"deprecated") == 0)
    _excludeDeprecated=FALSE;
  else if (_wcsicmp(pszParam, L"smt") == 0)
    _solutionType=SolutionType::STATIC_MT;
  else if (_wcsicmp(pszParam, L"smtd") == 0)
    _solutionType=SolutionType::STATIC_MTD;
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
    _quantumDepth=QuantumDepth::Q8;
  else if (_wcsicmp(pszParam, L"Q16") == 0)
    _quantumDepth=QuantumDepth::Q16;
  else if (_wcsicmp(pszParam, L"Q32") == 0)
    _quantumDepth=QuantumDepth::Q32;
  else if (_wcsicmp(pszParam, L"Q64") == 0)
    _quantumDepth=QuantumDepth::Q64;
  else if (_wcsicmp(pszParam, L"x86") == 0)
    _platform=Platform::X86;
  else if (_wcsicmp(pszParam, L"x64") == 0)
    _platform=Platform::X64;
  else if (_wcsicmp(pszParam, L"VS2015") == 0)
    _visualStudioVersion=VisualStudioVersion::VS2015;
  else if (_wcsicmp(pszParam, L"VS2017") == 0)
    _visualStudioVersion=VisualStudioVersion::VS2017;
  else if (_wcsicmp(pszParam, L"VS2019") == 0)
    _visualStudioVersion=VisualStudioVersion::VS2019;
  else if (_wcsicmp(pszParam, L"VS2022") == 0)
    _visualStudioVersion=VisualStudioVersion::VS2022;
  else if (_wcsicmp(pszParam, L"zeroConfigurationSupport") == 0)
    _zeroConfigurationSupport=true;
}
