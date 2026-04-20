#ifndef SYSMON_H
#define SYSMON_H

#define MAX_PROCESSES 10
#define MAX_NAME_LEN  64
#define MAX_MODEL_LEN 128

/* ── CPU ─────────────────────────────────────────── */
typedef struct {
    float  usage_percent;
    int    cores;
    char   model[MAX_MODEL_LEN];
} CpuInfo;

/* ── Memory ──────────────────────────────────────── */
typedef struct {
    unsigned long total;      /* kB */
    unsigned long available;  /* kB */
    unsigned long cached;     /* kB */
} MemInfo;

/* ── Disk ────────────────────────────────────────── */
typedef struct {
    unsigned long total;   /* kB */
    unsigned long free;    /* kB */
} DiskInfo;

/* ── Process ─────────────────────────────────────── */
typedef struct {
    int   pid;
    char  name[MAX_NAME_LEN];
    float cpu_percent;
    float mem_percent;
} ProcessInfo;

/* ── Function Declarations ───────────────────────── */
void get_cpu_info(CpuInfo *cpu);
void get_mem_info(MemInfo *mem);
void get_disk_info(DiskInfo *disk);
void get_uptime(long *uptime_seconds);
int  get_top_processes(ProcessInfo *procs, int max);

#endif /* SYSMON_H */
