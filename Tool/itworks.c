/* $Id$ */

#include "../config.h"

#include <stdio.h>

int main() {
	printf("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\n");
	printf("<html>\n");
	printf("	<head>\n");
	printf("		<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n");
	printf("		<title>Test Page for Tewi HTTPd Installation</title>\n");
	printf("	</head>\n");
	printf("	<body vlink=\"#000080\" text=\"#000000\" link=\"#0000FF\" bgcolor=\"#FFFFFF\" alink=\"#FF0000\">\n");
	printf("		<h1 align=\"center\">It works! - Tewi HTTPd is installed on this website!</h1>\n");
	printf("		<p>\n");
	printf("			If you can see this page, then the people who own this domain have just installed <a href=\"http://nishi.boats/tewi\">Tewi HTTPd</a> successfully. They now have to add content to this directory and replace this placeholder page, or else point the server at their real content.\n");
	printf("		</p>\n");
	printf("		<hr>\n");
	printf("		<blockquote>\n");
	printf("			If you are seeing this page instead of the website you expected, please <strong>contact the administrator of the website involved.</strong> (Try sending email to <samp>&lt;%s&gt;</samp>.) Although this site is running Tewi HTTPd it almost certainly has no other connection to the developers of Tewi HTTPd, so please do not send email about this website or its contents to the developers of Tewi HTTPd. If you do, your message will be <strong><big>ignored</big></strong>.\n", SERVER_ADMIN);
	printf("		</blockquote>\n");
	printf("		<hr>\n");
	printf("		<p>\n");
	printf("			For the document, take a look at <a href=\"https://trac.nishi.boats/tewi\">Tewi HTTPd Trac</a>.\n");
	printf("		</p>\n");
	printf("		<p>\n");
	printf("			The Webmaster of this site is free to use the image below on the Tewi HTTPd-powered Web server. Thanks for using Tewi HTTPd!\n");
	printf("		</p>\n");
	printf("		<div align=\"center\">\n");
	printf("			<img src=\"/pbtewi.gif\">\n");
	printf("		</div>\n");
	printf("	</body>\n");
	printf("</html>\n");
	return 0;
}
