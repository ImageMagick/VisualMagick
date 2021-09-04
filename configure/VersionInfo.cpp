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
#include "VersionInfo.h"
#include "Shared.h"

VersionInfo::VersionInfo()
{
}

wstring VersionInfo::interfaceVersion() const
{
  return(_libraryCurrent);
}

wstring VersionInfo::libAddendum() const
{
  return(L"-"+_patchlevel);
}

wstring VersionInfo::libVersion() const
{
  return(_libVersion);
}

wstring VersionInfo::libVersionNumber() const
{
  return(_libraryCurrent+L","+_libraryRevision+L","+_libraryAge);
}

bool VersionInfo::load()
{
  wifstream
    version;

  wstring
    line;

  version.open("..\\..\\ImageMagick\\m4\\version.m4");
  if (!version)
    return(false);

  while (getline(version,line))
  {
    loadValue(line,L"_major_version",&_major);
    loadValue(line,L"_minor_version",&_minor);
    loadValue(line,L"_micro_version",&_micro);
    loadValue(line,L"_patchlevel_version",&_patchlevel);
    loadValue(line,L"_library_current",&_libraryCurrent);
    loadValue(line,L"_library_revision",&_libraryRevision);
    loadValue(line,L"_library_age",&_libraryAge);
    loadValue(line,L"_lib_version",&_libVersion);
    loadValue(line,L"pp_library_current",&_ppLibraryCurrent);
    loadValue(line,L"pp_library_revision",&_ppLibraryRevision);
    loadValue(line,L"pp_library_age",&_ppLibraryAge);
  }

  version.close();

  return(_major != L"" && _minor != L"" && _micro != L"" && _patchlevel != L"" && _libraryCurrent != L"" &&
         _libraryRevision != L"" && _libraryAge != L"" && _libVersion != L"" && _ppLibraryCurrent != L"" &&
         _ppLibraryRevision != L"" && _ppLibraryAge != L"");
}

void VersionInfo::loadValue(const wstring line,const wstring keyword,wstring *value)
{
  size_t
    index;

  wstring
    line_start;

  line_start=L"m4_define([magick"+keyword+L"], [";
  index=line.find(line_start);
  if (index == string::npos)
    return;
  *value=line.substr(line_start.length(),line.length()-line_start.length()-2);
}

wstring VersionInfo::majorVersion() const
{
  return(_major);
}

wstring VersionInfo::ppLibVersionNumber() const
{
  return(_ppLibraryCurrent+L":"+_ppLibraryRevision+L":"+_ppLibraryAge);
}

wstring VersionInfo::ppInterfaceVersion() const
{
  return(_ppLibraryCurrent);
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

wstring VersionInfo::versionString() const
{
  return _major+L"."+_minor+L"."+_micro+L"-"+_patchlevel;
}