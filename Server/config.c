/* $Id$ */

#define SOURCE

#include "tw_config.h"
#include "tw_module.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cm_string.h>
#include <cm_log.h>

struct tw_config config;

struct tw_config_entry* tw_vhost_match(const char* name, int port) {
	int i;
	for(i = 0; i < config.vhost_count; i++) {
		if(strcmp(config.vhosts[i].name, name) == 0 && (config.vhosts[i].port == -1 ? 1 : config.vhosts[i].port == port)) {
			return &config.vhosts[i];
		}
	}
	return &config.root;
}

void tw_config_init(void) {
	int i;
	for(i = 0; i < MAX_PORTS + 1; i++) {
		config.ports[i] = -1;
	}
	for(i = 0; i < MAX_VHOSTS; i++) {
		config.vhosts[i].sslkey = NULL;
		config.vhosts[i].sslcert = NULL;
	}
	config.root.sslkey = NULL;
	config.root.sslcert = NULL;
	config.vhost_count = 0;
	config.module_count = 0;
	config.extension = NULL;
	config.server_root = cm_strdup(PREFIX);
	gethostname(config.hostname, 1024);
}

int tw_config_read(const char* path) {
	cm_log("Config", "Reading %s", path);
	char cbuf[2];
	cbuf[1] = 0;
	int ln = 0;
	FILE* f = fopen(path, "r");
	if(f != NULL) {
		char* line = malloc(1);
		line[0] = 0;
		int stop = 0;
		struct tw_config_entry* current = &config.root;
		char* vhost = NULL;
		while(stop == 0) {
			int c = fread(cbuf, 1, 1, f);
			if(cbuf[0] == '\n' || c <= 0) {
				ln++;
				char* l = cm_trim(line);
				if(strlen(l) > 0 && l[0] != '#') {
					char** r = cm_split(l, " \t");
					int i;
					if(cm_strcaseequ(r[0], "Include") || cm_strcaseequ(r[0], "IncludeOptional")) {
						for(i = 1; r[i] != NULL; i++) {
							if(tw_config_read(r[i]) != 0 && cm_strcaseequ(r[0], "Include")) {
								stop = 1;
								break;
							}
						}
					} else if(cm_strcaseequ(r[0], "BeginVirtualHost")) {
						if(vhost != NULL) {
							cm_log("Config", "Already in virtual host section at line %d", ln);
							stop = 1;
						} else {
							if(r[1] == NULL) {
								cm_log("Config", "Missing virtual host at line %d", ln);
								stop = 1;
							} else {
								vhost = cm_strdup(r[1]);
								current = &config.vhosts[config.vhost_count++];
								int i;
								current->name = cm_strdup(vhost);
								current->port = -1;
								for(i = 0; vhost[i] != 0; i++) {
									if(vhost[i] == ':') {
										current->name[i] = 0;
										current->port = atoi(current->name + i + 1);
										break;
									}
								}
							}
						}
					} else if(cm_strcaseequ(r[0], "EndVirtualHost")) {
						if(vhost == NULL) {
							cm_log("Config", "Not in virtual host section at line %d", ln);
							stop = 1;
						} else {
							free(vhost);
							vhost = NULL;
							current = &config.root;
						}
					} else if(cm_strcaseequ(r[0], "Listen") || cm_strcaseequ(r[0], "ListenSSL")) {
						for(i = 1; r[i] != NULL; i++) {
							uint64_t port = atoi(r[i]);
							cm_log("Config", "Going to listen at port %d%s", (int)port, cm_strcaseequ(r[0], "ListenSSL") ? " with SSL" : "");
							if(cm_strcaseequ(r[0], "ListenSSL")) port |= (1ULL << 32);
							int j;
							for(j = 0; config.ports[j] != -1; j++)
								;
							config.ports[j] = port;
						}
					} else if(cm_strcaseequ(r[0], "SSLKey")) {
						if(r[1] == NULL) {
							cm_log("Config", "Missing path at line %d", ln);
							stop = 1;
						} else {
							if(current->sslkey != NULL) free(current->sslkey);
							current->sslkey = cm_strdup(r[1]);
						}
					} else if(cm_strcaseequ(r[0], "SSLCertificate")) {
						if(r[1] == NULL) {
							cm_log("Config", "Missing path at line %d", ln);
							stop = 1;
						} else {
							if(current->sslcert != NULL) free(current->sslcert);
							current->sslcert = cm_strdup(r[1]);
						}
					} else if(cm_strcaseequ(r[0], "ServerRoot")) {
						if(r[1] == NULL) {
							cm_log("Config", "Missing path at line %d", ln);
							stop = 1;
						} else {
							if(config.server_root != NULL) free(config.server_root);
							config.server_root = cm_strdup(r[1]);
						}
					} else if(cm_strcaseequ(r[0], "LoadModule")) {
						for(i = 1; r[i] != NULL; i++) {
							void* mod = tw_module_load(r[i]);
							if(mod != NULL) {
								config.modules[config.module_count++] = mod;
								if(tw_module_init(mod) != 0) {
									stop = 1;
									break;
								}
							} else {
								stop = 1;
								break;
							}
						}
					} else {
						if(r[0] != NULL) {
							cm_log("Config", "Unknown directive `%s' at line %d", r[0], ln);
						}
						stop = 1;
					}
					for(i = 0; r[i] != NULL; i++) free(r[i]);
					free(r);
				}
				free(l);
				free(line);
				line = malloc(1);
				line[0] = 0;
				if(c <= 0) break;
			} else if(cbuf[0] != '\r') {
				char* tmp = line;
				line = cm_strcat(tmp, cbuf);
				free(tmp);
			}
		}
		free(line);
		fclose(f);
		return stop;
	} else {
		cm_log("Config", "Could not open the file");
		return 1;
	}
}
