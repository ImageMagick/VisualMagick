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
#include "Project.h"
#include "ProjectFile.h"
#include "Shared.h"
#include <algorithm>
#include <map>

ProjectFile::ProjectFile(const ConfigureWizard *wizard,Project *project,
  const wstring &prefix,const wstring &name)
  : _wizard(wizard),
    _project(project),
    _prefix(prefix),
    _name(name)
{
  initialize(project);
  loadAliases();
}

ProjectFile::ProjectFile(const ConfigureWizard *wizard,Project *project,
  const wstring &prefix,const wstring &name,const wstring &reference)
  : _wizard(wizard),
    _project(project),
    _prefix(prefix),
    _name(name),
    _reference(reference)
{
  initialize(project);
}

vector<wstring> &ProjectFile::dependencies()
{
  return(_dependencies);
}

wstring ProjectFile::fileName() const
{
  return(_fileName);
}

wstring ProjectFile::guid() const
{
  return(_guid);
}

wstring ProjectFile::name() const
{
  return(_prefix+L"_"+_name);
}

vector<wstring> &ProjectFile::aliases()
{
  return(_aliases);
}

void ProjectFile::initialize(Project* project)
{
  _minimumVisualStudioVersion=VSEARLIEST;
  setFileName();
  _guid=createGuid();

  foreach(wstring,dep,project->dependencies())
  {
    _dependencies.push_back(*dep);
  }

  foreach(wstring,inc,project->includes())
  {
    _includes.push_back(*inc);
  }

  foreach(wstring,inc,project->definesLib())
  {
    _definesLib.push_back(*inc);
  }
}

bool ProjectFile::isSrcFile(const wstring &fileName)
{
  if (endsWith(fileName,L".asm"))
    return(true);

  return(isValidSrcFile(fileName));
}

bool ProjectFile::isExcluded(const wstring &fileName)
{
  if (contains(_project->excludes(),fileName))
    return true;

  if (contains(_project->platformExcludes(_wizard->platform()),fileName))
    return true;

  if (endsWith(fileName,L".h"))
    return isExcluded(replace(fileName,L".h",L".c")) || isExcluded(replace(fileName,L".h",L".cc"));

  return false;
}

void ProjectFile::loadAliases()
{
  wifstream
    aliases;

  wstring
    fileName,
    line;

  if (!_project->isExe() || !_project->isModule())
    return;

  fileName=L"..\\" + _project->name() + L"\\Aliases." + _name + L".txt";

  aliases.open(fileName);
  if (!aliases)
    return;

  while (!aliases.eof())
  {
    line=readLine(aliases);
    if (!line.empty())
      _aliases.push_back(line);
  }

  aliases.close();
}

bool ProjectFile::isSupported(const VisualStudioVersion visualStudioVersion) const
{
  return(visualStudioVersion >= _minimumVisualStudioVersion);
}

void ProjectFile::loadConfig()
{
  wifstream
    config;

  wstring
    fileName,
    line;

  if (!_project->isModule())
    return;

  fileName=L"..\\" + _project->name() + L"\\Config." + _name + L".txt";

  config.open(fileName);
  if (!config)
    return;

  while (!config.eof())
  {
    line=readLine(config);
    if (line == L"[DEPENDENCIES]")
      addLines(config,_dependencies);
    else if (line == L"[INCLUDES]")
      addLines(config,_includes);
    else if (line == L"[CPP]")
      addLines(config,_cppFiles);
    else if (line == L"[VISUAL_STUDIO]")
      _minimumVisualStudioVersion=parseVisualStudioVersion(readLine(config));
    else if (line == L"[DEFINES_LIB]")
      addLines(config,_definesLib);
  }

  config.close();
}

void ProjectFile::merge(ProjectFile *projectFile)
{
  merge(projectFile->_dependencies,_dependencies);
  merge(projectFile->_includes,_includes);
  merge(projectFile->_cppFiles,_cppFiles);
  merge(projectFile->_definesLib,_definesLib);
}

