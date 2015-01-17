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
  ProjectFile(const ConfigureWizard *wizard,Project *project,const string &prefix,const string &name);

  vector<string> &dependencies();

  string fileName() const;

  string guid() const;

  string name() const;

  void loadConfig();

  void write(const vector<Project*> &allProjects);

private:

  bool isLib() const;

  void addLines(ifstream &config,vector<string> &container);

  string getIntermediateDirectoryName(const bool debug);

  string getTargetName(const bool debug);

  void loadConfig(const string &fileName);

  void loadModule(const string &directory);

  void loadSource();

  void loadSource(const string &directory);

  void setFileName();

  void setGuid();

  void writeAdditionalDependencies(ofstream &file,const string &separator);

  void writeAdditionalIncludeDirectories(ofstream &file,const string &separator);

  void writePreprocessorDefinitions(ofstream &file,const bool debug);

  void writeVS2002(ofstream &file);

  void writeVS2002Configuration(ofstream &file,const bool debug);

  void writeVS2002Files(ofstream &file,string name,const vector<string> &collection);

  void writeVS2010_2012(ofstream &file,const vector<Project*> &allProjects);

  void writeVS2010_2012ItemDefinitionGroup(ofstream &file,const bool debug);

  void writeVS2010_2012Files(ofstream &file,const vector<string> &collection);

  void writeVS2010_2012ProjectReferences(ofstream &file,const vector<Project*> &allProjects);

  string                 _fileName;
  string                 _guid;
  vector<string>         _cppFiles;
  vector<string>         _dependencies;
  vector<string>         _includeFiles;
  vector<string>         _includes;
  string                 _name;
  string                 _prefix;
  Project               *_project;
  vector<string>         _resourceFiles;
  vector<string>         _srcFiles;
  const ConfigureWizard *_wizard;
};

#endif // __ProjectFile__