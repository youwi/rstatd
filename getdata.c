/*
 * getdata.c -- common data gathering functions
 *
 * Software distributed under the GPL, for license terms see the file
 * COPYING in this directory
 *
 * (c) 2005 Dr. Andreas Mueller, Beratung und Entwicklung
 * $Id: getdata.c,v 1.3 2005/09/19 07:20:33 afm Exp $
 */
#include <getdata.h>
#include <sys/utsname.h>
#include <getdata22.h>
#include <getdata24.h>
#include <getdata26.h>
#include <getdata_darwin.h>
#include <sys/types.h>
#include <regex.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <utmp.h>
#include <paths.h>
#include <stdlib.h>
#include <stdio.h>

	#define	PATTERNS	6
char	*patterns[PATTERNS] = {
		"^((da|)s|h)d[a-z][a-z]*$",
		"^scd[0-9]$",
		"^(rd|ida|cciss)/c[0-9][0-9]*d[0-9]*$",
		"^i2o/hd[a-z]*$",
		"^amiradi/ar[0-9]*$",
		"^carmel/[0-9][0-9]*$"
	};
#define	MAXINT	0x80000000
static regex_t	*rx[PATTERNS];
extern int ninterfaces;
extern char **interfaces;

/* find out whether a device with a given name is a disk */
int	isdisk(const char *device) {
	int	i;
	for (i = 0; i < PATTERNS; i++) {
		if (0 == regexec(rx[i], device, 0, NULL, 0)) {
			return 1;
		}
	}
	return 0;
}

/* dummy functions */
static void	get_disk(struct statsusers *s) {
	syslog(LOG_CRIT, "dummy function, should never be called!");
	exit(EXIT_FAILURE);
}

/* cpu is identical in all version of linux */
static void	get_cpu(struct statsusers *s) {
	FILE	*stat;
	char	buf[1024];
	unsigned long long	cp[4];
	unsigned long long	l;
	int	i;

	if (NULL == (stat = fopen("/proc/stat", "r"))) {
		syslog(LOG_CRIT, "/proc/stat not found");
		exit(EXIT_FAILURE);
	}
	while (fgets(buf, sizeof(buf), stat)) {
		if (0 == strncmp(buf, "cpu ", 4)) {
			if (4 != sscanf(buf, "cpu %llu %llu %llu %llu\n",
				&cp[0], &cp[1],&cp[2], &cp[3])) {
				syslog(LOG_CRIT, "wrong format of cpu line");
				exit(EXIT_FAILURE);
			}
			/* convert the cp data to normal longs */
			for (i = 0; i < 4; i++) {
				s->cp_time[i] = cp[i] % MAXINT;
			}
		}
		if (0 == strncmp(buf, "intr ", 5)) {
			if (1 != sscanf(buf, "intr %llu ", &l)) {
				syslog(LOG_CRIT, "wrong format of intr line");
				exit(EXIT_FAILURE);
			}
			s->v_intr = l % MAXINT;
		}
		if (0 == strncmp(buf, "ctxt ", 5)) {
			if (1 != sscanf(buf, "ctxt %llu\n", &l)) {
				syslog(LOG_CRIT, "wrong format of ctxt line");
				exit(EXIT_FAILURE);
			}
			s->v_swtch = l % MAXINT;
		}
	} /* while */
	fclose(stat);
}

static void	get_vm(struct statsusers *s) {
	FILE	*stat;
	char	buf[1024];
	if (NULL == (stat = fopen("/proc/stat", "r"))) {
		syslog(LOG_CRIT, "/proc/stat not found");
		exit(EXIT_FAILURE);
	}
	while (fgets(buf, sizeof(buf), stat)) {
		if (0 == strncmp(buf, "page ", 5)) {
			if (2 != sscanf(buf, "page %u %u \n",
				&s->v_pgpgin, &s->v_pgpgout)) {
				syslog(LOG_CRIT, "wrong format of page line");
				exit(EXIT_FAILURE);
			}
		}

		if (0 == strncmp(buf, "swap ", 5)) {
			if (2 != sscanf(buf, "swap %u %u",
				&s->v_pswpin, &s->v_pswpout)) {
				syslog(LOG_CRIT, "wrong format of swap line");
				exit(EXIT_FAILURE);
			}
		}
	} /* while */
	fclose(stat);
}

static void	get_load(struct statsusers *stats) {
	FILE	*fp;
	double	avrun[3];
	if (NULL == (fp = fopen("/proc/loadavg", "r"))) {
		return;
	}
	if (!fscanf(fp , "%lf %lf %lf", &avrun[0], &avrun[1], &avrun[2])) {
		syslog(LOG_CRIT, "unexpected data format in /proc/loadavg");
		exit(EXIT_FAILURE);
	}
	stats->avenrun[0] = avrun[0] * FSCALE;
	stats->avenrun[1] = avrun[1] * FSCALE;
	stats->avenrun[2] = avrun[2] * FSCALE;
	fclose(fp);
}

/* make sure _PATH_UTMP is defined, some version may lack a definition
   in their header files */
#ifndef _PATH_UTMP
#define _PATH_UTMP "/var/adm/utmp"
#endif

static void	get_users(struct statsusers *s) {
#ifdef __APPLE__
        return;
#else
	int ufd, users = 0;
	struct utmp u;

	if ((ufd = open(_PATH_UTMP, O_RDONLY)) < 0) {
		syslog(LOG_CRIT, "cannot open UTMP file %s", _PATH_UTMP);
	}
	while (read(ufd, (char *) &u, sizeof(u)) == sizeof(u)) {
		if(u.ut_type == USER_PROCESS && u.ut_name[0] != '\0') {
			users++;
		}
	}
	(void)close(ufd);
	s->users = users;
#endif
}

