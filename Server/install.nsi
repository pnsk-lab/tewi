# $Id$

Name "Tewi HTTPd"
OutFile "install.exe"
InstallDir "C:\Tewi"
Icon "tewi.ico"
LicenseData ../LICENSE

!include "LogicLib.nsh"
!include "Sections.nsh"

Page license
Page components
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles
Section
	CreateDirectory "$INSTDIR\etc"
	CreateDirectory "$INSTDIR\www"
	CreateDirectory "$INSTDIR\www\icons"
	CreateDirectory "$INSTDIR\modules"
	CreateDirectory "$INSTDIR\bin"
	SetOutPath "$INSTDIR"
	File /oname=LICENSE.txt "../LICENSE"
	SetOutPath "$INSTDIR\modules"
	File "../Module/*.dll"
	SetOutPath "$INSTDIR\etc"
	SetOverWrite off
	File /oname=tewi.conf "../example-win.conf"
	SetOutPath "$INSTDIR\www"
	File /oname=index.html "../itworks.html"
	SetOutPath "$INSTDIR\www\icons"
	File "../Icons/*.png"
	SetOverWrite on

	CreateDirectory "$SMPROGRAMS\Tewi HTTPd"
	CreateShortcut "$SMPROGRAMS\Tewi HTTPd\License.lnk" "$INSTDIR\LICENSE.txt" ""
	CreateShortcut "$SMPROGRAMS\Tewi HTTPd\Start Tewi HTTPd.lnk" "$INSTDIR\bin\tewi.exe" ""
	CreateShortcut "$SMPROGRAMS\Tewi HTTPd\Start Tewi HTTPd (verbose).lnk" "$INSTDIR\bin\tewi.exe" "-v"
	CreateShortcut "$SMPROGRAMS\Tewi HTTPd\Uninstall Tewi HTTPd.lnk" "$INSTDIR\uninstall.exe" ""

	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Tewi HTTPd" "DisplayName" "Tewi HTTPd"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Tewi HTTPd" "UninstallString" '"$INSTDIR\uninstall.exe"'

	WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

Section "Install the executable only" SEL_EXEC
	SetOutPath "$INSTDIR\bin"
	File "../tewi.exe"
	WriteINIStr $INSTDIR\install.ini uninstall service false
SectionEnd

Section /o "Install the service too (NT-only)" SEL_SERVICE
	WriteINIStr $INSTDIR\install.ini uninstall service true
	FileOpen $9 $INSTDIR\install.bat w
	FileWrite $9 '"$SYSDIR\sc.exe" stop "TewiHTTPd"$\r$\n'
	FileClose $9
	nsExec::Exec '"$INSTDIR\install.bat"'
	Pop $0
	DetailPrint "Waiting for 1s so service can stop..."
	Sleep 1000
	CreateDirectory "$INSTDIR\logs"
	SetOutPath "$INSTDIR\bin"
	File "../tewi.exe"
	File /oname=tewisrv.exe "../tewi-service.exe"
	FileOpen $9 $INSTDIR\install.bat w
	FileWrite $9 '"$SYSDIR\sc.exe" delete "TewiHTTPd"$\r$\n'
	FileWrite $9 '"$SYSDIR\sc.exe" create "TewiHTTPd" DisplayName= "Tewi HTTPd" binpath= "$INSTDIR\bin\tewisrv.exe" start= "auto"$\r$\n'
	FileWrite $9 '"$SYSDIR\sc.exe" start "TewiHTTPd"$\r$\n'
	FileClose $9
	nsExec::Exec '"$INSTDIR\install.bat"'
	Pop $0
	Delete $INSTDIR\install.bat
SectionEnd

Function .onInit
	StrCpy $1 ${SEL_EXEC}
FunctionEnd

Function .onSelChange
	!insertmacro StartRadioButtons $1
	!insertmacro RadioButton ${SEL_EXEC}
	!insertmacro RadioButton ${SEL_SERVICE}
	!insertmacro EndRadioButtons
FunctionEnd

Section "Uninstall"
	ReadINIStr $8 $INSTDIR\install.ini uninstall service
	${If} $8 == "true"
		FileOpen $9 $INSTDIR\uninstall.bat w
		FileWrite $9 '"$SYSDIR\sc.exe" stop "TewiHTTPd"$\r$\n'
		FileClose $9
		nsExec::Exec '"$INSTDIR\uninstall.bat"'
		Pop $0
		FileOpen $9 $INSTDIR\uninstall.bat w
		DetailPrint "Waiting for 1s so service can stop..."
		Sleep 1000
		FileWrite $9 '"$SYSDIR\sc.exe" delete "TewiHTTPd"$\r$\n'
		FileClose $9
		nsExec::Exec '"$INSTDIR\uninstall.bat"'
		Pop $0
		Delete $INSTDIR\uninstall.bat
	${EndIf}

	RMDir /r "$INSTDIR"
	RMDir /r "$SMPROGRAMS\Tewi HTTPd"

	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Tewi HTTPd"
SectionEnd
