/* $Id$ */

#include "../config.h"

#include <stdio.h>

int main(int argc, char** argv){
	if(argc < 2){
		return 1;
	}
	printf("##\n");
	printf("## tewi.conf -- Tewi HTTPd configuration file\n");
	printf("##\n");
	printf("\n");
	printf("# This configuration file is auto-generated.\n");
	printf("\n");
	printf("ServerRoot %s\n", argv[1]);
	printf("\n");
	printf("ServerAdmin %s\n", SERVER_ADMIN);
	printf("\n");
	printf("LoadModule lib/tewi/mod_cgi.so\n");
	printf("#LoadModule lib/tewi/mod_proxy.so\n");
	printf("\n");
	printf("Listen 80\n");
	printf("#ListenSSL 443\n");
	printf("\n");
	printf("#SSLKey key.pem\n");
	printf("#SSLCertificate cert.pem\n");
	printf("\n");
	printf("MIMEType all application/octet-stream\n");
	printf("MIMEType .html text/html\n");
	printf("MIMEType .txt text/plain\n");
	printf("MIMEType .png image/png\n");
	printf("\n");
	printf("Icon all /icons/unknown.png\n");
	printf("Icon text/* /icons/text.png\n");
	printf("Icon image/* /icons/image.png\n");
	printf("Icon misc/dir /icons/folder.png\n");
	printf("Icon misc/parent /icons/parent.png\n");
	printf("DirectoryIndex index.html\n");
	printf("\n");
	printf("Readme README\n");
	printf("\n");
	printf("DocumentRoot %s/www\n", argv[1]);
	printf("\n");
	printf("BeginDirectory %s/www\n", argv[1]);
	printf("\tAllow all\n");
	printf("EndDirectory\n", argv[1]);
}
