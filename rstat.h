/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _RSTAT_H_RPCGEN
#define _RSTAT_H_RPCGEN

#define RPCGEN_VERSION	199506

#include <rpc/rpc.h>

#define FSHIFT  8   /* bits to right of fixed binary point */
#define FSCALE  (1<<FSHIFT)
#define CPUSTATES 4
#define DK_NDRIVE 4

struct rstat_timeval {
	u_int tv_sec;
	u_int tv_usec;
};
typedef struct rstat_timeval rstat_timeval;
#ifdef __cplusplus
extern "C" bool_t xdr_rstat_timeval(XDR *, rstat_timeval*);
#elif __STDC__
extern  bool_t xdr_rstat_timeval(XDR *, rstat_timeval*);
#else /* Old Style C */
bool_t xdr_rstat_timeval();
#endif /* Old Style C */


struct statsusers {
	int cp_time[CPUSTATES];
	int dk_xfer[DK_NDRIVE];
	u_int v_pgpgin;
	u_int v_pgpgout;
	u_int v_pswpin;
	u_int v_pswpout;
	u_int v_intr;
	int if_ipackets;
	int if_ierrors;
	int if_oerrors;
	int if_collisions;
	u_int v_swtch;
	int avenrun[3];
	rstat_timeval boottime;
	rstat_timeval curtime;
	int if_opackets;
	u_int users;
};
typedef struct statsusers statsusers;
#ifdef __cplusplus
extern "C" bool_t xdr_statsusers(XDR *, statsusers*);
#elif __STDC__
extern  bool_t xdr_statsusers(XDR *, statsusers*);
#else /* Old Style C */
bool_t xdr_statsusers();
#endif /* Old Style C */


struct statsvar {
	int cp_time[CPUSTATES];
	int dk_xfer[DK_NDRIVE];
	u_int v_pgpgin;
	u_int v_pgpgout;
	u_int v_pswpin;
	u_int v_pswpout;
	u_int v_intr;
	int if_ipackets;
	int if_ierrors;
	int if_opackets;
	int if_oerrors;
	int if_collisions;
	u_int v_swtch;
	long avenrun[3];
	rstat_timeval boottime;
	rstat_timeval curtime;
};
typedef struct statsvar statsvar;
#ifdef __cplusplus
extern "C" bool_t xdr_statsvar(XDR *, statsvar*);
#elif __STDC__
extern  bool_t xdr_statsvar(XDR *, statsvar*);
#else /* Old Style C */
bool_t xdr_statsvar();
#endif /* Old Style C */


struct statstime {
	int cp_time[CPUSTATES];
	int dk_xfer[DK_NDRIVE];
	u_int v_pgpgin;
	u_int v_pgpgout;
	u_int v_pswpin;
	u_int v_pswpout;
	u_int v_intr;
	int if_ipackets;
	int if_ierrors;
	int if_oerrors;
	int if_collisions;
	u_int v_swtch;
	int avenrun[3];
	rstat_timeval boottime;
	rstat_timeval curtime;
	int if_opackets;
};
typedef struct statstime statstime;
#ifdef __cplusplus
extern "C" bool_t xdr_statstime(XDR *, statstime*);
#elif __STDC__
extern  bool_t xdr_statstime(XDR *, statstime*);
#else /* Old Style C */
bool_t xdr_statstime();
#endif /* Old Style C */


struct statsswtch {
	int cp_time[CPUSTATES];
	int dk_xfer[DK_NDRIVE];
	u_int v_pgpgin;
	u_int v_pgpgout;
	u_int v_pswpin;
	u_int v_pswpout;
	u_int v_intr;
	int if_ipackets;
	int if_ierrors;
	int if_oerrors;
	int if_collisions;
	u_int v_swtch;
	u_int avenrun[3];
	rstat_timeval boottime;
	int if_opackets;
};
typedef struct statsswtch statsswtch;
#ifdef __cplusplus
extern "C" bool_t xdr_statsswtch(XDR *, statsswtch*);
#elif __STDC__
extern  bool_t xdr_statsswtch(XDR *, statsswtch*);
#else /* Old Style C */
bool_t xdr_statsswtch();
#endif /* Old Style C */


struct stats {
	int cp_time[CPUSTATES];
	int dk_xfer[DK_NDRIVE];
	u_int v_pgpgin;
	u_int v_pgpgout;
	u_int v_pswpin;
	u_int v_pswpout;
	u_int v_intr;
	int if_ipackets;
	int if_ierrors;
	int if_oerrors;
	int if_collisions;
	int if_opackets;
};
typedef struct stats stats;
#ifdef __cplusplus
extern "C" bool_t xdr_stats(XDR *, stats*);
#elif __STDC__
extern  bool_t xdr_stats(XDR *, stats*);
#else /* Old Style C */
bool_t xdr_stats();
#endif /* Old Style C */


#define RSTATPROG ((rpc_uint)100001)
#define RSTATVERS_USERS ((rpc_uint)5)

#ifdef __cplusplus
#define RSTATPROC_STATS ((rpc_uint)1)
extern "C" statsusers * rstatproc_stats_5(void *, CLIENT *);
extern "C" statsusers * rstatproc_stats_5_svc(void *, struct svc_req *);
#define RSTATPROC_HAVEDISK ((rpc_uint)2)
extern "C" u_int * rstatproc_havedisk_5(void *, CLIENT *);
extern "C" u_int * rstatproc_havedisk_5_svc(void *, struct svc_req *);

