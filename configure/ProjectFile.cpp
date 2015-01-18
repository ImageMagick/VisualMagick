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
#include "ProjectFile.h"
#include "Shared.h"
#include <algorithm>
#include <map>

ProjectFile::ProjectFile(const ConfigureWizard *wizard,Project* project,const string &prefix,const string &name)
{
  _wizard=wizard;
  _project=project;
  _prefix=prefix;
  _name=name;

  setFileName();
  setGuid();

  foreach(string,dep,project->dependencies())
  {
    _dependencies.push_back(*dep);
  }

  foreach(string,inc,project->includes())
  {
    _includes.push_back(*inc);
  }
}

vector<string> &ProjectFile::dependencies()
{
  return(_dependencies);
}

string ProjectFile::fileName() const
{
  return(_fileName);
}

string ProjectFile::guid() const
{
  return(_guid);
}

string ProjectFile::name() const
{
  return(_prefix+"_"+_name);
}

void ProjectFile::loadConfig()
{
  HANDLE
    fileHandle;

  string
    fileName;

  WIN32_FIND_DATA
    data;

  if (!_project->isModule())
    return;

  if (_wizard->solutionType() == DYNAMIC_MT)
  {
    fileName="..\\" + _project->name() + "\\Config." + _name + ".txt";
    loadConfig(fileName);
  }
  else
  {
    fileHandle=FindFirstFile(("..\\" + _project->name() + "\\*.txt").c_str(),&data);
    do
    {
      if (fileHandle == INVALID_HANDLE_VALUE)
        return;

      if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
        continue;

      if (strcmp(data.cFileName,"Config.txt") == 0)
        continue;

      loadConfig("..\\" + _project->name() + "\\" + data.cFileName);

    } while (FindNextFile(fileHandle,&data));
  }
}

void ProjectFile::write(const vector<Project*> &allprojects)
{
  ofstream
    file;

  file.open("..\\" + _project->name() + "\\" + _fileName);
  if (!file)
    return;

  loadSource();

  if (_wizard->visualStudioVersion() == VS2002)
    writeVS2002(file);
  else
    writeVS2010_2012(file,allprojects);

  file.close();
}

bool ProjectFile::isLib() const
{
  if (_project->isCom())
    return(false);

  return(_project->isLib() || (_wizard->solutionType() != DYNAMIC_MT && _project->isDll()));
}

void ProjectFile::addLines(ifstream &config,vector<string> &container)
{
  string
    line;

  while (!config.eof())
  {
    line=readLine(config);
    if (line.empty())
      return;

    if (!contains(container,line))
      container.push_back(line);
  }
}
string ProjectFile::getIntermediateDirectoryName(const bool debug)
{
  string
    directoryName;

  directoryName = (debug ? _wizard->intermediateDirectoryDebug() : _wizard->intermediateDirectoryRelease());
  directoryName += _wizard->solutionName() + "-" + _wizard->platform();
  directoryName += "\\" + _prefix + "_" + _name + "\\";
  return(directoryName);
}

string ProjectFile::getTargetName(const bool debug)
{
  string
    targetName;

  if (_project->isCom())
    return _name;

  targetName=_prefix+"_";
  if (_prefix.compare("FILTER") != 0)
    targetName+=string(debug ? "DB" : "RL")+"_";
  targetName+=_name+"_";
  return(targetName);
}

void ProjectFile::loadConfig(const string &fileName)
{
  ifstream
    config;

  string
    line;

  config.open(fileName);
  if (!config)
    return;

  while (!config.eof())
  {
    line=readLine(config);
    if (line == "[DEPENDENCIES]")
      addLines(config,_dependencies);
    else if (line == "[INCLUDES]")
      addLines(config,_includes);
    else if (line == "[CPP]")
      addLines(config,_cppFiles);
  }

  config.close();
}

void ProjectFile::loadModule(const string &directory)
{
  string
    file;

  file=directory + "\\" + _name + ".c";
  if (!PathFileExists(file.c_str()))
    file=directory + "\\" + _name + ".cpp";
  _srcFiles.push_back(file);
}

