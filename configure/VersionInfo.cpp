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
#include "VersionInfo.h"
#include "Shared.h"

VersionInfo::VersionInfo()
{
}

wstring VersionInfo::interfaceMinVersion() const
{
  return(_interfaceVersion);
}

wstring VersionInfo::interfaceVersion() const
{
  return(_interfaceVersion);
}

wstring VersionInfo::libAddendum() const
{
  return(L"-"+_release);
}

wstring VersionInfo::libVersion() const
{
  vector<wstring>
    result;

  wstring
    line;

  wstringstream
    inpput(_version),
    output;

  output << L"0x";

  while(std::getline(inpput,line,L'.'))
    output << std::uppercase << std::hex << std::stoi(line);

  return(output.str());
}

bool VersionInfo::load()
{
  wifstream
    version;

  size_t
    index;

  wstring
    line;

  version.open("..\\..\\ImageMagick\\version.sh");
  if (!version)
    return(false);

  while (getline(version,line))
  {
    index=line.find(L"PACKAGE_VERSION=");
    if (index != string::npos)
      _version=line.substr(17,line.length()-18);

    index=line.find(L"PACKAGE_RELEASE=");
    if (index != string::npos)
      _release=line.substr(17,line.length()-18);

    index=line.find(L"MAGICK_LIBRARY_CURRENT=");
    if (index != string::npos)
      _interfaceVersion=line.substr(23,line.length()-23);
  }

  version.close();

  return(true);
}

wstring VersionInfo::majorVersion() const
{
  size_t
    index;

  index=_version.find(L".");
  return(_version.substr(0,index));
}

wstring VersionInfo::releaseDate() const
{
  wchar_t
    buffer[11];

  struct tm
    tm;

  time_t
    t;

  time(&t);
  (void) localtime_s(&tm,&t);
  (void) wcsftime(buffer, sizeof(buffer), L"%Y-%m-%d", &tm);

  return(wstring(buffer));
}

wstring VersionInfo::version() const
{
  return(_version);
}

wstring VersionInfo::versionNumber() const
{
  return(replace(_version,L".",L",")+L","+_release);
}