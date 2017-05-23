/*
 *  rup version 3.03 rpc ruptime client for use with rpc.rstatd 3.03
 *  Copyright (C) 1995  Adam Migus, Memorial University of Newfoundland
 *	(MUN)
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
 *    amigus@ucs.mun.ca   
 *
 **************************************************************************/

#include <stdio.h>
#include <rpc/rpc.h>
#include <rpc/clnt.h>
#include <rpc/pmap_clnt.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "rstat.h"
#include <stdlib.h>

void bzero();
void bcopy();
char *rindex();

char *version = "@(#)rup version 3.03 Copyright (Adam Migus) 1/9/96\n";
char *program_name;

struct host_type {
	struct in_addr host_addr;
	struct host_type *next;
};

struct host_type *host_list;

int save_hostname(struct in_addr new_addr)
{
	struct host_type *new_item;

	if(!(new_item=(struct host_type *)malloc(sizeof(struct host_type)))) {
		fprintf(stderr,"%s: out of memory...\n",program_name);
		return(0);
	}
	new_item->host_addr.s_addr = new_addr.s_addr;
	new_item->next = host_list;
	host_list = new_item;
	return(1);
}
int find_hostname(struct in_addr the_addr)
{
	struct host_type *current_item;

	current_item = host_list;
	while(current_item != NULL) {
		if(current_item->host_addr.s_addr == the_addr.s_addr) {
			return(1);
		}
	current_item= current_item->next;
	}
	return(0);
}

int	rup_answer_v5(struct statsusers *replyp, struct sockaddr_in *raddrp)
{
	struct tm *tmp_time;
	struct hostent *hp;
	char *host;
	struct statsusers *host_stat;
	struct tm host_time;
	struct tm host_uptime;
	char days_buf[16];
	char hours_buf[16];
	time_t atime;

	if(find_hostname(raddrp->sin_addr)) {
		return(0);
	}

	save_hostname(raddrp->sin_addr);

	host_stat = (statsusers *)replyp;

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

	printf("%-*.*s  up %9.9s%9.9s%i users, load average: %.2f %.2f %.2f\n",
		21,21,host,days_buf,hours_buf,
		host_stat->users,
		(double)host_stat->avenrun[0]/FSCALE,
		(double)host_stat->avenrun[1]/FSCALE,
		(double)host_stat->avenrun[2]/FSCALE);

	return(0);
}
/* This function is mostly hacked NetBSD code */
int	rup_answer_v3(struct statstime *replyp, struct sockaddr_in *raddrp)
{

	struct tm *tmp_time;
	struct hostent *hp;
	char *host;
	struct statstime *host_stat;
	struct tm host_time;
	struct tm host_uptime;
	char days_buf[16];
	char hours_buf[16];

	if(find_hostname(raddrp->sin_addr)) {
		return(0);
	}

	save_hostname(raddrp->sin_addr);

	host_stat = (statstime *)replyp;

	hp = gethostbyaddr((char *)&raddrp->sin_addr.s_addr,
			   sizeof(struct in_addr), AF_INET);
	if (hp)
		host = (char *)hp->h_name;
	else
		host = (char *)inet_ntoa(raddrp->sin_addr);

	tmp_time = localtime((time_t *)&host_stat->curtime.tv_sec);
	host_time = *tmp_time;

	host_stat->curtime.tv_sec -= host_stat->boottime.tv_sec;

	tmp_time = gmtime((time_t *)&host_stat->curtime.tv_sec);
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

	printf("%-*.*s  up %9.9s%9.9s load average: %.2f %.2f %.2f\n",
		21,21,host,days_buf,hours_buf,
		(double)host_stat->avenrun[0]/FSCALE,
		(double)host_stat->avenrun[1]/FSCALE,
		(double)host_stat->avenrun[2]/FSCALE);

	return(0);
}
int	broadcast_v5()
{
	struct statsusers host_stat;
	enum clnt_stat clnt_stat;

	clnt_stat = clnt_broadcast(RSTATPROG, RSTATVERS_USERS, RSTATPROC_STATS,
		(xdrproc_t) xdr_void, 0, (void *)xdr_statsusers,
		(void *)&host_stat, (void *)rup_answer_v5);
	if (clnt_stat != RPC_SUCCESS && clnt_stat != RPC_TIMEDOUT) {
		fprintf(stderr, "rup: %s\n", clnt_sperrno(clnt_stat));
		exit(1);
	}
	exit(0);
}
int	broadcast_v3()
{
	struct statstime host_stat;
	enum clnt_stat clnt_stat;

	clnt_stat = clnt_broadcast(RSTATPROG, RSTATVERS_TIME, RSTATPROC_STATS,
		(xdrproc_t)xdr_void, 0, (void *)xdr_statstime,
		(void *)&host_stat, (void *)rup_answer_v3);
	if (clnt_stat != RPC_SUCCESS && clnt_stat != RPC_TIMEDOUT) {
		fprintf(stderr, "rup: %s\n", clnt_sperrno(clnt_stat));
		exit(1);
	}
	exit(0);
}
int pointopoint_v5(struct sockaddr_in *server_addr)
{
	struct statsusers host_stat;
	struct statsusers *s;
	struct timeval pertry_timeout, total_timeout;
	enum clnt_stat clnt_stat;
	int sock=RPC_ANYSOCK;
	CLIENT *client;

	pertry_timeout.tv_sec = 3;
	pertry_timeout.tv_usec = 0;
	total_timeout.tv_sec = 10;
	total_timeout.tv_usec = 0;
	s = &host_stat;

	if((client = clntudp_create(server_addr, RSTATPROG, RSTATVERS_USERS,
	pertry_timeout, &sock)) == NULL) {
		close(sock);
		return(0);
	}

	clnt_stat = clnt_call(client, RSTATPROC_STATS, (xdrproc_t) xdr_void,0,
	(xdrproc_t) xdr_statsusers, (void *)&host_stat, total_timeout);

	if(clnt_stat == RPC_SUCCESS) {
		rup_answer_v5(&host_stat, server_addr);
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

int pointopoint_v3(struct sockaddr_in *server_addr)
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
	clnt_stat = clnt_call(client, RSTATPROC_STATS, (xdrproc_t)xdr_void, 0,
		(xdrproc_t)xdr_statstime, (void *)&host_stat, total_timeout);

	if(clnt_stat == RPC_SUCCESS) {
		rup_answer_v3(&host_stat, server_addr);
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
	printf("usage: %s \"host\" -u -v\n"\
			"\t-u\tbroadcast request for info including users.\n"\
			"\t-v\tprint version and exit\n", program_name);
}

int main(int argc, char **argv)
{
	struct hostent *hp;
	struct sockaddr_in server_addr;
	char c;

	if(!(program_name = rindex(argv[0],'/'))) 
		program_name = argv[0];
	else 
		program_name++;
	
	if(argc == 1) {
		broadcast_v3();
	}

	while((c = getopt(argc,argv,"h?uv")) != EOF)
		switch(c) {
			case 'v':
						printf("%s", version);
						exit(0);
			case 'u':
						broadcast_v5();
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

	if(pointopoint_v5(&server_addr) == 1)
		exit(EXIT_SUCCESS);
	else if(pointopoint_v3(&server_addr) == 1)
		exit(EXIT_SUCCESS);

	clnt_pcreateerror(program_name);
	exit(EXIT_FAILURE);
}