void ProjectFile::loadSource()
{
  string
    resourceFile;

  foreach (string,d,_project->directories())
  {
    if ((_project->isModule()) && (_project->isExe() || (_project->isDll() && _wizard->solutionType() == DYNAMIC_MT)))
      loadModule(*d);
    else
      loadSource(*d);
  }

  resourceFile="..\\" + _project->name() + "\\Resource.rc";
  if (PathFileExists(resourceFile.c_str()))
    _resourceFiles.push_back(resourceFile);
}

void ProjectFile::loadSource(const string &directory)
{
  HANDLE
    fileHandle;

  WIN32_FIND_DATA
    data;

  fileHandle=FindFirstFile((directory + "\\*.*").c_str(),&data);
  do
  {
    if (fileHandle == INVALID_HANDLE_VALUE)
      return;

    if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
      continue;

    if (contains(_project->excludes(),data.cFileName))
      continue;

    if (contains((_wizard->build64bit() ? _project->excludesX64() : _project->excludesX86()),data.cFileName))
      continue;

    if (endsWith(data.cFileName,".asm") || endsWith(data.cFileName,".c") || endsWith(data.cFileName,".cpp") || endsWith(data.cFileName,".nasm"))
      _srcFiles.push_back(directory + "\\" + data.cFileName);
    else if (endsWith(data.cFileName,".h"))
      _includeFiles.push_back(directory + "\\" + data.cFileName);
    else if (endsWith(data.cFileName,".rc"))
      _resourceFiles.push_back(directory + "\\" + data.cFileName);

  } while (FindNextFile(fileHandle,&data));

  FindClose(fileHandle);
}

void ProjectFile::setFileName()
{
  _fileName=_prefix+"_"+_name+"_"+_wizard->solutionName();

  if (_wizard->visualStudioVersion() == VS2002)
    _fileName+=".vcproj";
  else
   _fileName+=".vcxproj";
}

void ProjectFile::setGuid()
{
  GUID
    guid;

  RPC_CSTR
    guidStr;

  CoCreateGuid(&guid);
  UuidToString(&guid,&guidStr);
  _guid=string((char *) guidStr);
  transform(_guid.begin(),_guid.end(),_guid.begin(),::toupper);
  RpcStringFree(&guidStr);
}

void ProjectFile::writeAdditionalDependencies(ofstream &file,const string &separator)
{
  foreach (string,lib,_project->libraries())
  {
    file << separator << *lib;
  }
}

void ProjectFile::writeAdditionalIncludeDirectories(ofstream &file,const string &separator)
{
  foreach (string,dir,_project->directories())
  {
    file << separator << *dir;
  }
  foreach (string,dir,_includes)
  {
    file << separator << *dir;
  }
  if (_wizard->useOpenCL())
    file << separator << _wizard->openCLIncludePath();
}

void ProjectFile::writePreprocessorDefinitions(ofstream &file,const bool debug)
{
  file << (debug ? "_DEBUG" : "NDEBUG") << ";_WINDOWS;WIN32;_VISUALC_;NeedFunctionPrototypes";
  foreach (string,def,_project->defines())
  {
    file << ";" << *def;
  }
  if (isLib() || (_wizard->solutionType() != DYNAMIC_MT && (_project->isExe() || _project->isCom())))
    file << ";_LIB";
  else if (_project->isDll())
    file << ";_DLL;_MAGICKMOD_";
  if (_project->isExe() && _wizard->solutionType() != STATIC_MT)
    file << ";_AFXDLL";
}