void ProjectFile::write(const vector<Project*> &allprojects)
{
  wofstream
    file;

  file.open(L"..\\" + _project->name() + L"\\" + _fileName);
  if (!file)
    return;

  loadSource();

  write(file,allprojects);

  file.close();

  file.open(L"..\\" + _project->name() + L"\\" + _fileName + L".filters");
  if (!file)
    return;

  writeFilter(file);

  file.close();
}

bool ProjectFile::isLib() const
{
  if (_project->isCom())
    return(false);

  return(_project->isLib() || (_wizard->solutionType() != SolutionType::DYNAMIC_MT && _project->isDll()));
}

wstring ProjectFile::outputDirectory() const
{
  if (_project->isFuzz())
    return(_wizard->fuzzBinDirectory());

  if (isLib())
    return(_wizard->libDirectory());

  return(_wizard->binDirectory());
}

void ProjectFile::addFile(const wstring &directory, const wstring &name)
{
  wstring
    header_file,
    src_file;

  foreach_const(wstring,ext,validSrcFiles)
  {
    src_file=directory + L"\\" + name + *ext;

    if (PathFileExists(src_file.c_str()))
    {
      _srcFiles.push_back(src_file);

      header_file=directory + L"\\" + name + L".h";
      if (PathFileExists(header_file.c_str()))
        _includeFiles.push_back(header_file);

      break;
    }
  }

  if (!_project->isExe())
    return;

  foreach_const(wstring,ext,validSrcFiles)
  {
    src_file=directory + L"\\main" + *ext;

    if (PathFileExists(src_file.c_str()))
    {
      _srcFiles.push_back(src_file);

      header_file=directory + L"\\" + name + L".h";
      if (PathFileExists(header_file.c_str()))
        _includeFiles.push_back(header_file);

      break;
    }
  }
}

