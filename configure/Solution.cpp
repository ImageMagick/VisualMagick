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
#include "Solution.h"
#include "Shared.h"

Solution::Solution()
{
}

int Solution::loadProjectFiles(const ConfigureWizard &wizard)
{
  int
    count;

  count=0;
  foreach (Project*,p,_projects)
  {
    if (!(*p)->loadFiles(wizard))
      continue;

    foreach (ProjectFile*,pf,(*p)->files())
    {
      (*pf)->loadConfig();
      count++;
    }
  }

  return(count);
}

void Solution::loadProjects()
{
  HANDLE
    fileHandle;

  WIN32_FIND_DATA
    data;

  fileHandle=FindFirstFile("..\\*.*",&data);
  do
  {
    if (fileHandle == INVALID_HANDLE_VALUE)
      return;

    if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
      continue;

    if (_stricmp(data.cFileName,".") == 0)
      continue;

    if (_stricmp(data.cFileName,"..") == 0)
      continue;

     Project* project = Project::create(data.cFileName);
     if (project != (Project *) NULL)
       _projects.push_back(project);
  } while (FindNextFile(fileHandle,&data));

  FindClose(fileHandle);
}

void Solution::write(const ConfigureWizard &wizard,WaitDialog &waitDialog)
{
  int
    steps;

  ofstream
    file;

  steps=loadProjectFiles(wizard);
  waitDialog.setSteps(steps+2); /* write solution and config */

  file.open(getFileName(wizard));
  if (!file)
    return;

  waitDialog.nextStep("Writing solution");

  write(wizard,file);

  file.close();

  foreach (Project*,p,_projects)
  {
    foreach (ProjectFile*,pf,(*p)->files())
    {
      waitDialog.nextStep("Writing: " + (*pf)->fileName());
      (*pf)->write(_projects);
    }
  }

  waitDialog.nextStep("Writing configuration");

  writeBaseConfig(wizard);
}

string Solution::getFileName(const ConfigureWizard &wizard)
{
  string
    fileName;

  fileName="..\\Visual" + wizard.solutionName();

  return(fileName+".sln");
}

void Solution::write(const ConfigureWizard &wizard,ofstream &file)
{
  size_t
    i;

  if (wizard.visualStudioVersion() == VS2002)
    file << "Microsoft Visual Studio Solution File, Format Version 7.00" << endl;
  else if (wizard.visualStudioVersion() == VS2010)
  {
    file << "Microsoft Visual Studio Solution File, Format Version 11.00" << endl;
    file << "# Visual Studio 2010" << endl;
  }
  else if (wizard.visualStudioVersion() == VS2012)
  {
    file << "Microsoft Visual Studio Solution File, Format Version 12.00" << endl;
    file << "# Visual Studio 2012" << endl;
  }

  foreach (Project*,p,_projects)
  {
    foreach (ProjectFile*,pf,(*p)->files())
    {
      file << "Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"" << (*pf)->name() << "\", ";
      file << "\".\\" << (*p)->name() << "\\" << (*pf)->fileName() << "\", \"{" << (*pf)->guid() << "}\"" << endl;
      file << "EndProject" << endl;
    }
  }
  file << "EndProject" << endl;

  file << "Global" << endl;
  if (wizard.visualStudioVersion() == VS2002)
  {
    file << "\tGlobalSection(SolutionConfiguration) = preSolution" << endl;
    file << "\t\tConfigName.0 = Debug" << endl;
    file << "\t\tConfigName.1 = Release" << endl;
  }
  else
  {
    file << "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution" << endl;
    file << "\t\tDebug|" << wizard.platform() << " = Debug|" << wizard.platform() << endl;
    file << "\t\tRelease|" << wizard.platform() << " = Release|" << wizard.platform() << endl;
  }
  file << "\tEndGlobalSection" << endl;

  if (wizard.visualStudioVersion() == VS2002)
  {
    file << "\tGlobalSection(ProjectDependencies) = postSolution" << endl;
    foreach (Project*,p,_projects)
    {
      foreach (ProjectFile*,pf,(*p)->files())
      {
        i=0;
        foreach (string,dep,(*pf)->dependencies())
        {
          foreach (Project*,depp,_projects)
          {
            if ((*depp)->name() != *dep)
              continue;

            foreach (ProjectFile*,deppf,(*depp)->files())
            {
              file << "\t\t{" << (*pf)->guid() << "}." << i++ << " = {" << (*deppf)->guid() << "}" << endl;
            }
          }
        }
      }
    }
    file << "\tEndGlobalSection" << endl;
  }

  if (wizard.visualStudioVersion() == VS2002)
    file << "\tGlobalSection(ProjectConfiguration) = postSolution" << endl;
  else
    file << "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution" << endl;
  foreach (Project*,p,_projects)
  {
    foreach (ProjectFile*,pf,(*p)->files())
    {
      if (wizard.visualStudioVersion() == VS2002)
      {
        file << "\t\t{" << (*pf)->guid() << "}.Debug.ActiveCfg = Debug|" << wizard.platform() << endl;
        file << "\t\t{" << (*pf)->guid() << "}.Debug.Build.0 = Debug|" << wizard.platform() << endl;
        file << "\t\t{" << (*pf)->guid() << "}.Release.ActiveCfg = Release|" << wizard.platform() << endl;
        file << "\t\t{" << (*pf)->guid() << "}.Release.Build.0 = Release|" << wizard.platform() << endl;
      }
      else
      {
        file << "\t\t{" << (*pf)->guid() << "}.Debug|" << wizard.platform() << ".ActiveCfg = Debug|" << wizard.platform() << endl;
        file << "\t\t{" << (*pf)->guid() << "}.Debug|" << wizard.platform() << ".Build.0 = Debug|" << wizard.platform() << endl;
        file << "\t\t{" << (*pf)->guid() << "}.Release|" << wizard.platform() << ".ActiveCfg = Release|" << wizard.platform() << endl;
        file << "\t\t{" << (*pf)->guid() << "}.Release|" << wizard.platform() << ".Build.0 = Release|" << wizard.platform() << endl;
      }
    }
  }
  file << "\tEndGlobalSection" << endl;
  file << "EndGlobal" << endl;
}

