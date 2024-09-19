/* $Id$ */

#define SOURCE

#include "../config.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>

#ifndef NO_SSL
#include <openssl/opensslv.h>
#endif

#include <cm_log.h>
#include <cm_string.h>

#include "tw_config.h"
#include "tw_server.h"
#include "tw_version.h"

#ifdef __MINGW32__
#include <windows.h>
#endif

extern bool cm_do_log;
extern struct tw_config config;
extern FILE* logfile;

char tw_server[2048];

int startup(int argc, char** argv);

#ifdef SERVICE
SERVICE_STATUS status;
SERVICE_STATUS_HANDLE status_handle;

void WINAPI servhandler(DWORD control) {
	switch(control) {
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
		status.dwCurrentState = SERVICE_STOP_PENDING;
		break;
	}
	SetServiceStatus(status_handle, &status);
}

void WINAPI servmain(DWORD argc, LPSTR* argv) {
	logfile = fopen(PREFIX "/logs/tewi.log", "a");
	if(logfile == NULL) logfile = stderr;
	status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	status.dwCurrentState = SERVICE_START_PENDING;
	status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	status.dwWin32ExitCode = NO_ERROR;
	status.dwServiceSpecificExitCode = 0;
	status.dwCheckPoint = 0;
	status.dwWaitHint = 0;
	status_handle = RegisterServiceCtrlHandler("Tewi HTTPd", servhandler);
	if(status_handle == NULL) return;
	if(SetServiceStatus(status_handle, &status) == 0) return;
	int st = startup(argc, argv);
	if(st != -1) {
		status.dwWin32ExitCode = NO_ERROR;
		status.dwServiceSpecificExitCode = st;
		status.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(status_handle, &status);
		return;
	}
	status.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(status_handle, &status);
	tw_server_loop();
	status.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(status_handle, &status);
}
#endif

int main(int argc, char** argv) {
	logfile = stderr;
#ifdef SERVICE
	SERVICE_TABLE_ENTRY table[] = {{"Tewi HTTPd", servmain}, {NULL, NULL}};
	StartServiceCtrlDispatcher(table);
#else
	int st = startup(argc, argv);
	if(st != -1) return st;
	tw_server_loop();
#endif
}

int startup(int argc, char** argv) {
	int i;
	const char* confpath = PREFIX "/etc/tewi.conf";
	if(argv != NULL) {
		for(i = 1; i < argc; i++) {
			if(argv[i][0] == '-') {
				if(strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
					if(!cm_do_log) {
						cm_do_log = true;
#ifndef NO_SSL
						cm_log("", "This is Tewi HTTPd, version %s, using %s", tw_get_version(), OPENSSL_VERSION_TEXT);
#else
						cm_log("", "This is Tewi HTTPd, version %s", tw_get_version());
#endif
					} else {
						cm_do_log = true;
					}
				} else if(strcmp(argv[i], "--config") == 0 || strcmp(argv[i], "-C") == 0) {
					i++;
					if(argv[i] == NULL) {
						fprintf(stderr, "Missing argument\n");
						return 1;
					}
					confpath = argv[i];
				} else if(strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-V") == 0) {
					printf("Tewi HTTPd Tewi/%s\n", tw_get_version());
					printf("Under public domain.\n");
					printf("Original by 2024 Nishi\n");
					printf("\n");
					printf("Usage: %s [--config|-C config] [--verbose|-v] [--version|-V]\n", argv[0]);
					printf("--config  | -C config     : Specify config\n");
					printf("--verbose | -v            : Verbose mode\n");
					printf("--version | -V            : Version information\n");
					return 0;
				} else {
					fprintf(stderr, "Unknown option: %s\n", argv[i]);
					return 1;
				}
			}
		}
	}
	tw_config_init();
	if(tw_config_read(confpath) != 0) {
		fprintf(stderr, "Could not read the config\n");
		return 1;
	}
	if(tw_server_init() != 0) {
		fprintf(stderr, "Could not initialize the server\n");
		return 1;
	}
	sprintf(tw_server, "Tewi/%s (%s)%s", tw_get_version(), tw_get_platform(), config.extension == NULL ? "" : config.extension);
	char* r = cm_strcat(tw_server, " running...");
	cm_force_log(r);
	free(r);
#ifndef __MINGW32__
	signal(SIGCHLD, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
#ifdef __HAIKU__
	signal(5, SIG_IGN);
#endif
#else
	SetConsoleTitle(tw_server);
#endif
	return -1;
}
