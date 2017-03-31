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
#ifndef __TargetPage__
#define __TargetPage__

class TargetPage : public CPropertyPage
{
  DECLARE_DYNCREATE(TargetPage)

public:

  TargetPage();

  ~TargetPage();

  bool build64bit() const;
  void build64bit(bool value);

  bool excludeDeprecated() const;
  void excludeDeprecated(bool value);

  bool includeIncompatibleLicense() const;
  void includeIncompatibleLicense(bool value);

  bool includeOptional() const;
  void includeOptional(bool value);

  bool installedSupport() const;
  void installedSupport(bool value);

  wstring openCLIncludePath() const;

  int quantumDepth() const;
  void quantumDepth(int value);

  int solutionType() const;
  void solutionType(int value);

  bool useHDRI() const;
  void useHDRI(bool value);

  bool useOpenCL() const;
  void useOpenCL(bool value);

  bool useOpenMP() const;
  void useOpenMP(bool value);

  int visualStudioVersion() const;
  void visualStudioVersion(int value);

protected:

  virtual void DoDataExchange(CDataExchange* pDX);

  virtual BOOL OnInitDialog();

  DECLARE_MESSAGE_MAP()

private:

  wstring getEnvironmentVariable(const wchar_t *name);

  bool hasVisualStudioFolder(const wchar_t *name);

  bool openCLIncludePathExists(const wchar_t *name);

  void setOpenCLIncludePath();

  void setVisualStudioVersion();

  BOOL    _build64bit;
  BOOL    _excludeDeprecated;
  BOOL    _includeIncompatibleLicense;
  BOOL    _includeOptional;
  BOOL    _installedSupport;
  wstring _openCLIncludePath;
  int     _quantumDepth;
  int     _solutionType;
  BOOL    _useHDRI;
  BOOL    _useOpenCL;
  BOOL    _useOpenMP;
  int     _visualStudioVersion;
};

#endif // __TargetPage__
