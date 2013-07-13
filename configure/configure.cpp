// Configure.cpp : Defines the class behaviors for the application.
//
//
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS

#ifndef __NO_MFC__
#include "stdafx.h"
#endif // __NO_MFC__
#include "configure.h"
#ifndef __NO_MFC__
#include "configure_wizard.h"
#include "WaitDlg.h"
#else // __NO_MFC__
#include <tchar.h>
#include <stdio.h>
#include <shlobj.h>
#endif // __NO_MFC__
//#include "CStringEx.h"

#define SYMBOLS_ALWAYS 1

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
ofstream debuglog;
#endif

const char *MODULE_PREFIX = "IM_MOD_";

// /base:"0x66200000"
unsigned long dllbaselocation = 0x67000000L;

/////////////////////////////////////////////////////////////////////////////
// CConfigureApp

#ifndef __NO_MFC__
BEGIN_MESSAGE_MAP(CConfigureApp, CWinApp)
  //{{AFX_MSG_MAP(CConfigureApp)
  //}}AFX_MSG
  ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()
#endif // __NO_MFC__

/////////////////////////////////////////////////////////////////////////////
// CConfigureApp construction

CConfigureApp::CConfigureApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CConfigureApp object

CConfigureApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CConfigureApp initialization

BOOL useX11Stubs = TRUE;
BOOL decorateFiles = FALSE;
BOOL optionalFiles = FALSE;
bool consoleMode = true;
BOOL standaloneMode = FALSE;
BOOL onebigdllMode = FALSE;
//BOOL generateFontmap = FALSE;
BOOL visualStudio7 = TRUE;
BOOL build64Bit = FALSE;
BOOL openMP = TRUE;
BOOL m_bigCoderDLL = FALSE;
#ifdef __NO_MFC__
int projectType = MULTITHREADEDSTATIC;
#endif // __NO_MFC__

string release_loc;
string debug_loc;
string bin_loc;
string lib_loc;

string final_release_loc;
string final_debug_loc;
string final_bin_loc;
string final_lib_loc;

list<ConfigureProject*> dependency_list;

//
// Get the full path to a file. The first argument is the
// relative path to the DSP directory within the tree. and
// the second argument is the relative path to the actual
// file or directory. All paths are relative to the place
// were configure runs from.
//
string get_full_path(string root,string part)
{
  if (!standaloneMode)
    {
#ifdef _DEBUG
      debuglog  << "get_full_path A"
                << "root:" << root.c_str() << ","
                << "return part:" << part.c_str() << endl;
#endif
      return part;
    }
  else
    {
      char full[_MAX_PATH];
      string combined;

      combined = root + part;

      if( _fullpath( full, combined.c_str(), _MAX_PATH ) != NULL )
        {
#ifdef _DEBUG
          debuglog  << "get_full_path B"
                    << "combined:" << combined.c_str() << ","
                    << "return full:" << full << endl;
#endif
          return full;
        }
      else
        {
#ifdef _DEBUG
          debuglog  << "get_full_path C"
                    << "root:" << root.c_str() << ","
                    << "return part:" << part.c_str() << endl;
#endif
          return part;
        }
    }
}

string get_project_name(int project, int runtime,
                        string root, string prefix, string name)
{
  string filename = root;
  filename += "\\";
  filename += prefix.c_str();
  filename += name.c_str();

  switch (project)
    {
    case DLLPROJECT:
      {
        switch(runtime)
          {
          case SINGLETHREADEDSTATIC:
            filename += "_st_dll";  // should never happen
            break;
          case MULTITHREADEDSTATIC:
            filename += "_xt_dll";  // should never happen
            break;
          case MULTITHREADEDSTATICDLL:
            filename += "_mtdll_dll";  // should never happen
            break;
          default:
          case MULTITHREADEDDLL:
            filename += "_mt_dll";
            break;
          }
        break;
      }
    case LIBPROJECT:
      {
        switch(runtime)
          {
          case SINGLETHREADEDSTATIC:
            filename += "_st_lib";
            break;
          case MULTITHREADEDSTATIC:
            filename += "_xt_lib";
            break;
          case MULTITHREADEDSTATICDLL:
            filename += "_mtdll_lib";
            break;
          default:
          case MULTITHREADEDDLL:
            filename += "_mt_lib";
            break;
          }
        break;
      }
    case EXEPROJECT:
      {
        switch(runtime)
          {
          case SINGLETHREADEDSTATIC:
            filename += "_st_exe";
            break;
          case MULTITHREADEDSTATIC:
            filename += "_xt_exe";
            break;
          case MULTITHREADEDSTATICDLL:
            filename += "_mtdll_exe";
            break;
          default:
          case MULTITHREADEDDLL:
            filename += "_mt_exe";
            break;
          }
      }
      break;
    }
  if (visualStudio7)
    filename += ".vcproj";
  else
    filename += ".dsp";
  return filename;
}

void LocalMakeLower(string &s)
{
  char* pszTemp = new char[s.length() + 1];
  _tcscpy(pszTemp, s.c_str());
  _tcslwr(pszTemp);
  s = pszTemp;
  delete [] pszTemp;
  return;
};

int LocalFindNoCase( string &a, char *b, int startpos )
{
  string sa = a;
  LocalMakeLower(sa);

  string sb = b;
  LocalMakeLower(sb);

  return sa.find( sb, startpos );
}

int LocalCompareNoCase( string &a, char *b )
{
  string sa = a;
  LocalMakeLower(sa);

  string sb = b;
  LocalMakeLower(sb);

  return sa.compare( sb );
}

int LocalFind( string &a, char *b, int startpos )
{
  return a.find( b, startpos );
}

void LocalFormat(string &a, const char *s, ... )
{
  va_list vl;
  va_start(vl, s);

  char *pszTemp = NULL;
  int nBufferSize = 0;
  int nRetVal = -1;

  do
    {
      // Increment the buffer 100 characters at a time
      // I'm not sure whether this is robust!
      nBufferSize += 1024;
      if (pszTemp)
        delete [] pszTemp;
      pszTemp = new char [nBufferSize];
      nRetVal = _vstprintf(pszTemp, s, vl);
    } while (nRetVal < 0);

  a = pszTemp;
  if (pszTemp)
    delete [] pszTemp;

  va_end(vl);
  return;
};

int LocalGetFieldCount( string &s, const TCHAR* psz )
{
  int len = _tcslen( psz );
  int pos = 0;
  int iCount = 1;

  while( (pos = s.find(psz, pos)) >= 0 )
    {
      pos += len;
      iCount++;
    }
  return iCount;
};

int LocalGetFieldCount( string &s, const TCHAR& ch )
{
  int pos = 0;
  int iCount = 1;

  while( (pos = s.find(ch, pos)) >= 0 )
    {
      pos++;
      iCount++;
    }
  return iCount;
};

string LocalGetField( string &s, const TCHAR* delim, int fieldnum )
{
  int lenDelim = _tcslen( delim );
  int pos = 0, term = 0;
  int retlen = 0, retpos = 0;

  while( fieldnum-- >= 0 )
    {
      term = s.find(delim, pos);
      if( term >= 0 )
        {
          retpos = pos;
          retlen = term - pos;
          pos = term + lenDelim;
        }
      else
        {
          retpos = pos;
          retlen = s.length() - pos;
          break;
        }
    }
  return s.substr(retpos, retlen);
};

string LocalGetField( string &s, const TCHAR& ch, int fieldnum )
{
  int pos = 0, term = 0;
  int retlen = 0, retpos = 0;

  while( fieldnum-- >= 0 )
    {
      term = s.find(ch, pos);
      if( term >= 0 )
        {
          retpos = pos;
          retlen = term - pos;
          pos = term + 1;
        }
      else
        {
          retpos = pos;
          retlen = s.length() - pos;
          break;
        }
    }
  return s.substr(retpos, retlen);
};

void CConfigureApp::generate_a_dependency( ConfigureWorkspace *w,ConfigureProject *p,
                                           char *s, bool flag1, bool flag2 )
{
  bool bDidSomething = false;
  string strDepends;
  string strName;
  for (
       list<ConfigureProject*>::iterator depit = dependency_list.begin();
       depit != dependency_list.end();
       depit++)
    {
      strName = (*depit)->name;
      if (flag1)
        {
          if (LocalFindNoCase(strName,"CORE_%szlib",0) == 0)
            continue;
          if (LocalFindNoCase(strName,"CORE_%sbzlib",0) == 0)
            continue;
          if (LocalFindNoCase(strName,"CORE_%sjpeg",0) == 0)
            continue;
        }
      if (LocalFindNoCase(strName,s,0) == 0)
        {
          // NOTE: This is case sensitive - so be warned!
          if (flag2)
            {
              if (LocalFind(strName,"CORE_%sMagickCore",0) == 0)
                continue;
              if (LocalFindNoCase(strName,"CORE_%sMagick++",0) == 0)
                continue;
            }
          LocalFormat(strDepends,strName.c_str(),"","");
          w->write_project_dependency(p,strDepends);
          bDidSomething = true;
        }
    }
#ifdef _DEBUG
  if (!bDidSomething)
    debuglog  << "gen dependency did nothing for: " << s << endl;
#endif
}

void CConfigureApp::generate_a_dependency_cs( ConfigureWorkspace *w,
                                              ConfigureProject *p,
                                              char *s )
{
  bool bDidSomething = false;
  string strDepends;
  string strName;
  for (
       list<ConfigureProject*>::iterator depit = dependency_list.begin();
       depit != dependency_list.end();
       depit++)
    {
      strName = (*depit)->name;
      if (LocalFind(strName,s,0) == 0)
        {
          LocalFormat(strDepends,strName.c_str(),"","");
          w->write_project_dependency(p,strDepends);
          bDidSomething = true;
        }
    }
#ifdef _DEBUG
  if (!bDidSomething)
    debuglog  << "gen dependency cs did nothing for: " << s << endl;
#endif
}

void CConfigureApp::generate_a_dependency_type( ConfigureWorkspace *w,
                                                ConfigureProject *p,
                                                int type)
{
  bool bDidSomething = false;
  string strDepends;
  string strName;
  int nProjectType;
  for (
       list<ConfigureProject*>::iterator depit = dependency_list.begin();
       depit != dependency_list.end();
       depit++)
    {
      strName = (*depit)->name;
      nProjectType = (*depit)->type;
      if (nProjectType == type)
        {
          LocalFormat(strDepends,strName.c_str(),"","");
          w->write_project_dependency(p,strDepends);
          bDidSomething = true;
        }
    }
#ifdef _DEBUG
  if (!bDidSomething)
    debuglog  << "gen dependency type did nothing for: " << type << endl;
#endif
}

void CConfigureApp::generate_dependencies( ConfigureProject *p,
                                           bool add_cpp_depends,
                                           bool add_util_depends,
                                           bool add_add_one_depdends)
{
  generate_a_dependency(workspace, p, "CORE_%szlib", false, true);
  generate_a_dependency(workspace, p, "CORE_%sbzlib", false, true);
  generate_a_dependency(workspace, p, "CORE_%sjpeg", false, true);
  generate_a_dependency(workspace, p, "LIBR_", true, false);
  generate_a_dependency(workspace, p, "CORE_", true, true);
  generate_a_dependency_cs(workspace, p, "CORE_%sMagickCore");
  generate_a_dependency_cs(workspace, p, "CORE_%sMagickWand");
  if (add_cpp_depends)
    generate_a_dependency_cs(workspace, p, "CORE_%sMagick++");
  generate_a_dependency(workspace, p, (char *)MODULE_PREFIX, true, false);
  if (add_util_depends)
    generate_a_dependency(workspace, p, "UTIL_", true, false);
  if (add_add_one_depdends)
    generate_a_dependency_type(workspace, p, ADD_ON);
}