void ProjectFile::addLines(wifstream &config,vector<wstring> &container)
{
  wstring
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

wstring ProjectFile::getFilter(const wstring &fileName,vector<wstring> &filters)
{
  wstring
    filter,
    folder;

  filter=replace(fileName,L"..\\",L"");
  if (filter.find_first_of(L"\\") == filter.find_last_of(L"\\"))
    return L"";

  filter=filter.substr(filter.find_first_of(L"\\") + 1);

  folder=filter;
  while (folder.find_first_of(L"\\") != -1)
  {
    folder=folder.substr(0, folder.find_last_of(L"\\"));
    if (!contains(filters, folder))
      filters.push_back(folder);
  }

  filter=filter.substr(0, filter.find_last_of(L"\\"));

  return filter;
}

wstring ProjectFile::getIntermediateDirectoryName(const bool debug)
{
  wstring
    directoryName;

  directoryName = (debug ? _wizard->intermediateDirectoryDebug() : _wizard->intermediateDirectoryRelease());
  directoryName += _wizard->solutionName() + L"-" + _wizard->platformName();
  directoryName += L"\\" + _prefix + L"_" + _name + L"\\";
  return(directoryName);
}

wstring ProjectFile::getTargetName(const bool debug)
{
  wstring
    targetName;

  if (_project->isCom())
    return _name;

  targetName=_prefix+L"_";
  if (_prefix.compare(L"FILTER") != 0)
    targetName+=wstring(debug ? L"DB" : L"RL")+L"_";
  targetName+=_name+L"_";
  return(targetName);
}

void ProjectFile::loadModule(const wstring &directory)
{
  if (!_reference.empty())
    addFile(directory, _reference);
  else
    addFile(directory, _name);
}

void ProjectFile::loadSource()
{
  wstring
    resourceFile;

  foreach (wstring,dir,_project->directories())
  {
    if ((_project->isModule()) && (_project->isExe() || (_project->isDll() && _wizard->solutionType() == SolutionType::DYNAMIC_MT)))
      loadModule(*dir);
    else
      loadSource(*dir);
  }

  resourceFile=L"..\\..\\" + _project->name() + L"\\ImageMagick\\ImageMagick.rc";
  if (PathFileExists(resourceFile.c_str()))
    _resourceFiles.push_back(resourceFile);

  /* This resource file is used by the ImageMagick projects */
  resourceFile=L"..\\" + _project->name() + L"\\ImageMagick.rc";
  if (PathFileExists(resourceFile.c_str()))
    _resourceFiles.push_back(resourceFile);
}

void ProjectFile::loadSource(const wstring &directory)
{
  HANDLE
    fileHandle;

  WIN32_FIND_DATA
    data;

  if (contains(_project->platformExcludes(_wizard->platform()),directory))
    return;

  fileHandle=FindFirstFile((directory + L"\\*.*").c_str(),&data);
  do
  {
    if (fileHandle == INVALID_HANDLE_VALUE)
      return;

    if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
      continue;

    if (isExcluded(data.cFileName))
      continue;

    if (isSrcFile(data.cFileName))
      _srcFiles.push_back(directory + L"\\" + data.cFileName);
    else if (endsWith(data.cFileName,L".h"))
      _includeFiles.push_back(directory + L"\\" + data.cFileName);
    else if (endsWith(data.cFileName,L".rc"))
      _resourceFiles.push_back(directory + L"\\" + data.cFileName);

  } while (FindNextFile(fileHandle,&data));

  FindClose(fileHandle);
}

void ProjectFile::merge(vector<wstring> &input, vector<wstring> &output)
{
  foreach (wstring,value,input)
  {
    if (std::find(output.begin(),output.end(), *value) == output.end())
      output.push_back(*value);
  }
}

void ProjectFile::setFileName()
{
  _fileName=_prefix+L"_"+_name+L"_"+_wizard->solutionName()+L".vcxproj";
}

wstring ProjectFile::createGuid()
{
  GUID
    guid;

  RPC_WSTR
    guidStr;

  wstring
    result;

  (void) CoCreateGuid(&guid);
  (void) UuidToString(&guid,&guidStr);
  result=wstring((wchar_t *) guidStr);
  transform(result.begin(),result.end(),result.begin(),::toupper);
  RpcStringFree(&guidStr);
  return result;
}

void ProjectFile::writeAdditionalDependencies(wofstream &file,const wstring &separator)
{
  foreach (wstring,lib,_project->libraries())
  {
    file << separator << *lib;
  }
}

void ProjectFile::writeAdditionalIncludeDirectories(wofstream &file,const wstring &separator)
{
  foreach (wstring,projectDir,_project->directories())
  {
    bool
      skip;

    skip=false;
    foreach (wstring,includeDir,_includes)
    {
      if ((*projectDir).find(*includeDir) == 0)
      {
        skip=true;
        break;
      }
    }

    if (!skip)
      file << separator << *projectDir;
  }
  foreach (wstring,includeDir,_includes)
  {
    file << separator << *includeDir;
  }
  if (_wizard->useOpenCL())
    file << separator << L"..\\..\\VisualMagick\\OpenCL";
}

void ProjectFile::writePreprocessorDefinitions(wofstream &file,const bool debug)
{
  file << (debug ? "_DEBUG" : "NDEBUG") << ";_WINDOWS;WIN32;_VISUALC_;NeedFunctionPrototypes";
  foreach (wstring,def,_project->defines())
  {
    file << ";" << *def;
  }
  if (isLib() || (_wizard->solutionType() != SolutionType::DYNAMIC_MT && (_project->isExe() || _project->isCom())))
  {
    foreach (wstring,def,_definesLib)
    {
      file << ";" << *def;
    }
    file << ";_LIB";
  }
  else if (_project->isDll())
  {
    foreach (wstring,def,_project->definesDll())
    {
      file << ";" << *def;
    }
    file << ";_DLL;_MAGICKMOD_";
  }
  if (_project->isExe() && _wizard->solutionType() != SolutionType::STATIC_MT)
    file << ";_AFXDLL";
  if (_wizard->includeIncompatibleLicense())
    file << ";_AFXDLL,_MAGICK_INCOMPATIBLE_LICENSES_";
}

void ProjectFile::write(wofstream &file,const vector<Project*> &allProjects)
{
  file << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl;
  file << "<Project DefaultTargets=\"Build\" ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">" << endl;
  file << "  <ItemGroup Label=\"ProjectConfigurations\">" << endl;
  file << "    <ProjectConfiguration Include=\"Debug|" << _wizard->platformName() << "\">" << endl;
  file << "      <Configuration>Debug</Configuration>" << endl;
  file << "      <Platform>" << _wizard->platformName() << "</Platform>" << endl;
  file << "    </ProjectConfiguration>" << endl;
  file << "    <ProjectConfiguration Include=\"Release|" << _wizard->platformName() << "\">" << endl;
  file << "      <Configuration>Release</Configuration>" << endl;
  file << "      <Platform>" << _wizard->platformName() << "</Platform>" << endl;
  file << "    </ProjectConfiguration>" << endl;
  file << "  </ItemGroup>" << endl;
  file << "  <PropertyGroup Label=\"Globals\">" << endl;
  file << "    <ProjectName>" << _prefix << "_" << _name << "</ProjectName>" << endl;
  file << "    <ProjectGuid>{" << _guid << "}</ProjectGuid>" << endl;
  file << "    <Keyword>" << _wizard->platformName() << "Proj</Keyword>" << endl;
  file << "  </PropertyGroup>" << endl;
  file << "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />" << endl;

  file << "  <PropertyGroup Label=\"Configuration\">" << endl;
  if (isLib())
    file << "    <ConfigurationType>StaticLibrary</ConfigurationType>" << endl;
  else if (_project->isDll())
    file << "    <ConfigurationType>DynamicLibrary</ConfigurationType>" << endl;
  else if (_project->isExe())
    file << "    <ConfigurationType>Application</ConfigurationType>" << endl;
  if (_wizard->visualStudioVersion() == VisualStudioVersion::VS2015)
    file << "    <PlatformToolset>v140_xp</PlatformToolset>" << endl;
  else if (_wizard->visualStudioVersion() == VisualStudioVersion::VS2017)
    file << "    <PlatformToolset>v141</PlatformToolset>" << endl;
  else if (_wizard->visualStudioVersion() == VisualStudioVersion::VS2019)
    file << "    <PlatformToolset>v142</PlatformToolset>" << endl;
  else if (_wizard->visualStudioVersion() == VisualStudioVersion::VS2022)
    file << "    <PlatformToolset>v143</PlatformToolset>" << endl;
  file << "    <UseOfMfc>false</UseOfMfc>" << endl;
  if (_project->useUnicode())
    file << "    <CharacterSet>Unicode</CharacterSet>" << endl;
  else
    file << "    <CharacterSet>MultiByte</CharacterSet>" << endl;
  file << "  </PropertyGroup>" << endl;

  file << "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />" << endl;

  file << "  <PropertyGroup>" << endl;
  file << "    <LinkIncremental>false</LinkIncremental>" << endl;
  file << "    <OutDir>" << outputDirectory() << "</OutDir>" << endl;
  if (_project->isExe())
  {
    file << "    <TargetName>" << _name << "</TargetName>" << endl;
  }
  else
  {
    file << "    <TargetName Condition=\"'$(Configuration)|$(Platform)'=='Debug|" << _wizard->platformName() << "'\">" << getTargetName(true) << "</TargetName>" << endl;
    file << "    <TargetName Condition=\"'$(Configuration)|$(Platform)'=='Release|" << _wizard->platformName() << "'\">" << getTargetName(false) << "</TargetName>" << endl;
  }
  file << "    <IntDir Condition=\"'$(Configuration)|$(Platform)'=='Debug|" << _wizard->platformName() << "'\">" << getIntermediateDirectoryName(true) << "</IntDir>" << endl;
  file << "    <IntDir Condition=\"'$(Configuration)|$(Platform)'=='Release|" << _wizard->platformName() << "'\">" << getIntermediateDirectoryName(false) << "</IntDir>" << endl;
  if (_wizard->visualStudioVersion() >= VisualStudioVersion::VS2019)
    file << "    <UseDebugLibraries Condition=\"'$(Configuration)|$(Platform)'=='Debug|" << _wizard->platformName() << "'\">true</UseDebugLibraries>" << endl;
  file << "  </PropertyGroup>" << endl;

  writeItemDefinitionGroup(file,true);
  writeItemDefinitionGroup(file,false);

  writeFiles(file,_srcFiles);
  writeFiles(file,_includeFiles);
  writeFiles(file,_resourceFiles);

  writeProjectReferences(file,allProjects);

  file << "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />" << endl;
  file << "</Project>" << endl;
}

void ProjectFile::writeItemDefinitionGroup(wofstream &file,const bool debug)
{
  wstring
    name;

  name=getTargetName(debug);

  file << "  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='" << (debug ? "Debug" : "Release") << "|" << _wizard->platformName() << "'\">" << endl;
  file << "    <ClCompile>" << endl;
  file << "      <RuntimeLibrary>MultiThreaded" << (debug ? "Debug" : "") << (_wizard->solutionType() == SolutionType::STATIC_MT ? "" : "DLL") << "</RuntimeLibrary>" << endl;
  file << "      <StringPooling>true</StringPooling>" << endl;
  file << "      <FunctionLevelLinking>true</FunctionLevelLinking>" << endl;
  if (_project->warningLevel() == 0)
    file << "      <WarningLevel>TurnOffAllWarnings</WarningLevel>" << endl;
  else
    file << "      <WarningLevel>Level" << _project->warningLevel() << "</WarningLevel>" << endl;
  if (_project->treatWarningAsError())
    file << "      <TreatWarningAsError>true</TreatWarningAsError>" << endl;
  file << "      <SuppressStartupBanner>true</SuppressStartupBanner>" << endl;
  file << "      <CompileAs>Default</CompileAs>" << endl;
  file << "      <InlineFunctionExpansion>" << (debug ? "Disabled" : "AnySuitable") << "</InlineFunctionExpansion>" << endl;
  file << "      <OpenMPSupport>" << (_wizard->useOpenMP() ? "true" : "false") << "</OpenMPSupport>" << endl;
  file << "      <DebugInformationFormat>ProgramDatabase</DebugInformationFormat>" << endl;
  file << "      <ProgramDatabaseFileName>" << _wizard->binDirectory() << (_project->isExe() ? _name : name) << ".pdb</ProgramDatabaseFileName>" << endl;
  file << "      <BasicRuntimeChecks>" << (debug ? "EnableFastChecks" : "Default") <<"</BasicRuntimeChecks>" << endl;
  file << "      <OmitFramePointers>" << (debug ? "false" : "true") <<"</OmitFramePointers>" << endl;
  file << "      <Optimization>" << (debug ? "Disabled" : "Full") <<"</Optimization>" << endl;
  file << "      <AdditionalIncludeDirectories>";
  writeAdditionalIncludeDirectories(file,L";");
  file << ";%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>" << endl;
  file << "      <PreprocessorDefinitions>";
  writePreprocessorDefinitions(file,debug);
  file << ";%(PreprocessorDefinitions)</PreprocessorDefinitions>" << endl;
  file << "      <MultiProcessorCompilation>true</MultiProcessorCompilation>" << endl;
  if (_wizard->visualStudioVersion() >= VisualStudioVersion::VS2015)
    file << "      <AdditionalOptions>/source-charset:utf-8 %(AdditionalOptions)</AdditionalOptions>" << endl;
  if (_wizard->visualStudioVersion() >= VisualStudioVersion::VS2017) {
    file << "      <LanguageStandard>stdcpp17</LanguageStandard>" << endl;
    file << "      <LanguageStandard_C>stdc17</LanguageStandard_C>" << endl;
  }
  file << "    </ClCompile>" << endl;
  file << "    <ResourceCompile>" << endl;
  file << "      <PreprocessorDefinitions>" << (debug ? "_DEBUG" : "NDEBUG") <<";%(PreprocessorDefinitions)</PreprocessorDefinitions>" << endl;
  file << "      <Culture>0x0409</Culture>" << endl;
  file << "    </ResourceCompile>" << endl;

  if (isLib())
  {
    file << "    <Lib>" << endl;
    file << "      <AdditionalLibraryDirectories>" << _wizard->libDirectory() << ";%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>" << endl;
    file << "      <AdditionalDependencies>/MACHINE:" << (_wizard->machineName());
    writeAdditionalDependencies(file,L";");
    file << ";%(AdditionalDependencies)</AdditionalDependencies>" << endl;
    file << "      <SuppressStartupBanner>true</SuppressStartupBanner>" << endl;
    file << "    </Lib>" << endl;
  }
  else
  {
    file << "    <Link>" << endl;
    file << "      <AdditionalLibraryDirectories>" << _wizard->libDirectory() << ";%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>" << endl;
    file << "      <AdditionalDependencies>/MACHINE:" << _wizard->machineName();
    writeAdditionalDependencies(file,L";");
    file << ";%(AdditionalDependencies)</AdditionalDependencies>" << endl;
    file << "      <SuppressStartupBanner>true</SuppressStartupBanner>" << endl;
    file << "      <TargetMachine>Machine" << _wizard->machineName() << "</TargetMachine>" << endl;
    file << "      <GenerateDebugInformation>" << (debug ? "true" : "false") << "</GenerateDebugInformation>" << endl;
    file << "      <ProgramDatabaseFile>" << _wizard->binDirectory() << (_project->isExe() ? _name : name) << ".pdb</ProgramDatabaseFile>" << endl;
    file << "      <ImportLibrary>" << _wizard->libDirectory() << name << ".lib</ImportLibrary>" << endl;
    if (!_project->isConsole())
    {
      if (_project->isDll())
        file << "      <LinkDLL>true</LinkDLL>" << endl;
      else if (_project->useUnicode())
        file << "    <EntryPointSymbol>wWinMainCRTStartup</EntryPointSymbol>" << endl;
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

void ProjectFile::writeFiles(wofstream &file,const vector<wstring> &collection)
{
  int
    count;

  map<wstring, int>
    fileCount;

  wstring
    fileName,
    folder,
    objFile;

  if (collection.size() == 0)
    return;

  file << "  <ItemGroup>" << endl;
  foreach_const (wstring,f,collection)
  {
    if (endsWith((*f),L".rc"))
      file << "    <ResourceCompile Include=\"" << *f << "\" />" << endl;
    else if (endsWith((*f),L".h"))
      file << "    <ClInclude Include=\"" << *f << "\" />" << endl;
    else if (endsWith((*f),L".asm"))
    {
      if (!_project->useNasm())
      {
        file << "    <CustomBuild Include=\"" << *f << "\">" << endl;
        file << "      <Command>ml" << (_wizard->platform() == Platform::X64 ? "64" : "") << " /nologo /c /Cx " << (_wizard->platform() == Platform::X64 ? "" : "/safeseh /coff") << " /Fo\"$(IntDir)%(Filename).obj\" \"%(FullPath)\"</Command>" << endl;
        file << "      <Outputs>$(IntDir)%(Filename).obj;%(Outputs)</Outputs>" << endl;
        file << "    </CustomBuild>" << endl;
      }
      else
      {
        folder=(*f).substr(0,(*f).find_last_of(L"\\") + 1);
        file << "    <CustomBuild Include=\"" << *f << "\">" << endl;
        file << "      <Command>..\\build\\nasm -fwin" << (_wizard->platform() == Platform::X64 ? "64" : "32") << " -DWIN" << (_wizard->platform() == Platform::X64 ? "64 -D__x86_64__" : "32") << " -I" << folder << " -o \"$(IntDir)%(Filename).obj\" \"%(FullPath)\"</Command>" << endl;
        file << "      <Outputs>$(IntDir)%(Filename).obj;%(Outputs)</Outputs>" << endl;
        file << "    </CustomBuild>" << endl;
      }
    }
    else
    {
      fileName=(*f).substr((*f).find_last_of(L"\\") + 1);

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
        objFile=(*f).substr(0,(*f).find_last_of(L"."));
        objFile=objFile.substr(objFile.find_last_of(L"\\") + 1);
        file << "      <ObjectFileName>$(IntDir)" << objFile << "_" << count << ".obj</ObjectFileName>" << endl;
      }
      file << "      <MultiProcessorCompilation>true</MultiProcessorCompilation>" << endl;
      file << "    </ClCompile>" << endl;
    }
  }
  file << "  </ItemGroup>" << endl;
}

void ProjectFile::writeFilter(wofstream &file)
{
  wstring
    filter;

  vector<wstring>
    filters;

  file << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl;
  file << "<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">" << endl;
  file << "  <ItemGroup>" << endl;
  foreach_const (wstring,f,_srcFiles)
  {
    filter=getFilter(*f,filters);
    if (filter != L"")
    {
      file << "    <ClCompile Include=\"" << *f << "\">" << endl;
      file << "      <Filter>" << filter << "</Filter>" << endl;
      file << "    </ClCompile>" << endl;
    }
    else
      file << "    <ClCompile Include=\"" << *f << "\" />" << endl;
  }
  file << "  </ItemGroup>" << endl;
  file << "  <ItemGroup>" << endl;
  foreach_const (wstring,f,_includeFiles)
  {
    filter=getFilter(*f,filters);
    if (filter != L"")
    {
      file << "    <CLInclude Include=\"" << *f << "\">" << endl;
      file << "      <Filter>" << filter << "</Filter>" << endl;
      file << "    </CLInclude>" << endl;
    }
    else
      file << "    <CLInclude Include=\"" << *f << "\" />" << endl;
  }
  file << "  </ItemGroup>" << endl;
  file << "  <ItemGroup>" << endl;
  foreach_const (wstring,f,filters)
  {
    file << "    <Filter Include=\"" << *f << "\">" << endl;
    file << "      <UniqueIdentifier>{" << createGuid() << "}</UniqueIdentifier>" << endl;
    file << "    </Filter>" << endl;
  }
  file << "  </ItemGroup>" << endl;
  file << "</Project>" << endl;
}

void ProjectFile::writeProjectReferences(wofstream &file,const vector<Project*> &allProjects)
{
  size_t
    index;

  wstring
    projectName,
    projectFileName;

  file << "  <ItemGroup>" << endl;

  foreach (wstring,dep,_dependencies)
  {
    projectName=*dep;
    projectFileName=L"";
    index=(*dep).find(L">");
    if (index != -1)
    {
      projectName=(*dep).substr(0,index);
      projectFileName=(*dep).substr(index+1);
    }

    foreach_const (Project*,depp,allProjects)
    {
      if ((*depp)->name() != projectName)
        continue;

      foreach (ProjectFile*,deppf,(*depp)->files())
      {
        if (projectFileName != L"" && (*deppf)->_name != projectFileName)
          continue;

        file << "    <ProjectReference Include=\"..\\" << (*deppf)-> _project->name() << "\\" << (*deppf)->_fileName << "\">" << endl;
        file << "      <Project>{" << (*deppf)->guid() << "}</Project>" << endl;
        file << "      <ReferenceOutputAssembly>false</ReferenceOutputAssembly>" << endl;
        file << "    </ProjectReference>" << endl;
      }
    }
  }

  file << "  </ItemGroup>" << endl;
}
