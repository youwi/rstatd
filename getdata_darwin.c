
#ifdef __APPLE__

#include <sys/types.h>
#include <sys/sysctl.h>
#include <syslog.h>
#include <utmpx.h>
#include <mach/host_info.h>

#include "getdata_darwin.h"

void get_net_darwin(struct statsusers *stats) {}
void get_disk_darwin(struct statsusers *s) {}

void get_vm_darwin(struct statsusers *s) {
    mach_port_t port;
    mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
    struct vm_statistics vm_info;
    kern_return_t ret;

    port = mach_host_self();
    ret = host_statistics(port, HOST_VM_INFO, (host_info_t)&vm_info, &count);
    if (ret == KERN_SUCCESS) {
        s->v_pgpgin = vm_info.pageins;
        s->v_pgpgout = vm_info.pageouts;
    } else {
        syslog(LOG_WARNING, "vm info not available: %i", ret);
    }
}

void get_cpu_darwin(struct statsusers *s) {
    mach_port_t port;
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    struct host_cpu_load_info cpu_info;
    kern_return_t ret;
    
    port = mach_host_self();
    ret = host_statistics(port, HOST_CPU_LOAD_INFO, (host_info_t)&cpu_info, &count);
    if (ret == KERN_SUCCESS) {
        // different order from linux
        s->cp_time[0] = cpu_info.cpu_ticks[CPU_STATE_USER];
        s->cp_time[1] = cpu_info.cpu_ticks[CPU_STATE_NICE];
        s->cp_time[2] = cpu_info.cpu_ticks[CPU_STATE_SYSTEM];
        s->cp_time[3] = cpu_info.cpu_ticks[CPU_STATE_IDLE];
    } else {
        syslog(LOG_WARNING, "cpu load not available: %i", ret);
    }
}

void get_load_darwin(struct statsusers *s) {
    double avrun[3];
    int i;
    
    if (getloadavg(avrun, 3) == -1) {
        syslog(LOG_WARNING, "load average not available\n");
    } else {
        for (i = 0; i < 3; i++) {
            s->avenrun[i] = avrun[i] * FSCALE;
        }
    }
}

void get_users_darwin(struct statsusers *s) {
    int users = 0;
    struct utmpx* u;
    setutxent();
    while (u = getutxent()) {
        if (u->ut_type == USER_PROCESS && u->ut_user[0] != '\0') {
            users ++;
        }
    }
    endutxent();
    s->users = users;
}

void get_uptime_darwin(struct statsusers *stats) {
    int mib[2];
    struct timeval boottime;
    size_t size;
        
    mib[0] = CTL_KERN;
    mib[1] = KERN_BOOTTIME;
    size = sizeof(boottime);
    gettimeofday((struct timeval *)&stats->curtime,
                 (struct timezone *)0);
    if (sysctl(mib, 2, &boottime, &size, NULL, 0) != -1 &&
        boottime.tv_sec != 0) {
        
        //stats->boottime.tv_sec = (stats->curtime.tv_sec - boottime.tv_sec);
        stats->boottime.tv_sec = boottime.tv_sec;
        stats->boottime.tv_usec = 0;
        return;
    }
    syslog(LOG_ERR, "bad kern.boottime");
    return;
}


#endif
