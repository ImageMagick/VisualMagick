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
#ifndef __ConfigureWizard__
#define __ConfigureWizard__

#include "Pages\WelcomePage.h"
#include "Pages\TargetPage.h"
#include "Pages\SystemPage.h"
#include "Pages\FinishedPage.h"

class CommandLineInfo;

class ConfigureWizard : public CPropertySheet
{
  DECLARE_DYNAMIC(ConfigureWizard)

public:
  ConfigureWizard(CWnd* pWndParent = (CWnd *) NULL);

  virtual ~ConfigureWizard();

  string binDirectory() const;

  bool build64bit() const;

  bool includeIncompatibleLicense() const;

  bool includeOptional() const;

  string intermediateDirectoryDebug() const;

  string intermediateDirectoryRelease() const;

  string libDirectory() const;

  string openCLIncludePath() const;

  string platform() const;

  int quantumDepth() const;

  string solutionName() const;

  int solutionType() const;

  bool useHDRI() const;

  bool useOpenCL() const;

  bool useOpenMP() const;

  int visualStudioVersion() const;

  void parseCommandLineInfo(const CommandLineInfo &info);

protected:

  DECLARE_MESSAGE_MAP()

private:

  FinishedPage _finishedPage;
  SystemPage   _systemPage;
  TargetPage   _targetPage;
  WelcomePage  _welcomePage;
};

#endif // __ConfigureWizard__
