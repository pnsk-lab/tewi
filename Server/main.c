/* $Id$ */

#define SOURCE

#include "../config.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

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

#ifdef _PSP
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspsdk.h>
#include <psputility.h>
#include <pspctrl.h>
#include <pspnet_apctl.h>
#include <pspwlan.h>

PSP_MODULE_INFO("Tewi HTTPd", PSP_MODULE_USER, 1, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

#define printf(...) pspDebugScreenPrintf(__VA_ARGS__)
#define STDERR_LOG(...) pspDebugScreenPrintf(__VA_ARGS__)
#else
#define STDERR_LOG(...) fprintf(stderr, __VA_ARGS__)
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

int running = 1;
#ifdef _PSP

int psp_exit_callback(int arg1, int arg2, void* arg3) { running = 0; }

int psp_callback_thread(SceSize args, void* argp) {
	int cid;
	cid = sceKernelCreateCallback("Exit Call Back", psp_exit_callback, NULL);
	sceKernelRegisterExitCallback(cid);
	sceKernelSleepThreadCB();
	return 0;
}
#endif

int main(int argc, char** argv) {
	logfile = stderr;
#ifdef SERVICE
	SERVICE_TABLE_ENTRY table[] = {{"Tewi HTTPd", servmain}, {NULL, NULL}};
	StartServiceCtrlDispatcher(table);
#else
#ifdef _PSP
	pspDebugScreenInit();
	pspDebugScreenSetXY(0, 0);
	printf("PSP Bootstrap, Tewi/%s\n", tw_get_version());
	int thid = sceKernelCreateThread("update_thread", psp_callback_thread, 0x11, 0xfa0, 0, NULL);
	if(thid >= 0) {
		sceKernelStartThread(thid, 0, NULL);
	} else {
		printf("Failed to start thread\n");
		while(running) sceKernelDelayThread(50 * 1000);
		sceKernelExitGame();
	}
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
	sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);
	sceUtilityLoadNetModule(PSP_NET_MODULE_INET);
	if(pspSdkInetInit()) {
		printf("Could not init the network\n");
		while(running) sceKernelDelayThread(50 * 1000);
		sceKernelExitGame();
	} else {
		printf("Network initialization successful\n");
	}
	if(sceWlanGetSwitchState() != 1) {
		printf("Turn the Wi-Fi switch on\n");
		while(sceWlanGetSwitchState() != 1) {
			sceKernelDelayThread(1000 * 1000);
		}
	} else {
		printf("Wi-Fi is turned on\n");
	}
	int i;
	int choice[100];
	int incr = 0;
	int last = 0;
	int cur = 0;
	for(i = 1; i < 100; i++) {
		choice[i - 1] = 0;
		netData name;
		netData data;
		if(sceUtilityCheckNetParam(i) != 0) continue;
		choice[incr++] = i;
		pspDebugScreenSetXY(0, 1 + 3 + incr - 1);
		if(incr == 1) printf("> ");
		pspDebugScreenSetXY(2, 1 + 3 + incr - 1);
		sceUtilityGetNetParam(i, 0, &name);
		sceUtilityGetNetParam(i, 1, &data);
		printf("SSID=%s", data.asString);
		sceUtilityGetNetParam(i, 4, &data);
		if(data.asString[0]) {
			sceUtilityGetNetParam(i, 5, &data);
			printf(" IPADDR=%s\n", data.asString);
		} else {
			printf(" DHCP\n");
		}
	}
	int press = 0;
	while(1) {
		if(!running) {
			sceKernelExitGame();
		}
		SceCtrlData c;
		sceCtrlReadBufferPositive(&c, 1);
		press = 0;
		if(c.Buttons & PSP_CTRL_DOWN) {
			if(cur < incr - 1) {
				cur++;
			}
			press = 1;
		} else if(c.Buttons & PSP_CTRL_UP) {
			if(cur > 0) {
				cur--;
			}
			press = -1;
		} else if(c.Buttons & PSP_CTRL_START) {
			break;
		}
		if(last != cur) {
			pspDebugScreenSetXY(0, 1 + 3 + last);
			printf("  ");
			pspDebugScreenSetXY(0, 1 + 3 + cur);
			printf("> ");
			last = cur;
		}
		if(press != 0) {
			while(1) {
				SceCtrlData c;
				sceCtrlReadBufferPositive(&c, 1);
				if(press == 1) {
					if(!(c.Buttons & PSP_CTRL_DOWN)) break;
				} else if(press == -1) {
					if(!(c.Buttons & PSP_CTRL_UP)) break;
				}
			}
		}
	}
	pspDebugScreenSetXY(0, 1 + 3 + incr + 1);
	int err = sceNetApctlConnect(choice[cur]);
	if(err != 0) {
		printf("Apctl initialization failure\n");
		while(running) sceKernelDelayThread(50 * 1000);
		sceKernelExitGame();
	} else {
		printf("Apctl initialization successful\n");
	}
	printf("Apctl connecting\n");
	while(1) {
		int state;
		err = sceNetApctlGetState(&state);
		if(err != 0) {
			printf("Apctl getting status failure\n");
			while(running) sceKernelDelayThread(50 * 1000);
			sceKernelExitGame();
		}
		if(state == 4) {
			break;
		}
		sceKernelDelayThread(50 * 1000);
	}
	union SceNetApctlInfo info;
	if(sceNetApctlGetInfo(8, &info) != 0) {
		printf("Got an unknown IP\n");
		while(running) sceKernelDelayThread(50 * 1000);
		sceKernelExitGame();
	}
	printf("Connected, My IP is %s\n", info.ip);
#endif
	int st = startup(argc, argv);
	if(st != -1) {
#ifdef _PSP
		printf("Error code %d\n", st);
		while(running) sceKernelDelayThread(50 * 1000);
		sceKernelExitGame();
#else
		return st;
#endif
	}
	tw_server_loop();
#endif
#ifdef _PSP
	sceKernelExitGame();
#endif
	return 0;
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
						STDERR_LOG("Missing argument\n");
						return 1;
					}
					confpath = argv[i];