#elif __STDC__
#define RSTATPROC_STATS ((rpc_uint)1)
extern  statsusers * rstatproc_stats_5(void *, CLIENT *);
extern  statsusers * rstatproc_stats_5_svc(void *, struct svc_req *);
#define RSTATPROC_HAVEDISK ((rpc_uint)2)
extern  u_int * rstatproc_havedisk_5(void *, CLIENT *);
extern  u_int * rstatproc_havedisk_5_svc(void *, struct svc_req *);

#else /* Old Style C */
#define RSTATPROC_STATS ((rpc_uint)1)
extern  statsusers * rstatproc_stats_5();
extern  statsusers * rstatproc_stats_5_svc();
#define RSTATPROC_HAVEDISK ((rpc_uint)2)
extern  u_int * rstatproc_havedisk_5();
extern  u_int * rstatproc_havedisk_5_svc();
#endif /* Old Style C */
#define RSTATVERS_VAR ((rpc_uint)4)

#ifdef __cplusplus
extern "C" statsvar * rstatproc_stats_4(void *, CLIENT *);
extern "C" statsvar * rstatproc_stats_4_svc(void *, struct svc_req *);
extern "C" u_int * rstatproc_havedisk_4(void *, CLIENT *);
extern "C" u_int * rstatproc_havedisk_4_svc(void *, struct svc_req *);

#elif __STDC__
extern  statsvar * rstatproc_stats_4(void *, CLIENT *);
extern  statsvar * rstatproc_stats_4_svc(void *, struct svc_req *);
extern  u_int * rstatproc_havedisk_4(void *, CLIENT *);
extern  u_int * rstatproc_havedisk_4_svc(void *, struct svc_req *);

#else /* Old Style C */
extern  statsvar * rstatproc_stats_4();
extern  statsvar * rstatproc_stats_4_svc();
extern  u_int * rstatproc_havedisk_4();
extern  u_int * rstatproc_havedisk_4_svc();
#endif /* Old Style C */
#define RSTATVERS_TIME ((rpc_uint)3)

#ifdef __cplusplus
extern "C" statstime * rstatproc_stats_3(void *, CLIENT *);
extern "C" statstime * rstatproc_stats_3_svc(void *, struct svc_req *);
extern "C" u_int * rstatproc_havedisk_3(void *, CLIENT *);
extern "C" u_int * rstatproc_havedisk_3_svc(void *, struct svc_req *);

#elif __STDC__
extern  statstime * rstatproc_stats_3(void *, CLIENT *);
extern  statstime * rstatproc_stats_3_svc(void *, struct svc_req *);
extern  u_int * rstatproc_havedisk_3(void *, CLIENT *);
extern  u_int * rstatproc_havedisk_3_svc(void *, struct svc_req *);

#else /* Old Style C */
extern  statstime * rstatproc_stats_3();
extern  statstime * rstatproc_stats_3_svc();
extern  u_int * rstatproc_havedisk_3();
extern  u_int * rstatproc_havedisk_3_svc();
#endif /* Old Style C */
#define RSTATVERS_SWTCH ((rpc_uint)2)

#ifdef __cplusplus
extern "C" statsswtch * rstatproc_stats_2(void *, CLIENT *);
extern "C" statsswtch * rstatproc_stats_2_svc(void *, struct svc_req *);
extern "C" u_int * rstatproc_havedisk_2(void *, CLIENT *);
extern "C" u_int * rstatproc_havedisk_2_svc(void *, struct svc_req *);

#elif __STDC__
extern  statsswtch * rstatproc_stats_2(void *, CLIENT *);
extern  statsswtch * rstatproc_stats_2_svc(void *, struct svc_req *);
extern  u_int * rstatproc_havedisk_2(void *, CLIENT *);
extern  u_int * rstatproc_havedisk_2_svc(void *, struct svc_req *);

#else /* Old Style C */
extern  statsswtch * rstatproc_stats_2();
extern  statsswtch * rstatproc_stats_2_svc();
extern  u_int * rstatproc_havedisk_2();
extern  u_int * rstatproc_havedisk_2_svc();
#endif /* Old Style C */
#define RSTATVERS_ORIG ((rpc_uint)1)

#ifdef __cplusplus
extern "C" stats * rstatproc_stats_1(void *, CLIENT *);
extern "C" stats * rstatproc_stats_1_svc(void *, struct svc_req *);
extern "C" u_int * rstatproc_havedisk_1(void *, CLIENT *);
extern "C" u_int * rstatproc_havedisk_1_svc(void *, struct svc_req *);

#elif __STDC__
extern  stats * rstatproc_stats_1(void *, CLIENT *);
extern  stats * rstatproc_stats_1_svc(void *, struct svc_req *);
extern  u_int * rstatproc_havedisk_1(void *, CLIENT *);
extern  u_int * rstatproc_havedisk_1_svc(void *, struct svc_req *);

#else /* Old Style C */
extern  stats * rstatproc_stats_1();
extern  stats * rstatproc_stats_1_svc();
extern  u_int * rstatproc_havedisk_1();
extern  u_int * rstatproc_havedisk_1_svc();
#endif /* Old Style C */

#endif /* !_RSTAT_H_RPCGEN */
