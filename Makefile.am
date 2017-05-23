## Process this file with automake to produce Makefile.in
#
# Software distributed under the GPL, for license terms see the file
# COPYING in this directory
#
# $Id: Makefile.am,v 1.4 2005/09/19 07:20:33 afm Exp $
#

AUTOMAKE_OPTIONS = foreign no-installman

bin_PROGRAMS = rup rsysinfo

sbin_PROGRAMS = rpc.rstatd

man_MANS = rpc.rstatd.8 rstatd.8 rsysinfo.1 rup.1

EXTRA_DIST = rstat.x COPYING INSTALL $(man_MANS)

rup_SOURCES = rstat.h rup.c rstat_xdr.c rstat_clnt.c

rsysinfo_SOURCES = rstat.h rsysinfo.c rstat_xdr.c rstat_clnt.c

rpc_rstatd_SOURCES = rstat.h rstat_svc.c rstat_xdr.c rstat_proc.c \
	rstat_main.c getdata.c getdata22.c getdata24.c getdata26.c getdata_darwin.c \
	config.h getdata22.h getdata24.h getdata26.h getdata_darwin.h getdata.h rstat_proc.h

CLEANFILES = rstat.h rstat_svc.c rstat_clnt.c rstat_xdr.c

BUILT_SOURCES = rstat.h

DISTCLEANFILES = @CLEANFILES@

rup.c rstat_proc.c: rstat.h

rstat.h: rstat.x
	-rm -f $@
	rpcgen -h -o $@ rstat.x

rstat_svc.c: rstat.x
	-rm -f $@
	rpcgen -m -o $@ rstat.x

rstat_clnt.c: rstat.x
	-rm -f $@
	rpcgen -l -o $@ rstat.x

rstat_xdr.c: rstat.x
	-rm -f $@
	rpcgen -c -o $@ rstat.x

dist-hook:
	-rm -f $(distdir)/rstat_{svc,clnt,xdr}.c $(distdir)/rstat.h
