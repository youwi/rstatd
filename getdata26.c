/*
 * getdata26.c -- implementations of kernel 2.4 specific data retrieval functions
 *
 * software distributed under the GPL, for license terms see the file
 * COPYING in this directory
 *
 * (c) 2005 Dr. Andreas Mueller, Beratung und Entwicklung
 * $Id: getdata26.c,v 1.2 2005/09/19 07:20:33 afm Exp $
 */
#include "getdata.h"
#include "getdata26.h"
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

/*
The 2.6 kernel moves disk statistics information from /proc/partitions
to /proc/diskstat, and removes the two line header
*/

void	get_disk26(struct statsusers *s) {
	FILE	*diskstat;
	unsigned long long	rio;
	unsigned long long	wio;
	char	line[1024];
	char	device[128];
	int	diskno;

	/* open the /proc/diskstat file */
	diskstat = fopen("/proc/diskstats", "r");
	if (NULL == diskstat) {
		return;
	}

	/* accumulate all the io statistics */
	diskno = 0;
	while ((diskno < 4) && (line == fgets(line, sizeof(line), diskstat))) {
		sscanf(line, " %*d %*d %s "
			"%lld %*d %*d %*d "
			"%lld %*d %*d %*d "
			"%*d %*d %*d\n",
			device, &rio, &wio);
		if (isdisk(device)) {
			s->dk_xfer[diskno++] = rio + wio;
		}
	}

	/* close the file */
	fclose(diskstat);
}

void	get_vm26(struct statsusers *s) {
	char	buf[1024];
	FILE	*vmstat;

	/* open the /proc/vmstat file */
	vmstat = fopen("/proc/vmstat", "r");
	if (NULL == vmstat) {
		return;
	}

	/* read the data from vmstat into the statsusers structure */
	while (buf == fgets(buf, sizeof(buf), vmstat)) {
		if (0 == strncmp(buf, "pgpgin", 6)) {
			if (1 != sscanf(buf, "pgpgin %u", &s->v_pgpgin)) {
				syslog(LOG_CRIT, "bad format in /proc/vmstat");
				exit(EXIT_FAILURE);
			}
		}
		if (0 == strncmp(buf, "pgpgout", 7)) {
			if (1 != sscanf(buf, "pgpgout %u", &s->v_pgpgout)) {
				syslog(LOG_CRIT, "bad format in /proc/vmstat");
				exit(EXIT_FAILURE);
			}
		}
		if (0 == strncmp(buf, "pswpin", 6)) {
			if (1 != sscanf(buf, "pswpin %u", &s->v_pswpin)) {
				syslog(LOG_CRIT, "bad format in /proc/vmstat");
				exit(EXIT_FAILURE);
			}
		}
		if (0 == strncmp(buf, "pswpout", 7)) {
			if (1 != sscanf(buf, "pswpout %u", &s->v_pswpout)) {
				syslog(LOG_CRIT, "bad format in /proc/vmstat");
				exit(EXIT_FAILURE);
			}
		}
	}

	/* close the file */
	fclose(vmstat);
}
