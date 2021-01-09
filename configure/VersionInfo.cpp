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
  return(L"-"+_patchlevel);
}

wstring VersionInfo::libVersion() const
{
  return L"0x"+_major+_minor+L"A";
}

bool VersionInfo::load()
{
  size_t
    index;

  wifstream
    configure,
    version;

  wstring
    line;

  configure.open("..\\..\\ImageMagick\\configure.ac");
  if (!configure)
    return(false);

  while (getline(configure,line))
  {
    loadValue(line,L"major",&_major);
    loadValue(line,L"minor",&_minor);
    loadValue(line,L"micro",&_micro);
    loadValue(line,L"patchlevel",&_patchlevel);
  }

  configure.close();

  version.open("..\\..\\ImageMagick\\version.sh");
  if (!version)
    return(false);

  while (getline(version,line))
  {
    index=line.find(L"MAGICK_LIBRARY_CURRENT=");
    if (index != string::npos)
      _interfaceVersion=line.substr(23,line.length()-23);
  }

  version.close();

  return(_major != L"" && _minor != L"" && _micro != L"" && _patchlevel != L"" && _interfaceVersion != L"");
}

void VersionInfo::loadValue(const wstring line,const wstring keyword,wstring *value)
{
  size_t
    index;

  wstring
    line_start;

  line_start=L"m4_define([magick_"+keyword+L"_version], [";
  index=line.find(line_start);
  if (index == string::npos)
    return;
  *value=line.substr(line_start.length(),line.length()-line_start.length()-2);
}

wstring VersionInfo::majorVersion() const
{
  return(_major);
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
  (void) wcsftime(buffer,11,L"%Y-%m-%d",&tm);

  return(wstring(buffer));
}

wstring VersionInfo::version() const
{
  return _major+L"."+_minor+L"."+_micro;
}

wstring VersionInfo::versionNumber() const
{
  return _major+L","+_minor+L","+_micro+L","+_patchlevel;
}