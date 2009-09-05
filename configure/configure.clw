; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CTargetPage
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "configure.h"
LastPage=0

ClassCount=6
Class1=CConfigureApp
Class2=CConfigureWizard
Class3=CFinishedPage
Class4=CSystemPage
Class5=CTargetPage
Class6=CWelcomePage

ResourceCount=5
Resource1=IDD_SYSTEM_PAGE
Resource2=IDD_WELCOME_PAGE
Resource3=IDD_TARGET_PAGE
Resource4=IDD_FINISHED_PAGE
Resource5=IDD_WAITDIALOG

[CLS:CConfigureApp]
Type=0
BaseClass=CWinApp
HeaderFile=configure.h
ImplementationFile=configure.cpp

[CLS:CConfigureWizard]
Type=0
BaseClass=CPropertySheet
HeaderFile=configure_wizard.h
ImplementationFile=configure_wizard.cpp

[CLS:CFinishedPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=finished_page.h
ImplementationFile=finished_page.cpp
Filter=D
VirtualFilter=idWC
LastObject=CFinishedPage

[CLS:CSystemPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=system_page.h
ImplementationFile=system_page.cpp
Filter=D
VirtualFilter=idWC

[CLS:CTargetPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=target_page.h
ImplementationFile=target_page.cpp
LastObject=IDC_INCLUDE_OPTIONAL
Filter=D
VirtualFilter=idWC

[CLS:CWelcomePage]
Type=0
BaseClass=CPropertyPage
HeaderFile=welcome_page.h
ImplementationFile=welcome_page.cpp

[DLG:IDD_FINISHED_PAGE]
Type=1
Class=CFinishedPage
ControlCount=2
Control1=IDC_FINISHED_TEXT,static,1342308352
Control2=IDC_BuildIt,button,1073807360

[DLG:IDD_SYSTEM_PAGE]
Type=1
Class=CSystemPage
ControlCount=16
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_TEMP_RELEASE,edit,1350631552
Control5=IDC_TEMP_RELEASE_BROWSE,button,1342242816
Control6=IDC_STATIC,static,1342308352
Control7=IDC_TEMP_DEBUG,edit,1350631552
Control8=IDC_TEMP_DEBUG_BROWSE,button,1342242816
Control9=IDC_STATIC,button,1342177287
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_OUTPUT_BIN,edit,1350631552
Control13=IDC_OUTPUT_BIN_BROWSE,button,1342242816
Control14=IDC_STATIC,static,1342308352
Control15=IDC_OUTPUT_LIB,edit,1350631552
Control16=IDC_OUTPUT_LIB_BROWSE,button,1342242816

[DLG:IDD_TARGET_PAGE]
Type=1
Class=CTargetPage
ControlCount=14
Control1=IDC_STATIC,button,1342177287
Control2=IDC_PROJECT_TYPE,button,1342308361
Control3=IDC_PROJECT_TYPE1,button,1342177289
Control4=IDC_PROJECT_TYPE2,button,1342177289
Control5=IDC_USE_X11_STUBS,button,1342242819
Control6=IDC_DECORATE_FILES,button,1342242819
Control7=IDC_STATIC,button,1342177287
Control8=IDC_PROJECT_TYPE3,button,1342177289
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_INCLUDE_OPTIONAL,button,1342242819
Control12=IDC_STANDALONE,button,1342242819
Control13=IDC_EditConfig,button,1342242816
Control14=IDC_GENERATE_FONTMAP,button,1342242819

[DLG:IDD_WELCOME_PAGE]
Type=1
Class=CWelcomePage
ControlCount=2
Control1=IDC_WELCOME_TEXT,static,1342308352
Control2=IDC_STATIC,static,1342177294

[DLG:IDD_WAITDIALOG]
Type=1
Class=?
ControlCount=3
Control1=IDC_MSGCTRL,static,1342308481
Control2=IDC_PROGRESSCTRL,msctls_progress32,1350565888
Control3=IDCANCEL,button,1342242816

