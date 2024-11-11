$! $Id$
$ set def [.Server]
$ build_server:
$ 	file = f$search("*.c")
$ 	if file .eqs. "" then goto quit_server
$	write sys$output "Compiling "'file'
$	cc 'file'
$ goto build_server
$ quit_server:
$	set def [-]
$ exit
