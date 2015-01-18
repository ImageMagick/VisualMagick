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
#ifndef __Project__
#define __Project__

#include "ConfigureWizard.h"
#include "ProjectFile.h"

class Project
{
public:

  string configDefine() const;

  vector<string> &defines();

  vector<string> &dependencies();

  vector<string> &directories();

  vector<string> &excludes();

  vector<string> &excludesX64();

  vector<string> &excludesX86();

  vector<ProjectFile*> &files();

  vector<string> &includes();

  bool isConsole() const;

  bool isCom() const;

  bool isDll() const;

  bool isExe() const;

  bool isLib() const;

  bool isModule() const;

  vector<string> &libraries();

  string moduleDefinitionFile() const;

  string name() const;

  vector<string> &references();

  int warningLevel() const;

  static Project* create(string name);

  bool loadFiles(const ConfigureWizard &wizard);

private:
  Project(string name);

  void addLines(ifstream &config,string &value);

  void addLines(ifstream &config,vector<string> &container);

  void loadConfig(ifstream &config);

  void loadModules(const ConfigureWizard &wizard);

  string               _configDefine;
  vector<string>       _defines;
  vector<string>       _dependencies;
  vector<string>       _directories;
  vector<string>       _excludes;
  vector<string>       _excludesX64;
  vector<string>       _excludesX86;
  vector<ProjectFile*> _files;
  bool                 _hasIncompatibleLicense;
  vector<string>       _includes;
  bool                 _isEnabled;
  bool                 _isOptional;
  vector<string>       _libraries;
  string               _moduleDefinitionFile;
  string               _modulePrefix;
  string               _name;
  vector<string>       _references;
  int                  _type;
  int                  _warningLevel;
};

#endif // __Project__