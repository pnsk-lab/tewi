# $Id$

Name "Tewi HTTPd"
OutFile "install.exe"
InstallDir "C:\Tewi"
Icon "tewi.ico"
LicenseData ../LICENSE
Page license
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
	SetOutPath "$INSTDIR\bin"
	File "tewi.exe"
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

Section "Uninstall"
	RMDir /r "$INSTDIR"
	RMDir /r "$SMPROGRAMS\Tewi HTTPd"

	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Tewi HTTPd"
SectionEnd
