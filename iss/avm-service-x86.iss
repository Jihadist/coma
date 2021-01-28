; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define Name "���-������"
#define GroupName "���-������"
#define EngName "AVM-Service"
#define Publisher "EvelSoft"
#define URL "http://www.avmenergo.ru"
#define ExeName "AVM-Service.exe"
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;#define Prefix "C:\Progs\out"
#define ARCH "win32"
#define QT_DIR "C:\Qt\5.14.2\msvc2017\bin"
;#define Redist_DIR "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Redist\MSVC\v142"
#define Redist_DIR "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.16.27012"
#define LIB_DIR "..\lib"
#define Build_DIR  "..\bin"
#define ApplicationVersion GetFileVersion('..\bin\AVM-Service\win32\release\AVM-Service.exe')



;[CustomMessages]
;Version={#Version}

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{4C4A32F2-8733-4C05-AF66-3996C08A228A}
AppName={#GroupName}
AppVerName={#EngName} {#ApplicationVersion}
;AppVersion={#AppVersion}
;VersionInfoVersion={#AppVersion}
;AppVersion= {#GetStringFileInfo("{#Build_DIR}\{#ARCH}\release\{#ExeName}", "FileVersion")}
AppPublisher={#Publisher}
AppPublisherURL={#URL}
AppSupportURL={#URL}
AppUpdatesURL={#URL}
;DefaultDirName={autopf}\{#GroupName}
;DisableProgramGroupPage=yes
DefaultDirName={commonpf}\{#EngName}
DefaultGroupName={#EngName}
; The [Icons] "quicklaunchicon" entry uses {userappdata} but its [Tasks] entry has a proper IsAdminInstallMode Check.
UsedUserAreasWarning=no
; Uncomment the following line to run in non administrative install mode (install for current user only.)
;PrivilegesRequired=lowest
SetupIconFile=..\coma.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern
OutputBaseFilename={#EngName}-{#ApplicationVersion}-x86
OutputDir=..\output
VersionInfoVersion={#ApplicationVersion}
LicenseFile="..\license.txt"
InfoAfterFile="..\AVM-Service.NOTES"

[Dirs]
Name: {userappdata}\{#EngName}


[Languages]
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 6.1; Check: not IsAdminInstallMode

[Files]
Source: "{#Build_DIR}\{#EngName}\{#ARCH}\release\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs
Source: "{#LIB_DIR}\{#ARCH}\release\*.dll"; DestDir: "{app}"; 
Source: "{#Redist_DIR}\vc_redist.x86.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall
; NOTE: Don't use "Flags: ignoreversion" on any shared system files


[Icons]
Name: "{group}\{#Name}"; Filename: "{app}\{#ExeName}"
;Name: "{group}\����������� ������������ ����"; Filename: "{app}\���� ����������� ������������.pdf"
Name: "{group}\������� ��������� {#Name}"; Filename: "{uninstallexe}"

[Run]
Filename: "{tmp}\vc_redist.x86.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: ��������������� ����� MSVC2017 Redistributable...