/*
 * $Id: rstat_proc.h,v 1.1 2005/09/07 08:07:28 afm Exp $
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
#ifndef _RSTAT_PROC_H
#define _RSTAT_PROC_H

extern int	havedisk (struct statsusers *stats);
extern void	rstat_service (struct svc_req *rqstp, SVCXPRT * transp);

extern int inetd_connect;
extern int ninterfaces;
extern char **interfaces;
extern int	debug;

#endif /* _RSTAT_PROC_H */