static bool doesDirExist(const char *name)
{
  // check to see if the path exists
  string libpath;
  WIN32_FIND_DATA libdata;
  HANDLE libhandle = FindFirstFile(name, &libdata);
  if (libhandle != INVALID_HANDLE_VALUE)
    {
      FindClose(libhandle);
      return ((libdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ==
              FILE_ATTRIBUTE_DIRECTORY);
    }
  return false;
}

static void add_includes( list<string> &includes_list,
                          string &libpath, int levels )
{
  WIN32_FIND_DATA libdata;
  HANDLE libhandle = FindFirstFile(libpath.c_str(), &libdata);
  if (libhandle != INVALID_HANDLE_VALUE)
    {
      FindClose(libhandle);
      if ((libdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
        {
          string ipath = "..\\";
          for (int i=0; i<levels; i++)
            ipath += "..\\";
          ipath += libpath;
          includes_list.push_back(ipath);

          ipath = libpath;
          ipath += "\\*";
          libhandle = FindFirstFile(ipath.c_str(), &libdata);
          if (libhandle != INVALID_HANDLE_VALUE)
            {
              static const char *exclude_from_build[] =
                {
                  ".",
                  "..",
                  "CVS",
                  NULL
                };

              do
                {
                  bool skip = false;

                  for (int i=0; exclude_from_build[i] != NULL; i++)
                    if (_stricmp(libdata.cFileName, exclude_from_build[i]) == 0)
                      skip = true;

                  if (skip) continue;

                  if ((libdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
                    {
                      ipath = "..\\";
                      ipath += libpath;
                      ipath += "\\";
                      ipath += libdata.cFileName;
                      if (doesDirExist(ipath.c_str()))
                        {
                          ipath = libpath;
                          ipath += "\\";
                          ipath += libdata.cFileName;
                          add_includes(includes_list, ipath, levels);
                        }
                    }
                } while (FindNextFile(libhandle, &libdata));
              FindClose(libhandle);
            }
        }
    }
}

bool CConfigureApp::process_one_entry(const char *entry, int nLinesRead, int runtime)
{
  string sTempString = entry;
  // check for a valid file by looking for the magick
  // sequence in the first four characters of the file.
  if ((nLinesRead == 0) &&
      (sTempString.substr(0, 4) != "#ECF"))
    return true;
  // skip any blank lines
  if (sTempString.length() <= 0)
    return true;
  // skip over any comments first - we allow three types
  if (sTempString[0] == '#')
    return true;
  if (sTempString.substr(0, 3) == "rem")
    return true;
  if (sTempString.substr(0, 3) == ";")
    return true;
  // now see if the line will parse as a name=value pair
  string sName = LocalGetField(sTempString," = ",0);
  string sValue = LocalGetField(sTempString," = ",1);
  if (!sName.empty() && !sValue.empty())
    {
      string temp;
      temp = sValue;
      if (_tcsicmp(sName.c_str(), "EXPORTSDEF") == 0)
        module_definition_file=temp;
      else if (_tcsicmp(sName.c_str(), "DEFINE") == 0)
        defines_list.push_back(temp);
      else if (_tcsicmp(sName.c_str(), "INCLUDE") == 0)
        includes_list.push_back(temp);
      else if (_tcsicmp(sName.c_str(), "SOURCE") == 0)
        source_list.push_back(temp);
      else if (_tcsicmp(sName.c_str(), "RESOURCE") == 0)
        resource_list.push_back(temp);
      else if (_tcsicmp(sName.c_str(), "EXCLUDE") == 0)
        exclude_list.push_back(temp);
      else if (_tcsicmp(sName.c_str(), "LIBRARY") == 0)
        {
          lib_release_list.push_back(temp);
          lib_debug_list.push_back(temp);
        }
      else if (_tcsicmp(sName.c_str(), "LIBRELEASE") == 0)
        {
          lib_release_list.push_back(temp);
        }
      else if (_tcsicmp(sName.c_str(), "LIBDEBUG") == 0)
        {
          lib_debug_list.push_back(temp);
        }
      else if (_tcsicmp(sName.c_str(), "SUBSYSTEM") == 0)
        {
          if (_tcsicmp(sValue.c_str(), "CONSOLE") == 0)
            consoleMode = true;
          else if (_tcsicmp(sValue.c_str(), "WINDOWS") == 0)
            consoleMode = false;
        }
      else if ((build64Bit == TRUE) && (_tcsicmp(sName.c_str(), "OBJECT64") == 0))
        object_list.push_back(temp);
      else if ((build64Bit == FALSE) && (_tcsicmp(sName.c_str(), "OBJECT32") == 0))
        object_list.push_back(temp);
      else if ((build64Bit == TRUE) && (_tcsicmp(sName.c_str(), "SOURCE64") == 0))
        source_list.push_back(temp);
      else if ((build64Bit == FALSE) && (_tcsicmp(sName.c_str(), "SOURCE32") == 0))
        source_list.push_back(temp);
      else if ((build64Bit == TRUE) && (_tcsicmp(sName.c_str(), "PREBUILD64") == 0))
        prebuild_cmd_list.push_back(temp);
      else if ((build64Bit == FALSE) && (_tcsicmp(sName.c_str(), "PREBUILD32") == 0))
        prebuild_cmd_list.push_back(temp);
      else if (_tcsicmp(sName.c_str(), "FILTER_OPENCL_SOURCE") == 0)
        filter_opencl_list.push_back(temp);

      switch(runtime)
        {
        default:
        case SINGLETHREADEDSTATIC:
        case MULTITHREADEDSTATIC:
        case MULTITHREADEDSTATICDLL:
          if (_tcsicmp(sName.c_str(), "DEFINESTATIC") == 0)
            defines_list.push_back(temp);
          break;
        case MULTITHREADEDDLL:
          if (_tcsicmp(sName.c_str(), "DEFINEDLL") == 0)
            defines_list.push_back(temp);
          break;
        }
    }
  return true;
}

int CConfigureApp::load_environment_file(const char *inputfile, int runtime)
{
  char szBuf[2048];
  int nLinesRead = 0;
  ifstream inpStream;
  inpStream.open( inputfile, ios::in );
  if ( inpStream ) {
    for (;;) {
      inpStream.getline( szBuf, 2047, '\n' );
      if ( inpStream.eof() ) {
        // last line may contain text also
        // (if it's not terminated with '\n' EOF is returned)
        return process_one_entry( szBuf, nLinesRead, runtime );
      }
      if ( !inpStream.good() ||
           !process_one_entry( szBuf, nLinesRead, runtime) )
        return false;
      nLinesRead++;
    }
  }
  inpStream.close();
  inpStream.clear();
  return true;
}

void CConfigureApp::process_utility(
  const char *root, const char *filename, int runtime, int project_type)
{
  string basename = filename;
  string name = LocalGetField(basename,'.',0);
  string extn = LocalGetField(basename,'.',1);
  if(extn.length() > 3)
    return;
  string prefix = "UTIL_";
  string staging = root;
  int levels;

  lib_release_list.clear();
  lib_debug_list.clear();
  includes_list.clear();
  defines_list.clear();
  source_list.clear();
  resource_list.clear();
  exclude_list.clear();
  object_list.clear();
  prebuild_cmd_list.clear();
  additional_libdir_list.clear();

  string envpath;
  string search;
  envpath = staging;
  if (project_type == UTILITY)
    {
      envpath += "\\UTILITY.txt";
      search = "";
      consoleMode = true; /* set the default */
      load_environment_file(envpath.c_str(), runtime);
    }
  else
    {
      envpath += "\\PROJECT.txt";
      search = "*";
      consoleMode = false; /* set the default */
      load_environment_file(envpath.c_str(), runtime);
    }

  for (list<string>::iterator it = exclude_list.begin();
       it != exclude_list.end();
       it++)
    {
      const char *s = (*it).c_str();
      if (_stricmp(filename, s) == 0)
        return;
    }

  if (runtime == MULTITHREADEDDLL)
    {
      defines_list.push_back("_DLL");
      defines_list.push_back("_MAGICKMOD_");
    }
  else
    {
      defines_list.push_back("_LIB");
    }

  levels = LocalGetFieldCount(staging,'\\');
  string extra;

  for (int i=0; i<levels; i++)
    extra += "..\\";
  includes_list.push_back(extra);

  extra = "..\\MagickCore";
  add_includes(includes_list, extra, levels-2);

  if (LocalFindNoCase(staging,"\\SDL",0) >= 0)
    {
      extra = "..\\SDL";
      add_includes(includes_list, extra, levels-2);
    }
  if (LocalFindNoCase(staging,"..\\jp2",0) == 0)
    {
      extra = "..\\jp2";
      add_includes(includes_list, extra, levels-2);
    }
  if (LocalFindNoCase(staging,"..\\Magick++",0) == 0)
    {
      extra = "..\\Magick++";
      add_includes(includes_list, extra, levels-2);
    }
  if (LocalFindNoCase(name,"jpeg",0) >= 0)
    {
      extra = "..\\jpeg";
      add_includes(includes_list, extra, levels-2);
    }
  if (LocalFindNoCase(name,"tiff",0) >= 0)
    {
      extra = "..\\tiff";
      add_includes(includes_list, extra, levels-2);
    }
  if (LocalFindNoCase(name,"dcraw",0) >= 0)
    {
      extra = "..\\jp2\\src\\libjasper\\include";
      add_includes(includes_list, extra, levels-2);
      extra = "..\\jpeg";
      add_includes(includes_list, extra, levels-2);
    }

  // Add OpenCL path
  if (with_opencl)
    includes_list.push_back(opencl_include);

#ifdef _DEBUG
  debuglog  << "process_utility "
            << "runtime:" << runtime <<","
            << "project_type:" << project_type << ","
            << "staging:" << staging.c_str() << ","
            << "search:" << search.c_str() << ","
            << "name:" << name.c_str() << ","
            << "prefix:" << prefix.c_str() << ","
            << "extn:" << extn.c_str() << endl;
#endif

  ConfigureProject *project = write_project_exe(runtime,
                                                project_type,
                                                staging,
                                                search,
                                                name,
                                                prefix,
                                                extn);

  project->name = prefix + "%s" + name + "%s";
  project->type = project_type;
  dependency_list.push_back(project);
#ifdef _DEBUG
  debuglog  << "dependency:" << project->name.c_str() << endl;
#endif

  string projectname;
  string pname;
  pname = prefix + name;
  projectname = get_project_name(EXEPROJECT,runtime,staging.substr(1),prefix,name);
  switch (runtime)
    {
    case MULTITHREADEDSTATIC:
    case SINGLETHREADEDSTATIC:
    case MULTITHREADEDSTATICDLL:
      workspace->write_begin_project(project, pname.c_str(), projectname.c_str());
      if (!standaloneMode)
        generate_dependencies(project,(extn.compare("cpp") == 0), false, false);
      workspace->write_end_project(project);
      break;
    default:
    case MULTITHREADEDDLL:
      workspace->write_begin_project(project, pname.c_str(), projectname.c_str());
      if (!standaloneMode)
        {
          bool exempt;

          static const char *magick_exempt[] =
          {
            "dcraw",
            "iptcutil",
            "jasper",
            "tiff",
            (const char *) NULL
          };

          int i;

          exempt=true;
          for (i=0; magick_exempt[i] != NULL; i++)
          {
            if (LocalFindNoCase(name,(char *) magick_exempt[i],0) >= 0)
              {
                exempt=false;
                break;
              }
          }
          if (exempt)
            {
              workspace->write_project_dependency(project,"CORE_MagickCore");
              workspace->write_project_dependency(project,"CORE_MagickWand");
              if (useX11Stubs)
                workspace->write_project_dependency(project,"CORE_xlib");
            }
          if (extn.compare("cpp") == 0)
            {
              workspace->write_project_dependency(project,"CORE_Magick++");
            }
          string strDepends = staging.c_str();
          if (LocalFindNoCase(strDepends,"\\SDL",0) >= 0)
            {
              workspace->write_project_dependency(project,"CORE_SDL");
            }
          if (LocalFindNoCase(strDepends,"..\\jp2",0) == 0)
            {
              workspace->write_project_dependency(project,"CORE_jp2");
            }
          if (LocalFindNoCase(name,"jpeg",0) >= 0)
            {
              workspace->write_project_dependency(project,"CORE_jpeg");
            }
          if (LocalFindNoCase(name,"tiff",0) >= 0)
            {
              workspace->write_project_dependency(project,"CORE_tiff");
            }
          if (LocalFindNoCase(name,"dcraw",0) >= 0)
            {
              workspace->write_project_dependency(project,"CORE_jp2");
              workspace->write_project_dependency(project,"CORE_jpeg");
            }
        }
      workspace->write_end_project(project);
      break;
    }
}

void AddExtraLibs( string &name,string root,
                   list<string> &lib_release_list,
                   list<string> &lib_debug_list )
{
  string libpath;
  string extra_path;
  string extra_lvls;
  WIN32_FIND_DATA libdata;
  bool gotRelease, gotDebug;

  gotRelease=gotDebug=false;

  int levels = LocalGetFieldCount(root,'\\');
  for (int j=0; j<(levels-2); j++)
    extra_lvls += "..\\";
  extra_path = "..\\";
  extra_path += root;

  // look for release libs that exist and add them into the module
  libpath = extra_path;
  libpath += "\\";
  libpath += name;
  libpath += "Release.lib";
  HANDLE libhandle = FindFirstFile(libpath.c_str(), &libdata);
  if (libhandle != INVALID_HANDLE_VALUE)
    {
      string extralibrary;
      do
        {
          extralibrary = extra_lvls;
          extralibrary += extra_path;
          extralibrary += "\\";
          extralibrary += libdata.cFileName;
          lib_release_list.push_back(extralibrary);
          gotRelease=true;
        } while (FindNextFile(libhandle, &libdata));
      FindClose(libhandle);
    }

  // look for debug libs that exist and add them into the module
  libpath = extra_path;
  libpath += "\\";
  libpath += name;
  libpath += "Debug.lib";
  libhandle = FindFirstFile(libpath.c_str(), &libdata);
  if (libhandle != INVALID_HANDLE_VALUE)
    {
      string extralibrary;
      do
        {
          extralibrary = extra_lvls;
          extralibrary += extra_path;
          extralibrary += "\\";
          extralibrary += libdata.cFileName;
          lib_debug_list.push_back(extralibrary);
          gotDebug=true;
        } while (FindNextFile(libhandle, &libdata));
      FindClose(libhandle);
    }

  if (gotRelease==false || gotDebug==false)
    {
      // look for ANY libs that exist and add them into the module
      libpath = extra_path;
      libpath += "\\*.lib";
      libhandle = FindFirstFile(libpath.c_str(), &libdata);
      if (libhandle != INVALID_HANDLE_VALUE)
        {
          string extralibrary;
          do
            {
              extralibrary = extra_lvls;
              extralibrary += extra_path;
              extralibrary += "\\";
              extralibrary += libdata.cFileName;
              lib_release_list.push_back(extralibrary);
              lib_debug_list.push_back(extralibrary);
            } while (FindNextFile(libhandle, &libdata));
          FindClose(libhandle);
        }
    }
}

void CConfigureApp::process_library( const char *root,
                                     const char *filename,
                                     int runtime,
                                     int project_type )
{
  bool dll = false;
  string basename = filename;
  string name = LocalGetField(basename,'.',0);
  string extn = LocalGetField(basename,'.',1);
  // filename is not a source file name so don't check extension length!
  string prefix = "CORE_";
  string staging = root;
  int levels;

  lib_release_list.clear();
  lib_debug_list.clear();
  includes_list.clear();
  defines_list.clear();
  source_list.clear();
  resource_list.clear();
  exclude_list.clear();
  module_definition_file.erase();
  object_list.clear();
  prebuild_cmd_list.clear();
  additional_libdir_list.clear();

  if (runtime == MULTITHREADEDDLL)
    {
      dll = true;
      defines_list.push_back("_DLL");
      defines_list.push_back("_MAGICKMOD_");
    }
  else
    {
      dll = false;
      defines_list.push_back("_LIB");
    }
  // For the static library case we force the system to build this
  // as a standard library instead of a DLL. Magick++ and some of
  // the other libraries are build this way for various reasons.
  if (project_type == STATICLIB)
    dll = false;

  levels = LocalGetFieldCount(staging,'\\');

  string extra;
  extra = "..\\zlib";
  add_includes(includes_list, extra, levels-2);
  extra = "..\\bzlib";
  add_includes(includes_list, extra, levels-2);
  extra = "..\\jpeg";
  add_includes(includes_list, extra, levels-2);
  //extra = "..\\tiff\\libtiff";
  //add_includes(includes_list, extra, levels-2);
  extra = "..\\lcms\\src";
  add_includes(includes_list, extra, levels-2);
  extra = "..\\lcms\\include";
  add_includes(includes_list, extra, levels-2);
  extra = "..\\tiff\\libtiff";
  add_includes(includes_list, extra, levels-2);
  extra = "..\\ttf\\include";
  add_includes(includes_list, extra, levels-2);
  extra = "..\\libxml\\include";
  add_includes(includes_list, extra, levels-2);
  add_includes(includes_list, staging, levels-2);

  // This is a kind of kludge that is on its way to becoming a more
  // general facility. It looks for special libraries that are named
  // a specific way and adds these in if it finds them. If it does
  // not find the specially named ones, it add anything it finds in.
  //AddExtraLibs(name,root,lib_release_list,lib_debug_list);

  string envpath;
  envpath = staging;
  if (project_type == LIBRARY)
    {
      envpath += "\\LIBRARY.txt";
      load_environment_file(envpath.c_str(), runtime);
    }
  if (project_type == STATICLIB)
    {
      envpath += "\\STATICLIB.txt";
      load_environment_file(envpath.c_str(), runtime);
    }

  // Add OpenCL path
  if (with_opencl)
  {
    if (strcmp(filename, "magick") == 0 || strcmp(filename, "ojpeg") == 0 || 
      strcmp(filename, "Magick++") == 0 || strcmp(filename, "wand") == 0)
      includes_list.push_back(opencl_include);
    if (strcmp(filename, "magick") == 0 || strcmp(filename, "ojpeg") == 0)
    {
      lib_release_list.push_back("OpenCL.lib");
      lib_debug_list.push_back("OpenCL.lib");
      additional_libdir_list.push_back(opencl_libdir);
    }
  }

#ifdef _DEBUG
  debuglog  << "process_library "
            << "dll:" << dll << ","
            << "runtime:" << runtime <<","
            << "project_type:" << project_type << ","
            << "staging:" << staging.c_str() << ","
            << "search:" << "*,"
            << "name:" << name.c_str() << ","
            << "prefix:" << prefix.c_str() << endl;
#endif

  ConfigureProject *project = write_project_lib( dll,
                                                 runtime, // multi-threaded
                                                 project_type,
                                                 staging,
                                                 "*",
                                                 name,
                                                 prefix,
                                                 extn );

  string projectname;
  string pname;
  pname = prefix + name;

  project->name = prefix + "%s" + name + "%s";
  project->type = project_type;
  dependency_list.push_back(project);
#ifdef _DEBUG
  debuglog  << "dependency:" << project->name.c_str() << endl;
#endif

  projectname = get_project_name(
                                 dll?DLLPROJECT:LIBPROJECT,runtime,staging.substr(1),prefix,name);
  if (dll && (runtime==MULTITHREADEDDLL))
    {
      workspace->write_begin_project(project, pname.c_str(), projectname.c_str());
      if (name.compare("MagickCore") == 0)
        {
          if (useX11Stubs)
            workspace->write_project_dependency(project,"CORE_xlib");
          //workspace->write_project_dependency(project,"CORE_tiff");
          workspace->write_project_dependency(project,"CORE_jpeg");
          workspace->write_project_dependency(project,"CORE_zlib");
          workspace->write_project_dependency(project,"CORE_bzlib");
          workspace->write_project_dependency(project,"CORE_lcms");
          workspace->write_project_dependency(project,"CORE_tiff");
          workspace->write_project_dependency(project,"CORE_ttf");
          workspace->write_project_dependency(project,"CORE_libxml");
        }
      if (name.compare("coders") == 0)
        {
          workspace->write_project_dependency(project,"CORE_zlib");
          workspace->write_project_dependency(project,"CORE_bzlib");
          workspace->write_project_dependency(project,"CORE_jpeg");
          workspace->write_project_dependency(project,"CORE_jbig");
          workspace->write_project_dependency(project,"CORE_jp2");
          workspace->write_project_dependency(project,"CORE_png");
          workspace->write_project_dependency(project,"CORE_libxml");
          workspace->write_project_dependency(project,"CORE_ttf");
          workspace->write_project_dependency(project,"CORE_tiff");
          workspace->write_project_dependency(project,"CORE_wmf");
          if (useX11Stubs)
            workspace->write_project_dependency(project,"CORE_xlib");
          workspace->write_project_dependency(project,"CORE_MagickWand");
          workspace->write_project_dependency(project,"CORE_MagickCore");
        }
      if (name.compare("filters") == 0)
        {
          workspace->write_project_dependency(project,"CORE_MagickCore");
        }
      if (name.compare("Magick++") == 0)
        {
          workspace->write_project_dependency(project,"CORE_MagickWand");
          workspace->write_project_dependency(project,"CORE_MagickCore");
        }
      if (name.compare("SDL") == 0)
        {
          workspace->write_project_dependency(project,"CORE_MagickCore");
        }
      if (name.compare("avi") == 0)
        {
          workspace->write_project_dependency(project,"CORE_jpeg");
        }
      if (name.compare("hdf") == 0)
        {
          workspace->write_project_dependency(project,"CORE_zlib");
        }
      if (name.compare("mat") == 0)
        {
          workspace->write_project_dependency(project,"CORE_zlib");
        }
      if (name.compare("pdf") == 0)
        {
          workspace->write_project_dependency(project,"CORE_zlib");
        }
      if (name.compare("ps2") == 0)
        {
          workspace->write_project_dependency(project,"CORE_zlib");
        }
      if (name.compare("ps3") == 0)
        {
          workspace->write_project_dependency(project,"CORE_zlib");
        }
      if (name.compare("png") == 0)
        {
          workspace->write_project_dependency(project,"CORE_zlib");
        }
      if (name.compare("tiff") == 0)
        {
          workspace->write_project_dependency(project,"CORE_jpeg");
          workspace->write_project_dependency(project,"CORE_zlib");
        }
      if (name.compare("MagickWand") == 0)
        {
          workspace->write_project_dependency(project,"CORE_MagickCore");
          if (useX11Stubs)
            workspace->write_project_dependency(project,"CORE_xlib");
        }
      if (name.compare("wmf") == 0)
        {
          workspace->write_project_dependency(project,"CORE_ttf");
          workspace->write_project_dependency(project,"CORE_zlib");
        }
      workspace->write_end_project(project);
    }
  else
    {
      workspace->write_begin_project(project, pname.c_str(), projectname.c_str());
      workspace->write_end_project(project);
    }
}

void CConfigureApp::process_module( const char *root,
                                    const char *filename,
                                    int runtime,
                                    int project_type)
{
  bool dll = false;
  string basename = filename;
  string name = LocalGetField(basename,'.',0);
  string extn = LocalGetField(basename,'.',1);
  if(extn.length() > 3)
    return;
  string prefix;
  string staging = root;
  int i,levels;

  //
  // Temporary kludge to disable any modules for anything but the DLL
  // versions of the projects.
  //
  if (runtime != MULTITHREADEDDLL)
    return;

  if (project_type == MODULE)
    prefix = MODULE_PREFIX;

  lib_release_list.clear();
  lib_debug_list.clear();
  includes_list.clear();
  defines_list.clear();
  source_list.clear();
  resource_list.clear();
  exclude_list.clear();
  object_list.clear();
  prebuild_cmd_list.clear();
  additional_libdir_list.clear();
  filter_opencl_list.clear();

  if (runtime == MULTITHREADEDDLL)
    {
      dll = true;
      defines_list.push_back("_DLL");
    }
  else
    {
      dll = false;
      defines_list.push_back("_LIB");
    }

  levels = LocalGetFieldCount(staging,'\\');

  string extra;
  for (i=0; i<levels; i++)
    extra += "..\\";
  includes_list.push_back(extra);
  extra = "..\\MagickCore";
  add_includes(includes_list, extra, levels-2);

  if (name.compare("avi") == 0)
    {
      extra = "..\\jpeg";
      add_includes(includes_list, extra, levels-2);
    }
  if (name.compare("mat") == 0)
    {
      extra = "..\\zlib";
      add_includes(includes_list, extra, levels-2);
    }
  if (name.compare("png") == 0)
    {
      extra = "..\\zlib";
      add_includes(includes_list, extra, levels-2);
    }
  if (name.compare("pdf") == 0)
    {
      extra = "..\\zlib";
      add_includes(includes_list, extra, levels-2);
    }
  if (name.compare("ps2") == 0)
    {
      extra = "..\\zlib";
      add_includes(includes_list, extra, levels-2);
    }
  if (name.compare("ps3") == 0)
    {
      extra = "..\\zlib";
      add_includes(includes_list, extra, levels-2);
    }
  if (name.compare("miff") == 0)
    {
      extra = "..\\zlib";
      add_includes(includes_list, extra, levels-2);
      extra = "..\\bzlib";
      add_includes(includes_list, extra, levels-2);
    }
  if ((name.compare("svg") == 0)
      || (name.compare("url") == 0)
      || (name.compare("msl") == 0)
      )
    {
      extra = "..\\libxml";
      add_includes(includes_list, extra, levels-2);
#ifdef USETHIS
      extra = "..\\autotrace";
      add_includes(includes_list, extra, levels-2);
#endif
    }
  if (name.compare("hdf") == 0)
    {
      extra = "..\\zlib";
      add_includes(includes_list, extra, levels-2);
    }
  if (name.compare("wmf") == 0)
    {
      extra = "..\\ttf";
      add_includes(includes_list, extra, levels-2);
      extra = "..\\zlib";
      add_includes(includes_list, extra, levels-2);
    }

  // add include paths to this modules directory and any subdirs
  extra = "..\\";
  extra += name;
  add_includes(includes_list, extra, levels-2);

  WIN32_FIND_DATA libdata;
  string libpath;
  string dependency;
  // look for xxxx.txt files to see if there is a third party
  // library or a core library that is needed for this module
  extra.erase();
  for (i=0; i<levels-1; i++)
    extra += "..\\";
  extra += name;
  libpath = extra;
  libpath += "\\LIBRARY.txt";
  HANDLE libhandle = FindFirstFile(libpath.c_str(), &libdata);
  if (libhandle != INVALID_HANDLE_VALUE)
    {
      FindClose(libhandle);
      dependency = "CORE_";
      dependency += name;
    }
  libpath = extra;
  libpath += "\\STATICLIB.txt";
  libhandle = FindFirstFile(libpath.c_str(), &libdata);
  if (libhandle != INVALID_HANDLE_VALUE)
    {
      FindClose(libhandle);
      dependency = "CORE_";
      dependency += name;
    }
  libpath = extra;
  libpath += "\\THIRDPARTY.txt";
  libhandle = FindFirstFile(libpath.c_str(), &libdata);
  if (libhandle != INVALID_HANDLE_VALUE)
    {
      FindClose(libhandle);
      dependency = "LIBR_";
      dependency += name;
    }

  // This is a kind of kludge that is on its way to becoming a more
  // general facility. It looks for special libraries that are named
  // a specific way and adds these in if it finds them. If it does
  // not find the specially named ones, it add anything it finds in.
  AddExtraLibs(name,root,lib_release_list,lib_debug_list);

  string envpath;
  string search;
  envpath = root;
  if (project_type == MODULE)
    {
      envpath += "\\MODULE.txt";
      search = "";
      load_environment_file(envpath.c_str(), runtime);
    }
  if (project_type == ADD_ON)
    {
      envpath += "\\ADD_ON.txt";
      search = "";
      //search = "*";
      // we force this to always be built as a dll
      dll = true;
      load_environment_file(envpath.c_str(), runtime);
    }

  // Add OpenCL path
  if (with_opencl)
  {
    if (name.compare("accelerate") == 0)
    {
      extra = "..\\ojpeg";
      add_includes(includes_list, extra, levels-2);
      lib_release_list.push_back("OpenCL.lib");
      lib_debug_list.push_back("OpenCL.lib");
      additional_libdir_list.push_back(opencl_libdir);
      dependency = "CORE_ojpeg";
    }
    if (name.compare("ojpeg") == 0)
    {
      extra = "..\\filters";
      add_includes(includes_list, extra, levels-2);
    }
    includes_list.push_back(opencl_include);
  }

#ifdef _DEBUG
  debuglog  << "process_module "
            << "dll:" << dll << ","
            << "runtime:" << runtime <<","
            << "project_type:" << project_type << ","
            << "staging:" << staging.c_str() << ","
            << "search:" << search.c_str() << ","
            << "name:" << name.c_str() << ","
            << "prefix:" << prefix.c_str() << endl;
#endif

  ConfigureProject *project = write_project_lib(
                                                dll,
                                                runtime, // multi-threaded
                                                project_type,
                                                root,
                                                search,
                                                name,
                                                prefix,
                                                extn);

  string projectname;
  string pname;
  pname = prefix + name;

  if (project_type == MODULE)
    {
      project->name = prefix + "%s" + name + "%s";
      project->type = project_type;
      dependency_list.push_back(project);
#ifdef _DEBUG
      debuglog  << "dependency:" << project->name.c_str() << endl;
#endif
    }
  if (project_type == ADD_ON)
    {
      project->name = name;
      project->type = project_type;
      dependency_list.push_back(project);
#ifdef _DEBUG
      debuglog  << "dependency:" << project->name.c_str() << endl;
#endif
    }

  projectname = get_project_name(
                                 dll?DLLPROJECT:LIBPROJECT,runtime,staging.substr(1),prefix,name);
  if(runtime == MULTITHREADEDDLL)
    {
      workspace->write_begin_project(project, pname.c_str(), projectname.c_str());
      {
        workspace->write_project_dependency(project,"CORE_MagickCore");
        if (dependency.length() > 0)
          workspace->write_project_dependency(project,dependency.c_str());
        if (name.compare("avi") == 0)
          {
            workspace->write_project_dependency(project,"CORE_jpeg");
          }
        if (name.compare("label") == 0)
          {
            if (useX11Stubs)
              workspace->write_project_dependency(project,"CORE_xlib");
          }
        if (name.compare("miff") == 0)
          {
            workspace->write_project_dependency(project,"CORE_zlib");
            workspace->write_project_dependency(project,"CORE_bzlib");
          }
        if (name.compare("mat") == 0)
          {
            workspace->write_project_dependency(project,"CORE_zlib");
          }
        if (name.compare("png") == 0)
          {
            workspace->write_project_dependency(project,"CORE_zlib");
          }
        if (name.compare("pdf") == 0)
          {
            workspace->write_project_dependency(project,"CORE_zlib");
          }
        if (name.compare("ps") == 0)
          {
            workspace->write_project_dependency(project,"CORE_zlib");
          }
        if (name.compare("ps2") == 0)
          {
            workspace->write_project_dependency(project,"CORE_zlib");
          }
        if (name.compare("ps3") == 0)
          {
            workspace->write_project_dependency(project,"CORE_tiff");
            workspace->write_project_dependency(project,"CORE_zlib");
          }
        if (name.compare("wmf") == 0)
          {
            workspace->write_project_dependency(project,"CORE_ttf");
            workspace->write_project_dependency(project,"CORE_zlib");
          }
        if (name.compare("x") == 0)
          {
            if (useX11Stubs)
              workspace->write_project_dependency(project,"CORE_xlib");
          }
        if (name.compare("xwd") == 0)
          {
            if (useX11Stubs)
              workspace->write_project_dependency(project,"CORE_xlib");
          }
        if ((name.compare("svg") == 0)
            || (name.compare("url") == 0)
            || (name.compare("msl") == 0)
            )
          {
            workspace->write_project_dependency(project,"CORE_libxml");
#ifdef USETHIS
            if (doesDirExist("..\\..\\autotrace"))
              workspace->write_project_dependency(project,"CORE_autotrace");
#endif
          }
        if (with_opencl && (name.compare("ojpeg") == 0))
          {
            workspace->write_project_dependency(project,"accelerate");
          }
        workspace->write_end_project(project);
      }
    }
  else
    {
      workspace->write_begin_project(project, pname.c_str(), projectname.c_str());
      workspace->write_end_project(project);
    }
}

void CConfigureApp::process_3rd_party_library( const char *root,
                                               const char *filename,
                                               int runtime,
                                               int project_type)
{
  bool dll = false;
  string basename = filename;
  string name = LocalGetField(basename,'.',0);
  string prefix = "LIBR_";
  string staging = root;

  lib_release_list.clear();
  lib_debug_list.clear();
  includes_list.clear();
  defines_list.clear();
  source_list.clear();
  resource_list.clear();
  exclude_list.clear();
  object_list.clear();
  prebuild_cmd_list.clear();
  additional_libdir_list.clear();

  if (runtime == MULTITHREADEDDLL)
    {
      dll = true;
      defines_list.push_back("_DLL");
      defines_list.push_back("_MAGICKMOD_");
    }
  else
    {
      dll = false;
      defines_list.push_back("_LIB");
    }

  string libpath;
  libpath = "..\\";
  libpath += staging;
  string dependency;
  WIN32_FIND_DATA libdata;
  HANDLE libhandle;

  libhandle = FindFirstFile(libpath.c_str(), &libdata);
  if (libhandle != INVALID_HANDLE_VALUE)
    {
      FindClose(libhandle);

      string projectname;
      string pname;
      pname = prefix + name;

      ConfigureProject *project;
      if (visualStudio7)
        project = new ConfigureVS7Project();
      else
        project = new ConfigureVS6Project();
      project->name = prefix + "%s" + name + "%s";
      project->type = project_type;
      dependency_list.push_back(project);
#ifdef _DEBUG
      debuglog  << "dependency:" << project->name.c_str() << endl;
#endif
      projectname = get_project_name(
                                     dll?DLLPROJECT:LIBPROJECT,runtime,staging.substr(1),prefix,name);

      libhandle = FindFirstFile(projectname.c_str(), &libdata);
      if (libhandle != INVALID_HANDLE_VALUE)
        {
          FindClose(libhandle);

          switch(runtime)
            {
            case MULTITHREADEDDLL:
              {
                workspace->write_begin_project(project, pname.c_str(),
                  projectname.c_str());
                if (name.compare("mat") == 0)
                  {
                    workspace->write_project_dependency(project,"CORE_zlib");
                  }
                if (name.compare("png") == 0)
                  {
                    workspace->write_project_dependency(project,"CORE_zlib");
                  }
                if (name.compare("pdf") == 0)
                  {
                    workspace->write_project_dependency(project,"CORE_zlib");
                  }
                if (name.compare("ps2") == 0)
                  {
                    workspace->write_project_dependency(project,"CORE_zlib");
                  }
                if (name.compare("ps3") == 0)
                  {
                    workspace->write_project_dependency(project,"CORE_zlib");
                  }
#ifdef HDF_SUPPORTED
                if (name.compare("hdf") == 0)
                  {
                    workspace->write_project_dependency(project,"CORE_jpeg");
                    workspace->write_project_dependency(project,"CORE_zlib");
                  }
#endif
                if (name.compare("tiff") == 0)
                  {
                    workspace->write_project_dependency(project,"CORE_jpeg");
                    workspace->write_project_dependency(project,"CORE_zlib");
                  }
                if (name.compare("wmf") == 0)
                  {
                    workspace->write_project_dependency(project,"CORE_ttf");
                    workspace->write_project_dependency(project,"CORE_zlib");
                  }
                workspace->write_end_project(project);
                break;
              }
            default:
              {
                workspace->write_begin_project(project, pname.c_str(), projectname.c_str());
                workspace->write_end_project(project);
                break;
              }
            }
        }
    }
}

void CConfigureApp::process_one_folder( const char *root,
                                        WIN32_FIND_DATA &data,
                                        int project_type,
                                        int runtimeOption)
{
  string subpath;

  if (!optionalFiles)
    {
      string strTest = root;
      if (LocalFindNoCase(strTest,"\\test",0) >= 0)
        return;
      if (LocalFindNoCase(strTest,"\\demo",0) >= 0)
        return;
      if (LocalFindNoCase(strTest,"\\contrib",0) >= 0)
        return;
      if (LocalFindNoCase(strTest,"\\appl",0) >= 0)
        return;
    }
  if (visualStudio7)
    {
      string strTest = root;
      if (LocalFindNoCase(strTest,"\\ATL",0) >= 0)
        return;
    }
  switch (project_type)
    {
    case UTILITY:
      {
        // Look for any C files first and generate a project for each file that
        // is found.
        subpath = "..\\";
        subpath += root;
        subpath += "\\*.c";
        WIN32_FIND_DATA subdata;
        HANDLE subhandle = FindFirstFile(subpath.c_str(), &subdata);
        if (subhandle != INVALID_HANDLE_VALUE)
          {
            do
              {
                process_utility(root, subdata.cFileName, runtimeOption, project_type);
              } while (FindNextFile(subhandle, &subdata));
            FindClose(subhandle);
          }

        // Look for any C++ files next and generate a project for each file that
        // is found.
        subpath = "..\\";
        subpath += root;
        subpath += "\\*.cpp";
        subhandle = FindFirstFile(subpath.c_str(), &subdata);
        if (subhandle != INVALID_HANDLE_VALUE)
          {
            do
              {
                process_utility(root, subdata.cFileName, runtimeOption, project_type);
              } while (FindNextFile(subhandle, &subdata));
            FindClose(subhandle);
          }
      }
      break;
    case PROJECT:
      {
        // MFC does not link for single threaded or multithreaded dll
        if ((runtimeOption == SINGLETHREADEDSTATIC) ||
            (runtimeOption == MULTITHREADEDSTATICDLL))
          return;
        // check to see if there seems to be some source code in the
        // location specified and then process the entire folder as one
        // project if this test passes.
        subpath = "..\\";
        subpath += root;
        subpath += "\\*.cpp";
        WIN32_FIND_DATA subdata;
        HANDLE subhandle = FindFirstFile(subpath.c_str(), &subdata);
        if (subhandle != INVALID_HANDLE_VALUE)
          {
            {
              string strTest = root;
              int count = LocalGetFieldCount(strTest,'\\');
              string parent = LocalGetField(strTest,'\\',count-1);
              parent += ".cpp";
              process_utility(root, parent.c_str(), runtimeOption, project_type);
            }
            FindClose(subhandle);
          }
      }
      break;
    case LIBRARY:
    case STATICLIB:
      {
        BOOL standaloneModeBackup = standaloneMode;
        onebigdllMode = FALSE;
        standaloneMode = FALSE;
        process_library(root, data.cFileName, runtimeOption, project_type);
        standaloneMode = standaloneModeBackup;
        onebigdllMode = FALSE;
      }
      break;
    case ADD_ON:
    case MODULE:
      {
        bool bFoundSomething = false;
        // Look for any C++ files first and generate a project for each file that
        // is found.
        subpath = "..\\";
        subpath += root;
        subpath += "\\*.cpp";
        WIN32_FIND_DATA subdata;
        HANDLE subhandle = FindFirstFile(subpath.c_str(), &subdata);
        if (subhandle != INVALID_HANDLE_VALUE)
          {
            bFoundSomething = true;
            do
              {
                BOOL standaloneModeBackup = standaloneMode;
                if (project_type == MODULE)
                  onebigdllMode = FALSE;
                else if (project_type == ADD_ON)
                  onebigdllMode = TRUE;
                process_module(root, subdata.cFileName,
                               runtimeOption, project_type);
                onebigdllMode = FALSE;
                standaloneMode = standaloneModeBackup;
                if (project_type == ADD_ON)
                  break;
              } while (FindNextFile(subhandle, &subdata));
            FindClose(subhandle);
          }

        if ((project_type == ADD_ON) && bFoundSomething)
          break;

        // Look for any C files next and generate a project for each file that
        // is found.
        subpath = "..\\";
        subpath += root;
        subpath += "\\*.c";
        if (project_type == ADD_ON)
        {
          bool bFilterAnalyze = false;
          bool bFilterOpenCL = false;
          onebigdllMode = TRUE;
          subhandle = FindFirstFile(subpath.c_str(), &subdata);
          if (subhandle != INVALID_HANDLE_VALUE)
          {
            do
            {
              // custom image filter: analyze
              if (strcmp(subdata.cFileName, "analyze.c") == 0)
                process_module(root, subdata.cFileName, runtimeOption, project_type);
              // custom image filter: OpenCL acceleration
              if (with_opencl && (strcmp(subdata.cFileName, "accelerate-filter.c") == 0))
              {
                onebigdllMode = FALSE;
                process_module(root, "accelerate.c", runtimeOption, project_type);
              }
            } while (FindNextFile(subhandle, &subdata));
            FindClose(subhandle);
          }
          onebigdllMode = FALSE;
        }
        else
        {
          subhandle = FindFirstFile(subpath.c_str(), &subdata);
          if (subhandle != INVALID_HANDLE_VALUE)
            {
              bFoundSomething = true;
              do
                {
                  BOOL standaloneModeBackup = standaloneMode;
                  if (project_type == MODULE)
                    onebigdllMode = FALSE;
                  else if (project_type == ADD_ON)
                    onebigdllMode = TRUE;
                  process_module(root, subdata.cFileName,
                                 runtimeOption, project_type);
                  onebigdllMode = FALSE;
                  standaloneMode = standaloneModeBackup;
                  if (project_type == ADD_ON)
                    break;
                } while (FindNextFile(subhandle, &subdata));
              FindClose(subhandle);
            }
        }
      }
      break;
    case THIRDPARTY:
      {
        BOOL standaloneModeBackup = standaloneMode;
        onebigdllMode = FALSE;
        standaloneMode = FALSE;
        process_3rd_party_library(root, data.cFileName, runtimeOption, project_type);
        standaloneMode = standaloneModeBackup;
        onebigdllMode = FALSE;
      }
      break;
    }
}

bool is_project_type(const char *root, const int project_type)
{
  HANDLE handle;
  WIN32_FIND_DATA data;

  string searchpath = root;
  searchpath += "\\";
  switch (project_type)
    {
    case PROJECT:
      searchpath += "PROJECT.txt";
      break;
    case UTILITY:
      searchpath += "UTILITY.txt";
      break;
    case LIBRARY:
      searchpath += "LIBRARY.txt";
      break;
    case STATICLIB:
      searchpath += "STATICLIB.txt";
      break;
    case MODULE:
      searchpath += "MODULE.txt";
      break;
    case ADD_ON:
      searchpath += "ADD_ON.txt";
      break;
    case THIRDPARTY:
      searchpath += "THIRDPARTY.txt";
      break;
    default:
      return false;
    }
  handle = FindFirstFile(searchpath.c_str(), &data);
  FindClose(handle);
  if (handle != INVALID_HANDLE_VALUE)
    return true;
  return false;
}

void GetFileTime(LPFILETIME lpft)
{
  SYSTEMTIME st;
  FILETIME ft;

  GetLocalTime(&st);

  if (st.wMinute > 30)
    st.wMinute = 30;
  else
    st.wMinute =  0;
  st.wSecond = 0;
  st.wMilliseconds = 0;

  SystemTimeToFileTime(&st,&ft);
  LocalFileTimeToFileTime(&ft,lpft);
}

BOOL SetFileTimeEx(LPCTSTR lpFileName)
{
  BOOL fSetFileTime;
  FILETIME FileTime;

  GetFileTime(&FileTime);
  HANDLE hFile = CreateFile(lpFileName,
                            GENERIC_WRITE,
                            0,
                            NULL,
                            OPEN_EXISTING,
                            FILE_FLAG_BACKUP_SEMANTICS,
                            NULL);

  if (hFile == INVALID_HANDLE_VALUE)
    return(FALSE);

  fSetFileTime = SetFileTime(hFile,&FileTime,&FileTime,&FileTime);

  CloseHandle(hFile);

  return(fSetFileTime);

}

void CConfigureApp::process_project_replacements( const char *root,
                                                  const char *top,
                                                  const char *stype,
                                                  const char *newstype,
                                                  int operation )
{
  int project_type = DISABLED;
  HANDLE tophandle;
  WIN32_FIND_DATA topdata;

  // Scan all top level directories and process the ones
  // that we are allowed to.
  string rootpath = root;
  rootpath += "\\";
  rootpath += top;
  tophandle = FindFirstFile(rootpath.c_str(), &topdata);
  do
    {
      if (tophandle == INVALID_HANDLE_VALUE)
        break;
      if ((topdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ==
          FILE_ATTRIBUTE_DIRECTORY)
        {
          HANDLE handle;
          WIN32_FIND_DATA data;

          if (_stricmp(topdata.cFileName,".") == 0)
            continue;
          if (_stricmp(topdata.cFileName,"..") == 0)
            continue;

          // Now look to see if there is a directory by the same name
          // one level above our config level
          string searchpath;
          searchpath = "..\\";
          searchpath += root;
          searchpath += "\\";
          searchpath += topdata.cFileName;
          handle = FindFirstFile(searchpath.c_str(), &data);
          if (handle == INVALID_HANDLE_VALUE)
            continue;
          FindClose(handle);

          if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ==
              FILE_ATTRIBUTE_DIRECTORY)
            {
              WIN32_FIND_DATA nestdata;
              HANDLE nesthandle;

              // Now look for all files with the specified spec in the
              // current directory
              string filepath;
              filepath = root;
              filepath += "\\";
              filepath += topdata.cFileName;
              filepath += "\\";
              filepath += stype;
              nesthandle = FindFirstFile(filepath.c_str(), &nestdata);
              do
                {
                  if (nesthandle == INVALID_HANDLE_VALUE)
                    break;

                  filepath = root;
                  filepath += "\\";
                  filepath += topdata.cFileName;
                  filepath += "\\";
                  filepath += nestdata.cFileName;
                  {
                    char drive[_MAX_DRIVE];
                    char dir[_MAX_DIR];
                    char fname[_MAX_FNAME];
                    char ext[_MAX_EXT];
                    const char *s1,*s2,*s3,*s4;

                    s1 = filepath.c_str();
                    _splitpath( s1, drive, dir, fname, ext );
                    if (operation == OP_REPLACEFILES)
                      {
                        rootpath = "..\\";
                        rootpath += root;
                        rootpath += "\\";
                        rootpath += topdata.cFileName;
                        s2 = rootpath.c_str();

                        string original;
                        original = rootpath;
                        original += "\\";
                        original += fname;
                        s3 = original.c_str();

                        handle = FindFirstFile(s2, &nestdata);
                        if (handle != INVALID_HANDLE_VALUE)
                          {
                            FindClose(handle);

                            string renamed;
                            renamed = rootpath;
                            renamed += "\\";
                            renamed += fname;
                            renamed += ".bak";
                            s4 = renamed.c_str();
                            MoveFile(s3,s4);
                          }
                        // If the file does not exist, then we are free to copy it over.
                        if (CopyFile(s1,s3,FALSE))
                          {
                            SetFileTimeEx(s3);
                          }
                      }
                    else if (operation == OP_RENAMEFILES)
                      {
                        string renamed;
                        const char *s5;

                        renamed = root;
                        renamed += "\\";
                        renamed += topdata.cFileName;
                        renamed += "\\";
                        renamed += newstype;
                        s5 = renamed.c_str();
                        MoveFile(s1,s5);
                      }
                  }
                } while (FindNextFile(nesthandle, &nestdata));
              FindClose(nesthandle);

              rootpath = root;
              rootpath += "\\";
              rootpath += data.cFileName;
              process_project_replacements(rootpath.c_str(),"*",stype,newstype,operation);
            }
        }
    } while (FindNextFile(tophandle, &topdata));
  FindClose(tophandle);
}

//
// This routine just traverses the VisualMagick directory structure
// in a top down fashion looking for any that have a corresponding
// directory of the same name in the root of the ImageMagick tree.
//
void CConfigureApp::process_project_type( const char *root,
                                          int runtime,
                                          const char *stype,
                                          const int btype)
{
  int project_type = DISABLED;
  HANDLE tophandle;
  // Scan all top level directories and process the ones
  // that we are allowed to.
  WIN32_FIND_DATA topdata;
  string rootpath = root;
  rootpath += "\\*"; // look just for directorties
  tophandle = FindFirstFile(rootpath.c_str(), &topdata);
  do
    {
      if (tophandle == INVALID_HANDLE_VALUE)
        break;
      // Look just for directories - not files.
      if ((topdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ==
          FILE_ATTRIBUTE_DIRECTORY)
        {
          HANDLE handle;
          WIN32_FIND_DATA data;

          // Ignore ones that we don't care about
          if (_stricmp(topdata.cFileName,".") == 0)
            continue;
          if (_stricmp(topdata.cFileName,"..") == 0)
            continue;

          // Check to see if the directory we found has the type of .txt
          // file we are looking for.
          string searchpath = root;
          searchpath += "\\";
          searchpath += topdata.cFileName;
          searchpath += "\\";
          searchpath += stype;
          handle = FindFirstFile(searchpath.c_str(), &data);
          if (handle == INVALID_HANDLE_VALUE)
            continue;
          FindClose(handle);

          // Next check to see if there is a correspoding directory of the
          // same name one level above the VisualMagick directory
          searchpath = "..\\";
          searchpath += root;
          searchpath += "\\";
          searchpath += topdata.cFileName;
          handle = FindFirstFile(searchpath.c_str(), &data);
          if (handle == INVALID_HANDLE_VALUE)
            continue;
          FindClose(handle);
          if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ==
              FILE_ATTRIBUTE_DIRECTORY)
            {
              rootpath = root;
              rootpath += "\\";
              rootpath += topdata.cFileName;
              process_one_folder(rootpath.c_str(),data,btype,runtime);
              // We allow only certain project types to be below the top level
              // of the tree structure. Most library types are like this, since
              // they are unlikely to contain more then one instance of the
              // particular project type. Another way of looking at this is that
              // for some project types - the entire tree of files is part of
              // the project, while others have multiple projects in a given
              // tree or subtree.
              if (btype == UTILITY || btype == PROJECT
                  || btype == ADD_ON || btype == MODULE)
                {
                  // We don't nest down for LIBRARY, STATICLIB, or THIRDPARTY
                  // because everything in the tree is assumed to be part of
                  // the project.
                  rootpath = root;
                  rootpath += "\\";
                  rootpath += data.cFileName;
                  process_project_type(rootpath.c_str(),runtime,stype,btype);
                }
            }
        }
    } while (FindNextFile(tophandle, &topdata));
  FindClose(tophandle);
}

#ifdef __NO_MFC__
struct MyWaitDlg {
	void Pumpit() { }
} waitdlg;
#else // __NO_MFC__
class MyWaitDlg : public CWaitDialog
{
public:
  MyWaitDlg (bool *status) :
    CWaitDialog (status, NULL, NULL, NULL)
  {
    nCurrent = 1;
    nTotal = 10;
    nPercent = 0;
	 canPump = TRUE;
  }

  void Pumpit()
  {
    nPercent = (nCurrent * 100)/nTotal;
    SetPercentComplete(nPercent);
    if (canPump)
      canPump = Pump();
    nCurrent++;
    if (nCurrent > nTotal)
      nCurrent = 0;
  }

  int
    nCurrent,
    nTotal,
    nPercent;
  BOOL
    canPump;
};

#define THIS_SUB_KEY FALSE
#define ALL_SUB_KEYS TRUE

#define SIGNAL_EVENT    TRUE
#define WAIT_FOR_CHANGE FALSE

class CRegistry
{
private:
  CRegistry( const CRegistry& ) {};
  CRegistry& operator=( const CRegistry& ) { return( *this ); };

private:
  void m_Initialize( void );

protected:
  HKEY m_KeyHandle;
  HKEY m_RegistryHandle;

  LONG m_ErrorCode;

  CString m_ClassName;
  CString m_ComputerName;
  CString m_KeyName;
  CString m_RegistryName;
  DWORD m_NumberOfSubkeys;
  DWORD m_NumberOfValues;

  /*
  ** Data items filled in by QueryInfo
  */

  DWORD    m_LongestSubkeyNameLength;
  DWORD    m_LongestClassNameLength;
  DWORD    m_LongestValueNameLength;
  DWORD    m_LongestValueDataLength;
  DWORD    m_SecurityDescriptorLength;
  FILETIME m_LastWriteTime;

public:

  enum _Keys
    {
      keyLocalMachine         = (DWORD) HKEY_LOCAL_MACHINE,
      keyClassesRoot          = (DWORD) HKEY_CLASSES_ROOT,
      keyPerformanceData      = (DWORD) HKEY_PERFORMANCE_DATA,
      keyUsers                = (DWORD) HKEY_USERS,
      keyCurrentUser          = (DWORD) HKEY_CURRENT_USER,
#if ( WINVER >= 0x400 )
      keyCurrentConfiguration = (DWORD) HKEY_CURRENT_CONFIG,
      keyDynamicData          = (DWORD) HKEY_DYN_DATA
#endif
    };

  enum KeyValueTypes
    {
      typeBinary                 = REG_BINARY,
      typeDoubleWord             = REG_DWORD,
      typeDoubleWordLittleEndian = REG_DWORD_LITTLE_ENDIAN,
      typeDoubleWordBigEndian    = REG_DWORD_BIG_ENDIAN,
      typeUnexpandedString       = REG_EXPAND_SZ,
      typeSymbolicLink           = REG_LINK,
      typeMultipleString         = REG_MULTI_SZ,
      typeNone                   = REG_NONE,
      typeResourceList           = REG_RESOURCE_LIST,
      typeString                 = REG_SZ
    };

  enum CreateOptions
    {
      optionsNonVolatile = REG_OPTION_NON_VOLATILE,
      optionsVolatile    = REG_OPTION_VOLATILE
    };

  enum CreatePermissions
    {
      permissionAllAccess        = KEY_ALL_ACCESS,
      permissionCreateLink       = KEY_CREATE_LINK,
      permissionCreateSubKey     = KEY_CREATE_SUB_KEY,
      permissionEnumerateSubKeys = KEY_ENUMERATE_SUB_KEYS,
      permissionExecute          = KEY_EXECUTE,
      permissionNotify           = KEY_NOTIFY,
      permissionQueryValue       = KEY_QUERY_VALUE,
      permissionRead             = KEY_READ,
      permissionSetValue         = KEY_SET_VALUE,
      permissionWrite            = KEY_WRITE
    };

  enum CreationDisposition
    {
      dispositionCreatedNewKey     = REG_CREATED_NEW_KEY,
      dispositionOpenedExistingKey = REG_OPENED_EXISTING_KEY
    };

  enum NotifyChangeFlag
    {
      changeKeyAndSubkeys    = TRUE,
      changeSpecifiedKeyOnly = FALSE
    };

  enum NotifyChangeFilter
    {
      notifyName       = REG_NOTIFY_CHANGE_NAME,
      notifyAttributes = REG_NOTIFY_CHANGE_ATTRIBUTES,
      notifyLastSet    = REG_NOTIFY_CHANGE_LAST_SET,
      notifySecurity   = REG_NOTIFY_CHANGE_SECURITY
    };

  CRegistry();

  virtual ~CRegistry();

  virtual BOOL Close( void );
  virtual BOOL Connect( HKEY key_to_open = HKEY_CURRENT_USER,
                        LPCTSTR computer_name = NULL );
  virtual BOOL EnumerateKeys( const DWORD subkey_index,
                              CString&    subkey_name,
                              CString&    class_name );
  virtual BOOL EnumerateValues( const DWORD    value_index,
                                string&       name_of_value,
                                KeyValueTypes& type_code,
                                LPBYTE         data_buffer,
                                DWORD&         size_of_data_buffer );
  virtual BOOL GetBinaryValue( LPCTSTR name_of_value, CByteArray& return_array );
  virtual void GetClassName( CString& class_name ) const;
  virtual void GetComputerName( CString& computer_name ) const;
  virtual BOOL GetDoubleWordValue( LPCTSTR name_of_value, DWORD& return_value );
  virtual BOOL GetErrorCode( void ) const;
  virtual void GetKeyName( CString& key_name ) const;
  virtual int GetNumberOfSubkeys( void ) const;
  virtual int GetNumberOfValues( void ) const;
  virtual void GetRegistryName( CString& registry_name ) const;
  virtual BOOL GetStringValue( LPCTSTR name_of_value, CString& return_string );
  virtual BOOL GetStringArrayValue( LPCTSTR name_of_value, CStringArray& return_array );
  virtual BOOL GetValue( LPCTSTR name_of_value, CByteArray& return_array );
  virtual BOOL GetValue( LPCTSTR name_of_value, DWORD& return_value );
  virtual BOOL GetValue( LPCTSTR name_of_value, CString& return_string );
  virtual BOOL GetValue( LPCTSTR name_of_value, CStringArray& return_array );
  virtual BOOL Open( LPCTSTR name_of_subkey_to_open,
                     const CreatePermissions security_access_mask = permissionAllAccess );
  virtual BOOL QueryInfo( void );
  virtual BOOL QueryValue( LPCTSTR        name_of_value,
                           KeyValueTypes& value_type,
                           LPBYTE         address_of_buffer,
                           DWORD&         size_of_buffer );

  virtual BOOL UnLoad( LPCTSTR name_of_subkey_to_unload );

};

CRegistry::CRegistry()
{
  m_Initialize();
}

CRegistry::~CRegistry()
{
  if ( m_RegistryHandle != (HKEY) NULL )
    Close();
  m_Initialize();
}

void CRegistry::m_Initialize( void )
{
  m_ClassName.Empty();
  m_ComputerName.Empty();
  m_KeyName.Empty();
  m_RegistryName.Empty();

  m_KeyHandle                    = (HKEY) NULL;
  m_ErrorCode                    = 0L;
  m_NumberOfSubkeys              = 0;
  m_LongestSubkeyNameLength      = 0;
  m_LongestClassNameLength       = 0;
  m_NumberOfValues               = 0;
  m_LongestValueNameLength       = 0;
  m_LongestValueDataLength       = 0;
  m_SecurityDescriptorLength     = 0;
  m_LastWriteTime.dwLowDateTime  = 0;
  m_LastWriteTime.dwHighDateTime = 0;
  m_RegistryHandle               = (HKEY) NULL;
}

BOOL CRegistry::Close( void )
{
  if ( m_KeyHandle != (HKEY) NULL )
    {
      ::RegCloseKey( m_KeyHandle );
      m_KeyHandle = (HKEY) NULL;
    }

  if ( m_RegistryHandle == (HKEY) NULL )
    return( TRUE );

  m_ErrorCode = ::RegCloseKey( m_RegistryHandle );

  if ( m_ErrorCode == ERROR_SUCCESS )
    {
      m_RegistryHandle = (HKEY) NULL;
      m_Initialize();
      return( TRUE );
    }
  else
    return( FALSE );
}

BOOL CRegistry::Connect( HKEY key_to_open, LPCTSTR name_of_computer )
{
  try
    {
      if ( key_to_open == (HKEY) keyClassesRoot ||
           key_to_open == (HKEY) keyCurrentUser )
        {
          if ( name_of_computer == NULL )
            {
              m_RegistryHandle = key_to_open;
              m_ErrorCode = ERROR_SUCCESS;
            }
          else
            m_ErrorCode = ERROR_INVALID_HANDLE;
        }
      else
        {
          m_ErrorCode = ::RegConnectRegistry( (LPTSTR) name_of_computer, key_to_open, &m_RegistryHandle );
        }

      if ( m_ErrorCode == ERROR_SUCCESS )
        {
          if ( name_of_computer == NULL )
            {
              TCHAR computer_name[ MAX_PATH ];
              DWORD size = MAX_PATH;

              if ( ::GetComputerName( computer_name, &size ) == FALSE )
                m_ComputerName.Empty();
              else
                m_ComputerName = computer_name;
            }
          else
            m_ComputerName = name_of_computer;

          if ( key_to_open == HKEY_LOCAL_MACHINE )
            m_RegistryName = "HKEY_LOCAL_MACHINE";
          else if ( key_to_open == HKEY_CLASSES_ROOT )
            m_RegistryName = "HKEY_CLASSES_ROOT";
          else if ( key_to_open == HKEY_USERS )
            m_RegistryName = "HKEY_USERS";
          else if ( key_to_open == HKEY_CURRENT_USER )
            m_RegistryName = "HKEY_CURRENT_USER";
          else if ( key_to_open == HKEY_PERFORMANCE_DATA )
            m_RegistryName = "HKEY_PERFORMANCE_DATA";
#if ( WINVER >= 0x400 )
          else if ( key_to_open == HKEY_CURRENT_CONFIG )
            m_RegistryName = "HKEY_CURRENT_CONFIG";
          else if ( key_to_open == HKEY_DYN_DATA )
            m_RegistryName = "HKEY_DYN_DATA";
#endif
          else
            m_RegistryName = "Unknown";

          return( TRUE );
        }
      else
        return( FALSE );
    }
  catch( ... )
    {
      m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
      return( FALSE );
    }
}

#define DIMENSION_OF( argument ) ( sizeof( argument ) / sizeof( *( argument ) ) )

BOOL CRegistry::EnumerateKeys( const DWORD subkey_index,
                               CString& subkey_name,
                               CString& class_name )
{
  TCHAR subkey_name_string[ 2048 ];
  TCHAR class_name_string[ 2048 ];

  DWORD size_of_subkey_name_string = DIMENSION_OF( subkey_name_string ) - 1;
  DWORD size_of_class_name_string  = DIMENSION_OF( class_name_string  ) - 1;

  ::ZeroMemory( subkey_name_string, sizeof( subkey_name_string ) );
  ::ZeroMemory( class_name_string,  sizeof( class_name_string  ) );

  m_ErrorCode = ::RegEnumKeyEx( m_KeyHandle,
                                subkey_index,
                                subkey_name_string,
                                &size_of_subkey_name_string,
                                NULL,
                                class_name_string,
                                &size_of_class_name_string,
                                &m_LastWriteTime );

  if ( m_ErrorCode == ERROR_SUCCESS )
    {
      subkey_name = subkey_name_string;
      class_name  = class_name_string;

      return( TRUE );
    }
  else
    {
      return( FALSE );
    }
}

BOOL CRegistry::EnumerateValues( const DWORD    value_index,
                                 string&        name_of_value,
                                 KeyValueTypes& type_code,
                                 LPBYTE         data_buffer,
                                 DWORD&         size_of_data_buffer )
{
  DWORD temp_type_code = type_code;

  TCHAR temp_name[ 2048 ];

  ::ZeroMemory( temp_name, sizeof( temp_name ) );

  DWORD temp_name_size = DIMENSION_OF( temp_name );

  try
    {
      m_ErrorCode = ::RegEnumValue( m_KeyHandle,
                                    value_index,
                                    temp_name,
                                    &temp_name_size,
                                    NULL,
                                    &temp_type_code,
                                    data_buffer,
                                    &size_of_data_buffer );

      if ( m_ErrorCode == ERROR_SUCCESS )
        {
          type_code     = (KeyValueTypes) temp_type_code;
          name_of_value = temp_name;

          return( TRUE );
        }
      else
        return( FALSE );
    }
  catch( ... )
    {
      m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
      return( FALSE );
    }
}

BOOL CRegistry::GetBinaryValue( LPCTSTR name_of_value, CByteArray& return_array )
{
  if ( name_of_value == NULL )
    {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
    }

  QueryInfo();

  DWORD size_of_buffer = m_LongestValueDataLength;

  LPBYTE memory_buffer = (LPBYTE) new BYTE[ size_of_buffer ];

  if ( memory_buffer == NULL )
    {
      m_ErrorCode = ::GetLastError();
      return( FALSE );
    }

  BOOL return_value = TRUE;

  KeyValueTypes type = typeBinary;

  if ( QueryValue( name_of_value, type, memory_buffer, size_of_buffer ) != FALSE )
    {
      /*
      ** We've got data so give it back to the caller
      */

      return_array.RemoveAll();

      DWORD index = 0;

      while( index < size_of_buffer )
        {
          return_array.Add( memory_buffer[ index ] );
          index++;
        }

      return_value = TRUE;
    }
  else
    return_value = FALSE;

  delete [] memory_buffer;

  return( return_value );
}

void CRegistry::GetClassName( CString& class_name ) const
{
  class_name = m_ClassName;
}

void CRegistry::GetComputerName( CString& computer_name ) const
{
  computer_name = m_ComputerName;
}

BOOL CRegistry::GetDoubleWordValue( LPCTSTR name_of_value, DWORD& return_value )
{
  if ( name_of_value == NULL )
    {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
    }

  DWORD size_of_buffer = sizeof( DWORD );

  KeyValueTypes type = typeDoubleWord;

  return( QueryValue( name_of_value, type, (LPBYTE) &return_value, size_of_buffer ) );
}

BOOL CRegistry::GetErrorCode( void ) const
{
  return( m_ErrorCode );
}

void CRegistry::GetKeyName( CString& key_name ) const
{
  key_name = m_KeyName;
}

int CRegistry::GetNumberOfSubkeys( void ) const
{
  return( (int) m_NumberOfSubkeys );
}

int CRegistry::GetNumberOfValues( void ) const
{
  return( (int) m_NumberOfValues );
}

void CRegistry::GetRegistryName( CString& registry_name ) const
{
  registry_name = m_RegistryName;
}

BOOL CRegistry::GetStringValue( LPCTSTR name_of_value, CString& return_string )
{
  if ( name_of_value == NULL )
    {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
    }

  TCHAR temp_string[ 2048 ];
  DWORD size_of_buffer = 2048;

  ::ZeroMemory( temp_string, sizeof( temp_string ) );

  KeyValueTypes type = typeString;

  if ( QueryValue( name_of_value, type, (LPBYTE) temp_string, size_of_buffer ) != FALSE )
    {
      return_string = temp_string;
      return( TRUE );
    }
  else
    {
      return_string.Empty();
      return( FALSE );
    }
}

BOOL CRegistry::GetStringArrayValue( LPCTSTR name_of_value, CStringArray& return_array )
{
  if ( name_of_value == NULL )
    {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
    }

  QueryInfo();

  DWORD size_of_buffer = m_LongestValueDataLength;

  LPBYTE memory_buffer = (LPBYTE) new BYTE[ size_of_buffer ];

  if ( memory_buffer == NULL )
    {
      m_ErrorCode = ::GetLastError();
      return( FALSE );
    }

  BOOL return_value = TRUE;

  KeyValueTypes type = typeMultipleString; // A double NULL terminated string

  if ( QueryValue( name_of_value, type, memory_buffer, size_of_buffer ) != FALSE )
    {
      LPTSTR strings = (LPTSTR) memory_buffer;

      return_array.RemoveAll();

      while( strings[ 0 ] != 0x00 )
        {
          return_array.Add( (LPCTSTR) strings );
          strings += ( _tcslen( (LPCTSTR) strings ) + 1 ); // Paul Ostrowski [postrowski@xantel.com]
        }

      return_value = TRUE;
    }
  else
    return_value = FALSE;

  delete [] memory_buffer;
  return( return_value );
}

BOOL CRegistry::GetValue( LPCTSTR name_of_value, CByteArray& return_array )
{
  if ( name_of_value == NULL )
    {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
    }

  return( GetBinaryValue( name_of_value, return_array ) );
}

BOOL CRegistry::GetValue( LPCTSTR name_of_value, DWORD& return_value )
{
  if ( name_of_value == NULL )
    {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
    }

  return( GetDoubleWordValue( name_of_value, return_value ) );
}

BOOL CRegistry::GetValue( LPCTSTR name_of_value, CString& return_string )
{
  if ( name_of_value == NULL )
    {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
    }

  return( GetStringValue( name_of_value, return_string ) );
}

BOOL CRegistry::GetValue( LPCTSTR name_of_value, CStringArray& return_array )
{
  if ( name_of_value == NULL )
    {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
    }

  return( GetStringArrayValue( name_of_value, return_array ) );
}

BOOL CRegistry::Open( LPCTSTR name_of_subkey_to_open,
                      const CreatePermissions security_access_mask )
{
  try
    {
      if ( m_KeyHandle != (HKEY) NULL )
        {
          ::RegCloseKey( m_KeyHandle );
          m_KeyHandle = (HKEY) NULL;
        }

      m_ErrorCode = ::RegOpenKeyEx( m_RegistryHandle, name_of_subkey_to_open,
                                    NULL, security_access_mask, &m_KeyHandle );

      if ( m_ErrorCode == ERROR_SUCCESS )
        {
          QueryInfo();
          m_KeyName = name_of_subkey_to_open;

          return( TRUE );
        }
      else
        return( FALSE );
    }
  catch( ... )
    {
      m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
      return( FALSE );
    }
}

BOOL CRegistry::QueryInfo( void )
{
  TCHAR class_name[ 2048 ];

  ::ZeroMemory( class_name, sizeof( class_name ) );
  DWORD size_of_class_name = DIMENSION_OF( class_name ) - 1;
  m_ErrorCode = ::RegQueryInfoKey( m_KeyHandle,
                                   class_name,
                                   &size_of_class_name,
                                   (LPDWORD) NULL,
                                   &m_NumberOfSubkeys,
                                   &m_LongestSubkeyNameLength,
                                   &m_LongestClassNameLength,
                                   &m_NumberOfValues,
                                   &m_LongestValueNameLength,
                                   &m_LongestValueDataLength,
                                   &m_SecurityDescriptorLength,
                                   &m_LastWriteTime );

  if ( m_ErrorCode == ERROR_SUCCESS )
    {
      m_ClassName = class_name;
      return( TRUE );
    }
  else
    {
      return( FALSE );
    }
}

BOOL CRegistry::QueryValue( LPCTSTR        name_of_value,
                            KeyValueTypes& value_type,
                            LPBYTE         address_of_buffer,
                            DWORD&         size_of_buffer )
{
  /*
  ** address_of_buffer and size_of_buffer can be NULL
  */

  if ( name_of_value == NULL )
    {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
    }

  // We were passed a pointer, do not trust it

  try
    {
      DWORD temp_data_type = (DWORD) value_type;

      m_ErrorCode = ::RegQueryValueEx( m_KeyHandle,
                                       (LPTSTR) name_of_value,
                                       NULL,
                                       &temp_data_type,
                                       address_of_buffer,
                                       &size_of_buffer );

      if ( m_ErrorCode == ERROR_SUCCESS )
        {
          value_type = (KeyValueTypes) temp_data_type;
          return( TRUE );
        }
      else
        {
          return( FALSE );
        }
    }
  catch( ... )
    {
      m_ErrorCode = ERROR_EXCEPTION_IN_SERVICE;
      return( FALSE );
    }
}

BOOL CRegistry::UnLoad( LPCTSTR name_of_subkey_to_unload )
{
  if ( name_of_subkey_to_unload == NULL )
    {
      m_ErrorCode = ERROR_INVALID_PARAMETER;
      return( FALSE );
    }
  try
    {
      m_ErrorCode = ::RegUnLoadKey( m_KeyHandle, name_of_subkey_to_unload );
      if ( m_ErrorCode == ERROR_SUCCESS )
        return( TRUE );
      else
        return( FALSE );
    }
  catch( ... )
    {
      return( FALSE );
    }
}
#endif // __NO_MFC__

inline int LocalFindReplace(string &s, const TCHAR* psz,
  const TCHAR* pszNew, bool bGlobal)
{
  int iReplaced = 0;

  int pos = 0;
  int len = _tcslen( psz );
  int lenNew = _tcslen( pszNew );
  while( (pos = s.find(psz, pos)) >= 0 )
    {
      s.replace( pos, len, pszNew );
      pos += lenNew;
      iReplaced++;
      if( !bGlobal ) break;
    }

  return iReplaced;
};

BOOL CConfigureApp::InitInstance()
{
#ifndef __NO_MFC__
  // Standard initialization
#ifdef _AFXDLL
  //Enable3dControls();     // Call this when using MFC in a shared DLL
#else
  //Enable3dControlsStatic(); // Call this when linking to MFC statically
#endif
  CConfigureWizard wizard;
#endif // __NO_MFC__

#ifdef _DEBUG
  debuglog.open ("configure.log", ofstream::out | ofstream::app);
  debuglog << "The log has been opened" << endl;;
#endif

#ifndef __NO_MFC__
  m_pMainWnd = &wizard.m_Page1;
#endif // __NO_MFC__

  release_loc = "..\\Release\\";
  debug_loc = "..\\Debug\\";
  bin_loc = "..\\bin\\";
  lib_loc = "..\\lib\\";

  process_opencl_path();

#ifndef __NO_MFC__
  wizard.m_Page2.m_useX11Stubs = useX11Stubs;
  wizard.m_Page2.m_decorateFiles = decorateFiles;
  wizard.m_Page2.m_optionalFiles = optionalFiles;
  wizard.m_Page2.m_standalone = standaloneMode;
  wizard.m_Page2.m_visualStudio7 = visualStudio7;
  wizard.m_Page2.m_build64Bit = build64Bit;
  wizard.m_Page2.m_openMP = openMP;
  //wizard.m_Page2.m_bigCoderDLL = m_bigCoderDLL;

  wizard.m_Page3.m_tempRelease = release_loc.c_str();
  wizard.m_Page3.m_tempDebug = debug_loc.c_str();
  wizard.m_Page3.m_outputBin = bin_loc.c_str();
  wizard.m_Page3.m_outputLib = lib_loc.c_str();
#endif // __NO_MFC__

#ifdef GEN_FONTMAP_OPTION
  if (!generateFontmap)
    {
      CRegistry registry;
      if ( registry.Connect( HKEY_LOCAL_MACHINE ) == TRUE )
        {
          if (
              (registry.Open( "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",
                              CRegistry::permissionRead) == TRUE)
              ||
              (registry.Open( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Fonts",
                              CRegistry::permissionRead) == TRUE)
              ) {
            string filename;
            registry.QueryInfo();

            BOOL result = TRUE;

            filename = bin_loc + "type.xml";
            ofstream fontmap(filename.c_str());

            fontmap << "<?xml version=\"1.0\"?>" << endl;
            for (int index=0; index<registry.GetNumberOfValues(); index++)
              {
                string name_of_value;
                CRegistry::KeyValueTypes type_code;
                BYTE data_buffer[2048];
                LPBYTE ptr_data_buffer = data_buffer;
                DWORD size_of_data_buffer = 2048;
                result = registry.EnumerateValues( index, name_of_value, type_code,
                                                   data_buffer, size_of_data_buffer );

                // The font name looks like this: "XYZ Bold Italic (TrueType)"
                string font_format, font_name, font_full_name;
                int name_length = name_of_value.length();
                int type_pos = name_of_value.rfind('(', -1);
                if (type_pos > 0)
                  {
                    font_full_name = name_of_value.substr(0, type_pos-1);
                    font_name = font_full_name;
                    LocalFindReplace(font_name," ","-",TRUE);
                    font_format = name_of_value.substr(type_pos);
                    if (LocalFindNoCase(font_format,"(TrueType)",0) == 0)
                      {
                        // Information we need to create:
                        //  format="type1"
                        //  metrics="/usr/share/fonts/afms/adobe/phvb8an.afm"
                        //  glyphs="/usr/share/fonts/default/Type1/n019044l.pfb"
                        //  name="Helvetica-Narrow-Bold"
                        //  fullname="Helvetica Narrow Bold"
                        //  familyname="Helvetica"
                        //  weight="Bold"
                        //  version="0.1"
                        //  alias="NimbusSanL-BoldCond"
                        //
                        fontmap << "<fontmap>" << endl;
                        fontmap << "  <font"
                                << " format=\"truetype\""
                                << " glyphs=\"c:\\Windows\\Fonts\\" << data_buffer << "\""
                                << " fullname=\"" << font_full_name << "\""
                                << " name=\"" << font_name << "\""
                                << " />" << endl;
                        fontmap << "</fontmap>" << endl;
                      }
                  }
              }
            fontmap << "</xml>" << endl;
            registry.Close();
          }
        }
    }
#endif

#ifndef __NO_MFC__
  int nResponse = wizard.DoModal();
  // ref. http://redux.imagemagick.org/discussion-server/viewtopic.php?t=7892
  MessageBox(NULL,"B","info",MB_OK);
  if (nResponse == ID_WIZFINISH)
#else // __NO_MFC__
  if (TRUE)
#endif // __NO_MFC__
    {
      bool
        bContinue = TRUE;

#ifndef __NO_MFC__
      useX11Stubs = wizard.m_Page2.m_useX11Stubs;
      decorateFiles = wizard.m_Page2.m_decorateFiles;
      optionalFiles = wizard.m_Page2.m_optionalFiles;
      standaloneMode = wizard.m_Page2.m_standalone;
      visualStudio7 = wizard.m_Page2.m_visualStudio7;
      build64Bit = wizard.m_Page2.m_build64Bit;
      openMP = wizard.m_Page2.m_openMP;
      //m_bigCoderDLL = wizard.m_Page2.m_bigCoderDLL;
      release_loc = wizard.m_Page3.m_tempRelease;
      debug_loc = wizard.m_Page3.m_tempDebug;
      bin_loc = wizard.m_Page3.m_outputBin;
      lib_loc = wizard.m_Page3.m_outputLib;

      int projectType = wizard.m_Page2.m_projectType;

      MyWaitDlg waitdlg(&bContinue);
#endif // __NO_MFC__

      string theprojectname;
      switch (projectType)
        {
        case SINGLETHREADEDSTATIC:
          theprojectname = "..\\VisualStaticST";
          m_bigCoderDLL = TRUE;
          break;
        case MULTITHREADEDSTATIC:
          theprojectname = "..\\VisualStaticMT";
          m_bigCoderDLL = TRUE;
          break;
        case MULTITHREADEDSTATICDLL:
          theprojectname = "..\\VisualStaticMTDLL";
          m_bigCoderDLL = TRUE;
          break;
        default:
        case MULTITHREADEDDLL:
          theprojectname = "..\\VisualDynamicMT";
          m_bigCoderDLL = FALSE;
          break;
        }
      if (visualStudio7)
        theprojectname += ".sln";
      else
        theprojectname += ".dsw";
      //ofstream dsw(theprojectname.c_str());
      if (visualStudio7)
        workspace = new ConfigureVS7Workspace();
      else
        workspace = new ConfigureVS6Workspace();
      workspace->m_stream.open(theprojectname.c_str());

      workspace->write_start();

      waitdlg.Pumpit();

      standard_includes_list.push_back("..\\..");
      //standard_includes_list.push_back("..\\..\\MagickCore");
      standard_includes_list.push_back("..\\..\\xlib");
      standard_includes_list.push_back("..\\..\\Magick++\\lib");
      //standard_includes_list.push_back("..\\..\\MagickArgs");

      // Write all library project files:
      if (projectType == MULTITHREADEDDLL)
        {
          // FIXME: Only CORE_MagickCore, UTIL_animate, UTIL_display, &
          // UTIL_import should link with X11
          if (!useX11Stubs)
            {
              lib_shared_list.push_back("X11.lib");
            }
        }
      lib_shared_list.push_back("kernel32.lib");
      lib_shared_list.push_back("user32.lib");
      lib_shared_list.push_back("gdi32.lib");
      lib_shared_list.push_back("odbc32.lib");
      lib_shared_list.push_back("odbccp32.lib");
      lib_shared_list.push_back("ole32.lib");
      lib_shared_list.push_back("oleaut32.lib");
      lib_shared_list.push_back("winmm.lib");
      // lib_shared_list.push_back("dxguid.lib");
      lib_shared_list.push_back("wsock32.lib");
      lib_shared_list.push_back("advapi32.lib");
      //lib_shared_list.push_back("scrnsave.lib");

      ConfigureProject *dummy_project;
      if (visualStudio7)
        dummy_project = new ConfigureVS7Project();
      else
        dummy_project = new ConfigureVS6Project();
      dummy_project->name = "All";
      dummy_project->type = -1;
      dependency_list.push_back(dummy_project);
#ifdef _DEBUG
      debuglog  << "dependency:" << dummy_project->name.c_str() << endl;
#endif
      if (visualStudio7)
        workspace->write_begin_project(dummy_project, "All", ".\\All\\All.vcproj");

      waitdlg.Pumpit();
      process_project_replacements("..","*","*.in",NULL,OP_REPLACEFILES);
      //
      // This is pretty kludgy. It looks in the coder directory and swaps the library and
      // module files back and forth. These are mutually exclusive setups, so only one can
      // be used at a time.
      //
      if (m_bigCoderDLL)
        {
          process_project_replacements("..","coders","LIBRARY.bak","LIBRARY.txt",OP_RENAMEFILES);
          process_project_replacements("..","coders","MODULE.txt","MODULE.bak",OP_RENAMEFILES);
          if (optionalFiles)
            {
              process_project_replacements("..","contrib","LIBRARY.bak","LIBRARY.txt",OP_RENAMEFILES);
              process_project_replacements("..","contrib","MODULE.txt","MODULE.bak",OP_RENAMEFILES);
            }
          process_project_replacements("..","filters","LIBRARY.bak","LIBRARY.txt",OP_RENAMEFILES);
          process_project_replacements("..","filters","MODULE.txt","MODULE.bak",OP_RENAMEFILES);
        }
      else
        {
          process_project_replacements("..","coders","LIBRARY.txt","LIBRARY.bak",OP_RENAMEFILES);
          process_project_replacements("..","coders","MODULE.bak","MODULE.txt",OP_RENAMEFILES);
          if (optionalFiles)
            {
              process_project_replacements("..","contrib","LIBRARY.txt","LIBRARY.bak",OP_RENAMEFILES);
              process_project_replacements("..","contrib","MODULE.bak","MODULE.txt",OP_RENAMEFILES);
            }
          process_project_replacements("..","filters","LIBRARY.txt","LIBRARY.bak",OP_RENAMEFILES);
          process_project_replacements("..","filters","MODULE.bak","MODULE.txt",OP_RENAMEFILES);
        }
      //waitdlg.Pumpit();
      process_project_type("..",projectType,"THIRDPARTY.txt",THIRDPARTY);
      waitdlg.Pumpit();
      process_project_type("..",projectType,"LIBRARY.txt",   LIBRARY);
      waitdlg.Pumpit();
      process_project_type("..",projectType,"STATICLIB.txt", STATICLIB);
      waitdlg.Pumpit();
      process_project_type("..",projectType,"MODULE.txt",    MODULE);
      waitdlg.Pumpit();
      process_project_type("..",projectType,"ADD_ON.txt",    ADD_ON);
      waitdlg.Pumpit();
      // consoleMode = true;
      process_project_type("..",projectType,"UTILITY.txt",   UTILITY);
      waitdlg.Pumpit();
      // consoleMode = false;
      process_project_type("..",projectType,"PROJECT.txt",   PROJECT);
      waitdlg.Pumpit();

      consoleMode = true;
      if (!visualStudio7)
        workspace->write_begin_project(dummy_project, "All", ".\\All\\All.dsp");
      waitdlg.Pumpit();
      generate_dependencies(dummy_project, true, true, true);
      workspace->write_end_project(dummy_project);

      waitdlg.Pumpit();
      workspace->write_end();

      // Let's be good citizens and clean up all of our project objects
      for (
           list<ConfigureProject*>::iterator depit = dependency_list.begin();
           depit != dependency_list.end();
           depit++)
        {
          delete (*depit);
        }
      if (workspace)
        delete workspace;
    }
#ifdef _DEBUG
  debuglog.close();
#endif
  // Since the dialog has been closed, return FALSE so that we exit the
  //  application, rather than start the application's message pump.
  return FALSE;
}

typedef struct _ConfigureInfo
{
  char
    *extn,
    *group;
} ConfigureInfo;

void CConfigureApp::process_opencl_path()
{
  const string opencl_include_amd   = "$(AMDAPPSDKROOT)include";
  const string opencl_include_cuda  = "$(CUDA_PATH)include";
  const string opencl_include_intel = "$(INTELOCLSDKROOT)include";
  const string opencl_libdir_x86_amd   = "$(AMDAPPSDKROOT)lib\\x86";
  const string opencl_libdir_x64_amd   = "$(AMDAPPSDKROOT)lib\\x86_64";
  const string opencl_libdir_x86_cuda  = "$(CUDA_PATH)lib\\Win32";
  const string opencl_libdir_x64_cuda  = "$(CUDA_PATH)lib\\Win64";
  const string opencl_libdir_x86_intel = "$(INTELOCLSDKROOT)lib\\x86";
  const string opencl_libdir_x64_intel = "$(INTELOCLSDKROOT)lib\\x64";

  with_opencl = false;
  if (getenv("AMDAPPSDKROOT") != NULL)
  {
    with_opencl = true;
    opencl_include = opencl_include_amd;
    opencl_libdir = (build64Bit == TRUE) ? opencl_libdir_x64_amd : opencl_libdir_x86_amd;
  }
  else if (getenv("CUDA_PATH") != NULL)
  {
    with_opencl = true;
    opencl_include = opencl_include_cuda;
    opencl_libdir = (build64Bit == TRUE) ? opencl_libdir_x64_cuda : opencl_libdir_x86_cuda;
  }
  else if (getenv("INTELOCLSDKROOT") != NULL)
  {
    with_opencl = true;
    opencl_include = opencl_include_intel;
    opencl_libdir = (build64Bit == TRUE) ? opencl_libdir_x64_intel : opencl_libdir_x86_intel;
  }

  if (with_opencl)
  {
    string chcfg = "\r\n/* Define to use OpenCL */\r\n"
      "#define MAGICKCORE__OPENCL 1\r\n"
#if defined __APPLE__
      "#define MAGICKCORE_HAVE_OPENCL_CL_H 1\r\n";
#else
      "#define MAGICKCORE_HAVE_CL_CL_H 1\r\n";
#endif

    // enable MAGICKCORE__OPENCL in config file
    string str, line;
    const char *cfg_filename             = "..\\magick\\magick-baseconfig.h.in";
    ifstream infile(cfg_filename, ifstream::in | ifstream::binary);
    if (infile.is_open())
    {
      while (getline(infile, line))
        str += line + "\n";
      infile.close();
      size_t pos = str.find("MAGICKCORE__OPENCL");
      if (pos == string::npos)
        str += chcfg;
      ofstream outfile(cfg_filename, ofstream::out | ofstream::binary);
      if (outfile.is_open())
      {
        outfile << str;
        outfile.close();
      }
    }
  }
}

ConfigureProject *CConfigureApp::write_project_lib( bool dll,
                                                    int runtime,
                                                    int project_type,
                                                    string root,
                                                    string search,
                                                    string dspname,
                                                    string prefix,
                                                    string extn )
{
  string libname = prefix.c_str();
  libname += dspname.c_str();
  bool bNeedsRelo = (search.compare("*") != 0);
  bool bNeedsMagickpp = (extn.compare("cpp") == 0);

  //ofstream dsp(get_project_name(
  //  dll?DLLPROJECT:LIBPROJECT,runtime,root,prefix,dspname).c_str());
  if (visualStudio7)
    project = new ConfigureVS7Project();
  else
    project = new ConfigureVS6Project();
  project->m_stream.open(get_project_name(dll?DLLPROJECT:LIBPROJECT,runtime,
                                          root,prefix,dspname).c_str());

  string outname;
  string bin_path;
  string lib_path;
  string debug_path;
  string release_path;
  string extra_path;
  int levels = LocalGetFieldCount(root,'\\');
  {
    for (int j=0; j<(levels-2); j++)
      extra_path += "..\\";
  }
  if (bin_loc[0]=='.')
    bin_path += extra_path;
  bin_path += bin_loc;
  if (lib_loc[0]=='.')
    lib_path += extra_path;
  lib_path += lib_loc;
  if (debug_loc[0]=='.')
    debug_path += extra_path;
  debug_path += debug_loc;
  if (release_loc[0]=='.')
    release_path += extra_path;
  release_path += release_loc;

  // Temporary kludge to identify COM DLL's that need to be registered as part
  // of the build process.
  bool isCOMproject = false;
  string strPath = root.c_str();
  if (LocalFindNoCase(strPath,"\\ATL\\",0) > 0)
    isCOMproject = true;

  project->write_begin_project(libname.c_str(), dll?DLLPROJECT:LIBPROJECT);

  project->write_configuration(libname.c_str(), (build64Bit ? "x64 Release" : "Win32 Release"), 0);

  project->write_properties(libname.c_str(),
                            get_full_path(root + "\\",lib_path).c_str(), // output
                            get_full_path(root + "\\",release_path).c_str(), // intermediate
                            "", // target
                            dll?DLLPROJECT:LIBPROJECT, 0);

  project->write_pre_build_event(prebuild_cmd_list);

  outname = prefix.c_str();
  if (project_type != ADD_ON)
    outname += "RL_";
  outname += dspname.c_str();
  if (project_type != ADD_ON)
    outname += "_";

  project->write_cpp_compiler_tool(root,extra_path,
                                   includes_list,standard_includes_list,defines_list,
                                   runtime, project_type, dll?DLLPROJECT:LIBPROJECT, 0);

  project->write_midl_compiler_tool(root,extra_path,
                                    outname,get_full_path(root + "\\",lib_path).c_str(),
                                    runtime, project_type, dll?DLLPROJECT:LIBPROJECT, 0,
                                    isCOMproject);

  project->write_res_compiler_tool(root,extra_path,
                                   runtime, project_type, dll?DLLPROJECT:LIBPROJECT, 0);

  // FIXME: update to handle .def files
  project->write_link_tool(root,extra_path,module_definition_file,outname,bNeedsMagickpp,
                           bNeedsRelo,lib_shared_list,lib_release_list,additional_libdir_list,
                           get_full_path(root + "\\",lib_path).c_str(),
                           dll?get_full_path(root + "\\",bin_path).c_str():get_full_path(root + "\\",lib_path).c_str(),
                           runtime, project_type, dll?DLLPROJECT:LIBPROJECT, 0);

  if (dll && isCOMproject)
    {
      string trigger, target;

      trigger = get_full_path(root + "\\",release_path);
      trigger += libname;
      trigger += "\\regsvr32.trg";

      target = get_full_path(root + "\\",bin_path);
      target += outname;
      target += ".dll";

      project->write_custom_tool(trigger.c_str(), target.c_str(),
                                 runtime, project_type, dll?DLLPROJECT:LIBPROJECT, 0, isCOMproject);
    }

  project->write_configuration(libname.c_str(), (build64Bit ? "x64 Debug" : "Win32 Debug"), 1);

  project->write_properties(libname.c_str(),
                            get_full_path(root + "\\",lib_path).c_str(), // output
                            get_full_path(root + "\\",debug_path).c_str(), // intermediate
                            "", // target
                            dll?DLLPROJECT:LIBPROJECT, 1);

  project->write_pre_build_event(prebuild_cmd_list);

  outname = prefix.c_str();
  if (project_type != ADD_ON)
    outname += "DB_";
  outname += dspname.c_str();
  if (project_type != ADD_ON)
    outname += "_";

  project->write_cpp_compiler_tool(root,extra_path,
                                   includes_list,standard_includes_list,defines_list,
                                   runtime, project_type, dll?DLLPROJECT:LIBPROJECT, 1);

  project->write_midl_compiler_tool(root,extra_path,
                                    outname,get_full_path(root + "\\",lib_path).c_str(),
                                    runtime, project_type, dll?DLLPROJECT:LIBPROJECT, 1, isCOMproject);

  project->write_res_compiler_tool(root,extra_path,
                                   runtime, project_type, dll?DLLPROJECT:LIBPROJECT, 1);

  project->write_link_tool(root,extra_path,module_definition_file,outname,bNeedsMagickpp,
                           bNeedsRelo,lib_shared_list,lib_debug_list,additional_libdir_list,
                           get_full_path(root + "\\",lib_path).c_str(),
                           dll?get_full_path(root + "\\",bin_path).c_str():get_full_path(root + "\\",lib_path).c_str(),
                           runtime, project_type, dll?DLLPROJECT:LIBPROJECT, 1);

  if (dll && isCOMproject)
    {
      string trigger, target;

      trigger = get_full_path(root + "\\",debug_path);
      trigger += libname;
      trigger += "\\regsvr32.trg";

      target = get_full_path(root + "\\",bin_path);
      target += outname;
      target += ".dll";

      project->write_custom_tool(trigger.c_str(), target.c_str(),
                                 runtime, project_type, dll?DLLPROJECT:LIBPROJECT, 1,
                                 isCOMproject);
    }

  project->write_configuration(libname.c_str(), "", 2);

  project->write_begin_target(libname.c_str());

  string dir;
  string spec;
  string group;

  const ConfigureInfo
    valid_dirs[] = {
      { ".c",   "src" },
      { ".cpp", "src" },
      { ".def", "src" },
      { ".idl", "src" },
      { ".h",   "include" },
      { ".rc",  "resource" },
      { ".obj", "object"},
      { NULL,   NULL  }
    };

  int i = 0;
  group = valid_dirs[i].group;
  project->write_begin_group(group.c_str());
  while (1)
    {
      if (valid_dirs[i].group == NULL)
        break;

      dir = root.c_str();
      dir += "\\";
      if (search.length() > 0)
        spec = search.c_str();
      else
        spec = dspname.c_str();
      spec += valid_dirs[i].extn;
      project->generate_dir(dir.c_str(), spec.c_str(), 0, project_type, exclude_list);
      i++;
      if ((valid_dirs[i].group == NULL) || (group.compare(valid_dirs[i].group) != 0))
        {
          // add in any hard coded sources from the config file
          if (group.compare("src") == 0)
            {
              for (list<string>::iterator it = source_list.begin();
                   it != source_list.end(); it++)
                project->write_file((*it).c_str());
              if (dspname.compare("accelerate") == 0)
              {
                for (list<string>::iterator it = filter_opencl_list.begin();
                     it != filter_opencl_list.end(); it++)
                {
                  if ((*it).find(".c") != string::npos)
                    project->write_file((*it).c_str());
                }
              }
            }
          // add in any hard coded headers from the config file
          if (group.compare("include") == 0)
            {
              if (dspname.compare("accelerate") == 0)
              {
                for (list<string>::iterator it = filter_opencl_list.begin();
                     it != filter_opencl_list.end(); it++)
                {
                  if ((*it).find(".h") != string::npos)
                    project->write_file((*it).c_str());
                }
              }
            }
          // add in any hard coded resources from the config file
          if (group.compare("resource") == 0)
            {
              for (list<string>::iterator it2 = resource_list.begin();
                   it2 != resource_list.end(); it2++)
                project->write_file((*it2).c_str());
            }
          // add in any hard coded objects from the config file
          if (group.compare("object") == 0)
            {
              for (list<string>::iterator it = object_list.begin();
                   it != object_list.end(); it++)
                project->write_file((*it).c_str());
            }
          project->write_end_group();
          if (valid_dirs[i].group != NULL)
            {
              group = valid_dirs[i].group;
              if ((group.compare("object") == 0) && (object_list.empty()))
              {
                i++;
                if (valid_dirs[i].group != NULL)
                  group = valid_dirs[i].group;
                else
                  break;
              }
              project->write_begin_group(group.c_str());
            }
        }
    }
  //project->write_end_group();
  project->write_end_target();
  project->write_end_project();

  project->m_stream.close();
  return project;
}

ConfigureProject *CConfigureApp::write_project_exe(
  int runtime,
  int project_type,
  string root,
  string search,
  string dspname,
  string prefix,
  string extn)
{
  string libname = prefix.c_str();
  libname += dspname.c_str();
  bool bNeedsMagickpp = (extn.compare("cpp") == 0);

  //ofstream dsp(get_project_name(
  //  EXEPROJECT,runtime,root,prefix,dspname).c_str());
  if (visualStudio7)
    project = new ConfigureVS7Project();
  else
    project = new ConfigureVS6Project();
  project->m_stream.open(get_project_name(
                                          EXEPROJECT,runtime,root,prefix,dspname).c_str());

  string outname;
  string bin_path;
  string lib_path;
  string debug_path;
  string release_path;
  string extra_path;

  int levels = LocalGetFieldCount(root,'\\');
  {
    for (int j=0; j<(levels-2); j++)
      extra_path += "..\\";
  }
  if (bin_loc[0]=='.')
    bin_path += extra_path;
  bin_path += bin_loc;
  if (lib_loc[0]=='.')
    lib_path += extra_path;
  lib_path += lib_loc;
  if (debug_loc[0]=='.')
    debug_path += extra_path;
  debug_path += debug_loc;
  if (release_loc[0]=='.')
    release_path += extra_path;
  release_path += release_loc;

  project->write_begin_project(libname.c_str(), EXEPROJECT);

  project->write_configuration(libname.c_str(), (build64Bit ? "x64 Release" : "Win32 Release"), 0);

  project->write_properties(libname.c_str(),
                            get_full_path(root + "\\",bin_path).c_str(), // output
                            get_full_path(root + "\\",release_path).c_str(), // intermediate
                            "", // target
                            EXEPROJECT, 0);

  if (decorateFiles)
    outname = prefix.c_str();
  else
    outname = "";
  if (decorateFiles)
    outname += "RL_";
  outname += dspname.c_str();

  project->write_cpp_compiler_tool(root,extra_path,
                                   includes_list,standard_includes_list,defines_list,
                                   runtime, project_type, EXEPROJECT, 0);

  project->write_midl_compiler_tool(root,extra_path,
                                    outname,get_full_path(root + "\\",lib_path).c_str(),
                                    runtime, project_type, EXEPROJECT, 0, false);

  project->write_res_compiler_tool(root,extra_path,
                                   runtime, project_type, EXEPROJECT, 0);

  project->write_link_tool(root,extra_path,module_definition_file,outname,bNeedsMagickpp,
                           false,lib_shared_list,lib_release_list,additional_libdir_list,
                           get_full_path(root + "\\",lib_path).c_str(),
                           get_full_path(root + "\\",bin_path).c_str(),
                           runtime, project_type, EXEPROJECT, 0);

  project->write_configuration(libname.c_str(), (build64Bit ? "x64 Debug" : "Win32 Debug"), 1);

  project->write_properties(libname.c_str(),
                            get_full_path(root + "\\",bin_path).c_str(), // output
                            get_full_path(root + "\\",debug_path).c_str(), // intermediate
                            "", // target
                            EXEPROJECT, 1);

  if (decorateFiles)
    outname = prefix.c_str();
  else
    outname = "";
  if (decorateFiles)
    outname += "DB_";
  outname += dspname.c_str();

  project->write_cpp_compiler_tool(root,extra_path,
                                   includes_list,standard_includes_list,defines_list,
                                   runtime, project_type, EXEPROJECT, 1);

  project->write_midl_compiler_tool(root,extra_path,
                                    outname,get_full_path(root + "\\",lib_path).c_str(),
                                    runtime, project_type, EXEPROJECT, 1, false);

  project->write_res_compiler_tool(root,extra_path,
                                   runtime, project_type, EXEPROJECT, 1);

  project->write_link_tool(root,extra_path,module_definition_file,outname,bNeedsMagickpp,
                           false,lib_shared_list,lib_debug_list,additional_libdir_list,
                           get_full_path(root + "\\",lib_path).c_str(),
                           get_full_path(root + "\\",bin_path).c_str(),
                           runtime, project_type, EXEPROJECT, 1);

  project->write_configuration(libname.c_str(), "", 2);

  project->write_begin_target(libname.c_str());

  string dir;
  string spec;
  string group;

  const ConfigureInfo
    valid_dirs[] = {
      { ".c",   "src" },
      { ".cpp", "src" },
      { ".def", "src" },
      { ".idl", "src" },
      { ".h",   "include" },
      { ".rc",  "resource" },
      { NULL,   NULL }
    };

  group = valid_dirs[0].group;
  project->write_begin_group(group.c_str());
  {
    for (list<string>::iterator it = source_list.begin();
         it != source_list.end();
         it++)
      {
        project->write_file((*it).c_str());
      }
  }
  for (int i=0; valid_dirs[i].group != NULL; i++)
    {
      if (_tcsicmp(group.c_str(), valid_dirs[i].group) != 0)
        {
          project->write_end_group();
          group = valid_dirs[i].group;
          project->write_begin_group(group.c_str());
        }

      dir = root.c_str();
      dir += "\\";
      if (search.length() > 0)
        spec = search.c_str();
      else
        spec = dspname.c_str();
      spec += valid_dirs[i].extn;
      project->generate_dir(dir.c_str(), spec.c_str(), 0, project_type, exclude_list);
    }
  project->write_end_group();
  project->write_end_target();
  project->write_end_project();

  project->m_stream.close();
  return project;
}

// Destructor
ConfigureProject::~ConfigureProject ( void )
{
  // Nothing to do
  int i = 0;
}

ConfigureProject& ConfigureProject::operator=(const ConfigureProject& obj)
{
  return *this;
}

ConfigureProject::ConfigureProject(const ConfigureProject& obj)
{
  *this = obj;
}

// Obtain list of sources from directory?
void ConfigureProject::generate_dir(
    const char *dir, const char *spec,
    int nestinglevel, int project_type,
    list<string> &exclude_list)
{
  static const char *exclude_from_build[] =
    {
      ".", "..", "CVS", NULL
    };

  string path = dir;
  if (path[path.length() - 1] != '\\') {
    path += "\\";
  }

  WIN32_FIND_DATA data;

  string searchfor = path + "*";
  HANDLE handle = FindFirstFile(searchfor.c_str(), &data);
  if (handle != INVALID_HANDLE_VALUE)
    {
      do
        {
          bool skip = false;

          for (int i=0; exclude_from_build[i] != NULL; i++)
            {
              if (_stricmp(data.cFileName, exclude_from_build[i]) == 0)
                skip = true;
            }

          if (skip) continue;

          if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
            {
              searchfor = path + data.cFileName;
              if (is_project_type(searchfor.c_str(), project_type) == true)
                generate_dir(searchfor.c_str(), spec,
                             ++nestinglevel, project_type, exclude_list);
            }
        } while (FindNextFile(handle, &data));
      FindClose(handle);
    }

  string otherpath = "..\\" + path;
  string specpath = otherpath + spec;
  handle = FindFirstFile(specpath.c_str(), &data);
  if (handle != INVALID_HANDLE_VALUE)
    {
      do
        {
          bool skip = false;

          for (int i=0; exclude_from_build[i] != NULL; i++)
            if (_stricmp(data.cFileName, exclude_from_build[i]) == 0)
              skip = true;

          for (list<string>::iterator it = exclude_list.begin();
               it != exclude_list.end();
               it++)
            {
              if (_stricmp(data.cFileName, (*it).c_str()) == 0)
                skip = true;
            }

          if (skip) continue;

          if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
            continue;
          if (project_type == UTILITY || project_type == PROJECT
              || project_type == ADD_ON || project_type == MODULE)
            {
              int levels = LocalGetFieldCount(path,'\\');
              string relpath = "..\\";
              for (int j=0; j<(levels-3); j++)
                relpath += "..\\";
              relpath += path;
              relpath += data.cFileName;
              write_file(get_full_path(dir,relpath).c_str());
            }
          else
            {
              string relpath = otherpath + data.cFileName;
              write_file(get_full_path("",relpath).c_str());
            }

        } while (FindNextFile(handle, &data));
      FindClose(handle);
    }
}

// --------------------------------------------------------------------------------------------------
// Visual Studio 6
// --------------------------------------------------------------------------------------------------

ConfigureProject::ConfigureProject()
{
  UuidCreate((UUID *)&m_guid);
  wsprintf(
           m_GuidText,
           "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
           m_guid.Data1,
           m_guid.Data2,
           m_guid.Data3,
           m_guid.Data4[0], m_guid.Data4[1],
           m_guid.Data4[2], m_guid.Data4[3], m_guid.Data4[4], m_guid.Data4[5], m_guid.Data4[6], m_guid.Data4[7]
           );
}

ConfigureWorkspace::ConfigureWorkspace()
{
  UuidCreate((UUID *)&m_guid);
  wsprintf(
           m_GuidText,
           //"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
           //"%04X-%02X-%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
           "8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942",
           m_guid.Data1,
           m_guid.Data2,
           m_guid.Data3,
           m_guid.Data4[0], m_guid.Data4[1],
           m_guid.Data4[2], m_guid.Data4[3], m_guid.Data4[4], m_guid.Data4[5], m_guid.Data4[6], m_guid.Data4[7]
           );
}

void ConfigureVS6Workspace::write_start()
{
  m_stream << "Microsoft Developer Studio Workspace File, Format Version 6.00" << endl;
  m_stream << "# WARNING: DO NOT EDIT OR DELETE THIS WORKSPACE FILE!" << endl;
  m_stream << endl;
  m_stream << "###############################################################################" << endl;
  m_stream << endl;
}

void ConfigureVS6Workspace::write_end()
{
  m_stream << "Global:" << endl;
  m_stream << endl;
  m_stream << "Package=<5>" << endl;
  m_stream << "{{{" << endl;
  m_stream << "}}}" << endl;
  m_stream << endl;
  m_stream << "Package=<3>" << endl;
  m_stream << "{{{" << endl;
  m_stream << "}}}" << endl;
  m_stream << endl;
  m_stream << "###############################################################################" << endl;
  m_stream << endl;
}

void ConfigureVS6Workspace::write_begin_project(ConfigureProject *project,const char *name, const char *filename)
{
  m_stream << "Project: \"" << name << "\"=\"" << filename << "\" - Package Owner=<4>" << endl;
  m_stream << endl;
  m_stream << "Package=<5>" << endl;
  m_stream << "{{{" << endl;
  m_stream << "}}}" << endl;
  m_stream << endl;
  m_stream << "Package=<4>" << endl;
  m_stream << "{{{" << endl;
}

void ConfigureVS6Workspace::write_end_project(ConfigureProject *project)
{
  m_stream << "}}}" << endl;
  m_stream << endl;
  m_stream << "###############################################################################" << endl;
  m_stream << endl;
}

void ConfigureVS6Workspace::write_project_dependency(ConfigureProject *project,
  string &dep_name)
{
  m_stream << "    Begin Project Dependency" << endl;
  m_stream << "    Project_Dep_Name " << dep_name << endl;
  m_stream << "    End Project Dependency" << endl;
}

void ConfigureVS6Workspace::write_project_dependency(ConfigureProject *project,
  const char *dep_name)
{
  string name = dep_name;
  write_project_dependency(project,name);
}

// Destructor
ConfigureVS6Project::~ConfigureVS6Project ( void )
{
  // Nothing to do
  int i = 0;
}

void ConfigureVS6Project::write_begin_project(const char *name, int type)
{
  switch (type)
    {
    case DLLPROJECT:
    case LIBPROJECT:
      // --------------------------------------------------------------------------------------------------------
      m_stream << "# Microsoft Developer Studio Project File - Name=\"" << name << "\" - Package Owner=<4>" << endl;
      m_stream << "# Microsoft Developer Studio Generated Build File, Format Version 6.00" << endl;
      m_stream << "# ** DO NOT EDIT **" << endl;
      m_stream << endl;
      if (type == DLLPROJECT)
        m_stream << "# TARGTYPE \"Win32 (x86) Dynamic-Link Library\" 0x0102" << endl;
      else
        m_stream << "# TARGTYPE \"Win32 (x86) Static Library\" 0x0104" << endl;
      m_stream << endl;
      m_stream << "CFG=" << name << " - Win32 Release" << endl;
      m_stream << "!MESSAGE This is not a valid makefile. To build this project using NMAKE," << endl;
      m_stream << "!MESSAGE use the Export Makefile command and run" << endl;
      m_stream << "!MESSAGE " << endl;
      m_stream << "!MESSAGE NMAKE /f \"" << name << ".mak\"." << endl;
      m_stream << "!MESSAGE " << endl;
      m_stream << "!MESSAGE You can specify a configuration when running NMAKE" << endl;
      m_stream << "!MESSAGE by defining the macro CFG on the command line. For example:" << endl;
      m_stream << "!MESSAGE " << endl;
      m_stream << "!MESSAGE NMAKE /f \"" << name << ".mak\" CFG=\"" << name << " - Win32 Release\"" << endl;
      m_stream << "!MESSAGE " << endl;
      m_stream << "!MESSAGE Possible choices for configuration are:" << endl;
      m_stream << "!MESSAGE " << endl;
      if (type == DLLPROJECT)
        {
          m_stream << "!MESSAGE \"" << name << " - Win32 Release\" (based on \"Win32 (x86) Dynamic-Link Library\")" << endl;
          m_stream << "!MESSAGE \"" << name << " - Win32 Debug\" (based on \"Win32 (x86) Dynamic-Link Library\")" << endl;
        }
      else
        {
          m_stream << "!MESSAGE \"" << name << " - Win32 Release\" (based on \"Win32 (x86) Static Library\")" << endl;
          m_stream << "!MESSAGE \"" << name << " - Win32 Debug\" (based on \"Win32 (x86) Static Library\")" << endl;
        }
      m_stream << "!MESSAGE " << endl;
      // --------------------------------------------------------------------------------------------------------
      break;
    case EXEPROJECT:
      // --------------------------------------------------------------------------------------------------------
      m_stream << "# Microsoft Developer Studio Project File - Name=\"" << name << "\" - Package Owner=<4>" << endl;
      m_stream << "# Microsoft Developer Studio Generated Build File, Format Version 6.00" << endl;
      m_stream << "# ** DO NOT EDIT **" << endl;
      m_stream << endl;
      m_stream << "# TARGTYPE \"Win32 (x86) Console Application\" 0x0103" << endl;
      m_stream << endl;
      m_stream << "CFG=" << name << " - Win32 Release" << endl;
      m_stream << "!MESSAGE This is not a valid makefile. To build this project using NMAKE," << endl;
      m_stream << "!MESSAGE use the Export Makefile command and run" << endl;
      m_stream << "!MESSAGE " << endl;
      m_stream << "!MESSAGE NMAKE /f \"" << name << ".mak\"." << endl;
      m_stream << "!MESSAGE " << endl;
      m_stream << "!MESSAGE You can specify a configuration when running NMAKE" << endl;
      m_stream << "!MESSAGE by defining the macro CFG on the command line. For example:" << endl;
      m_stream << "!MESSAGE " << endl;
      m_stream << "!MESSAGE NMAKE /f \"" << name << ".mak\" CFG=\"" << name << " - Win32 Release\"" << endl;
      m_stream << "!MESSAGE " << endl;
      m_stream << "!MESSAGE Possible choices for configuration are:" << endl;
      m_stream << "!MESSAGE " << endl;
      m_stream << "!MESSAGE \"" << name << " - Win32 Release\" (based on \"Win32 (x86) Console Application\")" << endl;
      m_stream << "!MESSAGE \"" << name << " - Win32 Debug\" (based on \"Win32 (x86) Console Application\")" << endl;
      m_stream << "!MESSAGE " << endl;
      // --------------------------------------------------------------------------------------------------------
      break;
    }

  m_stream << endl;
  m_stream << "# Begin Project" << endl;
  m_stream << "# PROP AllowPerConfigDependencies 0" << endl;
  m_stream << "# PROP Scc_ProjName \"\"" << endl;
  m_stream << "# PROP Scc_LocalPath \"\"" << endl;
  m_stream << "CPP=cl.exe" << endl;
  m_stream << "MTL=midl.exe" << endl;
  m_stream << "RSC=rc.exe" << endl;
  m_stream << endl;
}

void ConfigureVS6Project::write_end_project()
{
  m_stream << "# End Project" << endl;
}

void ConfigureVS6Project::write_configuration(const char *name, const char *subname, int type)
{
  switch (type)
    {
    case 0:
      m_stream << "!IF  \"$(CFG)\" == \"" << name << " - " << subname << "\"" << endl;
      m_stream << endl;
      break;
    case 1:
      m_stream << endl;
      m_stream << "!ELSEIF  \"$(CFG)\" == \"" << name << " - " << subname << "\"" << endl;
      m_stream << endl;
      break;
    case 2:
      m_stream << endl;
      m_stream << "!ENDIF " << endl;
      break;
    }
}

void ConfigureVS6Project::write_properties(const char *name,
  const char *outputpath, const char *intermediatepath, const char *targetpath,
    int type, int mode)
{
  switch (type)
    {
    case DLLPROJECT:
    case LIBPROJECT:
    case EXEPROJECT:
      m_stream << "# PROP Use_MFC 0" << endl;
      switch (mode)
        {
        case 0:
          m_stream << "# PROP Use_Debug_Libraries 0" << endl;
          break;
        case 1:
          m_stream << "# PROP Use_Debug_Libraries 1" << endl;
          break;
        }
      m_stream << "# PROP Output_Dir \"" << outputpath << "\"" << endl;
      m_stream << "# PROP Intermediate_Dir \"" << intermediatepath << name << "\"" << endl;
      m_stream << "# PROP Target_Dir \"" << targetpath << "\"" << endl;
      m_stream << "BSC32=bscmake.exe" << endl;
      m_stream << "# ADD BSC32 /nologo" << endl;
      break;
    }
}

void ConfigureVS6Project::write_pre_build_event(list<string> &prebuild_cmd_list)
{
  if (!prebuild_cmd_list.empty())
  {
  }
}

void ConfigureVS6Project::write_res_compiler_tool( string &root, string &extra_path,
                                                   int runtime, int project_type,
                                                   int type, int mode)
{
  switch (mode)
    {
    case 0:
      m_stream << "# ADD RSC /l 0x409 /d \"NDEBUG\"" << endl;
      break;
    case 1:
      m_stream << "# ADD RSC /l 0x409 /d \"_DEBUG\"" << endl;
      break;
    }
}

void ConfigureVS6Project::write_midl_compiler_tool( string &root, string &extra_path,
                                                    string &outname, const char *output_path,
                                                    int runtime, int project_type, int type,
                                                    int mode, bool isCOMproject)
{
  switch (type)
    {
    case DLLPROJECT:
    case LIBPROJECT:
      // --------------------------------------------------------------------------------------------------------
      switch (mode)
        {
        case 0:
          if ((type==DLLPROJECT) && isCOMproject)
            {
              string sources;

              sources = "..\\";
              sources += extra_path;
              sources += root;
              sources += "\\";
              //m_stream << "# ADD MTL /Oicf /out \".\\\" /tlb \".\\" << outname.c_str() << ".tlb\"" << endl;
              m_stream << "# ADD MTL /Oicf /out \"" << get_full_path("",sources).c_str()
                       << "\" /tlb \"" << get_full_path("",sources).c_str() << "\\" << outname.c_str() << ".tlb\"" << endl;
            }
          else
            {
              m_stream << "# ADD MTL /nologo /D \"NDEBUG\" /mktyplib203 /win32" << endl;
            }
          break;
        case 1:
          if ((type==DLLPROJECT) && isCOMproject)
            {
              string sources;

              sources = "..\\";
              sources += extra_path;
              sources += root;
              sources += "\\";
              //m_stream << "# ADD MTL /Oicf /out \".\\\" /tlb \".\\" << outname.c_str() << ".tlb\"" << endl;
              m_stream << "# ADD MTL /Oicf /out \"" << get_full_path("",sources).c_str()
                       << "\" /tlb \"" << get_full_path("",sources).c_str() << "\\" << outname.c_str() << ".tlb\"" << endl;
            }
          else
            {
              m_stream << "# ADD MTL /nologo /D \"_DEBUG\" /mktyplib203 /win32" << endl;
            }
          break;
        }
      // --------------------------------------------------------------------------------------------------------
      break;

    case EXEPROJECT:
      // --------------------------------------------------------------------------------------------------------
      switch (mode)
        {
        case 0:
          m_stream << "# ADD MTL /nologo /D \"NDEBUG\" /mktyplib203 /win32" << endl;
          break;
        case 1:
          m_stream << "# ADD MTL /nologo /D \"_DEBUG\" /mktyplib203 /win32" << endl;
          break;
        }
      // --------------------------------------------------------------------------------------------------------
      break;
    }
}

void ConfigureVS6Project::write_custom_tool( const char *trigger, const char *target,
                                             int runtime, int project_type, int type,
                                             int mode, bool isCOMproject)
{
  switch (type)
    {
    case DLLPROJECT:
      // --------------------------------------------------------------------------------------------------------
      switch (mode)
        {
        case 0:
          if (isCOMproject)
            {
              m_stream << "# Begin Custom Build - Performing registration" << endl;
              m_stream << "\"" << trigger << "\" : \"" << target << "\"" << endl;
              m_stream << "\tregsvr32 /s /c \"" << target << "\"" << endl;
              m_stream << "\techo regsvr32 exec. time >\"" << trigger << "\"" << endl;
              m_stream << "# End Custom Build" << endl;
            }
          break;
        case 1:
          if (isCOMproject)
            {
              m_stream << "# Begin Custom Build - Performing registration" << endl;
              m_stream << "\"" << trigger << "\" : \"" << target << "\"" << endl;
              m_stream << "\tregsvr32 /s /c \"" << target << "\"" << endl;
              m_stream << "\techo regsvr32 exec. time >\"" << trigger << "\"" << endl;
              m_stream << "# End Custom Build" << endl;
            }
          break;
        }
      // --------------------------------------------------------------------------------------------------------
      break;

    case LIBPROJECT:
    case EXEPROJECT:
      break;
    }
}

void ConfigureVS6Project::write_cpp_compiler_tool_begin(
  int type, int mode)
{
  m_stream << "# ADD CPP /nologo";
}

void ConfigureVS6Project::write_cpp_compiler_tool_runtime(
  int runtime, int type, int mode)
{
  switch (mode)
    {
    case 0:
      switch(runtime)
        {
        case SINGLETHREADEDSTATIC:
          m_stream << " /ML";
          break;
        case MULTITHREADEDSTATIC:
          m_stream << " /MT";
          break;
        default:
        case MULTITHREADEDSTATICDLL:
        case MULTITHREADEDDLL:
          m_stream << " /MD";
          break;
        }
      break;
    case 1:
      switch(runtime)
        {
        case SINGLETHREADEDSTATIC:
          m_stream << " /MLd";
          break;
        case MULTITHREADEDSTATIC:
          m_stream << " /MTd";
          break;
        default:
        case MULTITHREADEDSTATICDLL:
        case MULTITHREADEDDLL:
          m_stream << " /MDd";
          break;
        }
    }
}

void ConfigureVS6Project::write_cpp_compiler_tool_options( int type, int mode )
{
  switch (mode)
    {
    case 0:
#ifndef SYMBOLS_ALWAYS
      m_stream << " /W3 /GX /O2";
#else
      m_stream << " /W3 /GX /Zi /O2";
#endif
      break;
    case 1:
      m_stream << " /W3 /Gm /GX /Zi /Od";
      break;
    }
}

void ConfigureVS6Project::write_cpp_compiler_tool_includes( string &root, string &extra_path,
                                                            list<string> &includes_list,
                                                            list<string> &standard_include,
                                                            BOOL bAbsolutePaths )
{
  // The following was only done for the EXEPROJECT case
  // m_stream << " /I \".\"";
  {
    for (
         list<string>::iterator it = standard_include.begin();
         it != standard_include.end();
         it++)
      {
        string relpath;
        if (!bAbsolutePaths)
          relpath = extra_path;
        relpath += *it;
        m_stream << " /I \"" << get_full_path("",relpath).c_str() << "\"";
      }
  }
  {
    for (
         list<string>::iterator it = includes_list.begin();
         it != includes_list.end();
         it++)
      {
        m_stream << " /I \"" << get_full_path(root + "\\",*it).c_str() << "\"";
      }
  }
}

void ConfigureVS6Project::write_cpp_compiler_tool_defines( list<string> &defines_list,
                                                           int type, int mode,
                                                           bool bConsoleMode,
                                                           bool bDynamicMFC)
{
  switch (mode)
    {
    case 0:
      m_stream << " /D \"NDEBUG\"";
      break;
    case 1:
      m_stream << " /D \"_DEBUG\"";
      break;
    }
  if (bConsoleMode && (type == EXEPROJECT))
    m_stream << " /D \"_CONSOLE\"";
  else
    m_stream << " /D \"_WINDOWS\"";
  m_stream << " /D \"WIN32\" /D \"_VISUALC_\" /D \"NeedFunctionPrototypes\"";
  {
    for (
         list<string>::iterator it = defines_list.begin();
         it != defines_list.end();
         it++)
      {
        m_stream << " /D \"" << (*it).c_str() << "\"";
      }
  }
  if (bDynamicMFC&& (type == EXEPROJECT))
    m_stream << "/D \"_AFXDLL\"";
}

void ConfigureVS6Project::write_cpp_compiler_tool_end( int type, int mode )
{
  m_stream << " /FD /c" << endl;
}

void ConfigureVS6Project::write_cpp_compiler_tool( string &root, string &extra_path,
                                                   list<string> &includes_list,
                                                   list<string> &standard_includes_list,
                                                   list<string> &defines_list,
                                                   int runtime, int project_type,
                                                   int type, int mode)
{
  bool bDynamicMFC = ((project_type == PROJECT) && (runtime == MULTITHREADEDDLL));

  write_cpp_compiler_tool_begin(type, mode);
  write_cpp_compiler_tool_runtime(runtime, type, mode);
  write_cpp_compiler_tool_options(type, mode);
  write_cpp_compiler_tool_includes(root, extra_path, includes_list,
                                   standard_includes_list, standaloneMode);
  write_cpp_compiler_tool_defines(defines_list, type, mode, consoleMode, bDynamicMFC);
  write_cpp_compiler_tool_end(type, mode);
}

void ConfigureVS6Project::write_link_tool_begin( const char *input_path,
                                                 const char *output_path,
                                                 list<string> &additional_libdir_list,
                                                 int type, int mode )
{
  switch (type)
    {
    case DLLPROJECT:
    case EXEPROJECT:
      m_stream << "LIB32=link.exe -lib" << endl;
      m_stream << "LINK32=link.exe" << endl;
      m_stream << "# ADD LINK32";
      m_stream << " /libpath:\"";
      for (list<string>::iterator it = additional_libdir_list.begin(); 
           it != additional_libdir_list.end(); it++)
        m_stream << *it << ";";
      m_stream << input_path << "\"";
      break;
    case LIBPROJECT:
      m_stream << "LIB32=link.exe -lib" << endl;
      m_stream << "# ADD LIB32";
      break;
    }
}

void ConfigureVS6Project::write_link_tool_dependencies( string &root, bool bNeedsMagickpp,
                                                        list<string> &shared_lib_list,
                                                        list<string> &lib_list,
                                                        int runtime, int type, int mode)
{
  char *strmode;
  switch (mode)
    {
    case 0:
      strmode = "RL_";
      break;
    case 1:
      strmode = "DB_";
      break;
    }
  if (onebigdllMode || (standaloneMode && (type == EXEPROJECT)))
    {
      switch (runtime)
        {
        case MULTITHREADEDSTATIC:
        case SINGLETHREADEDSTATIC:
        case MULTITHREADEDSTATICDLL:
          {
            string strDepends;
            for (
                 list<ConfigureProject*>::iterator it1a = dependency_list.begin();
                 it1a != dependency_list.end();
                 it1a++)
              {
                /* look for entries with a xxx%syyy%szzz format */
                if (LocalFindNoCase((*it1a)->name,"%s",0) > 0)
                  {
                    /* ignore any entries for utility programs as well */
                    if (LocalFindNoCase((*it1a)->name,"UTIL_",0) != 0)
                      {
                        LocalFormat(strDepends,(*it1a)->name.c_str(),strmode,"_.lib");
                        m_stream << " " << strDepends.c_str() << "";
                      }
                  }
              }
          }
          break;
        default:
        case MULTITHREADEDDLL:
          {
            string strDepends;
            LocalFormat(strDepends,"CORE_%sMagickCore%s",strmode,"_.lib");
            m_stream << " " << strDepends.c_str() << "";
            if (useX11Stubs)
              {
                LocalFormat(strDepends,"CORE_%sxlib%s",strmode,"_.lib");
                m_stream << " " << strDepends.c_str() << "";
              }
            if (bNeedsMagickpp)
              {
                LocalFormat(strDepends,"CORE_%sMagick++%s",strmode,"_.lib");
                m_stream << " " << strDepends.c_str() << "";
              }
            string strPath = root;
            if (LocalFindNoCase(strPath,"\\SDL",0) >= 0)
              {
                LocalFormat(strDepends,"CORE_%sSDL%s",strmode,"_.lib");
                m_stream << " " << strDepends.c_str() << "";
              }
            if (LocalFindNoCase(strPath,"..\\jp2",0) == 0)
              {
                LocalFormat(strDepends,"CORE_%sjp2%s",strmode,"_.lib");
                m_stream << " " << strDepends.c_str() << "";
              }
          }
          break;
        }
    }
  if ((type == DLLPROJECT) || (type == EXEPROJECT))
    {
      {
        for (
             list<string>::iterator it = shared_lib_list.begin();
             it != shared_lib_list.end();
             it++)
          {
            m_stream << " " << (*it).c_str() << "";
          }
      }
    }
  {
    for (
         list<string>::iterator it = lib_list.begin();
         it != lib_list.end();
         it++)
      {
        m_stream << " " << (*it).c_str() << "";
      }
  }
}

void ConfigureVS6Project::write_link_tool_options( bool bNeedsRelo, int type, int mode,
                                                   const string &defs_path)
{
  m_stream << " /nologo /machine:I386";
  switch(type)
    {
    case EXEPROJECT:
      if (consoleMode)
        m_stream << " /incremental:no /subsystem:console";
      else
        m_stream << " /incremental:no /subsystem:windows";
#ifdef SYMBOLS_ALWAYS
      m_stream << " /debug";
#else
      if (mode == 1)
        m_stream << " /debug";
#endif
      break;
    case DLLPROJECT:
      m_stream << " /incremental:no /subsystem:windows";
#ifdef SYMBOLS_ALWAYS
      m_stream << " /debug";
#else
      if (mode == 1)
        m_stream << " /debug";
#endif
      m_stream << " /dll";
      if (bNeedsRelo)
        {
          string strBase;
          LocalFormat(strBase,"%lx",dllbaselocation);
          m_stream << " /base:\"0x" << strBase.c_str() << "\"";
          dllbaselocation += 0x00010000L;
        }
      break;
    case LIBPROJECT:
      m_stream << " /subsystem:windows";
      //if (mode == 1)
      //  m_stream << " /debug";
      break;
    }
}

void ConfigureVS6Project::write_link_tool_output( const char *input_path,
                                                  const char *output_path,
                                                  string &outname,
                                                  int type, int mode )
{
  switch(type)
    {
    case EXEPROJECT:
      m_stream << " /pdb:\"" << output_path << outname.c_str();
      if (decorateFiles)
        m_stream << "_";
      m_stream << ".pdb\"";
      m_stream << " /out:\"" << output_path << outname.c_str();
      if (decorateFiles)
        m_stream << "_";
      m_stream << ".exe\"";
      break;
    case DLLPROJECT:
      m_stream << " /pdb:\"" << output_path << outname.c_str() << ".pdb\"";
      m_stream << " /out:\"" << output_path << outname.c_str() << ".dll\"";
      break;
    case LIBPROJECT:
      m_stream << " /out:\"" << output_path << outname.c_str() << ".lib\"";
      break;
    }
}

void ConfigureVS6Project::write_link_tool_end( int type, int mode )
{
  m_stream << endl;
}

void ConfigureVS6Project::write_link_tool( string &root,
                                           string &extra_path,
                                           const string &defs_path,
                                           string &outname,
                                           bool bNeedsMagickpp,
                                           bool bNeedsRelo,
                                           list<string> &shared_lib_list,
                                           list<string> &lib_list,
                                           list<string> &additional_libdir_list,
                                           const char *input_path,
                                           const char *output_path,
                                           int runtime,
                                           int project_type,
                                           int type,
                                           int mode )
{
  write_link_tool_begin(input_path,output_path,additional_libdir_list,type,mode);
  write_link_tool_dependencies(root,bNeedsMagickpp,shared_lib_list,lib_list,runtime,type,mode);
  write_link_tool_options(bNeedsRelo,type,mode,defs_path);
  write_link_tool_output(input_path,output_path,outname,type,mode);
  write_link_tool_end(type,mode);
}

void ConfigureVS6Project::write_begin_target(const char *name)
{
  m_stream << endl;
  m_stream << "# Begin Target" << endl;
  m_stream << endl;
  m_stream << "# Name \"" << name << " - Win32 Release\"" << endl;
  m_stream << "# Name \"" << name << " - Win32 Debug\"" << endl;
}

void ConfigureVS6Project::write_end_target()
{
  m_stream << "# End Target" << endl;
}

void ConfigureVS6Project::write_begin_group(const char *name)
{
  m_stream << "# Begin Group \"" << name << "\"" << endl;
  m_stream << "# PROP Default_Filter \"\"" << endl;
}

void ConfigureVS6Project::write_end_group()
{
  m_stream << "# End Group" << endl;
}

void ConfigureVS6Project::write_file(const char *filename)
{
  m_stream << "# Begin Source File" << endl;
  m_stream << "SOURCE=" << filename << endl;
  m_stream << "# End Source File" << endl;
}

// --------------------------------------------------------------------------------------------------
// Visual Studio 7
// --------------------------------------------------------------------------------------------------

void ConfigureVS7Workspace::write_start()
{
  m_stream << "Microsoft Visual Studio Solution File, Format Version 7.00" << endl;
}

static ConfigureProject *find_project(string &name)
{
  string strDepends;
  for (
       list<ConfigureProject*>::iterator depit = dependency_list.begin();
       depit != dependency_list.end();
       depit++)
    {
      ConfigureProject *project = (*depit);
      LocalFormat(strDepends,project->name.c_str(),"","");
      if (LocalCompareNoCase(strDepends,(char *)name.c_str()) == 0)
        {
          return project;
        }
    }
#ifdef _DEBUG
  debuglog  << "find_project found nothing for: " << name.c_str() << endl;
#endif
  return (ConfigureProject *) NULL;
}

void ConfigureVS7Workspace::write_end()
{
  m_stream << "Global" << endl;
  m_stream << "\tGlobalSection(SolutionConfiguration) = preSolution" << endl;
  m_stream << "\t\tConfigName.0 = Debug" << endl;
  m_stream << "\t\tConfigName.1 = Release" << endl;
  m_stream << "\tEndGlobalSection" << endl;

  m_stream << "\tGlobalSection(ProjectDependencies) = postSolution" << endl;
  for (
       list<ConfigureProject*>::iterator it0 = dependency_list.begin();
       it0 != dependency_list.end();
       it0++)
    {
      int counter = 0;
      ConfigureProject *project = (*it0);
      for (
           list<string>::iterator it = project->depends.begin();
           it != project->depends.end();
           it++)
        {
          ConfigureProject *dependent_project = find_project((*it));
#ifdef _DEBUG
          debuglog  << "write_end:" << project->name.c_str() << ", " << dependent_project->name.c_str() << endl;
#endif
          if (dependent_project)
            {
              m_stream << "\t\t{"
                       << project->m_GuidText
                       << "}." << counter++ << " = {"
                       << dependent_project->m_GuidText
                       << "}" << endl;
            }
        }
    }
  m_stream << "\tEndGlobalSection" << endl;

  m_stream << "\tGlobalSection(ProjectConfiguration) = postSolution" << endl;
  for (
       list<ConfigureProject*>::iterator it1 = dependency_list.begin();
       it1 != dependency_list.end();
       it1++)
    {
      string guid = (*it1)->m_GuidText;
      m_stream << "\t\t{" << guid.c_str() << "}.Debug.ActiveCfg = Debug|" << (build64Bit ? "x64" : "Win32") << endl;
      m_stream << "\t\t{" << guid.c_str() << "}.Debug.Build.0 = Debug|" << (build64Bit ? "x64" : "Win32") << endl;
      m_stream << "\t\t{" << guid.c_str() << "}.Release.ActiveCfg = Release|" << (build64Bit ? "x64" : "Win32") << endl;
      m_stream << "\t\t{" << guid.c_str() << "}.Release.Build.0 = Release|" << (build64Bit ? "x64" : "Win32") << endl;
    }
  m_stream << "\tEndGlobalSection" << endl;

  m_stream << "\tGlobalSection(ExtensibilityGlobals) = postSolution" << endl;
  m_stream << "\tEndGlobalSection" << endl;
  m_stream << "\tGlobalSection(ExtensibilityAddIns) = postSolution" << endl;
  m_stream << "\tEndGlobalSection" << endl;
  m_stream << "EndGlobal" << endl;
}

void ConfigureVS7Workspace::write_begin_project( ConfigureProject *project,
                                                 const char *name,
                                                 const char *filename)
{
  m_stream << "Project(\"{"
           << m_GuidText
           << "}\") = \""
           << name
           << "\", \""
           << filename
           << "\", \"{"
           << project->m_GuidText
           << "}\""
           << endl;
}

void ConfigureVS7Workspace::write_end_project(ConfigureProject *project)
{
  m_stream << "EndProject" << endl;
}

void ConfigureVS7Workspace::write_project_dependency(ConfigureProject *project,
  string &dep_name)
{
  project->depends.push_back(dep_name);
}

void ConfigureVS7Workspace::write_project_dependency(ConfigureProject *project,
  const char *dep_name)
{
  string name = dep_name;
  write_project_dependency(project,name);
}

// Destructor
ConfigureVS7Project::~ConfigureVS7Project ( void )
{
  // Nothing to do
  int i = 0;
}

void ConfigureVS7Project::write_begin_project(const char *name, int type)
{
  m_stream << "<?xml version=\"1.0\" encoding = \"Windows-1252\"?>" << endl;
  m_stream << "<VisualStudioProject" << endl;
  m_stream << "  ProjectType=\"Visual C++\"" << endl;
  m_stream << "  Version=\"7.00\"" << endl;
  m_stream << "  Name=\"" << name << "\"" << endl;
  m_stream << "  ProjectGUID=\"{" << m_GuidText << "}\"" << endl;
  m_stream << "  Keyword=\"" << (build64Bit ? "x64" : "Win32") << "Proj\">" << endl;
  m_stream << "  <Platforms>" << endl;
  m_stream << "    <Platform Name=\""<< (build64Bit ? "x64" : "Win32") << "\"/>" << endl;
  m_stream << "  </Platforms>" << endl;
}

void ConfigureVS7Project::write_end_project()
{
  m_stream << "  <Globals>" << endl;
  m_stream << "  </Globals>" << endl;
  m_stream << "</VisualStudioProject>" << endl;
}

void ConfigureVS7Project::write_configuration( const char *name,
                                               const char *subname,
                                               int type)
{
  switch (type)
    {
    case 0:
      m_stream << "  <Configurations>" << endl;
      m_stream << "    <Configuration" << endl;
      break;
    case 1:
      m_stream << "    </Configuration>" << endl;
      m_stream << "    <Configuration" << endl;
      break;
    case 2:
      m_stream << "    </Configuration>" << endl;
      m_stream << "  </Configurations>" << endl;
      break;
    }
}

void ConfigureVS7Project::write_properties( const char *name,
                                            const char *outputpath,
                                            const char *intermediatepath,
                                            const char *targetpath,
                                            int type, int mode )
{
  switch (mode)
    {
    case 0:
      m_stream << "      Name=\"Release|" << (build64Bit ? "x64" : "Win32") << "\"" << endl;
      break;
    case 1:
      m_stream << "      Name=\"Debug|" << (build64Bit ? "x64" : "Win32") << "\"" << endl;
      break;
    }
  m_stream << "      OutputDirectory=\"" << outputpath << "\"" << endl;
  m_stream << "      IntermediateDirectory=\"" << intermediatepath << name << "\"" << endl;
  switch (type)
    {
    case EXEPROJECT:
      m_stream << "      ConfigurationType=\"1\"" << endl;
      break;
    case DLLPROJECT:
      m_stream << "      ConfigurationType=\"2\"" << endl;
      break;
    case LIBPROJECT:
      m_stream << "      ConfigurationType=\"4\"" << endl;
      break;
    }
  m_stream << "      UseOfMFC=\"0\"" << endl;
  m_stream << "      ATLMinimizesCRunTimeLibraryUsage=\"FALSE\"" << endl;
  m_stream << "      CharacterSet=\"2\">" << endl;
}

void ConfigureVS7Project::write_pre_build_event(list<string> &prebuild_cmd_list)
{
  if (!prebuild_cmd_list.empty())
  {
    m_stream << "      <Tool" << endl;
    m_stream << "        Name=\"VCPreBuildEventTool\"" << endl;
    if (build64Bit == TRUE)
      m_stream << "        Description=\"Building x86_64 libjpeg-turbo SIMD extensions...\"" << endl;
    else
      m_stream << "        Description=\"Building i386 libjpeg-turbo SIMD extensions...\"" << endl;
    m_stream << "        CommandLine=\"";
    for (list<string>::iterator it = prebuild_cmd_list.begin(); it != prebuild_cmd_list.end(); it++)
    {
      if (it != prebuild_cmd_list.begin())
        m_stream << endl << "                     ";
      m_stream << *it;
    }
    m_stream << "\"" << endl;
    m_stream << "        />" << endl;
  }
}

void ConfigureVS7Project::write_midl_compiler_tool( string &root,
                                                    string &extra_path,
                                                    string &outname,
                                                    const char *output_path,
                                                    int runtime,
                                                    int project_type,
                                                    int type,
                                                    int mode,
                                                    bool isCOMproject )
{
  switch (type)
    {
    case DLLPROJECT:
      if (isCOMproject)
        {
          string sources;

          sources = "..\\";
          sources += extra_path;
          sources += root;
          sources += "\\";
#ifdef _DEBUG
          debuglog  << "write_midl root:" << root.c_str() << " extra: " << extra_path.c_str() << endl;
          debuglog  << "write_midl output:" << output_path << " sources: " << sources.c_str() << endl;
#endif
          m_stream << "      <Tool" << endl;
          m_stream << "        Name=\"VCMIDLTool\"" << endl;
          m_stream << "        TargetEnvironment=\"1\"" << endl;
          m_stream << "        GenerateStublessProxies=\"TRUE\"" << endl;
          // *****************  UNTESTED CODE *****************
          //m_stream << "        TypeLibraryName=\"" << output_path << outname.c_str() << ".tlb" << "\"" << endl;
          m_stream << "        TypeLibraryName=\"" << ".\\" <<outname.c_str() << ".tlb" << "\"" << endl;
          // *****************  UNTESTED CODE *****************
          m_stream << "        OutputDirectory=\"" << get_full_path("",sources).c_str() << "\"/>" << endl;
        }
      else
        {
          m_stream << "      <Tool" << endl;
          m_stream << "        Name=\"VCMIDLTool\"/>" << endl;
        }
      break;

    case LIBPROJECT:
    case EXEPROJECT:
      m_stream << "      <Tool" << endl;
      m_stream << "        Name=\"VCMIDLTool\"/>" << endl;
      break;
    }
}

void ConfigureVS7Project::write_custom_tool( const char *trigger,
                                             const char *target,
                                             int runtime,
                                             int project_type,
                                             int type,
                                             int mode,
                                             bool isCOMproject)
{
  switch (type)
    {
    case DLLPROJECT:
      if (isCOMproject)
        {
          m_stream << "      <Tool" << endl;
          m_stream << "        Name=\"VCCustomBuildTool\"" << endl;
          m_stream << "        Description=\"Performing registration\"" << endl;
          m_stream << "        CommandLine=\"regsvr32 /s /c &quot;" << target << "&quot;" << endl;
          m_stream << "echo regsvr32 exec. time &gt;&quot;" << trigger << "&quot;" << endl;
          m_stream << "\"" << endl;
          m_stream << "        Outputs=\"" << trigger << "\"/>" << endl;
        }
      else
        {
          m_stream << "      <Tool" << endl;
          m_stream << "        Name=\"VCMIDLTool\"/>" << endl;
        }
      break;

    case LIBPROJECT:
    case EXEPROJECT:
      m_stream << "      <Tool" << endl;
      m_stream << "        Name=\"VCMIDLTool\"/>" << endl;
      break;
    }
}

void ConfigureVS7Project::write_res_compiler_tool( string &root,
                                                   string &extra_path,
                                                   int runtime,
                                                   int project_type,
                                                   int type,
                                                   int mode )
{
  switch (mode)
    {
    case 0:
      m_stream << "      <Tool" << endl;
      m_stream << "        Name=\"VCResourceCompilerTool\"" << endl;
      m_stream << "        PreprocessorDefinitions=\"NDEBUG\"" << endl;
      m_stream << "        Culture=\"1033\"/>" << endl;
      break;
    case 1:
      m_stream << "      <Tool" << endl;
      m_stream << "        Name=\"VCResourceCompilerTool\"" << endl;
      m_stream << "        PreprocessorDefinitions=\"_DEBUG\"" << endl;
      m_stream << "        Culture=\"1033\"/>" << endl;
      break;
    }
}

void ConfigureVS7Project::write_cpp_compiler_tool_begin( int type, int mode )
{
  m_stream << "      <Tool" << endl;
  m_stream << "        Name=\"VCCLCompilerTool\"" << endl;
}

void ConfigureVS7Project::write_cpp_compiler_tool_runtime( int runtime,
                                                           int type,
                                                           int mode)
{
  // rtMultiThreaded 0
  // rtMultiThreadedDebug 1
  // rtMultiThreadedDLL 2
  // rtMultiThreadedDebugDLL 3
  // rtSingleThreaded 4
  // rtSingleThreadedDebug 5
  switch (mode)
    {
    case 0:
      switch(runtime)
        {
        case SINGLETHREADEDSTATIC:
          m_stream << "        RuntimeLibrary=\"4\"" << endl;
          break;
        case MULTITHREADEDSTATIC:
          m_stream << "        RuntimeLibrary=\"0\"" << endl;
          break;
        default:
        case MULTITHREADEDSTATICDLL:
        case MULTITHREADEDDLL:
          m_stream << "        RuntimeLibrary=\"2\"" << endl;
          break;
        }
      break;
    case 1:
      switch(runtime)
        {
        case SINGLETHREADEDSTATIC:
          m_stream << "        RuntimeLibrary=\"5\"" << endl;
          break;
        case MULTITHREADEDSTATIC:
          m_stream << "        RuntimeLibrary=\"1\"" << endl;
          break;
        default:
        case MULTITHREADEDSTATICDLL:
        case MULTITHREADEDDLL:
          m_stream << "        RuntimeLibrary=\"3\"" << endl;
          break;
        }
    }
}

void ConfigureVS7Project::write_cpp_compiler_tool_options( int type,
                                                           int mode )
{
  m_stream << "        StringPooling=\"TRUE\"" << endl;
  //m_stream << "        ShowIncludes=\"TRUE\"" << endl;
  m_stream << "        EnableFunctionLevelLinking=\"TRUE\"" << endl; // /Gy
  m_stream << "        WarningLevel=\"3\"" << endl; // /W3
  // pchNone 0,pchCreateUsingSpecific 1,pchGenerateAuto 2,pchUseUsingSpecific 3
  m_stream << "        UsePrecompiledHeader=\"0\"" << endl;
  m_stream << "        SuppressStartupBanner=\"TRUE\"" << endl; // /nologo
  // compileAsDefault 0,compileAsC 1,compileAsCPlusPlus 2
  m_stream << "        CompileAs=\"0\"" << endl; // C or C++ compile
  // expandDisable 0,expandOnlyInline 1,expandAnySuitable 2
  m_stream << "        InlineFunctionExpansion=\"2\"" << endl;
  if (openMP)
    m_stream << "        OpenMP=\"TRUE\"" << endl;
  switch (mode)
    {
    case 0:
#ifndef SYMBOLS_ALWAYS
      // debugDisabled 0,debugOldStyleInfo 1,debugLineInfoOnly 2,debugEnabled 3,debugEditAndContinue 4
      m_stream << "        DebugInformationFormat=\"0\"" << endl; // /Z7,/Zd,/Zi,/ZI
#else
      // debugDisabled 0,debugOldStyleInfo 1,debugLineInfoOnly 2,debugEnabled 3,debugEditAndContinue 4
      m_stream << "        DebugInformationFormat=\"3\"" << endl; // /Z7,/Zd,/Zi,/ZI
#endif
      // runtimeBasicCheckNone 0,runtimeCheckStackFrame 1,runtimeCheckUninitVariables 2,runtimeBasicCheckAll 3
      m_stream << "        BasicRuntimeChecks=\"0\"" << endl;
      m_stream << "        OmitFramePointers=\"TRUE\"" << endl; // /nologo
      // optimizeDisabled 0,optimizeMinSpace 1,optimizeMaxSpeed 2,optimizeFull 3,optimizeCustom 4
      m_stream << "        Optimization=\"3\"" << endl;
      break;
    case 1:
      // runtimeBasicCheckNone 0,runtimeCheckStackFrame 1,runtimeCheckUninitVariables 2,runtimeBasicCheckAll 3
      m_stream << "        BasicRuntimeChecks=\"3\"" << endl;
      m_stream << "        OmitFramePointers=\"FALSE\"" << endl; // /nologo
      // debugDisabled 0,debugOldStyleInfo 1,debugLineInfoOnly 2,debugEnabled 3,debugEditAndContinue 4
      m_stream << "        DebugInformationFormat=\"3\"" << endl; // /Z7,/Zd,/Zi,/ZI
      // optimizeDisabled 0,optimizeMinSpace 1,optimizeMaxSpeed 2,optimizeFull 3,optimizeCustom 4
      m_stream << "        Optimization=\"0\"" << endl;
      break;
    }

}

void ConfigureVS7Project::write_cpp_compiler_tool_includes( string &root,
                                                            string &extra_path,
                                                            list<string> &includes_list,
                                                            list<string> &standard_include,
                                                            BOOL bAbsolutePaths )
{
  // The following was only done for the EXEPROJECT case
  // m_stream ".";
  bool bFirstTime = true;
  m_stream << "        AdditionalIncludeDirectories=\"";
  {
    for (
         list<string>::iterator it = standard_include.begin();
         it != standard_include.end();
         it++)
      {
        string relpath;
        if (!bAbsolutePaths)
          relpath = extra_path;
        relpath += *it;
        if (!bFirstTime)
          m_stream << ",";
        bFirstTime=false;
        m_stream << get_full_path("",relpath).c_str();
      }
  }
  {
    for (
         list<string>::iterator it = includes_list.begin();
         it != includes_list.end();
         it++)
      {
        if (!bFirstTime)
          m_stream << ",";
        bFirstTime=false;
        m_stream << get_full_path(root + "\\",*it).c_str();
      }
  }
  m_stream << "\"" << endl;
}

void ConfigureVS7Project::write_cpp_compiler_tool_defines( list<string> &defines_list,
                                                           int type,
                                                           int mode,
                                                           bool bConsoleMode,
                                                           bool bDynamicMFC )
{
  m_stream << "        PreprocessorDefinitions=\"";
  switch (mode)
    {
    case 0:
      m_stream << "NDEBUG";
      break;
    case 1:
      m_stream << "_DEBUG";
      break;
    }
  if (bConsoleMode&& (type == EXEPROJECT))
    m_stream << ";_CONSOLE";
  else
    m_stream << ";_WINDOWS";
  m_stream << ";WIN32;_VISUALC_;NeedFunctionPrototypes";
  {
    for (
         list<string>::iterator it = defines_list.begin();
         it != defines_list.end();
         it++)
      {
        m_stream << ";" << (*it).c_str();
      }
  }
  if (bDynamicMFC&& (type == EXEPROJECT))
    m_stream << ";_AFXDLL";
  m_stream << "\"" << endl;
}

void ConfigureVS7Project::write_cpp_compiler_tool_end( int type, int mode )
{
  m_stream << "        />" << endl;
}

void ConfigureVS7Project::write_cpp_compiler_tool( string &root,
                                                   string &extra_path,
                                                   list<string> &includes_list,
                                                   list<string> &standard_includes_list,
                                                   list<string> &defines_list,
                                                   int runtime,
                                                   int project_type,
                                                   int type,
                                                   int mode )
{
  bool bDynamicMFC = ((project_type == PROJECT) && (runtime == MULTITHREADEDDLL));

  write_cpp_compiler_tool_begin(type, mode);
  write_cpp_compiler_tool_runtime(runtime, type, mode);
  write_cpp_compiler_tool_options(type, mode);
  write_cpp_compiler_tool_includes(root, extra_path, includes_list,
                                   standard_includes_list, standaloneMode);
  write_cpp_compiler_tool_defines(defines_list, type, mode, consoleMode, bDynamicMFC);
  write_cpp_compiler_tool_end(type, mode);
}

void ConfigureVS7Project::write_link_tool_begin( const char *input_path,
                                                 const char *output_path,
                                                 list<string> &additional_libdir_list,
                                                 int type, int mode )
{
  m_stream << "      <Tool" << endl;
  switch (type)
    {
    case DLLPROJECT:
    case EXEPROJECT:
      m_stream << "        Name=\"VCLinkerTool\"" << endl;
      m_stream << "        AdditionalLibraryDirectories=\"";
      for (list<string>::iterator it = additional_libdir_list.begin(); 
           it != additional_libdir_list.end(); it++)
        m_stream << *it << ";";
      m_stream << input_path << "\"" << endl;
      break;
    case LIBPROJECT:
      m_stream << "        Name=\"VCLibrarianTool\"" << endl;
      break;
    }
}

void ConfigureVS7Project::write_link_tool_dependencies( string &root,
                                                        bool bNeedsMagickpp,
                                                        list<string> &shared_lib_list,
                                                        list<string> &lib_list,
                                                        int runtime, int type, int mode )
{
  bool bFirstTime = true;
  char *strmode;
  switch (mode)
    {
    case 0:
      strmode = "RL_";
      break;
    case 1:
      strmode = "DB_";
      break;
    }
  if (!build64Bit)
    m_stream << "        AdditionalDependencies=\" ";
  else
    m_stream << "        AdditionalDependencies=\"/MACHINE:AMD64 ";
  if (onebigdllMode || (standaloneMode && (type == EXEPROJECT)))
    {
      switch (runtime)
        {
        case MULTITHREADEDSTATIC:
        case SINGLETHREADEDSTATIC:
        case MULTITHREADEDSTATICDLL:
          {
            string strDepends;
            for (
                 list<ConfigureProject*>::iterator it1a = dependency_list.begin();
                 it1a != dependency_list.end();
                 it1a++)
              {
                /* look for entries with a xxx%syyy%szzz format */
                if (LocalFindNoCase((*it1a)->name,"%s",0) > 0)
                  {
                    /* ignore any entries for utility programs as well */
                    if (LocalFindNoCase((*it1a)->name,"UTIL_",0) != 0)
                      {
                        LocalFormat(strDepends,(*it1a)->name.c_str(),strmode,"_.lib");
                        if (!bFirstTime)
                          m_stream << " ";
                        bFirstTime=false;
                        m_stream << strDepends.c_str();
                      }
                  }
              }
          }
          break;
        default:
        case MULTITHREADEDDLL:
          {
            string strDepends;
            LocalFormat(strDepends,"CORE_%sMagickCore%s",strmode,"_.lib");
            if (!bFirstTime)
              m_stream << " ";
            bFirstTime=false;
            m_stream << strDepends.c_str();
            LocalFormat(strDepends,"CORE_%sMagickWand%s",strmode,"_.lib");
            if (!bFirstTime)
              m_stream << " ";
            bFirstTime=false;
            m_stream << strDepends.c_str();
            if (useX11Stubs)
              {
                LocalFormat(strDepends,"CORE_%sxlib%s",strmode,"_.lib");
                if (!bFirstTime)
                  m_stream << " ";
                bFirstTime=false;
                m_stream << strDepends.c_str();
              }
            if (bNeedsMagickpp)
              {
                LocalFormat(strDepends,"CORE_%sMagick++%s",strmode,"_.lib");
                if (!bFirstTime)
                  m_stream << " ";
                bFirstTime=false;
                m_stream << strDepends.c_str();
              }
            string strPath = root;
            if (LocalFindNoCase(strPath,"\\SDL",0) >= 0)
              {
                LocalFormat(strDepends,"CORE_%sSDL%s",strmode,"_.lib");
                if (!bFirstTime)
                  m_stream << " ";
                bFirstTime=false;
                m_stream << strDepends.c_str();
              }
            if (LocalFindNoCase(strPath,"..\\jp2",0) == 0)
              {
                LocalFormat(strDepends,"CORE_%sjp2%s",strmode,"_.lib");
                if (!bFirstTime)
                  m_stream << " ";
                bFirstTime=false;
                m_stream << strDepends.c_str();
              }
          }
          break;
        }
    }
  if ((type == DLLPROJECT) || (type == EXEPROJECT))
    {
      {
        for (
             list<string>::iterator it = shared_lib_list.begin();
             it != shared_lib_list.end();
             it++)
          {
            if (!bFirstTime)
              m_stream << " ";
            bFirstTime=false;
            m_stream << (*it).c_str();
          }
      }
    }
  {
    for (
         list<string>::iterator it = lib_list.begin();
         it != lib_list.end();
         it++)
      {
        if (!bFirstTime)
          m_stream << " ";
        bFirstTime=false;
        m_stream << (*it).c_str();
      }
  }
  m_stream << "\"" << endl;
}

void ConfigureVS7Project::write_link_tool_options( bool bNeedsRelo,
                                                   int type,
                                                   int mode,
                                                   const string &defs_path)
{
  m_stream << "        SuppressStartupBanner=\"TRUE\"" << endl; // /nologo
  // linkIncrementalDefault 0, linkIncrementalNo 1, linkIncrementalYes 2
  m_stream << "        LinkIncremental=\"1\"" << endl;
  m_stream << "        TargetMachine=\"1\"" << endl;

  //subSystemNotSet 0,subSystemConsole 1,subSystemWindows 2
  switch(type)
    {
    case EXEPROJECT:
      if (consoleMode)
        m_stream << "        SubSystem=\"1\"" << endl;
      else
        m_stream << "        SubSystem=\"2\"" << endl;
#ifdef SYMBOLS_ALWAYS
      m_stream << "        GenerateDebugInformation=\"TRUE\"" << endl;
#else
      if (mode == 1)
        m_stream << "        GenerateDebugInformation=\"TRUE\"" << endl;
#endif
      break;
    case DLLPROJECT:
      m_stream << "        SubSystem=\"2\"" << endl;
      // If library sources include a .def file, then it must be the
      // module definition file. FIXME
      if (!defs_path.empty())
        m_stream << "        ModuleDefinitionFile=\"" << defs_path << "\"" << endl;
#ifdef SYMBOLS_ALWAYS
      m_stream << "        GenerateDebugInformation=\"TRUE\"" << endl;
#else
      if (mode == 1)
        m_stream << "        GenerateDebugInformation=\"TRUE\"" << endl;
#endif
      m_stream << "        LinkDLL=\"TRUE\"" << endl;
      if (bNeedsRelo)
        {
          string strBase;
          LocalFormat(strBase,"%lx",dllbaselocation);
          m_stream << "        BaseAddress=\"0x" << strBase.c_str() << "\"";
          dllbaselocation += 0x00010000L;
        }
      break;
    case LIBPROJECT:
      m_stream << "        SubSystem=\"2\"" << endl;
      //if (mode == 1)
      //  m_stream << "        GenerateDebugInformation=\"TRUE\"" << endl;
      break;
    }
}

void ConfigureVS7Project::write_link_tool_output( const char *input_path,
                                                  const char *output_path,
                                                  string &outname,
                                                  int type,
                                                  int mode )
{
  switch(type)
    {
    case EXEPROJECT:
      m_stream << "        ProgramDatabaseFile=\"" << output_path << outname.c_str();
      if (decorateFiles)
        m_stream << "_";
      m_stream << ".pdb\"" << endl;
      m_stream << "        OutputFile=\"" << output_path << outname.c_str();
      if (decorateFiles)
        m_stream << "_";
      m_stream << ".exe\"" << endl;
      break;
    case DLLPROJECT:
      m_stream << "        ProgramDatabaseFile=\"" << output_path << outname.c_str() << ".pdb\"" << endl;
      m_stream << "        ImportLibrary=\"" << input_path << outname.c_str() << ".lib\"" << endl;
      m_stream << "        OutputFile=\"" << output_path << outname.c_str() << ".dll\"" << endl;
      break;
    case LIBPROJECT:
      m_stream << "        OutputFile=\"" << output_path << outname.c_str() << ".lib\"" << endl;
      break;
    }
}

void ConfigureVS7Project::write_link_tool_end( int type, int mode )
{
  m_stream << "        />" << endl;
}

void ConfigureVS7Project::write_link_tool( string &root,
                                           string &extra_path,
                                           const string &defs_path,
                                           string &outname,
                                           bool bNeedsMagickpp,
                                           bool bNeedsRelo,
                                           list<string> &shared_lib_list,
                                           list<string> &lib_list,
                                           list<string> &additional_libdir_list,
                                           const char *input_path,
                                           const char *output_path,
                                           int runtime,
                                           int project_type,
                                           int type,
                                           int mode )
{
  write_link_tool_begin(input_path,output_path,additional_libdir_list,type,mode);
  write_link_tool_dependencies(root,bNeedsMagickpp,shared_lib_list,lib_list,runtime,type,mode);
  write_link_tool_options(bNeedsRelo,type,mode,defs_path); //FIXME Support .def
  write_link_tool_output(input_path,output_path,outname,type,mode);
  write_link_tool_end(type,mode);
}

void ConfigureVS7Project::write_begin_target(const char *name)
{
  m_stream << "  <Files>" << endl;
}

void ConfigureVS7Project::write_end_target()
{
  m_stream << "  </Files>" << endl;
}

void ConfigureVS7Project::write_begin_group(const char *name)
{
  m_stream << "    <Filter Name=\"" << name << "\" Filter=\"\">" << endl;
}

void ConfigureVS7Project::write_end_group()
{
  m_stream << "    </Filter>" << endl;
}

void ConfigureVS7Project::write_file(const char *filename)
{
  m_stream << "      <File RelativePath=\"" << filename << "\"/>" << endl;
}

BOOL BrowseForFolder(HWND hOwner, char* szTitle, char* szRetval)
{
  BROWSEINFO info;
  LPITEMIDLIST itemidlist;
  char szDirectory[_MAX_PATH];
  LPMALLOC pMalloc;

  memset(szDirectory, '\0', _MAX_PATH);

  if (::SHGetMalloc(&pMalloc) == NOERROR)
    {
      info.hwndOwner = hOwner;
      info.pidlRoot = NULL;
      info.pszDisplayName = szDirectory;
      info.lpszTitle = szTitle;
      info.ulFlags = 0;
      info.lpfn = NULL;

      itemidlist = SHBrowseForFolder(&info);
      if (itemidlist != NULL)
        {
          SHGetPathFromIDList(itemidlist, szRetval);
          pMalloc->Free(itemidlist);
          pMalloc->Release();
          return TRUE;
        }
      else // User clicked Cancel
        {
          pMalloc->Release();
          return FALSE;
        }
    }
  else
    return FALSE;
}

#ifdef __NO_MFC__
int main() {
	CConfigureApp app;
	app.InitInstance();

	return 0;
}
#endif // __NO_MFC__

// Local Variables:
// c-file-style: "gnu"
// End:
