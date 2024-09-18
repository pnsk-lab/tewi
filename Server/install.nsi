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
	CreateDirectory "$INSTDIR\bin"
	SetOutPath "$INSTDIR\bin"
	File "tewi.exe"
	SetOutPath "$INSTDIR\etc"
	File /oname=tewi.conf "../example-win.conf"
	SetOutPath "$INSTDIR\www"
	File /oname=index.html "../itworks.html"

	CreateDirectory "$SMPROGRAMS\Tewi HTTPd"
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
