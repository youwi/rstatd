#!/usr/bin/env bash
patch -p0 <patches/Makefile.am.patch
patch -p0 <patches/getdata.c.patch
patch -p0 <patches/rstat.x.patch
patch -p0 <patches/rstat_main.c.patch
patch -p0 <patches/rsysinfo.c.patch
patch -p0 <patches/rup.c.64.patch
patch -p0 <patches/rup.c.patch
