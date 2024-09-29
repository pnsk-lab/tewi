/* $Id$ */

#define SOURCE

#include "../config.h"

#include <unistd.h>
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
#elif defined(__ps2sdk__)
#include <debug.h>
#include <sifrpc.h>

#define printf(...) scr_printf(__VA_ARGS__)
#define STDERR_LOG(...) scr_printf(__VA_ARGS__)
#elif defined(__PPU__)
#include <rsx/gcm_sys.h>
#include <rsx/rsx.h>
#include <sysutil/video.h>
#include <malloc.h>
#include <sys/thread.h>
#include <stdarg.h>
#include <png.h>

#define printf(...) tt_printf(__VA_ARGS__)
#define STDERR_LOG(...) tt_printf(__VA_ARGS__)
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

#ifdef __PPU__
uint32_t depth_pitch;
uint32_t depth_offset;
uint32_t* depth_buffer;

#define CB_SIZE 0x100000
#define HOST_SIZE (32 * 1024 * 1024)

struct rsx_buffer {
	int width, height, id;
	uint32_t* ptr;
	uint32_t offset;
};

void wait_rsx(gcmContextData* ctx, uint32_t label) {
	rsxSetWriteBackendLabel(ctx, GCM_INDEX_TYPE_32B, label);

	rsxFlushBuffer(ctx);

	while(*(uint32_t*)gcmGetLabelAddress(GCM_INDEX_TYPE_32B) != label) usleep(50);

	label++;
}

void wait_rsx_until_idle(gcmContextData* ctx) {
	uint32_t label = 1;
	rsxSetWriteBackendLabel(ctx, GCM_INDEX_TYPE_32B, label);
	rsxSetWaitLabel(ctx, GCM_INDEX_TYPE_32B, label);
	label++;
	wait_rsx(ctx, label);
}

void get_resolution(int* width, int* height) {
	videoState state;
	videoResolution res;
	if(videoGetState(0, 0, &state) != 0) {
		return;
	}

	if(state.state != 0) {
		return;
	}

	if(videoGetResolution(state.displayMode.resolution, &res) != 0) {
		return;
	}
	*width = res.width;
	*height = res.height;
}

void make_buffer(struct rsx_buffer* buffer, int id) {
	int w, h;
	get_resolution(&w, &h);

	buffer->ptr = (uint32_t*)rsxMemalign(64, 4 * w * h);
	if(buffer->ptr == NULL) return;

	if(rsxAddressToOffset(buffer->ptr, &buffer->offset) != 0) return;

	if(gcmSetDisplayBuffer(id, buffer->offset, 4 * w, w, h) != 0) return;
	buffer->width = w;
	buffer->height = h;
	buffer->id = id;
}

gcmContextData* init_screen(void) {
	void* host = memalign(1024 * 1024, HOST_SIZE);
	gcmContextData* ctx = NULL;
	videoState state;
	videoConfiguration vconfig;
	videoResolution res;
	rsxInit(&ctx, CB_SIZE, HOST_SIZE, host);
	if(ctx == NULL) {
		free(host);
		return NULL;
	}

	if(videoGetState(0, 0, &state) != 0) {
		rsxFinish(ctx, 0);
		free(host);
		return NULL;
	}

	if(state.state != 0) {
		rsxFinish(ctx, 0);
		free(host);
		return NULL;
	}

	if(videoGetResolution(state.displayMode.resolution, &res) != 0) {
		rsxFinish(ctx, 0);
		free(host);
		return NULL;
	}

	memset(&vconfig, 0, sizeof(vconfig));
	vconfig.resolution = state.displayMode.resolution;
	vconfig.format = VIDEO_BUFFER_FORMAT_XRGB;
	vconfig.pitch = res.width * 4;
	vconfig.aspect = state.displayMode.aspect;

	wait_rsx_until_idle(ctx);

	if(videoConfigure(0, &vconfig, NULL, 0) != 0) {
		rsxFinish(ctx, 0);
		free(host);
		return NULL;
	}

	if(videoGetState(0, 0, &state) != 0) {
		rsxFinish(ctx, 0);
		free(host);
		return NULL;
	}
	gcmSetFlipMode(GCM_FLIP_VSYNC);

	depth_pitch = res.width * 4;
	depth_buffer = (uint32_t*)rsxMemalign(64, (res.height * depth_pitch) * 2);
	rsxAddressToOffset(depth_buffer, &depth_offset);

	gcmResetFlipStatus();

	return ctx;
}

