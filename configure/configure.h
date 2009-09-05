// Configure.h : main header file for the CONFIGURE application
//

#if !defined(AFX_CONFIGURE_H__38147984_A99A_11D3_8F4C_002018383984__INCLUDED_)
#define AFX_CONFIGURE_H__38147984_A99A_11D3_8F4C_002018383984__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __NO_MFC__
#ifndef __AFXWIN_H__
  #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"   // main symbols
#else // __NO_MFC__
#include <windows.h>
#endif // __NO_MFC__

#ifndef _USE_OLD_IOSTREAMS
#include <fstream>
#else
#include <fstream.h> // WTR
#endif

#include <string>
#include <list>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CConfigureApp:
// See Configure.cpp for the implementation of this class
/////////////////////////////////////////////////////////////////////////////

enum {MULTITHREADEDDLL, SINGLETHREADEDSTATIC, MULTITHREADEDSTATIC, MULTITHREADEDSTATICDLL};
enum {DLLPROJECT, LIBPROJECT, EXEPROJECT};
enum {DISABLED, UTILITY, LIBRARY, STATICLIB, MODULE, ADD_ON, THIRDPARTY, PROJECT};
enum {CPPCOMPILETOOL, RESCOMPILETOOL, MIDLCOMPILETOOL, LIBRARYTOOL, LINKERTOOL};
enum {OP_REPLACEFILES, OP_RENAMEFILES};

class ConfigureProject
{
public:
  ConfigureProject();
  GUID m_guid;
  TCHAR m_GuidText[250];
  ofstream m_stream;
  string name;
  list<string> depends;
  int type;

  // Assignment operator
  ConfigureProject& operator=(const ConfigureProject& obj);
  // Copy constructor
  ConfigureProject(const ConfigureProject& obj);
  // Destructor
  virtual ~ConfigureProject( void );

  void generate_dir(const char *dir,
                    const char *specS, int nestinglevel, int project_type,
                    std::list<std::string> &exclude_list);

  virtual void write_begin_group(const char *name) = 0;
  virtual void write_end_group(void) = 0;
  virtual void write_begin_target(const char* name) = 0;
  virtual void write_end_target(void) = 0;
  virtual void write_begin_project(const char *name,int type) = 0;
  virtual void write_end_project(void) = 0;
  virtual void write_file(const char *filename) = 0;
  virtual void write_configuration(const char *name, const char *subname,
                                   int state) = 0;
  virtual void write_properties(const char *name,
                                const char *outputpath,
                                const char *intermediatepath,
                                const char *targetpath, int type,
                                int mode) = 0;

  virtual void write_cpp_compiler_tool(string &root, string &extra_path,
                                       list<string> &includes_list,
                                       list<string> &standard_includes_list,
                                       list<string> &defines_list,
                                       int runtime, int project_type, int type,
                                       int mode) = 0;
  virtual void write_cpp_compiler_tool_begin(int type, int mode) = 0;
  virtual void write_cpp_compiler_tool_runtime(int runtime, int type, int mode) = 0;
  virtual void write_cpp_compiler_tool_options(int type, int mode) = 0;
  virtual void write_cpp_compiler_tool_includes(string &root, string &extra_path,
                                                list<string> &includes_list,
                                                list<string> &standard_includes_list,
                                                BOOL bAbsolutePaths) = 0;
  virtual void write_cpp_compiler_tool_defines(list<string> &defines_list,
                                               int type, int mode,
                                               bool bConsoleMode,
                                               bool bDynamicMFC) = 0;
  virtual void write_cpp_compiler_tool_end(int type, int mode) = 0;

