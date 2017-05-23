
#ifndef GETDATA_DARWIN
#define GETDATA_DARWIN

#include "getdata.h"

extern void get_net_darwin(struct statsusers *stats);
extern void get_cpu_darwin(struct statsusers *s);
extern void get_disk_darwin(struct statsusers *s);
extern void get_uptime_darwin(struct statsusers *stats);
extern void get_vm_darwin(struct statsusers *s);
extern void get_users_darwin(struct statsusers *s);
extern void get_load_darwin(struct statsusers *s);
    
#endif
