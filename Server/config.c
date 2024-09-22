/* $Id$ */

#define SOURCE

#include "tw_config.h"
#include "tw_module.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __MINGW32__
#include <winsock2.h>
#endif

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

bool tw_permission_allowed(const char* path, SOCKADDR addr, struct tw_http_request req, struct tw_config_entry* vhost) {
	int i;
	bool found = false;
	bool pathstart = false;
	bool perm = false;
again:
	for(i = 0; i < vhost->dir_count; i++) {
		struct tw_dir_entry* e = &vhost->dirs[i];
		pathstart = false;
		if(strlen(path) >= strlen(e->dir)) {
			pathstart = true;
			int j;
			for(j = 0; path[j] != 0 && e->dir[j] != 0; j++) {
				if(path[j] != e->dir[j]) {
					pathstart = false;
					break;
				}
			}
		}
		char* noslash = cm_strdup(e->dir);
		noslash[strlen(noslash) - 1] = 0;
		if(strcmp(e->dir, path) == 0 || strcmp(noslash, path) == 0 || pathstart) {
			found = true;
			if(strcmp(e->name, "all") == 0) {
				perm = e->type == TW_DIR_ALLOW;
			}
		}
		free(noslash);
	}
	if(!found && vhost != &config.root) {
		vhost = &config.root;
		goto again;
	}
	return perm;
}

void tw_config_init(void) {
	int i;
	for(i = 0; i < MAX_PORTS + 1; i++) {
		config.ports[i] = -1;
	}
	for(i = 0; i < MAX_VHOSTS; i++) {
		config.vhosts[i].sslkey = NULL;
		config.vhosts[i].sslcert = NULL;
		config.vhosts[i].root = NULL;
	}
	config.root.sslkey = NULL;
	config.root.sslcert = NULL;
	config.root.root = NULL;
	config.root.mime_count = 0;
	config.root.dir_count = 0;
	config.root.icon_count = 0;
	config.root.index_count = 0;
	config.root.readme_count = 0;
	config.root.hideport = 0;
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
		char* dir = NULL;
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
					} else if(cm_strcaseequ(r[0], "BeginDirectory")) {
						if(dir != NULL) {
							cm_log("Config", "Already in directory section at line %d", ln);
							stop = 1;
						} else {
							if(r[1] == NULL) {
								cm_log("Config", "Missing directory at line %d", ln);
								stop = 1;
							} else {
								dir = cm_strcat(r[1], r[1][strlen(r[1]) - 1] == '/' ? "" : "/");
							}
						}
					} else if(cm_strcaseequ(r[0], "EndDirectory")) {
						if(dir == NULL) {
							cm_log("Config", "Not in directory section at line %d", ln);
							stop = 1;
						} else {
							free(dir);
							dir = NULL;
						}
					} else if(cm_strcaseequ(r[0], "Allow")) {
						if(dir == NULL) {
							cm_log("Config", "Not in directory section at line %d", ln);
							stop = 1;
						} else {
							if(r[1] == NULL) {
								cm_log("Config", "Missing argument at line %d", ln);
								stop = 1;
							} else {
								struct tw_dir_entry* e = &current->dirs[current->dir_count++];
								e->name = cm_strdup(r[1]);
								e->dir = cm_strdup(dir);
								e->type = TW_DIR_ALLOW;
							}
						}
					} else if(cm_strcaseequ(r[0], "Deny")) {
						if(dir == NULL) {
							cm_log("Config", "Not in directory section at line %d", ln);
							stop = 1;
						} else {
							if(r[1] == NULL) {
								cm_log("Config", "Missing argument at line %d", ln);
								stop = 1;
							} else {
								struct tw_dir_entry* e = &current->dirs[current->dir_count++];
								e->name = cm_strdup(r[1]);
								e->dir = cm_strdup(dir);
								e->type = TW_DIR_DENY;
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
								current->dir_count = 0;
								current->mime_count = 0;
								current->icon_count = 0;
								current->index_count = 0;
								current->readme_count = 0;
								current->hideport = -1;
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
					} else if(cm_strcaseequ(r[0], "HidePort")) {
						current->hideport = 1;
					} else if(cm_strcaseequ(r[0], "ShowPort")) {
						current->hideport = 0;
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
							chdir(r[1]);
						}
					} else if(cm_strcaseequ(r[0], "DocumentRoot")) {
						if(r[1] == NULL) {
							cm_log("Config", "Missing path at line %d", ln);
							stop = 1;
						} else {
							if(current->root != NULL) free(current->root);
							current->root = cm_strdup(strcmp(r[1], "/") == 0 ? "" : r[1]);
						}
					} else if(cm_strcaseequ(r[0], "ServerRoot")) {
						if(r[1] == NULL) {
							cm_log("Config", "Missing path at line %d", ln);
							stop = 1;
						} else {
							if(config.server_root != NULL) free(config.server_root);
							config.server_root = cm_strdup(r[1]);
						}
					} else if(cm_strcaseequ(r[0], "MIMEType")) {
						if(r[1] == NULL) {
							cm_log("Config", "Missing extension at line %d", ln);
							stop = 1;
						} else if(r[2] == NULL) {
							cm_log("Config", "Missing MIME at line %d", ln);
							stop = 1;
						} else {
							struct tw_mime_entry* e = &current->mimes[current->mime_count++];
							e->ext = cm_strdup(r[1]);
							e->mime = cm_strdup(r[2]);
						}
					} else if(cm_strcaseequ(r[0], "Icon")) {
						if(r[1] == NULL) {
							cm_log("Config", "Missing MIME at line %d", ln);
							stop = 1;
						} else if(r[2] == NULL) {
							cm_log("Config", "Missing path at line %d", ln);
							stop = 1;
						} else {
							struct tw_icon_entry* e = &current->icons[current->icon_count++];
							e->mime = cm_strdup(r[1]);
							e->icon = cm_strdup(r[2]);
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
					} else if(cm_strcaseequ(r[0], "DirectoryIndex")) {
						for(i = 1; r[i] != NULL; i++) {
							current->indexes[current->index_count++] = cm_strdup(r[i]);
						}
					} else if(cm_strcaseequ(r[0], "Readme")) {
						for(i = 1; r[i] != NULL; i++) {
							current->readmes[current->readme_count++] = cm_strdup(r[i]);
						}
					} else {
						stop = 1;
						if(r[0] != NULL) {
							int argc;
							for(argc = 0; r[argc] != NULL; argc++)
								;
							stop = 0;
							int i;
							bool called = false;
							struct tw_tool tools;
							tw_init_tools(&tools);
							for(i = 0; i < config.module_count; i++) {
								tw_mod_config_t mod_config = (tw_mod_config_t)tw_module_symbol(config.modules[i], "mod_config");
								int resp;
								if(mod_config != NULL && (resp = mod_config(&tools, r, argc)) == TW_CONFIG_PARSED) {
									called = true;
									break;
								}
								if(resp == TW_CONFIG_ERROR) {
									stop = 1;
									called = true;
									break;
								}
							}
							if(!called) {
								cm_log("Config", "Unknown directive `%s' at line %d", r[0], ln);
								stop = 1;
							}
						}
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