static void	get_uptime(struct statsusers *stats) {
	int fd;
	double up_secs, idle_secs;
	char buf[BUFSIZ];

	if ((fd = open("/proc/uptime", O_RDONLY)) < 0) {
		syslog(LOG_CRIT, "no /proc/uptime file");
		exit(EXIT_FAILURE);
	}
	if ((read(fd,(char *)&buf,sizeof(buf))) < 0) {
		syslog(LOG_CRIT, "unexpected /proc/uptime content");
		exit(EXIT_FAILURE);
	}
	(void)close(fd);
	if(sscanf(buf,"%lf %lf", &up_secs, &idle_secs) < 2) {
		syslog(LOG_CRIT, "bad /proc/uptime");
		exit(EXIT_FAILURE);
	}
	gettimeofday((struct timeval *)&stats->curtime,
		(struct timezone *)0);
	stats->boottime.tv_sec = (stats->curtime.tv_sec - up_secs);
	stats->boottime.tv_usec = 0;
}

static void	get_net(struct statsusers *stats) {
	int	i;
	FILE	*fp;
	char	buf[1024];
	char	*bufp, *iface;
	int	found_interface;
	unsigned int	if_ipackets, if_ierrors,
			if_opackets, if_oerrors, if_collisions;

	if ((fp = fopen("/proc/net/dev","r")) == NULL) {
		syslog(LOG_CRIT, "no /proc/net/dev");
		exit(EXIT_FAILURE);
	}

	/* gobble up the two header lines in /proc/net/dev */
	fgets(buf, sizeof(buf), fp);
	fgets(buf, sizeof(buf), fp);

	/* add up data from all interfaces besides the loopback interface.
	If interfaces where specified on the commandline take only these. */
	while (fgets(buf, sizeof(buf), fp)) {
		bufp = buf;

		while (*bufp == ' ') *bufp++; /* find beginning of interface
						name, by skipping all blanks */
		if (*bufp == '\0')
			continue;  /* skip empty line */
		iface = bufp;
		syslog(LOG_DEBUG, "interface: %s", iface);

		/* search for the end of the interface name (':' or \0) */
		while (*bufp != ':' && *bufp != '\0') bufp++;
		if (*bufp == '\0') {
			/* no ':', no interface name?! */
			syslog(LOG_CRIT, "no interface name");
			exit(EXIT_FAILURE);
		}
		*bufp = '\0'; /* mark the end of the interface name
				(Access is via iface) */

		bufp++;	/* work on the rest of the interface line */

		if (sscanf(bufp,"%*u %u %u %*u %*u %*u %*u %*u %*u %u %u "
			"%*u %*u %u %*u %*u\n", &if_ipackets, &if_ierrors,
			&if_opackets, &if_oerrors, &if_collisions) != 5) {
			syslog(LOG_CRIT, "incorrect format for interface %s",
				iface);
			exit(EXIT_FAILURE);
		}

		found_interface = 0;
		for (i = 0; i < ninterfaces; i++) {
			if (strcmp(iface, interfaces[i]) == 0) {
				found_interface = 1;
				break;
			}
		}
		/* skip lo interfaces */
		stats->if_ipackets = 0;
		stats->if_ierrors = 0;
		stats->if_opackets = 0;
		stats->if_oerrors = 0;
		stats->if_collisions = 0;
		if ((ninterfaces == 0 && strcmp(iface, "lo") != 0) ||
			found_interface) {
			stats->if_ipackets += if_ipackets;
			stats->if_ierrors  += if_ierrors;
			stats->if_opackets += if_opackets;
			stats->if_oerrors += if_oerrors;
			stats->if_collisions += if_collisions;
		}
	}

	fclose(fp);
}

getdata_t	getdata;

void	getdata_setup() {
	struct utsname	u;
	int i;

	/* set the default values */
	getdata.disk = get_disk;
	getdata.load = get_load;
	getdata.cpu = get_cpu;
	getdata.net = get_net;
	getdata.uptime = get_uptime;
	getdata.users = get_users;
	getdata.vm = get_vm;

	/* find out about the current kernel version */
	memset(&u, sizeof(u), 0);
	uname(&u);
        if (strcasecmp(u.sysname, "Darwin") == 0) {
            getdata.net = get_net_darwin;
            getdata.cpu = get_cpu_darwin;
            getdata.disk = get_disk_darwin;
            getdata.uptime = get_uptime_darwin;
            getdata.vm = get_vm_darwin;
            getdata.users = get_users_darwin;
            getdata.load = get_load_darwin;
        } else {
	if (0 == strncmp(u.release, "2.2", 3)) {
		getdata.disk = get_disk22;
	}
	if (0 == strncmp(u.release, "2.4", 3)) {
		getdata.disk = get_disk24;
	}
	if (0 == strncmp(u.release, "2.6", 3)) {
		getdata.disk = get_disk26;
		getdata.vm = get_vm26;
	}
        }
	/* prepare the disk patterns */
	for (i = 0; i < PATTERNS; i++) {
		rx[i] = (regex_t *)malloc(sizeof(regex_t));
		memset(rx[i], sizeof(regex_t), 0);
		regcomp(rx[i], patterns[i], REG_EXTENDED | REG_NOSUB);
	}
}

void	getdata_collect(struct statsusers *s) {
	memset(s, sizeof(struct statsusers), 0);
	getdata.net(s);
	getdata.cpu(s);
	getdata.load(s);
	getdata.disk(s);
	getdata.uptime(s);
	getdata.users(s);
	getdata.vm(s);
}