void ProjectFile::writeVS2002(ofstream &file)
{
  file << "<?xml version=\"1.0\" encoding = \"Windows-1252\"?>" << endl;
  file << "<VisualStudioProject" << endl;
  file << "  ProjectType=\"Visual C++\"" << endl;
  file << "  Version=\"7.00\"" << endl;
  file << "  Name=\"" << _prefix << "_" << _name << "\"" << endl;
  file << "  ProjectGUID=\"{" << _guid << "}\"" << endl;
  file << "  Keyword=\"" << _wizard->platform() << "Proj\">" << endl;
  file << "  <Platforms>" << endl;
  file << "    <Platform Name=\"" << _wizard->platform() << "\"/>" << endl;
  file << "  </Platforms>" << endl;
  file << "  <Configurations>" << endl;

  writeVS2002Configuration(file,false);
  writeVS2002Configuration(file,true);

  file << "  </Configurations>" << endl;
  file << "  <Files>" << endl;

  writeVS2002Files(file,"src",_srcFiles);
  writeVS2002Files(file,"include",_includeFiles);
  writeVS2002Files(file,"resource",_resourceFiles);

  file << "  </Files>" << endl;
  file << "</VisualStudioProject>" << endl;
}

void ProjectFile::writeVS2002Configuration(ofstream &file,const bool debug)
{
  string
    name;

  file << "    <Configuration" << endl;
  file << "      Name=\"" << (debug ? "Debug" : "Release") << "|" << _wizard->platform() << "\"" << endl;
  file << "      OutputDirectory=\"" << (isLib() ? _wizard->libDirectory() : _wizard->binDirectory()) << "\"" << endl;
  file << "      IntermediateDirectory=\"" << getIntermediateDirectoryName(debug) << "\""  << endl;
  if (isLib())
    file << "      ConfigurationType=\"4\""  << endl;
  else if (_project->isDll())
    file << "      ConfigurationType=\"2\""  << endl;
  else if (_project->isExe())
    file << "      ConfigurationType=\"1\""  << endl;
  file << "      UseOfMFC=\"0\""  << endl;
  file << "      ATLMinimizesCRunTimeLibraryUsage=\"FALSE\""  << endl;
  file << "      CharacterSet=\"2\">"  << endl;
  file << "      <Tool" << endl;
  file << "        Name=\"VCCLCompilerTool\"" << endl;
  file << "        RuntimeLibrary=\"";
  if (_wizard->solutionType() == STATIC_MT)
     file << (debug ? "1" : "0") << "\"" << endl;
  else
     file << (debug ? "3" : "2") << "\"" << endl;
  file << "        StringPooling=\"TRUE\"" << endl;
  file << "        EnableFunctionLevelLinking=\"TRUE\"" << endl;
  file << "        WarningLevel=\"" << _project->warningLevel() << "\"" << endl;
  file << "        UsePrecompiledHeader=\"0\"" << endl;
  file << "        SuppressStartupBanner=\"TRUE\"" << endl;
  file << "        CompileAs=\"0\"" << endl;
  file << "        InlineFunctionExpansion=\"2\"" << endl;
  file << "        OpenMP=\"" << (_wizard->useOpenMP() ? "TRUE" : "FALSE") << "\"" << endl;
  file << "        DebugInformationFormat=\"3\"" << endl;
  file << "        BasicRuntimeChecks=\"" << (debug ? "3" : "0") << "\"" << endl;
  file << "        OmitFramePointers=\"" << (debug ? "FALSE" : "TRUE") << "\"" << endl;
  file << "        Optimization=\"" << (debug ? "0" : "3") << "\"" << endl;
  file << "        AdditionalIncludeDirectories=\".";
  writeAdditionalIncludeDirectories(file, ",");
  file << "\"" << endl;
  file << "        PreprocessorDefinitions=\"";
  writePreprocessorDefinitions(file,debug);
  file << "\"/>" << endl;
  file << "      <Tool" << endl;;
  file << "        Name=\"VCMIDLTool\"/>" << endl;
  file << "      <Tool" << endl;
  file << "        Name=\"VCResourceCompilerTool\"" << endl;
  file << "        PreprocessorDefinitions=\"" << (debug ? "_DEBUG" : "NDEBUG") << "\"" << endl;;
  file << "        Culture=\"1033\"/>" << endl;
  file << "      <Tool" << endl;
  file << "        Name=\"" << (isLib() ? "VCLibrarianTool" : "VCLinkerTool") << "\"" << endl;
  file << "        AdditionalLibraryDirectories=\"" << _wizard->libDirectory() << "\"" << endl;
  file << "        AdditionalDependencies=\"/MACHINE:" << (_wizard->build64bit() ? "AMD64" : "X86") << "";
  writeAdditionalDependencies(file," ");
  file << "\"" << endl;
  file << "        SuppressStartupBanner=\"TRUE\"" << endl;
  file << "        LinkIncremental=\"1\"" << endl;
  file << "        TargetMachine=\"" << (_wizard->build64bit() ? "17" : "1") << "\"" << endl;
  file << "        SubSystem=\"" << (_project->isConsole() ? "1" : "2") << "\"" << endl;

  name=getTargetName(debug);

  if (isLib())
    file << "        OutputFile=\"" << _wizard->libDirectory() << name << ".lib\"/>" << endl;
  else if (_project->isDll())
  {
    if (!_project->moduleDefinitionFile().empty())
      file << "        ModuleDefinitionFile=\"" << _project->moduleDefinitionFile() << "\"" << endl;
    file << "        LinkDLL=\"TRUE\"" << endl;
    file << "        GenerateDebugInformation=\"TRUE\"" << endl;
    file << "        ProgramDatabaseFile=\"" << _wizard->binDirectory() << name << ".pdb\"" << endl;
    file << "        ImportLibrary=\"" << _wizard->libDirectory() << name << ".lib\"" << endl;
    file << "        OutputFile=\"" << _wizard->binDirectory() << name << ".dll\"/>" << endl;
  }
  else if (_project->isExe())
  {
    file << "        GenerateDebugInformation=\"TRUE\"" << endl;
    file << "        ProgramDatabaseFile=\"" << _wizard->binDirectory() << _name << ".pdb\"" << endl;
    file << "        ImportLibrary=\"" << _wizard->libDirectory() << name << ".lib\"" << endl;
    file << "        OutputFile=\"" << _wizard->binDirectory() << _name << ".exe\"/>" << endl;
  }
  file << "    </Configuration>" << endl;
}