  virtual void write_link_tool(string &root,
                               string &extra_path,
                               const string &defs_path,
                               string &outname,
                               bool bNeedsMagickpp,
                               bool bNeedsRelo,
                               list<string> &shared_lib_list,
                               list<string> &lib_list,
                               const char *input_path,
                               const char *output_path,
                               int runtime,
                               int project_type,
                               int type,
                               int mode) = 0;
  virtual void write_link_tool_begin(const char *input_path,
                                     const char *output_path,
                                     int type, int mode) = 0;
  virtual void write_link_tool_dependencies(string &root, bool bNeedsMagickpp,
                                            list<string> &shared_lib_list,
                                            list<string> &lib_list,
                                            int runtime, int type, int mode) = 0;
  virtual void write_link_tool_options(bool bNeedsRelo, int type, int mode,
                                       const string &defs_path) = 0;
  virtual void write_link_tool_output(const char *input_path,
                                      const char *output_path,
                                      string &outname,
                                      int type, int mode) = 0;
  virtual void write_link_tool_end(int type, int mode) = 0;

  virtual void write_midl_compiler_tool(string &root, string &extra_path,
                                        string &outname, const char *output_path,
                                        int runtime, int project_type, int type,
                                        int mode, bool isCOMproject) = 0;
  virtual void write_custom_tool(const char *target, const char *trigger,
                                 int runtime, int project_type, int type,
                                 int mode, bool isCOMproject) = 0;
  virtual void write_res_compiler_tool(string &root, string &extra_path,
                                       int runtime, int project_type,
                                       int type, int mode) = 0;
};

class ConfigureWorkspace
{
public:
  ConfigureWorkspace();
  GUID m_guid;
	TCHAR m_GuidText[250];
  ofstream m_stream;
  string name;

  // Assignment operator
  ConfigureWorkspace& operator=(const ConfigureWorkspace& obj) { return *this; };
  // Copy constructor
  ConfigureWorkspace(const ConfigureWorkspace& obj)  { *this = obj; }

  virtual void write_start(void) = 0;
  virtual void write_end(void) = 0;
  virtual void write_begin_project(ConfigureProject *project, const char *name,
                                   const char *filename) = 0;
  virtual void write_end_project(ConfigureProject *project) = 0;
  virtual void write_project_dependency(ConfigureProject *project,
                                        string &dep_name) = 0;
  virtual void write_project_dependency(ConfigureProject *project,
                                        const char *dep_name) = 0;
};

class ConfigureVS6Project : public ConfigureProject
{
public:
  ConfigureVS6Project() {};
  ~ConfigureVS6Project( void );

  void write_begin_group(const char *name);
  void write_end_group(void);
  void write_begin_target(const char* name);
  void write_end_target(void);
  void write_begin_project(const char *name,int type);
  void write_end_project(void);
  void write_file(const char *filename);
  void write_configuration(const char *name, const char *subname, int state);
  void write_properties(const char *name,
                        const char *outputpath, const char *intermediatepath,
                        const char *targetpath, int type, int mode);
  void write_cpp_compiler_tool(string &root, string &extra_path,
                               list<string> &includes_list,
                               list<string> &standard_includes_list,
                               list<string> &defines_list,
                               int runtime, int project_type, int type, int mode);

  void write_link_tool(string &root,
                       string &extra_path,
                       const string &defs_path,
                       string &outname,
                       bool bNeedsMagickpp,
                       bool bNeedsRelo,
                       list<string> &shared_lib_list,
                       list<string> &lib_list,
                       const char *input_path,
                       const char *output_path,
                       int runtime,
                       int project_type,
                       int type,
                       int mode);
  void write_link_tool_begin(const char *input_path, const char *output_path,
                             int type, int mode);
  void write_link_tool_dependencies(string &root, bool bNeedsMagickpp,
                                    list<string> &shared_lib_list,
                                    list<string> &lib_list,
                                    int runtime, int type, int mode);
  void write_link_tool_options(bool bNeedsRelo, int type, int mode,
                               const string &defs_path);
  void write_link_tool_output(const char *input_path, const char *output_path,
                              string &outname,
                              int type, int mode);
  void write_link_tool_end(int type, int mode);

