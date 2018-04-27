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
#ifndef __Shared__
#define __Shared__

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

enum {DYNAMIC_MT, STATIC_MTD, STATIC_MT};

enum {UNDEFINEDTYPE, APPTYPE, COMTYPE, DLLTYPE, DLLMODULETYPE, EXETYPE, EXEMODULETYPE, STATICTYPE};

enum {VS2002, VS2010, VS2012, VS2013, VS2015, VS2017};

enum {Q8, Q16, Q32, Q64};

static const wstring validSrcFilesArr[] = {L".c", L".cpp", L".cc"};
const vector<wstring> validSrcFiles(validSrcFilesArr, std::end(validSrcFilesArr));

#define foreach(type,var,collection) for(std::vector<type>::iterator var = collection.begin(); var != collection.end(); var++)
#define foreach_const(type,var,collection) for(std::vector<type>::const_iterator var = collection.begin(); var != collection.end(); var++)

static inline bool contains(const vector<wstring> &container, const wstring s)
{
  foreach_const(wstring,c,container)
  {
    if (*c == s)
      return(true);
  }
  return(false);
}

static inline bool endsWith(const wstring &s,const wstring &end)
{
  size_t
    index;

  index=s.rfind(end);
  if (index == wstring::npos)
    return false;

  return(s.substr(index) == end);
}

static inline bool startsWith(const wstring &s,const wstring &start)
{
  size_t
    index;

  index=s.rfind(start);
  return(index != wstring::npos);
}

static inline wstring trim(const wstring &s)
{
  wstring
    result;

  result=s;
  result.erase(result.begin(), find_if(result.begin(), result.end(), not1(ptr_fun<int, int>(isspace))));
  result.erase(find_if(result.rbegin(), result.rend(), not1(ptr_fun<int, int>(isspace))).base(), result.end());
  return(result);
}

static inline wstring readLine(wifstream &stream)
{
  wstring
    line;

  getline(stream,line);
  return(trim(line));
}

static inline wstring replace(const wstring &str, const wstring &from, const wstring &to)
{
  size_t
    index;

  wstring
    result;

  result=str;
  index=result.find(from);
  while (index != string::npos)
  {
    result.replace(index,from.length(),to);
    index=result.find(from);
  }
  return(result);
}

static inline int parseVisualStudioVersion(const wstring &version)
{
  if (version == L"2010")
    return(VS2010);
  else if (version == L"2012")
    return(VS2012);
  else if (version == L"2013")
    return(VS2013);
  else if (version == L"2015")
    return(VS2015);
  else if (version == L"2017")
    return(VS2017);

  return(VS2002);
}

static inline bool isValidSrcFile(const wstring &fileName)
{
  foreach_const(wstring,ext,validSrcFiles)
  {
    if (endsWith(fileName,*ext))
      return(true);
  }

  return(false);
}

#endif // __Shared__