void ProjectFile::writeVS2002Files(ofstream &file,string name,const vector<string> &collection)
{
  int
    count;

  map<string, int>
    fileCount;

  string
    fileName,
    folder,
    objFile;

  file << "    <Filter Name=\"" << name << "\" Filter=\"\">" << endl;
  foreach_const (string,f,collection)
  {
    if (endsWith((*f),".asm"))
    {
      file << "      <File RelativePath=\"" << *f << "\">" << endl;
      file << "        <FileConfiguration Name=\"Debug|" << _wizard->platform() << "\">" << endl;
      file << "          <Tool Name=\"VCCustomBuildTool\" CommandLine=\"ml" << (_wizard->build64bit() ? "64" : "") << " /nologo /c /Cx " << (_wizard->build64bit() ? "" : "/safeseh /coff") << " /Fo&quot;$(IntDir)\\$(InputName).obj&quot; &quot;$(InputPath)&quot;\" Outputs=\"$(IntDir)\\$(InputName).obj\"/>" << endl;
      file << "        </FileConfiguration>" << endl;
      file << "        <FileConfiguration Name=\"Release|" << _wizard->platform() << "\">" << endl;
      file << "          <Tool Name=\"VCCustomBuildTool\" CommandLine=\"ml" << (_wizard->build64bit() ? "64" : "") << " /nologo /c /Cx " << (_wizard->build64bit() ? "" : "/safeseh /coff") << " /Fo&quot;$(IntDir)\\$(InputName).obj&quot; &quot;$(InputPath)&quot;\" Outputs=\"$(IntDir)\\$(InputName).obj\"/>" << endl;
      file << "        </FileConfiguration>" << endl;
      file << "      </File>" << endl;
    }
    else if (endsWith((*f),".nasm"))
    {
      folder=(*f).substr(0,(*f).find_last_of("\\") + 1);
      file << "      <File RelativePath=\"" << *f << "\">" << endl;
      file << "        <FileConfiguration Name=\"Debug|" << _wizard->platform() << "\">" << endl;
      file << "          <Tool Name=\"VCCustomBuildTool\" CommandLine=\"..\\build\\nasm -fwin" << (_wizard->build64bit() ? "64" : "32") << " -DWIN" << (_wizard->build64bit() ? "64 -D__x86_64__" : "32") << " -I" << folder << " -o &quot;$(IntDir)\\$(InputName).obj&quot; &quot;$(InputPath)&quot;\" Outputs=\"$(IntDir)\\$(InputName).obj\"/>" << endl;
      file << "        </FileConfiguration>" << endl;
      file << "        <FileConfiguration Name=\"Release|" << _wizard->platform() << "\">" << endl;
      file << "          <Tool Name=\"VCCustomBuildTool\" CommandLine=\"..\\build\\nasm -fwin" << (_wizard->build64bit() ? "64" : "32") << " -DWIN" << (_wizard->build64bit() ? "64 -D__x86_64__" : "32") << " -I" << folder << " -o &quot;$(IntDir)\\$(InputName).obj&quot; &quot;$(InputPath)&quot;\" Outputs=\"$(IntDir)\\$(InputName).obj\"/>" << endl;
      file << "        </FileConfiguration>" << endl;
      file << "      </File>" << endl;
    }
    else if (endsWith((*f),".c"))
    {
      if (contains(_cppFiles,(*f)))
      {
        file << "      <File RelativePath=\"" << *f << "\">" << endl;
        file << "        <FileConfiguration Name=\"Debug|" << _wizard->platform() << "\">" << endl;
        file << "          <Tool Name=\"VCCLCompilerTool\" CompileAs=\"2\"/>" << endl;
        file << "        </FileConfiguration>" << endl;
        file << "        <FileConfiguration Name=\"Release|" << _wizard->platform() << "\">" << endl;
        file << "          <Tool Name=\"VCCLCompilerTool\" CompileAs=\"2\"/>" << endl;
        file << "        </FileConfiguration>" << endl;
        file << "      </File>" << endl;
      }
      else
      {
        fileName=(*f).substr((*f).find_last_of("\\") + 1);

        count=1;
        if (fileCount.find(fileName) == fileCount.end())
          fileCount.insert(make_pair(fileName,count));
        else
          count=++fileCount[fileName];

        if (count == 1)
          {
            file << "      <File RelativePath=\"" << *f << "\"/>" << endl;
          }
        else
        {
          objFile=(*f).substr(0,(*f).find_last_of(".") + 1);
          objFile=objFile.substr(objFile.find_last_of("\\") + 1);
          file << "      <File RelativePath=\"" << *f << "\">" << endl;
          file << "        <FileConfiguration Name=\"Debug|" << _wizard->platform() << "\">" << endl;
          file << "          <Tool Name=\"VCCLCompilerTool\" ObjectFile=\"$(IntDir)\\" << objFile << count << ".obj\"/>" << endl;
          file << "        </FileConfiguration>" << endl;
          file << "        <FileConfiguration Name=\"Release|" << _wizard->platform() << "\">" << endl;
          file << "          <Tool Name=\"VCCLCompilerTool\" ObjectFile=\"$(IntDir)\\" << objFile << count << ".obj\"/>" << endl;
          file << "        </FileConfiguration>" << endl;
          file << "      </File>" << endl;
        }
      }
    }
    else
      file << "      <File RelativePath=\"" << *f << "\"/>" << endl;
  }
  file << "    </Filter>" << endl;
}

