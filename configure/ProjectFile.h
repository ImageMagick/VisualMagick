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
#ifndef __ProjectFile__
#define __ProjectFile__

#include "ConfigureWizard.h"

class Project;

class ProjectFile
{
public:
  ProjectFile(const ConfigureWizard *wizard,Project *project,const wstring &prefix,const wstring &name);

  vector<wstring> &dependencies();

  wstring fileName() const;

  wstring guid() const;

  wstring name() const;

  void loadConfig();

  void write(const vector<Project*> &allProjects);

private:

  bool isLib() const;

  void addLines(wifstream &config,vector<wstring> &container);

  wstring getIntermediateDirectoryName(const bool debug);

  wstring getTargetName(const bool debug);

  void loadConfig(const wstring &fileName);

  void loadModule(const wstring &directory);

  void loadSource();

  void loadSource(const wstring &directory);

  void setFileName();

  void setGuid();

  void writeAdditionalDependencies(wofstream &file,const wstring &separator);

  void writeAdditionalIncludeDirectories(wofstream &file,const wstring &separator);

  void writePreprocessorDefinitions(wofstream &file,const bool debug);

  void writeVS2002(wofstream &file);

  void writeVS2002Configuration(wofstream &file,const bool debug);

  void writeVS2002Files(wofstream &file,wstring name,const vector<wstring> &collection);

  void writeVS2010_2012(wofstream &file,const vector<Project*> &allProjects);

  void writeVS2010_2012ItemDefinitionGroup(wofstream &file,const bool debug);

  void writeVS2010_2012Files(wofstream &file,const vector<wstring> &collection);

  void writeVS2010_2012ProjectReferences(wofstream &file,const vector<Project*> &allProjects);

  wstring                _fileName;
  wstring                _guid;
  vector<wstring>        _cppFiles;
  vector<wstring>        _dependencies;
  vector<wstring>        _includeFiles;
  vector<wstring>        _includes;
  wstring                _name;
  wstring                _prefix;
  Project               *_project;
  vector<wstring>        _resourceFiles;
  vector<wstring>        _srcFiles;
  const ConfigureWizard *_wizard;
};

#endif // __ProjectFile__