void Solution::writeBaseConfig(const ConfigureWizard &wizard)
{
  string
    folder,
    line;

  ifstream
    configIn;

  ofstream
    config;

  folder="MagickCore";
  if (!PathFileExists(("..\\" + folder).c_str()))
    folder="Magick";

  configIn.open("..\\" + folder + "\\magick-baseconfig.h.in");
  if (!configIn)
    return;

  config.open("..\\..\\" + folder + "\\magick-baseconfig.h");
  if (!config)
    return;

  while (getline(configIn,line))
  {
    if (trim(line).compare("$$CONFIG$$") != 0)
    {
      config << line << endl;
      continue;
    }

    config << "/*" << endl;
    config << "  Specify size of PixelPacket color Quantums (8, 16, or 32)." << endl;
    config << "  A value of 8 uses half the memory than 16 and typically runs 30% faster," << endl;
    config << "  but provides 256 times less color resolution than a value of 16." << endl;
    config << "*/" << endl;
    if (wizard.quantumDepth() == Q8)
      config << "#define MAGICKCORE_QUANTUM_DEPTH 8" << endl;
    else if (wizard.quantumDepth() == Q16)
      config << "#define MAGICKCORE_QUANTUM_DEPTH 16" << endl;
    else if (wizard.quantumDepth() == Q32)
      config << "#define MAGICKCORE_QUANTUM_DEPTH 32" << endl;
    else if (wizard.quantumDepth() == Q64)
      config << "#define MAGICKCORE_QUANTUM_DEPTH 64" << endl;
    config << endl;

    config << "/*" << endl;
    config << "  Define to enable high dynamic range imagery (HDRI)" << endl;
    config << "*/" << endl;
    if (wizard.useHDRI())
      config << "#define MAGICKCORE_HDRI_ENABLE 1" << endl;
    else
      config << "#define MAGICKCORE_HDRI_ENABLE 0" << endl;
    config << endl;

    config << "/*" << endl;
    config << "  Define to enable OpenCL" << endl;
    config << "*/" << endl;
    if (wizard.useOpenCL())
      config << "#define MAGICKCORE__OPENCL\n#define MAGICKCORE_HAVE_CL_CL_H" << endl;
    else
      config << "#undef MAGICKCORE__OPENCL\n#undef MAGICKCORE_HAVE_CL_CL_H" << endl;

    foreach (Project*,p,_projects)
    {
      if ((*p)->files().size() == 0)
        continue;

      if ((*p)->configDefine().empty())
        continue;

      config << endl;
      config << (*p)->configDefine();
    }
  }
}