void ProjectFile::writeVS2010_2012(ofstream &file,const vector<Project*> &allProjects)
{
  file << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl;
  file << "<Project DefaultTargets=\"Build\" ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">" << endl;
  file << "  <ItemGroup Label=\"ProjectConfigurations\">" << endl;
  file << "    <ProjectConfiguration Include=\"Debug|" << _wizard->platform() << "\">" << endl;
  file << "      <Configuration>Debug</Configuration>" << endl;
  file << "      <Platform>" << _wizard->platform() << "</Platform>" << endl;
  file << "    </ProjectConfiguration>" << endl;
  file << "    <ProjectConfiguration Include=\"Release|" << _wizard->platform() << "\">" << endl;
  file << "      <Configuration>Release</Configuration>" << endl;
  file << "      <Platform>" << _wizard->platform() << "</Platform>" << endl;
  file << "    </ProjectConfiguration>" << endl;
  file << "  </ItemGroup>" << endl;
  file << "  <PropertyGroup Label=\"Globals\">" << endl;
  file << "    <ProjectName>" << _prefix << "_" << _name << "</ProjectName>" << endl;
  file << "    <ProjectGuid>{" << _guid << "}</ProjectGuid>" << endl;
  file << "    <Keyword>" << _wizard->platform() << "Proj</Keyword>" << endl;
  file << "  </PropertyGroup>" << endl;
  file << "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />" << endl;

  file << "  <PropertyGroup Label=\"Configuration\">" << endl;
  if (isLib())
    file << "    <ConfigurationType>StaticLibrary</ConfigurationType>" << endl;
  else if (_project->isDll())
    file << "    <ConfigurationType>DynamicLibrary</ConfigurationType>" << endl;
  else if (_project->isExe())
    file << "    <ConfigurationType>Application</ConfigurationType>" << endl;
  if (_wizard->visualStudioVersion() == VS2010)
    file << "    <PlatformToolset>v100</PlatformToolset>" << endl;
  else if (_wizard->visualStudioVersion() == VS2012)
    file << "    <PlatformToolset>v110</PlatformToolset>" << endl;
  file << "    <UseOfMfc>false</UseOfMfc>" << endl;
  file << "    <CharacterSet>MultiByte</CharacterSet>" << endl;
  file << "  </PropertyGroup>" << endl;

  file << "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />" << endl;

  file << "  <PropertyGroup>" << endl;
  if (_wizard->visualStudioVersion() == VS2010)
    file << "    <_ProjectFileVersion>10.0.40219.1</_ProjectFileVersion>" << endl;
  else if (_wizard->visualStudioVersion() == VS2012)
    file << "    <_ProjectFileVersion>11.0.61030.0</_ProjectFileVersion>" << endl;
  file << "    <LinkIncremental>false</LinkIncremental>" << endl;
  file << "    <OutDir>" << (isLib() ? _wizard->libDirectory() : _wizard->binDirectory()) << "</OutDir>" << endl;
  if (_project->isExe())
  {
    file << "    <TargetName>" << _name << "</TargetName>" << endl;
  }
  else
  {
    file << "    <TargetName Condition=\"'$(Configuration)|$(Platform)'=='Debug|" << _wizard->platform() << "'\">" << getTargetName(true) << "</TargetName>" << endl;
    file << "    <TargetName Condition=\"'$(Configuration)|$(Platform)'=='Release|" << _wizard->platform() << "'\">" << getTargetName(false) << "</TargetName>" << endl;
  }
  file << "    <IntDir Condition=\"'$(Configuration)|$(Platform)'=='Debug|" << _wizard->platform() << "'\">" << getIntermediateDirectoryName(true) << "</IntDir>" << endl;
  file << "    <IntDir Condition=\"'$(Configuration)|$(Platform)'=='Release|" << _wizard->platform() << "'\">" << getIntermediateDirectoryName(false) << "</IntDir>" << endl;
  file << "  </PropertyGroup>" << endl;

  writeVS2010_2012ItemDefinitionGroup(file,true);
  writeVS2010_2012ItemDefinitionGroup(file,false);

  writeVS2010_2012Files(file,_srcFiles);
  writeVS2010_2012Files(file,_includeFiles);
  writeVS2010_2012Files(file,_resourceFiles);

  writeVS2010_2012ProjectReferences(file,allProjects);

  file << "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />" << endl;
  file << "</Project>" << endl;
}

