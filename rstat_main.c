/*
 * $Id: rstat_main.c,v 1.3 2005/09/07 08:07:28 afm Exp $
 *
 *  rpc.rstatd version 3.07 rpc remote statistics daemon. 
 *  Copyright (C) 1995  Adam Migus, Memorial University of Newfoundland
 *	 (MUN)
 *  Copyright (C) 2001 Andreas Klingler, University of Erlangen-Nuernberg
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 1, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  If you make modifications to the source, I would be happy to have
 *  them to include in future releases.  Feel free to send them to:
 *      Adam Migus	      				
 *		amigus@cs.mun.ca 
 *      amigus@ucs.mun.ca   
 *
 *		Modified by Adam Migus (amigus@cs.mun.ca)
 *              Modified by Andreas Klingler (andreas.klingler@cyber-wizard.com)
 *
 * Original Author:
 * Copyright (c) 1993, John Brezak
 * All rights reserved.
 *
 **************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <rpc/rpc.h>
#include <rpc/pmap_clnt.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include "rstat.h"
#include "getdata.h"
#include "rstat_proc.h"

static char * id = "@(#) $Id: rstat_main.c,v 1.3 2005/09/07 08:07:28 afm Exp $ $Revision: 1.3 $";

extern void rstat_service();
extern void test_connect();

#ifdef __APPLE__
int _rpcsvcdirty;
#endif

int inetd_connect = 1;

void background()
{
	int pid;
	
	pid = fork();
	switch(pid) {
		case 0:
			break;
		case -1: 
			perror("fork");
			break;
		default:
			exit(0);
	}
	setsid();
}
void cleanup()
{
        (void) pmap_unset(RSTATPROG, RSTATVERS_USERS);
        (void) pmap_unset(RSTATPROG, RSTATVERS_TIME);
        (void) pmap_unset(RSTATPROG, RSTATVERS_SWTCH);
        (void) pmap_unset(RSTATPROG, RSTATVERS_ORIG);
        exit(0);
}

int ninterfaces = 0;
char **interfaces;

int main(int argc, char **argv)
{
	SVCXPRT *transp;
        int fromlen, sock = 0, proto = 0;
	struct sockaddr_in from;
	int count = 1;

	if (argc > 1) {
		if (strcmp("-d", argv[count]) == 0) {
			debug = 1;
			count++;
		}
		ninterfaces = argc - count; /* everything on the commandline besides -d */
		interfaces = &argv[count];  /* is interpreted as interface names */
	}
	getdata_setup();
        
	if (getsockname(sock,(struct sockaddr *)&from, &fromlen) < 0) {
		inetd_connect = 0;
		sock = RPC_ANYSOCK;
		proto = IPPROTO_UDP;
	}

	if (!(inetd_connect)) {
		(void)pmap_unset(RSTATPROG, RSTATVERS_USERS);
		(void)pmap_unset(RSTATPROG, RSTATVERS_TIME);
		(void)pmap_unset(RSTATPROG, RSTATVERS_SWTCH);
		(void)pmap_unset(RSTATPROG, RSTATVERS_ORIG);
		(void) signal(SIGINT, cleanup);
		(void) signal(SIGTERM, cleanup);
		(void) signal(SIGHUP, cleanup);
		openlog("rpc.rstatd", LOG_CONS|LOG_PID, LOG_DAEMON);
		if (!debug) background();
	}
	(void)signal(SIGALRM, test_connect);

	transp = svcudp_create(sock);
	if (transp == NULL) {
		syslog(LOG_ERR, "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, RSTATPROG, RSTATVERS_USERS, rstat_service, proto)) {
		syslog(LOG_ERR, "unable to register (RSTATPROG, RSTATVERS_USERS, udp).");
		exit(1);
	}
	if (!svc_register(transp, RSTATPROG, RSTATVERS_TIME, rstat_service, proto)) {
		syslog(LOG_ERR, "unable to register (RSTATPROG, RSTATVERS_TIME, udp).");
		exit(1);
	}
	if (!svc_register(transp, RSTATPROG, RSTATVERS_SWTCH, rstat_service, proto)) {
		syslog(LOG_ERR, "unable to register (RSTATPROG, RSTATVERS_SWTCH, udp).");
		exit(1);
	}
	if (!svc_register(transp, RSTATPROG, RSTATVERS_ORIG, rstat_service, proto)) {
		syslog(LOG_ERR, "unable to register (RSTATPROG, RSTATVERS_ORIG, udp).");
		exit(1);
	}

	svc_run();
	syslog(LOG_ERR, "svc_run returned");
	exit(1);
}