void set_render_target(gcmContextData* context, struct rsx_buffer* buffer) {
	gcmSurface sf;

	sf.colorFormat = GCM_SURFACE_X8R8G8B8;
	sf.colorTarget = GCM_SURFACE_TARGET_0;
	sf.colorLocation[0] = GCM_LOCATION_RSX;
	sf.colorOffset[0] = buffer->offset;
	sf.colorPitch[0] = depth_pitch;

	sf.colorLocation[1] = GCM_LOCATION_RSX;
	sf.colorLocation[2] = GCM_LOCATION_RSX;
	sf.colorLocation[3] = GCM_LOCATION_RSX;
	sf.colorOffset[1] = 0;
	sf.colorOffset[2] = 0;
	sf.colorOffset[3] = 0;
	sf.colorPitch[1] = 64;
	sf.colorPitch[2] = 64;
	sf.colorPitch[3] = 64;

	sf.depthFormat = GCM_SURFACE_ZETA_Z16;
	sf.depthLocation = GCM_LOCATION_RSX;
	sf.depthOffset = depth_offset;
	sf.depthPitch = depth_pitch;

	sf.type = GCM_TEXTURE_LINEAR;
	sf.antiAlias = GCM_SURFACE_CENTER_1;

	sf.width = buffer->width;
	sf.height = buffer->height;
	sf.x = 0;
	sf.y = 0;

	rsxSetSurface(context, &sf);
}

void wait_flip(void) {
	while(gcmGetFlipStatus() != 0) usleep(200);
	gcmResetFlipStatus();
}

void flip(gcmContextData* ctx, uint32_t buffer) {
	if(gcmSetFlip(ctx, buffer) == 0) {
		rsxFlushBuffer(ctx);
		gcmSetWaitFlip(ctx);
	}
}

uint8_t* tvram;

extern uint8_t font[];

int tt_x = 0;
int tt_y = 0;
int tt_width;
int tt_height;

void tt_putstr(const char* str) {
	int i;
	for(i = 0; str[i] != 0; i++) {
		tvram[tt_y * tt_width + tt_x] = str[i];
		if(str[i] == '\n') {
			tt_x = 0;
			tt_y++;
		} else {
			tt_x++;
			if(tt_x == tt_width) {
				tt_x = 0;
				tt_y++;
			}
		}
		if(tt_y == tt_height) {
			tt_y--;
			int x, y;
			for(y = 0; y < tt_height - 1; y++) {
				for(x = 0; x < tt_width; x++) {
					tvram[y * tt_width + x] = tvram[(y + 1) * tt_width + x];
				}
			}
			for(x = 0; x < tt_width; x++) {
				tvram[(tt_height - 1) * tt_width + x] = 0x20;
			}
		}
	}
}

void tt_putchar(struct rsx_buffer* buffer, int x, int y, uint8_t c) {
	int i, j;
	if(c == 0) return;
	if(c < 0x20) c = 0x20;
	if(c >= 0x7f) c = 0x20;
	for(i = 0; i < 8; i++) {
		uint8_t l = i == 7 ? 0 : font[(c - 0x20) * 8 + i];
		for(j = 0; j < 6; j++) {
			uint32_t col = 0;
			if(l & (1 << 7)) {
				col = 0xffffff;
			}
			l = l << 1;
			buffer->ptr[(y * 8 + i) * buffer->width + x * 6 + j] = col;
		}
	}
}

void draw(struct rsx_buffer* buffer, int current) {
	int i, j, c;
	for(i = 0; i < buffer->height / 8; i++) {
		for(j = 0; j < buffer->width / 6; j++) {
			uint8_t c = tvram[i * (buffer->width / 6) + j];
			tt_putchar(buffer, j, i, c);
		}
	}
}

#define BUFFERS 1
gcmContextData* ctx;
struct rsx_buffer buffers[BUFFERS];

void text_thread(void* arg) {
	int current = 0;
	while(1) {
		wait_flip();
		draw(&buffers[current], current);
		flip(ctx, buffers[current].id);
		current++;
		if(current >= BUFFERS) current = 0;
	}
}

