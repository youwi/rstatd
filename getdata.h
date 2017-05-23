/*
 * getdata.h -- declarations for data retrieval functions
 *
 * Software distributed under the GPL, for license terms see the file
 * COPYING in this directory
 *
 * (c) 2005 Dr. Andreas Mueller, Beratung und Entwicklung
 * $Id: getdata.h,v 1.2 2005/09/19 07:20:33 afm Exp $
 */
#ifndef _GETDATA_H
#define _GETDATA_H

#include "rstat.h"

typedef struct getdata_s {
	void	(*net)(struct statsusers *);
	void	(*cpu)(struct statsusers *);
	void	(*load)(struct statsusers *);
	void	(*disk)(struct statsusers *);
	void	(*uptime)(struct statsusers *);
	void	(*users)(struct statsusers *);
	void	(*vm)(struct statsusers *);
} getdata_t;

extern getdata_t	getdata;

/*
 * depending on the kernel version, use different 
 * data gathering methods, the getdata_setup method
 * makes sure the right ones are linked in the getdata 
 * structure.
 */
extern void	getdata_setup();
extern void	getdata_collect(struct statsusers *);

/*
 * functions for internal use by the getdata methods
 */
extern int	isdisk(const char *device);

#endif /* _GETDATA_H */
