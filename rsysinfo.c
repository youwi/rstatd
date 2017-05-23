/*
 * $Id: rsysinfo.c,v 1.4 2005/09/07 08:07:28 afm Exp $
 *  rsysinfo version 3.03 rpc ruptime client for use with rpc.rstatd 3.03
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
 *      Adam Migus	      				
 *		amigus@cs.mun.ca 
 *      amigus@ucs.mun.ca   
 *
 **************************************************************************/

#include <stdio.h>
#include <rpc/rpc.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "rstat.h"
#include <stdlib.h>

void bzero();
void bcopy();
char *rindex();

static char *id = "@(#) $Id: rsysinfo.c,v 1.4 2005/09/07 08:07:28 afm Exp $";
static char *version0 = "@(#)rsysinfo version 3.06 (c) Andreas Klingler 3/5/2001 based on\n";
char *version = "@(#)rsysinfo version 3.03 Copyright (Adam Migus) 1/9/96\n";
char *program_name;

int	rsysinfo_answer_v3(struct statstime *replyp,
	struct sockaddr_in *raddrp)
{
	struct hostent *hp;
	char *host;
	struct statstime *host_stat;
	struct tm *tmp_time;
	struct tm host_time;
	struct tm host_uptime;
	char days_buf[16];
	char hours_buf[16];
	time_t atime;

	host_stat = (statstime *)replyp;

	hp = gethostbyaddr((char *)&raddrp->sin_addr.s_addr,
			   sizeof(struct in_addr), AF_INET);
	if (hp)
		host = (char *)hp->h_name;
	else
		host = (char *)inet_ntoa(raddrp->sin_addr);


	atime = (time_t)host_stat->curtime.tv_sec;
	tmp_time = localtime(&atime);
	host_time = *tmp_time;

	host_stat->curtime.tv_sec -= host_stat->boottime.tv_sec;

	atime = (time_t)host_stat->curtime.tv_sec;
	tmp_time = gmtime(&atime);
	host_uptime = *tmp_time;


	if (host_uptime.tm_yday != 0)
		sprintf(days_buf, "%3d day%s, ", host_uptime.tm_yday,
			(host_uptime.tm_yday > 1) ? "s" : "");
	else
		days_buf[0] = '\0';

	if (host_uptime.tm_hour != 0)
		sprintf(hours_buf, "%2d:%02d, ",
			host_uptime.tm_hour, host_uptime.tm_min);
	else
		if (host_uptime.tm_min != 0)
			sprintf(hours_buf, "%2d mins, ", host_uptime.tm_min);
		else
			hours_buf[0] = '\0';


	printf("System Information for: %s\n",host);
	printf("uptime: %s%sload averge: %.2f %.2f %.2f\n",
		days_buf, hours_buf, host_stat->avenrun[0]/256.0,
		host_stat->avenrun[1]/256.0, host_stat->avenrun[2]/256.0);

	printf("cpu usage (jiffies): user %d  nice %d  system %d  idle %d\n",
		host_stat->cp_time[0], host_stat->cp_time[1],
		host_stat->cp_time[2], host_stat->cp_time[3]);

	printf("page in: %d  page out: %d   swap in: %d  swap out: %d\n",
		host_stat->v_pgpgin, host_stat->v_pgpgout,
		host_stat->v_pswpin, host_stat->v_pswpout);
	printf("intr: %d     context switches: %d\n",
		host_stat->v_intr, host_stat->v_swtch);
	printf ("disks: %d %d %d %d\n",
		host_stat->dk_xfer[0], host_stat->dk_xfer[1],
		host_stat->dk_xfer[2], host_stat->dk_xfer[3]);
	printf("ethernet:  rx: %u   rx-err: %u\n"
			"           tx: %u   tx-err: %u    collisions: %d\n",
		host_stat->if_ipackets, host_stat->if_ierrors,
		host_stat->if_opackets, host_stat->if_oerrors,
		host_stat->if_collisions);

	return(0);
}

int pointopoint_v3(server_addr)
struct sockaddr_in *server_addr;
{
	struct statstime host_stat;
	struct timeval pertry_timeout, total_timeout;
	enum clnt_stat clnt_stat;
	int sock=RPC_ANYSOCK;
	CLIENT *client;

	pertry_timeout.tv_sec = 3;
	pertry_timeout.tv_usec = 0;
	total_timeout.tv_sec = 10;
	total_timeout.tv_usec = 0;

	if((client = clntudp_create(server_addr, RSTATPROG, RSTATVERS_TIME,
	pertry_timeout, &sock)) == NULL) {
		close(sock);
		return(0);
	}
	clnt_stat = clnt_call(client, RSTATPROC_STATS, (xdrproc_t) xdr_void, 0,
		(xdrproc_t)xdr_statstime, (void *)&host_stat,
		total_timeout);

	if(clnt_stat == RPC_SUCCESS) {
		rsysinfo_answer_v3(&host_stat, server_addr);
		clnt_destroy(client);
		close(sock);
		return(1);
	}
	else {
		clnt_destroy(client);
		close(sock);
		return(0);
	}
	return(0);
}
void usage()
{
	printf("usage: %s \"host\" | -v\n"\
			"\t-v\tprint version and exit\n", program_name);
}

int main(int argc, char **argv)
{
	struct hostent *hp;
	struct sockaddr_in server_addr;
	char c;

	program_name = rindex(argv[0],'/');
	if(!(program_name)) 
		program_name = argv[0];
	else 
		program_name++;
	
	if(argc == 1) {
		usage();
		exit(0);
	}

	while((c = getopt(argc,argv,"h?v")) != EOF)
		switch(c) {
			case 'v':
						printf(version0);
						printf(version);

						exit(0);
			case '?':
			case 'h':
			default:
						usage();
						exit(0);
		}

	if((hp = gethostbyname(argv[1])) == NULL) {
		fprintf(stderr,"%s: cannot get hostname %s\n",program_name,argv[1]);
		exit(-1);
	}

	bcopy(hp->h_addr,(caddr_t)&server_addr.sin_addr, hp->h_length);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = 0;

	if(pointopoint_v3(&server_addr) == 1)
		exit(0);
	clnt_pcreateerror(program_name);
	exit(EXIT_FAILURE);
}
