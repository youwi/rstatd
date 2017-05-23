cmake_minimum_required(VERSION 3.3)
project(rpc_rstatd_4_0_1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    autom4te.cache/output.0
    autom4te.cache/output.1
    autom4te.cache/requests
    autom4te.cache/traces.0
    autom4te.cache/traces.1
    aclocal.m4
    AUTHORS
    ChangeLog
    config.h
    config.h.in
    config.log
    config.status
    configure
    configure.in
    COPYING
    depcomp
    getdata.c
    getdata.c.orig
    getdata.h
    getdata22.c
    getdata22.h
    getdata24.c
    getdata24.h
    getdata26.c
    getdata26.h
    getdata_darwin.c
    getdata_darwin.h
    INSTALL
    install-sh
    Makefile
    Makefile.am
    Makefile.in
    missing
    mkinstalldirs
    README
    rpc.rstatd.8
    rpc.rstatd.plist
    rstat.h
    rstat.x
    rstat_clnt.c
    rstat_main.c
    rstat_main.c.orig
    rstat_proc.c
    rstat_proc.h
    rstat_svc.c
    rstat_xdr.c
    rstatd.8
    rsysinfo
    rsysinfo.1
    rsysinfo.c
    rup
    rup.1
    rup.c
    stamp-h1)

add_executable(rpc_rstatd_4_0_1 ${SOURCE_FILES})