#ifndef _PSP
				} else if(strcmp(argv[i], "--logfile") == 0 || strcmp(argv[i], "-l") == 0) {
					i++;
					if(argv[i] == NULL) {
						STDERR_LOG("Missing argument\n");
						return 1;
					}
					if(logfile != NULL && logfile != stderr) {
						fclose(logfile);
					}
					logfile = fopen(argv[i], "a");
					if(logfile == NULL) {
						STDERR_LOG("Failed to open logfile\n");
						return 1;
					}
#endif
				} else if(strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-V") == 0) {
					printf("Tewi HTTPd Tewi/%s\n", tw_get_version());
					printf("Under public domain.\n");
					printf("Original by 2024 Nishi\n");
					printf("\n");
					printf("Usage: %s [--config|-C config] [--verbose|-v] [--version|-V]\n", argv[0]);
					printf("--config  | -C config      : Specify config\n");
#ifndef _PSP
					printf("--logfile | -l logfile     : Specify logfile\n");
#endif
					printf("--verbose | -v             : Verbose mode\n");
					printf("--version | -V             : Version information\n");
					return 0;
				} else {
					STDERR_LOG("Unknown option: %s\n", argv[i]);
					return 1;
				}
			}
		}
	}
	tw_config_init();
	if(tw_config_read(confpath) != 0) {
		STDERR_LOG("Could not read the config\n");
		return 1;
	}
	if(tw_server_init() != 0) {
		STDERR_LOG("Could not initialize the server\n");
		return 1;
	}
	sprintf(tw_server, "Tewi/%s (%s)%s", tw_get_version(), tw_get_platform(), config.extension == NULL ? "" : config.extension);
	char* r = cm_strcat(tw_server, " running...");
	cm_force_log(r);
	free(r);
#ifndef __MINGW32__
	signal(SIGCHLD, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
#else
	SetConsoleTitle(tw_server);
#endif
	return -1;
}
