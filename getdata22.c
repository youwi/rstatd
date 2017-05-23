/*
 * getdata22.c -- implementations of kernel 2.2 specific data retrieval functions
 *
 * software distributed under the GPL, for license terms see the file
 * COPYING in this directory
 *
 * (c) 2005 Dr. Andreas Mueller, Beratung und Entwicklung
 * $Id: getdata22.c,v 1.2 2005/09/19 07:20:33 afm Exp $
 */
#include "getdata22.h"
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>

/*
The 2.2 kernel has disk activity as in four numbers found in /etc/stat
on the line beginning with 'disk '.
*/

void	get_disk22(struct statsusers *stats) {
	FILE	*stat;
	char	buffer[1024];
	/* open the /proc/partitions file */
	stat = fopen("/proc/stat", "r");
	if (NULL == stat) {
		return;
	}

	/* use the common function to read diskstat data */
	while (buffer == fgets(buffer, sizeof(buffer), stat)) {
		if (0 == strncmp(buffer, "disk ", 5)) {
			if (sscanf(buffer, "disk %u %u %u %u\n",
				&stats->dk_xfer[0],
				&stats->dk_xfer[1],
				&stats->dk_xfer[2],
				&stats->dk_xfer[3]) != 4) {
				syslog(LOG_CRIT, "/proc/stat has wrong format "
					"on disk line: %s", buffer);
				exit(EXIT_FAILURE);
			}
			break;	/* exit the loop */
		}
	}

	/* close the file */
	fclose(stat);
}
