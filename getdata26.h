/*
 * getdata26.h -- declarations for data retrieval functions
 *                for the 2.4 kernel
 *
 * Software distributed under the GPL, for license terms see the file
 * COPYING in the current directory
 *
 * (c) 2005 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _GETDATA26_H
#define _GETDATA26_H

#include "getdata.h"

extern void get_disk26(struct statsusers *s);
extern void get_vm26(struct statsusers *s);

#endif /* _GETDATA26_H */