void tt_printf(const char* tmpl, ...) {
	va_list va;
	va_start(va, tmpl);
	int i;
	char cbuf[2];
	cbuf[1] = 0;
	char* log = cm_strdup("");
	for(i = 0; tmpl[i] != 0; i++) {
		if(tmpl[i] == '%') {
			i++;
			if(tmpl[i] == 's') {
				char* tmp = log;
				log = cm_strcat(tmp, va_arg(va, char*));
				free(tmp);
			} else if(tmpl[i] == 'd') {
				char buf[513];
				sprintf(buf, "%d", va_arg(va, int));
				char* tmp = log;
				log = cm_strcat(tmp, buf);
				free(tmp);
			} else if(tmpl[i] == '%') {
				char* tmp = log;
				log = cm_strcat(tmp, "%");
				free(tmp);
			}
		} else {
			cbuf[0] = tmpl[i];
			char* tmp = log;
			log = cm_strcat(tmp, cbuf);
			free(tmp);
		}
	}
	va_end(va);
	tt_putstr(log);
}

void show_png(void) {
	FILE* f = fopen(PREFIX "/pbtewi.png", "rb");
	if(f == NULL) {
		f = fopen(PREFIX "/../ICON0.PNG", "rb");
	}
	if(f == NULL) return;
	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info = png_create_info_struct(png);
	if(setjmp(png_jmpbuf(png))) {
		png_destroy_read_struct(&png, &info, NULL);
		fclose(f);
		return;
	}

	png_init_io(png, f);
	png_read_info(png, info);

	int width = png_get_image_width(png, info);
	int height = png_get_image_height(png, info);
	int depth = png_get_bit_depth(png, info);
	int type = png_get_color_type(png, info);

	if(depth == 16) png_set_strip_16(png);
	if(type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
	if(type == PNG_COLOR_TYPE_GRAY && depth < 8) png_set_expand_gray_1_2_4_to_8(png);
	if(png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
	if(type == PNG_COLOR_TYPE_RGB || type == PNG_COLOR_TYPE_GRAY || type == PNG_COLOR_TYPE_PALETTE) png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
	if(type == PNG_COLOR_TYPE_GRAY || type == PNG_COLOR_TYPE_GRAY_ALPHA) png_set_gray_to_rgb(png);
	png_read_update_info(png, info);
	png_bytep* rows = (png_bytep*)malloc(sizeof(*rows) * (height));

	int i;

	for(i = 0; i < height; i++) {
		rows[i] = (png_byte*)malloc(png_get_rowbytes(png, info));
	}

	png_read_image(png, rows);

	for(i = 0; i < height; i++) {
		int j;
		for(j = 0; j < width; j++) {
			png_bytep byte = &(rows[i][j * 4]);
			uint32_t col = (byte[0] << 16) | (byte[1] << 8) | (byte[2]);
			int k;
			for(k = 0; k < BUFFERS; k++) {
				buffers[k].ptr[buffers[k].width * i - width + j] = col;
			}
		}
	}

	png_destroy_read_struct(&png, &info, NULL);
	fclose(f);

	for(i = 0; i < height; i++) {
		free(rows[i]);
	}
	free(rows);
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
#elif defined(__PPU__)
	int i;
	ctx = init_screen();
	int w, h;
	get_resolution(&w, &h);
	tt_width = w / 6;
	tt_height = h / 8;
	tvram = malloc((w / 6) * (h / 8));
	for(i = 0; i < BUFFERS; i++) make_buffer(&buffers[i], i);
	flip(ctx, BUFFERS - 1);
	sys_ppu_thread_t id;
	sysThreadCreate(&id, text_thread, NULL, 1500, 0x1000, THREAD_JOINABLE, "TextThread");
	printf("PS3 Bootstrap, Tewi/%s\n", tw_get_version());
	show_png();
	netInitialize();
#elif defined(__ps2sdk__)
	SifInitRpc(0);
	init_scr();
	scr_printf("PS2 Bootstrap, Tewi/%s\n", tw_get_version());
	while(1)
		;
#endif
	int st = startup(argc, argv);
	if(st != -1) {
#ifdef _PSP
		printf("Error code %d\n", st);
		while(running) sceKernelDelayThread(50 * 1000);
		sceKernelExitGame();
#else
#ifdef __PPU__
		printf("Error code %d\n", st);
		while(1)
			;
#endif
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
