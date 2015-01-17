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
#ifndef __Solution__
#define __Solution__

#include "Project.h"
#include "ConfigureWizard.h"
#include "WaitDialog.h"

class Solution
{
public:
  Solution();

  int loadProjectFiles(const ConfigureWizard &wizard);

  void loadProjects();

  void write(const ConfigureWizard &wizard,WaitDialog &waitDialog);

private:

  string getFileName(const ConfigureWizard &wizard);

  void writeBaseConfig(const ConfigureWizard &wizard);

  void write(const ConfigureWizard &wizard,ofstream &file);

  vector<Project*> _projects;
};

#endif // __Solution__