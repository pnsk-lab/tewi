$! $Id$
$ set def [.Common]
$ objects = ""
$ comobjs = ""
$ build_common:
$ 	file = f$search("*.c")
$ 	if file .eqs. "" then goto quit_server
$	write sys$output "Compiling "'file'
$	cc 'file'
$	if file .eqs. "" then
$		comobjs = "''file'"
$	else
$		comobjs = "''objects'+''file'"
$	endif
$ goto build_common
$ quit_common:
$ 	set def [-.Server]
$ build_server:
$ 	file = f$search("*.c")
$ 	if file .eqs. "" then goto quit_server
$	write sys$output "Compiling "'file'
$	cc 'file'
$	if file .eqs. "" then
$		objects = "''file'"
$	else
$		objects = "''objects'+''file'"
$	endif
$ goto build_server
$ quit_server:
$	link /executable=tewi.exe "''comobjs'+''objects'"
$	set def [-]
$ exit
