/* $Id$ */

#ifndef __TW_CONFIG_H__
#define __TW_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../config.h"

#include "tw_http.h"

#include <stdint.h>
#include <stdbool.h>

#if defined(__MINGW32__) || defined(_MSC_VER) || defined(__BORLANDC__) || (defined(__WATCOMC__) && !defined(__OS2__))
#ifdef USE_WINSOCK1
#include <winsock.h>
#else
#include <winsock2.h>
#endif
#define NO_IPV6
#else
#ifdef __PPU__
#include <net/net.h>
#endif
#include <netinet/in.h>
#ifdef __HAIKU__
#define NO_IPV6
#endif
#endif

#ifdef NO_IPV6
#define SOCKADDR struct sockaddr_in
#else
#define SOCKADDR struct sockaddr_in6
#endif

#define MAX_PORTS 1024
#define MAX_VHOSTS 1024
#define MAX_MODULES 1024
#define MAX_DIRS 1024
#define MAX_MIME 4096
#define MAX_ICON 1024
#define MAX_INDEX 32
#define MAX_README 8

#if defined(_MSC_VER) || defined(__BORLANDC__)
#define NUM1024 1024UL
#else
#define NUM1024 1024ULL
#endif

enum TW_DIR_TYPE {
	TW_DIR_ALLOW = 0,
	TW_DIR_DENY
};

struct tw_dir_entry {
	char* name;
	char* dir;
	int type;
};

struct tw_mime_entry {
	char* ext;
	char* mime;
};

struct tw_icon_entry {
	char* mime;
	char* icon;
};

struct tw_config_entry {
	char* name;
	int port;
#ifndef NO_SSL
	char* sslkey;
	char* sslcert;
#endif
	char* root;
	int hideport;
	struct tw_dir_entry dirs[MAX_DIRS];
	int dir_count;
	struct tw_mime_entry mimes[MAX_MIME];
	int mime_count;
	struct tw_icon_entry icons[MAX_ICON];
	int icon_count;
	char* indexes[MAX_INDEX];
	int index_count;
	char* readmes[MAX_README];
	int readme_count;
#ifdef HAS_CHROOT
	char* chroot_path;
#endif
};

struct tw_config {
#if defined(_MSC_VER) || defined(__BORLANDC__)
	uint32_t ports[MAX_PORTS + 1];
#else
	uint64_t ports[MAX_PORTS + 1]; /* If port & (1 << 32) is non-zero, it is SSL */
#endif
	char hostname[1025];
	char* defined[1025];
	struct tw_config_entry root;
	struct tw_config_entry vhosts[MAX_VHOSTS];
	void* modules[MAX_MODULES];
	int module_count;
	int vhost_count;
	char* server_admin;
	char* server_root;
	char* extension;
};

void tw_config_init(void);
int tw_config_read(const char* path);
struct tw_config_entry* tw_vhost_match(const char* name, int port);
bool tw_permission_allowed(const char* path, SOCKADDR addr, struct tw_http_request req, struct tw_config_entry* vhost);

#ifdef __cplusplus
}
#endif

#endif
