/*
 * $Id: rstat_proc.c,v 1.8 2005/09/07 08:07:28 afm Exp $
 *
 *  rpc.rstatd version 3.07 rpc remote statistics daemon.
 *  Copyright (C) 1995  Adam Migus, Memorial University of Newfoundland
 *	(MUN)
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
 *    Adam Migus	      				
 *	  amigus@cs.mun.ca 
 *    amigus@ucs.mun.ca   
 *
 *	Original Sun version by:
 * Copyright (c) 1984 by Sun Microsystems, Inc.
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <limits.h>
#include <rpc/rpc.h>
#include <sys/socket.h>
#ifndef __GLIBC__
#include <nlist.h>
#endif
#include <syslog.h>
#include <sys/errno.h>
#include <sys/param.h>
#include <unistd.h>
#include "rstat.h"
#include "rstat_proc.h"
#include "getdata.h"

#define BUFLEN 2048

int	debug = 0;

static char *id =
	"@(#) $Id: rstat_proc.c,v 1.8 2005/09/07 08:07:28 afm Exp $ $Revision: 1.8 $";

union
{
	struct stats s1;
	struct statsswtch s2;
	struct statstime s3;
	struct statsvar s4;
	struct statsusers s5;
} stats_all;

void test_connect()
{
	if (inetd_connect != 0) {
		exit (0);
	}
}

static void get_stats (struct statsusers *stats)
{
	getdata_collect(stats);
	if (inetd_connect)
		alarm (1);
}

statsusers *rstatproc_stats_5_svc(void *arg, struct svc_req *rqstp)
{
	get_stats (&stats_all.s5);
	return &stats_all.s5;
}

statsvar *rstatproc_stats_4_svc(void *arg, struct svc_req *rqstp)
{
	get_stats (&stats_all.s5);
	return &stats_all.s4;
}

statstime *rstatproc_stats_3_svc(void *arg, struct svc_req *rqstp)
{
	get_stats (&stats_all.s5);
	/* can return s3, since there is only the last member missing */
	return &stats_all.s3;
}

statsswtch *rstatproc_stats_2_svc(void *arg, struct svc_req *rqstp)
{
	get_stats (&stats_all.s5);

	/* put opackets from statsusers in the right position for statsswtch 
	   this is ugly 'union' hacking */
	stats_all.s2.if_opackets = stats_all.s5.if_opackets;
	return (&stats_all.s2);
}

stats *rstatproc_stats_1_svc(void *arg, struct svc_req *rqstp)
{
	get_stats (&stats_all.s5);

	/* put opackets from stats in the right position for statsswtch 
	   this is ugly 'union' hacking */
	stats_all.s1.if_opackets = stats_all.s5.if_opackets;

	return (&stats_all.s1);
}

u_int * rstatproc_havedisk_5_svc(void *arg, struct svc_req *rqstp)
{
	static u_int have;

	get_stats (&stats_all.s5);
	have = havedisk (&stats_all.s5);
	return (&have);
}

u_int *rstatproc_havedisk_4_svc (void *arg, struct svc_req *rqstp)
{
	return (rstatproc_havedisk_5_svc (arg, rqstp));
}

u_int *rstatproc_havedisk_3_svc(void *arg, struct svc_req *rqstp)
{
	return (rstatproc_havedisk_5_svc (arg, rqstp));
}

u_int *rstatproc_havedisk_2_svc(void *arg, struct svc_req *rqstp)
{
	return (rstatproc_havedisk_5_svc (arg, rqstp));
}

u_int *rstatproc_havedisk_1_svc(void *arg, struct svc_req *rqstp)
{
	return (rstatproc_havedisk_5_svc (arg, rqstp));
}

int havedisk (struct statsusers *stats)
{

	int i, cnt;

	cnt = 0;
	for (i = 0; i < DK_NDRIVE; i++)
		cnt += stats->dk_xfer[i];
	return (cnt != 0);
}

void rstat_service (struct svc_req *rqstp, SVCXPRT * transp)
{
	union
	{
		int fill;
	} argument;
	char *result;
	xdrproc_t xdr_argument, xdr_result;
	char *(*local) __P ((void *, struct svc_req *));

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void,
				      (char *) NULL);
		if (inetd_connect)
			exit (0);

	case RSTATPROC_STATS:
		xdr_argument = (xdrproc_t) xdr_void;
		xdr_result = (xdrproc_t) xdr_statsusers;
		switch (rqstp->rq_vers) {
		case RSTATVERS_ORIG:
			local = (char *(*)__P ((void *, struct svc_req *)))
				rstatproc_stats_1_svc;
			break;
		case RSTATVERS_SWTCH:
			local = (char *(*)__P ((void *, struct svc_req *)))
				rstatproc_stats_2_svc;
			break;
		case RSTATVERS_TIME:
			local = (char *(*)__P ((void *, struct svc_req *)))
				rstatproc_stats_3_svc;
			break;
		case RSTATVERS_VAR:
			local = (char *(*)__P ((void *, struct svc_req *)))
				rstatproc_stats_4_svc;
			break;
		case RSTATVERS_USERS:
			local = (char *(*)__P ((void *, struct svc_req *)))
				rstatproc_stats_5_svc;
			break;
		default:
			svcerr_progvers (transp, RSTATVERS_ORIG,
					 RSTATVERS_USERS);
			if (inetd_connect)
				exit (0);
		}
		break;

	case RSTATPROC_HAVEDISK:
		xdr_argument = (xdrproc_t) xdr_void;
		xdr_result = (xdrproc_t) xdr_u_int;
		switch (rqstp->rq_vers) {
		case RSTATVERS_ORIG:
			local = (char *(*)__P ((void *, struct svc_req *)))
				rstatproc_havedisk_1_svc;
			break;
		case RSTATVERS_SWTCH:
			local = (char *(*)__P ((void *, struct svc_req *)))
				rstatproc_havedisk_2_svc;
			break;
		case RSTATVERS_TIME:
			local = (char *(*)__P ((void *, struct svc_req *)))
				rstatproc_havedisk_3_svc;
			break;
		case RSTATVERS_VAR:
			local = (char *(*)__P ((void *, struct svc_req *)))
				rstatproc_stats_4_svc;
			break;
		case RSTATVERS_USERS:
			local = (char *(*)__P ((void *, struct svc_req *)))
				rstatproc_havedisk_5_svc;
			break;
		default:
			svcerr_progvers (transp, RSTATVERS_ORIG,
					 RSTATVERS_USERS);
			if (inetd_connect)
				exit (0);
		}
		break;
	default:
		svcerr_noproc (transp);
		if (inetd_connect)
			exit (0);
	}
	bzero ((char *) &argument, sizeof (argument));
	if (!svc_getargs (transp, xdr_argument, (caddr_t) & argument)) {
		svcerr_decode (transp);
		if (inetd_connect)
			exit (0);
	}
	result = (*local) (&argument, rqstp);
	if (result != NULL && !svc_sendreply (transp, xdr_result, result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, xdr_argument, (caddr_t) & argument)) {
		syslog (LOG_ERR, "unable to free arguments\n");
		exit (1);
	}
}