  void write_midl_compiler_tool(string &root, string &extra_path, string &outname,
                                const char *output_path,
                                int runtime, int project_type, int type, int mode,
                                bool isCOMproject);
  void write_cpp_compiler_tool_begin(int type, int mode);
  void write_cpp_compiler_tool_runtime(int runtime, int type, int mode);
  void write_cpp_compiler_tool_options(int type, int mode);
  void write_cpp_compiler_tool_includes(string &root, string &extra_path,
                                        list<string> &includes_list,
                                        list<string> &standard_includes_list,
                                        BOOL bAbsolutePaths);
  void write_cpp_compiler_tool_defines(list<string> &defines_list, int type,
                                       int mode, bool bConsoleMode,
                                       bool bDynamicMFC);
  void write_cpp_compiler_tool_end(int type, int mode);

  void write_custom_tool(const char *target, const char *trigger,
                         int runtime, int project_type, int type, int mode,
                         bool isCOMproject);
  void write_res_compiler_tool(string &root, string &extra_path, int runtime,
                               int project_type, int type, int mode);
};

class ConfigureVS6Workspace : public ConfigureWorkspace
{
public:
  ConfigureVS6Workspace() {};
  void write_start(void);
  void write_end(void);
  void write_begin_project(ConfigureProject *project, const char *name,
                           const char *filename);
  void write_end_project(ConfigureProject *project);
  void write_project_dependency(ConfigureProject *project, string &dep_name);
  void write_project_dependency(ConfigureProject *project, const char *dep_name);
};

class ConfigureVS7Project : public ConfigureProject
{
public:
  ConfigureVS7Project() {};
  ~ConfigureVS7Project( void );

  void write_begin_group(const char *name);
  void write_end_group(void);
  void write_begin_target(const char* name);
  void write_end_target(void);
  void write_begin_project(const char *name,int type);
  void write_end_project(void);
  void write_file(const char *filename);
  void write_configuration(const char *name, const char *subname, int state);
  void write_properties(const char *name,
                        const char *outputpath,
                        const char *intermediatepath,
                        const char *targetpath,
                        int type, int mode);

  void write_cpp_compiler_tool(string &root, string &extra_path,
                               list<string> &includes_list,
                               list<string> &standard_includes_list,
                               list<string> &defines_list,
                               int runtime, int project_type, int type,
                               int mode);
  void write_cpp_compiler_tool_begin(int type, int mode);
  void write_cpp_compiler_tool_runtime(int runtime, int type, int mode);
  void write_cpp_compiler_tool_options(int type, int mode);
  void write_cpp_compiler_tool_includes(string &root, string &extra_path,
                                        list<string> &includes_list,
                                        list<string> &standard_includes_list,
                                        BOOL bAbsolutePaths);
  void write_cpp_compiler_tool_defines(list<string> &defines_list, int type,
                                       int mode, bool bConsoleMode,
                                       bool bDynamicMFC);
  void write_cpp_compiler_tool_end(int type, int mode);

  void write_link_tool(string &root,
                       string &extra_path,
                       const string &defs_path,
                       string &outname,
                       bool bNeedsMagickpp,
                       bool bNeedsRelo,
                       list<string> &shared_lib_list,
                       list<string> &lib_list,
                       const char *input_path,
                       const char *output_path,
                       int runtime,
                       int project_type,
                       int type,
                       int mode);
  void write_link_tool_begin(const char *input_path, const char *output_path,
                             int type, int mode);
  void write_link_tool_dependencies(string &root, bool bNeedsMagickpp,
                                    list<string> &shared_lib_list,
                                    list<string> &lib_list,
                                    int runtime, int type, int mode);
  void write_link_tool_options(bool bNeedsRelo, int type, int mode,
                               const string &defs_path);
  void write_link_tool_output(const char *input_path, const char *output_path,
                              string &outname,
                              int type, int mode);
  void write_link_tool_end(int type, int mode);

