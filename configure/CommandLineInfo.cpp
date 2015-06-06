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
#include "CommandLineInfo.h"
#include "Shared.h"

CommandLineInfo::CommandLineInfo(const ConfigureWizard &wizard)
{
  _build64bit=wizard.build64bit();
  _includeIncompatibleLicense=wizard.includeIncompatibleLicense();
  _includeOptional=wizard.includeOptional();
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

bool CommandLineInfo::includeIncompatibleLicense() const
{
  return(_includeIncompatibleLicense);
}

bool CommandLineInfo::includeOptional() const
{
  return(_includeOptional);
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

void CommandLineInfo::ParseParam(const char* pszParam, BOOL bFlag, BOOL bLast)
{
  if (!bFlag)
    return;

  if (_strcmpi(pszParam, "dmt") == 0)
    _solutionType=DYNAMIC_MT;
  else if (_strcmpi(pszParam, "smt") == 0)
    _solutionType=STATIC_MT;
  else if (_strcmpi(pszParam, "smtd") == 0)
    _solutionType=STATIC_MTD;
  else if (_strcmpi(pszParam, "incompatibleLicense") == 0)
    _includeIncompatibleLicense=true;
  else if (_strcmpi(pszParam, "noHdri") == 0)
    _useHDRI=false;
  else if (_strcmpi(pszParam, "noOpenMP") == 0)
    _useOpenMP=false;
  else if (_strcmpi(pszParam, "noWizard") == 0)
    _noWizard=true;
  else if (_strcmpi(pszParam, "openCL") == 0)
    _useOpenCL=true;
  else if (_strcmpi(pszParam, "optional") == 0)
    _includeOptional=true;
  else if (_strcmpi(pszParam, "Q8") == 0)
    _quantumDepth=Q8;
  else if (_strcmpi(pszParam, "Q16") == 0)
    _quantumDepth=Q16;
  else if (_strcmpi(pszParam, "Q32") == 0)
    _quantumDepth=Q32;
  else if (_strcmpi(pszParam, "Q64") == 0)
    _quantumDepth=Q64;
  else if (_strcmpi(pszParam, "x64") == 0)
    _build64bit=true;
  else if (_strcmpi(pszParam, "VS2002") == 0)
    _visualStudioVersion=VS2002;
  else if (_strcmpi(pszParam, "VS2010") == 0)
    _visualStudioVersion=VS2010;
  else if (_strcmpi(pszParam, "VS2012") == 0)
    _visualStudioVersion=VS2012;
}
