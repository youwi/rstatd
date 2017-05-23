/*
 * getdata24.c -- implementations of kernel 2.4 specific data retrieval functions
 *
 * software distributed under the GPL, for license terms see the file
 * COPYING in this directory
 *
 * (c) 2005 Dr. Andreas Mueller, Beratung und Entwicklung
 * $Id: getdata24.c,v 1.2 2005/09/19 07:20:33 afm Exp $
 */
#include "getdata24.h"
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

/*
The 2.4 kernel keeps disk statistics information in /proc/partitions
in the following format:

major		device major number
minor		device minor number
#blocks		device size in blocks
name		device name (without /dev/ prefix)
rio		read io operations since boot
rmerge		read merges
rsect		sector reads
ruse		miliseconds the disk was in use
wio		write io operations since boot
wmerge		write merge operations since boot
wsect		sector writes since boot
wuse		time disk was in use for write operations
running		number of in flight operations
use		time in use (read and write)
aveq		average queue length (ms) = Integral of number of operations
		over time (in ms)

To compute the io statistics for a disk, use the rio and wio operations,
as they are closest to what we used to know as unix io operations. Use
the wsect and rsect numbers to report block reads/writes.

*/

void	get_disk24(struct statsusers *s) {
	FILE	*partitions;
	char	buffer[128];
	unsigned long long	rio;
	unsigned long long	wio;
	char	line[1024];
	char	device[128];
	int	diskno;

	/* open the /proc/partitions file */
	partitions = fopen("/proc/partitions", "r");
	if (NULL == partitions) {
		return;
	}

	/* skip two lines */
	fgets(buffer, sizeof(buffer), partitions);	/* 1. line */
	fgets(buffer, sizeof(buffer), partitions);	/* 2. line */

	/* accumulate all the io statistics */
	diskno = 0;
	while ((diskno < 4) && (line == fgets(line, sizeof(line), partitions))) {
		sscanf(line, " %*d %*d %*d %s "
			"%lld %*d %*d %*d "
			"%lld %*d %*d %*d "
			"%*d %*d %*d\n",
			device, &rio, &wio);
		if (isdisk(device)) {
			s->dk_xfer[diskno++] = rio + wio;
		}
	}

	/* close the file */
	fclose(partitions);
}