  void write_midl_compiler_tool(string &root, string &extra_path,
                                string &outname, const char *output_path,
                                int runtime, int project_type, int type,
                                int mode, bool isCOMproject);
  void write_custom_tool(const char *target, const char *trigger,
                         int runtime, int project_type, int type,
                         int mode, bool isCOMproject);
  void write_res_compiler_tool(string &root, string &extra_path, int runtime,
                               int project_type, int type, int mode);
};

class ConfigureVS7Workspace : public ConfigureWorkspace
{
public:
  ConfigureVS7Workspace() {};
  void write_start(void);
  void write_end(void);
  void write_begin_project(ConfigureProject *project, const char *name,
                           const char *filename);
  void write_end_project(ConfigureProject *project);
  void write_project_dependency(ConfigureProject *project, string &dep_name);
  void write_project_dependency(ConfigureProject *project, const char *dep_name);
};

#ifndef __NO_MFC__
class CConfigureApp : public CWinApp
#else // __NO_MFC__
class CConfigureApp
#endif // __NO_MFC__
{
public:
  CConfigureApp();
  ConfigureWorkspace *workspace;
  ConfigureProject *project;

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CConfigureApp)
public:
  virtual BOOL InitInstance();
  //}}AFX_VIRTUAL

  // Implementation

  //{{AFX_MSG(CConfigureApp)
  //}}AFX_MSG
#ifndef __NO_MFC__
  DECLARE_MESSAGE_MAP()
#endif // __NO_MFC__

    private:
  list<string> defines_list;
  list<string> includes_list;
  list<string> source_list;
  list<string> resource_list;
  list<string> exclude_list;
  list<string> lib_release_list;
  list<string> lib_debug_list;
  list<string> lib_shared_list;
  list<string> standard_includes_list;
  string       module_definition_file;  // DLL .DEF file.

  void process_project_type(
                            const char *root, const int runtime,
                            const char *stype, const int btype);
  void process_project_replacements(const char *root, const char *top,
                                    const char *stype, const char *newstype,
                                    int operation);
  ConfigureProject *write_project_lib(bool dll,
                                      int runtime,
                                      int project_type,
                                      std::string root,
                                      std::string search,
                                      std::string name,
                                      std::string prefix,
                                      std::string extn);
  ConfigureProject *write_project_exe(int runtime,
                                      int project_type,
                                      std::string root,
                                      std::string search,
                                      std::string name,
                                      std::string prefix,
                                      std::string extn);
  void process_utility(const char *root, const char *filename,
                       int runtime, int project_type);
  void process_library(const char *root, const char *filename,
                       int runtime, int project_type);
  void process_3rd_party_library(const char *root, const char *filename,
                                 int runtime, int project_type);
  void process_module(const char *root, const char *filename,
                      int runtime, int project_type);
  void process_one_folder(const char *root, WIN32_FIND_DATA &data,
                          int project_type, int projectType);
  void generate_dependencies(ConfigureProject *project,
                             bool gen_cpp, bool gen_util, bool gen_type);
  void generate_a_dependency(ConfigureWorkspace *w,ConfigureProject *p,
                             char *s, bool flag1, bool flag2);
  void generate_a_dependency_cs(ConfigureWorkspace *w,ConfigureProject *p,
                                char *s);
  void generate_a_dependency_type(ConfigureWorkspace *w,ConfigureProject *p,
                                  int t);
  bool CConfigureApp::process_one_entry(const char *entry, int nLinesRead,
                                        int runtime);
  int CConfigureApp::load_environment_file( const char *inputfile, int runtime);
};

bool is_project_type(const char *root, const int project_type);
BOOL BrowseForFolder(HWND hOwner, char* szTitle, char* szRetval);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGURE_H__38147984_A99A_11D3_8F4C_002018383984__INCLUDED_)
// Local Variables:
// c-file-style: "gnu"
// End:
