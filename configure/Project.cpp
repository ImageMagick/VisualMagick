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
#include "Project.h"
#include "Shared.h"

string Project::configDefine() const
{
  return(_configDefine);
}

vector<string> &Project::defines()
{
  return(_defines);
}

vector<string> &Project::dependencies()
{
  return(_dependencies);
}

vector<string> &Project::directories()
{
  return(_directories);
}

vector<string> &Project::excludes()
{
  return(_excludes);
}

vector<string> &Project::excludesX64()
{
  return(_excludesX64);
}

vector<string> &Project::excludesX86()
{
  return(_excludesX86);
}

vector<ProjectFile*> &Project::files()
{
  return(_files);
}

vector<string> &Project::includes()
{
  return(_includes);
}

bool Project::isConsole() const
{
  if (!isExe())
    return(false);
  return(_type != APPTYPE);
}

bool Project::isCom() const
{
  return(_type == COMTYPE);
}

bool Project::isDll() const
{
  return((_type == DLLTYPE) || (_type == DLLMODULETYPE) || (_type == COMTYPE));
}

bool Project::isExe() const
{
  return((_type == EXETYPE) || (_type == EXEMODULETYPE) || (_type == APPTYPE));
}

bool Project::isLib() const
{
  return((_type == STATICTYPE));
}

bool Project::isModule() const
{
  return((_type == DLLMODULETYPE) || (_type == EXEMODULETYPE));
}

vector<string> &Project::libraries()
{
  return(_libraries);
}

string Project::moduleDefinitionFile() const
{
  return(_moduleDefinitionFile);
}

string Project::name() const
{
  return(_name);
}

vector<string> &Project::references()
{
  return(_references);
}

int Project::warningLevel() const
{
  return(_warningLevel);
}

Project* Project::create(string name)
{
  ifstream
    config;

  config.open("..\\" + name + "\\Config.txt");
  if (!config)
    return((Project *) NULL);

  Project* project = new Project(name);
  project->loadConfig(config);

  config.close();
  return(project);
}

bool Project::loadFiles(const ConfigureWizard &wizard)
{
  ProjectFile
    *projectFile;

  _files.clear();

  if (!_isEnabled)
    return(false);

  if (_hasIncompatibleLicense && !wizard.includeIncompatibleLicense())
    return(false);

  if (_isOptional && !wizard.includeOptional())
    return(false);

  switch(_type)
  {
    case COMTYPE:
    {
      projectFile=new ProjectFile(&wizard,this,"COM",_name);
      _files.push_back(projectFile);
      break;
    }
    case DLLMODULETYPE:
    {
      if (wizard.solutionType() == DYNAMIC_MT)
        loadModules(wizard);
      else
      {
        projectFile=new ProjectFile(&wizard,this,"CORE",_name);
        _files.push_back(projectFile);
      }
      break;
    }
    case DLLTYPE:
    {
      projectFile=new ProjectFile(&wizard,this,"CORE",_name);
      _files.push_back(projectFile);
      break;
    }
    case APPTYPE:
    case EXETYPE:
    {
      projectFile=new ProjectFile(&wizard,this,"UTIL",_name);
      _files.push_back(projectFile);
      break;
    }
    case EXEMODULETYPE:
    {
      loadModules(wizard);
      break;
    }
    case STATICTYPE:
    {
      projectFile=new ProjectFile(&wizard,this,"CORE",_name);
      _files.push_back(projectFile);
      break;
    }
  }

  return(true);
}

Project::Project(string name)
{
  _name=name;

  _hasIncompatibleLicense=false;
  _isEnabled=true;
  _isOptional=false;
  _type=UNDEFINEDTYPE;
  _warningLevel=0;
}

void Project::addLines(ifstream &config,string &value)
{
  string
    line;

  while (!config.eof())
  {
    getline(config,line);
    if (trim(line).empty())
      return;

    value+=line+"\n";
  }
}

void Project::addLines(ifstream &config,vector<string> &container)
{
  string
    line;

  while (!config.eof())
  {
    line=readLine(config);
    if (line.empty())
      return;

    container.push_back(line);
  }
}

void Project::loadConfig(ifstream &config)
{
  string
    line;

  while (!config.eof())
  {
    line=readLine(config);
    if (line == "[APP]")
      _type=APPTYPE;
    else if (line == "[COM]")
      _type=COMTYPE;
    else if (line == "[CONFIG_DEFINE]")
      addLines(config,_configDefine);
    else if (line ==  "[DEFINES]")
      addLines(config,_defines);
    else if (line == "[DEPENDENCIES]")
      addLines(config,_dependencies);
    else if (line == "[DIRECTORIES]")
      addLines(config,_directories);
    else if (line == "[DISABLED]")
      _isEnabled=false;
    else if (line == "[DLL]")
      _type=DLLTYPE;
    else if (line == "[DLLMODULE]")
      _type=DLLMODULETYPE;
    else if (line == "[EXE]")
      _type=EXETYPE;
    else if (line == "[EXEMODULE]")
      _type=EXEMODULETYPE;
    else if (line == "[EXCLUDES]")
      addLines(config,_excludes);
    else if (line == "[EXCLUDES_X86]")
      addLines(config,_excludesX86);
    else if (line == "[EXCLUDES_X64]")
      addLines(config,_excludesX64);
    else if (line == "[INCLUDES]")
      addLines(config,_includes);
    else if (line == "[INCOMPATIBLE_LICENSE]")
      _hasIncompatibleLicense=true;
    else if (line == "[STATIC]")
      _type=STATICTYPE;
    else if (line == "[LIBRARIES]")
      addLines(config,_libraries);
    else if (line == "[MODULE_DEFINITION_FILE]")
      _moduleDefinitionFile=readLine(config);
    else if (line == "[MODULE_PREFIX]")
      _modulePrefix=readLine(config);
    else if (line == "[OPTIONAL]")
      _isOptional=true;
    else if (line == "[REFERENCES]")
      addLines(config,_references);
    else if (line == "[WARNING_LEVEL]")
      _warningLevel=stoi(readLine(config));
  }
}

void Project::loadModules(const ConfigureWizard &wizard)
{
  HANDLE
    fileHandle;

  ProjectFile
    *projectFile;

  string
    name;

  WIN32_FIND_DATA
    data;

  foreach (string,dir,_directories)
  {
    fileHandle=FindFirstFile((*dir + "\\*.*").c_str(),&data);
    do
    {
      if (fileHandle == INVALID_HANDLE_VALUE)
        return;

      if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
        continue;

      if (contains(_excludes,data.cFileName))
        continue;

      name=data.cFileName;

      if (!endsWith(name,".c") && !endsWith(name,".cpp"))
        continue;

      name=name.substr(0, name.find_last_of("."));
      projectFile=new ProjectFile(&wizard,this,_modulePrefix,name);
      _files.push_back(projectFile);

    } while (FindNextFile(fileHandle,&data));
  }
}