void ProjectFile::writeVS2010_2012ItemDefinitionGroup(ofstream &file,const bool debug)
{
  string
    name;

  file << "  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='" << (debug ? "Debug" : "Release") << "|" << _wizard->platform() << "'\">" << endl;
  file << "    <ClCompile>" << endl;
  file << "      <RuntimeLibrary>MultiThreaded" << (debug ? "Debug" : "") << (_wizard->solutionType() == STATIC_MT ? "" : "DLL") << "</RuntimeLibrary>" << endl;
  file << "      <StringPooling>true</StringPooling>" << endl;
  file << "      <FunctionLevelLinking>true</FunctionLevelLinking>" << endl;
  if (_project->warningLevel() == 0)
    file << "      <WarningLevel>TurnOffAllWarnings</WarningLevel>" << endl;
  else
    file << "      <WarningLevel>Level" << _project->warningLevel() << "</WarningLevel>" << endl;
  file << "      <SuppressStartupBanner>true</SuppressStartupBanner>" << endl;
  file << "      <CompileAs>Default</CompileAs>" << endl;
  file << "      <InlineFunctionExpansion>AnySuitable</InlineFunctionExpansion>" << endl;
  file << "      <OpenMPSupport>" << (_wizard->useOpenMP() ? "true" : "false") << "</OpenMPSupport>" << endl;
  file << "      <DebugInformationFormat>ProgramDatabase</DebugInformationFormat>" << endl;
  file << "      <BasicRuntimeChecks>" << (debug ? "EnableFastChecks" : "Default") <<"</BasicRuntimeChecks>" << endl;
  file << "      <OmitFramePointers>" << (debug ? "false" : "true") <<"</OmitFramePointers>" << endl;
  file << "      <Optimization>" << (debug ? "Disabled" : "Full") <<"</Optimization>" << endl;
  file << "      <AdditionalIncludeDirectories>";
  writeAdditionalIncludeDirectories(file,";");
  file << ";%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>" << endl;
  file << "      <PreprocessorDefinitions>";
  writePreprocessorDefinitions(file,debug);
  file << ";%(PreprocessorDefinitions)</PreprocessorDefinitions>" << endl;
  file << "      <MultiProcessorCompilation>true</MultiProcessorCompilation>" << endl;
  file << "    </ClCompile>" << endl;
  file << "    <ResourceCompile>" << endl;
  file << "      <PreprocessorDefinitions>" << (debug ? "_DEBUG" : "NDEBUG") <<";%(PreprocessorDefinitions)</PreprocessorDefinitions>" << endl;
  file << "      <Culture>0x0409</Culture>" << endl;
  file << "    </ResourceCompile>" << endl;

  name=getTargetName(debug);

  if (isLib())
  {
    file << "    <Lib>" << endl;
    file << "      <AdditionalLibraryDirectories>" << _wizard->libDirectory() << ";%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>" << endl;
    file << "      <AdditionalDependencies>/MACHINE:" << (_wizard->build64bit() ? "AMD64" : "X86");
    writeAdditionalDependencies(file,";");
    file << ";%(AdditionalDependencies)</AdditionalDependencies>" << endl;
    file << "      <SuppressStartupBanner>true</SuppressStartupBanner>" << endl;
    file << "    </Lib>" << endl;
  }
  else
  {
    file << "    <Link>" << endl;
    file << "      <AdditionalLibraryDirectories>" << _wizard->libDirectory() << ";%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>" << endl;
    file << "      <AdditionalDependencies>/MACHINE:" << (_wizard->build64bit() ? "AMD64" : "X86");
    writeAdditionalDependencies(file,";");
    file << ";%(AdditionalDependencies)</AdditionalDependencies>" << endl;
    file << "      <SuppressStartupBanner>true</SuppressStartupBanner>" << endl;
    file << "      <TargetMachine>Machine" << (_wizard->build64bit() ? "X64" : "X86") << "</TargetMachine>" << endl;
    file << "      <GenerateDebugInformation>true</GenerateDebugInformation>" << endl;
    file << "      <ProgramDatabaseFile>" << _wizard->binDirectory() << (_project->isExe() ? _name : name) << ".pdb</ProgramDatabaseFile>" << endl;
    file << "      <ImportLibrary>" << _wizard->libDirectory() << name << ".lib</ImportLibrary>" << endl;
    if (!_project->isConsole())
    {
      if (_project->isDll())
        file << "      <LinkDLL>true</LinkDLL>" << endl;
      file << "      <SubSystem>Windows</SubSystem>" << endl;
      if ((_project->isDll()) && (!_project->moduleDefinitionFile().empty()))
        file << "      <ModuleDefinitionFile>" << _project->moduleDefinitionFile() << "</ModuleDefinitionFile>" << endl;
    }
    else
      file << "      <SubSystem>Console</SubSystem>" << endl;
    file << "    </Link>" << endl;
  }
  file << "  </ItemDefinitionGroup>" << endl;
}

