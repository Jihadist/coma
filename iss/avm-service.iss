; -- avm-service.iss --

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

#define Name "���-������"
#define GroupName "���-������"
#define EngName "���-������"
#define Version "0.0.41"
#define Publisher "EvelSoft"
#define URL "http://www.avmenergo.ru"
#define ExeName "AVM-Service.exe"
#define SetupName "AVM-Service-0.0.41-x86"
#define Prefix "D:\Progs\out"

[CustomMessages]
Version={#Version}

[Languages]
Name: Russian; MessagesFile: "compiler:Languages\Russian.isl" 

[InstallDelete]
Type: files; Name: "{app}\*.dll"
Type: filesandordirs; Name: "{app}\platforms"
Type: files; Name: "{app}\{#ExeName}"
; Type: files; Name: "{app}\���� ����������� ������������.pdf"

[Setup]
UsePreviousLanguage=No
AppName={#Name}
AppVersion={#Version}
AppPublisher={#Publisher}
AppPublisherURL={#URL}
AppSupportURL={#URL}
AppUpdatesURL={#URL}
DefaultDirName={cf}\{#EngName}
DefaultGroupName={#GroupName}
; UninstallDisplayIcon={app}\MyProg.exe
Compression=lzma2
SolidCompression=yes
OutputDir="{#Prefix}\out\"
LicenseFile="{#Prefix}\license.txt"

OutputBaseFilename={#SetupName}

[Dirs]
Name: {userappdata}\{#EngName}

[Files]
Source: "{#Prefix}\src x86\Qt5Core.dll"; DestDir: "{app}"
Source: "{#Prefix}\src x86\Qt5Gui.dll"; DestDir: "{app}"
Source: "{#Prefix}\src x86\Qt5Network.dll"; DestDir: "{app}"
Source: "{#Prefix}\src x86\Qt5SerialPort.dll"; DestDir: "{app}"
Source: "{#Prefix}\src x86\Qt5Widgets.dll"; DestDir: "{app}"
Source: "{#Prefix}\src x86\platforms\qwindows.dll"; DestDir: "{app}\platforms"
Source: "{#Prefix}\3rdparty-x86\Qt5Xlsx.dll"; DestDir: "{app}"
Source: "{#Prefix}\3rdparty-x86\hidapi.dll"; DestDir: "{app}"
Source: "{#Prefix}\3rdparty-x86\liblzma.dll"; DestDir: "{app}"
Source: "{#Prefix}\3rdparty-x86\limereport.dll"; DestDir: "{app}"
Source: "{#Prefix}\3rdparty-x86\QtZint.dll"; DestDir: "{app}"
Source: "{#Prefix}\prg\{#ExeName}"; DestDir: "{app}"; DestName: {#ExeName}; Flags: ignoreversion
Source: "{#Prefix}\coma\ermsgs.dat"; DestDir: "{userappdata}\{#EngName}"; Flags: ignoreversion
Source: "{#Prefix}\coma\reports\*.*"; DestDir: "{userappdata}\{#EngName}"; Flags: ignoreversion
Source: "{#Prefix}\coma\images\*.*"; DestDir: "{app}\images"; Flags: ignoreversion
Source: "{#Prefix}\3rdparty-x86\vc_redist.x86.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall
; Source: "{#Prefix}\coma\readme.txt"; DestDir: "{app}"

[Icons]
Name: "{group}\{#Name}"; Filename: "{app}\{#ExeName}"
;Name: "{group}\����������� ������������ ����"; Filename: "{app}\���� ����������� ������������.pdf"
Name: "{group}\������� ��������� {#Name}"; Filename: "{uninstallexe}"

[Run]
; add the Parameters, WorkingDir and StatusMsg as you wish, just keep here
; the conditional installation Check
Filename: "{tmp}\vc_redist.x86.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: ��������������� ����� MSVC2017 Redistributable...