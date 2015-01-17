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
#ifndef __SystemPage__
#define __SystemPage__

class SystemPage : public CPropertyPage
{
  DECLARE_DYNCREATE(SystemPage)

public:

  SystemPage();

  ~SystemPage();

  string binDirectory() const;

  string intermediateDirectoryDebug() const;

  string intermediateDirectoryRelease() const;

  string libDirectory() const;

  virtual BOOL OnSetActive();

protected:

  virtual void DoDataExchange(CDataExchange* pDX);

  afx_msg void OnBinDirBrowse();

  virtual BOOL OnInitDialog();

  afx_msg void OnIntermediateDirectoryDebugBrowse();

  afx_msg void OnIntermediateDirectoryReleaseBrowse();

  afx_msg void OnLibDirBrowse();

  DECLARE_MESSAGE_MAP()

private:

  string addBackslash(const CString &directory) const;

  bool browseForFolder(const char *title, char *retval);

  void setDirectory(const string &title,CString &directory);

  CString _binDirectory;
  CString _intermediateDirectoryDebug;
  CString _intermediateDirectoryRelease;
  CString _libDirectory;
};

#endif // __SystemPage__