void ProjectFile::writeVS2010_2012Files(ofstream &file,const vector<string> &collection)
{
  int
    count;

  map<string, int>
    fileCount;

  string
    fileName,
    folder,
    objFile;

  if (collection.size() == 0)
    return;

  file << "  <ItemGroup>" << endl;
  foreach_const (string,f,collection)
  {
    if (endsWith((*f),".rc"))
      file << "    <ResourceCompile Include=\"" << *f << "\" />" << endl;
    else if (endsWith((*f),".h"))
      file << "    <ClInclude Include=\"" << *f << "\" />" << endl;
    else if (endsWith((*f),".asm"))
    {
      file << "    <CustomBuild Include=\"" << *f << "\">" << endl;
      file << "      <Command>ml" << (_wizard->build64bit() ? "64" : "") << " /nologo /c /Cx " << (_wizard->build64bit() ? "" : "/safeseh /coff") << " /Fo\"$(IntDir)%(Filename).obj\" \"%(FullPath)\"</Command>" << endl;
      file << "      <Outputs>$(IntDir)%(Filename).obj;%(Outputs)</Outputs>" << endl;
      file << "    </CustomBuild>" << endl;
    }
    else if (endsWith((*f),".nasm"))
    {
      folder=(*f).substr(0,(*f).find_last_of("\\") + 1);
      file << "    <CustomBuild Include=\"" << *f << "\">" << endl;
      file << "      <Command>..\\build\\nasm -fwin" << (_wizard->build64bit() ? "64" : "32") << " -DWIN" << (_wizard->build64bit() ? "64 -D__x86_64__" : "32") << " -I" << folder << " -o \"$(IntDir)%(Filename).obj\" \"%(FullPath)\"</Command>" << endl;
      file << "      <Outputs>$(IntDir)%(Filename).obj;%(Outputs)</Outputs>" << endl;
      file << "    </CustomBuild>" << endl;
    }
    else
    {
      fileName=(*f).substr((*f).find_last_of("\\") + 1);

      count=1;
      if (fileCount.find(fileName) == fileCount.end())
        fileCount.insert(make_pair(fileName,count));
      else
        count=++fileCount[fileName];

      file << "    <ClCompile Include=\"" << *f << "\">" << endl;
      if (contains(_cppFiles,(*f)))
        file << "      <CompileAs>CompileAsCpp</CompileAs>" << endl;
      else if (count > 1)
      {
        objFile=(*f).substr(0,(*f).find_last_of(".") + 1);
        objFile=objFile.substr(objFile.find_last_of("\\") + 1);
        file << "      <ObjectFileName>$(IntDir)" << objFile << "</ObjectFileName>" << endl;
      }
      file << "      <MultiProcessorCompilation>true</MultiProcessorCompilation>" << endl;
      file << "    </ClCompile>" << endl;
    }
  }
  file << "  </ItemGroup>" << endl;
}

void ProjectFile::writeVS2010_2012ProjectReferences(ofstream &file,const vector<Project*> &allProjects)
{
  file << "  <ItemGroup>" << endl;

  foreach (string,dep,_dependencies)
  {
    foreach_const (Project*,depp,allProjects)
    {
      if ((*depp)->name() != *dep)
        continue;

      foreach (ProjectFile*,deppf,(*depp)->files())
      {
        file << "    <ProjectReference Include=\"..\\" << (*deppf)-> _project->name() << "\\" << (*deppf)->_fileName << "\">" << endl;
        file << "      <Project>{" << (*deppf)->guid() << "}</Project>" << endl;
        file << "      <ReferenceOutputAssembly>false</ReferenceOutputAssembly>" << endl;
        file << "    </ProjectReference>" << endl;
      }
    }
  }

  file << "  </ItemGroup>" << endl;
}