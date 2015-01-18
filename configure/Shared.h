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
#ifndef __Shared__
#define __Shared__

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

enum {DYNAMIC_MT, STATIC_MTD, STATIC_MT};

enum {UNDEFINEDTYPE, APPTYPE, COMTYPE, DLLTYPE, DLLMODULETYPE, EXETYPE, EXEMODULETYPE, STATICTYPE};

enum {VS2002, VS2010, VS2012};

enum {Q8, Q16, Q32, Q64};

#define foreach(type,var,collection) for(std::vector<type>::iterator var = collection.begin(); var != collection.end(); var++)
#define foreach_const(type,var,collection) for(std::vector<type>::const_iterator var = collection.begin(); var != collection.end(); var++)

static inline bool contains(vector<string> &container, const string s)
{
  foreach(string,c,container)
  {
    if (*c == s)
      return(true);
  }
  return(false);
}

static inline bool endsWith(const std::string &s,const std::string &end)
{
  size_t
    index;

  index=s.rfind(end);
  if (index == string::npos)
    return false;

  return(s.substr(index) == end);
}

static inline string trim(const string &s)
{
  string
    result;

  result=s;
  result.erase(result.begin(), find_if(result.begin(), result.end(), not1(ptr_fun<int, int>(isspace))));
  result.erase(find_if(result.rbegin(), result.rend(), not1(ptr_fun<int, int>(isspace))).base(), result.end());
  return(result);
}

static inline string readLine(ifstream &stream)
{
  string
    line;

  getline(stream,line);
  return(trim(line));
}

static inline string replace(const std::string &str, const std::string &from, const std::string &to)
{
  size_t
    index;

  string
    result;

  result=str;
  index=result.find(from);
  if (index == string::npos)
    result.replace(index,from.length(),to);
  return(result);
}

#endif // __Shared__