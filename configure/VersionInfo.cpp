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
#include "VersionInfo.h"
#include "Shared.h"

VersionInfo::VersionInfo()
{
}

string VersionInfo::libAddendum() const
{
  return("-"+_release);
}

string VersionInfo::libVersion() const
{
  return("0x"+replace(_version,".",""));
}

bool VersionInfo::load()
{
  ifstream
    version;

  size_t
    index;

  string
    line;

  version.open("..\\..\\ImageMagick\\version.sh");
  if (!version)
    return(false);

  while (getline(version,line))
  {
    index=line.find("PACKAGE_VERSION=");
    if (index != string::npos)
      _version=line.substr(17,line.length()-18);

    index=line.find("PACKAGE_RELEASE=");
    if (index != string::npos)
      _release=line.substr(17,line.length()-18);
  }

  return(true);
}

string VersionInfo::majorVersion() const
{
  size_t
    index;

  index=_version.find(".");
  return(_version.substr(0,index));
}

string VersionInfo::releaseDate() const
{
  char
    buffer[11];

  struct tm
    tm;

  time_t
    t;

  time(&t);
  (void) localtime_s(&tm,&t);
  (void) strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tm);

  return(string(buffer));
}

string VersionInfo::version() const
{
  return(_version);
}

string VersionInfo::versionNumber() const
{
  return(replace(_version,".",",")+","+_release);
}