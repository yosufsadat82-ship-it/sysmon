#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/statvfs.h>
#include "../include/sysmon.h"

/* ── CPU ─────────────────────────────────────────── */
static long long prev_idle = 0, prev_total = 0;

void get_cpu_info(CpuInfo *cpu) {
    FILE *f;
    char line[256];
    long long user, nice, system, idle, iowait, irq, softirq, steal;
    long long total, idle_time, diff_idle, diff_total;

    /* Usage from /proc/stat */
    f = fopen("/proc/stat", "r");
    if (f) {
        fgets(line, sizeof(line), f);
        sscanf(line, "cpu %lld %lld %lld %lld %lld %lld %lld %lld",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
        fclose(f);

        idle_time  = idle + iowait;
        total      = user + nice + system + idle + iowait + irq + softirq + steal;
        diff_idle  = idle_time - prev_idle;
        diff_total = total - prev_total;

        if (diff_total > 0)
            cpu->usage_percent = (float)(diff_total - diff_idle) / diff_total * 100.0f;
        else
            cpu->usage_percent = 0.0f;

        prev_idle  = idle_time;
        prev_total = total;
    }

    /* Core count */
    cpu->cores = 0;
    f = fopen("/proc/cpuinfo", "r");
    if (f) {
        cpu->model[0] = '\0';
        while (fgets(line, sizeof(line), f)) {
            if (strncmp(line, "processor", 9) == 0) cpu->cores++;
            if (strncmp(line, "model name", 10) == 0 && cpu->model[0] == '\0') {
                char *colon = strchr(line, ':');
                if (colon) {
                    colon += 2;
                    strncpy(cpu->model, colon, MAX_MODEL_LEN - 1);
                    cpu->model[MAX_MODEL_LEN - 1] = '\0';
                    /* trim newline */
                    cpu->model[strcspn(cpu->model, "\n")] = '\0';
                    /* truncate for display */
                    if (strlen(cpu->model) > 35) cpu->model[35] = '\0';
                }
            }
        }
        fclose(f);
    }
    if (cpu->cores == 0) cpu->cores = 1;
    if (cpu->model[0] == '\0') strncpy(cpu->model, "Unknown", MAX_MODEL_LEN);
}

/* ── Memory ──────────────────────────────────────── */
void get_mem_info(MemInfo *mem) {
    FILE *f = fopen("/proc/meminfo", "r");
    char line[128];
    mem->total = mem->available = mem->cached = 0;

    if (!f) return;
    while (fgets(line, sizeof(line), f)) {
        unsigned long val;
        if (sscanf(line, "MemTotal: %lu kB", &val) == 1)     mem->total     = val;
        if (sscanf(line, "MemAvailable: %lu kB", &val) == 1) mem->available = val;
        if (sscanf(line, "Cached: %lu kB", &val) == 1)       mem->cached    = val;
    }
    fclose(f);
}

/* ── Disk ────────────────────────────────────────── */
void get_disk_info(DiskInfo *disk) {
    struct statvfs st;
    if (statvfs("/", &st) == 0) {
        unsigned long bsize = st.f_bsize / 1024;  /* bytes -> kB factor */
        disk->total = (st.f_blocks * bsize);
        disk->free  = (st.f_bfree  * bsize);
    } else {
        disk->total = disk->free = 0;
    }
}

/* ── Uptime ──────────────────────────────────────── */
void get_uptime(long *uptime_seconds) {
    FILE *f = fopen("/proc/uptime", "r");
    if (f) {
        double up;
        fscanf(f, "%lf", &up);
        *uptime_seconds = (long)up;
        fclose(f);
    } else {
        *uptime_seconds = 0;
    }
}

/* ── Top Processes ───────────────────────────────── */
static int cmp_cpu(const void *a, const void *b) {
    const ProcessInfo *pa = (const ProcessInfo *)a;
    const ProcessInfo *pb = (const ProcessInfo *)b;
    return (pb->cpu_percent > pa->cpu_percent) ? 1 : -1;
}

int get_top_processes(ProcessInfo *procs, int max) {
    DIR *dir = opendir("/proc");
    struct dirent *entry;
    int count = 0;
    char path[64], comm[64], state;
    long utime, stime;
    unsigned long vsize;
    long rss;
    int pid;

    if (!dir) return 0;

    MemInfo mem;
    get_mem_info(&mem);
    long page_kb = sysconf(_SC_PAGE_SIZE) / 1024;

    while ((entry = readdir(dir)) != NULL && count < max) {
        /* only numeric directories = processes */
        int is_pid = 1;
        for (int i = 0; entry->d_name[i]; i++)
            if (!isdigit((unsigned char)entry->d_name[i])) { is_pid = 0; break; }
        if (!is_pid) continue;

        pid = atoi(entry->d_name);
        snprintf(path, sizeof(path), "/proc/%d/stat", pid);
        FILE *f = fopen(path, "r");
        if (!f) continue;

        char buf[512];
        if (fgets(buf, sizeof(buf), f)) {
            /* parse: pid (comm) state ... utime stime ... vsize rss */
            char *start = strchr(buf, '(');
            char *end   = strrchr(buf, ')');
            if (start && end && end > start) {
                int namelen = (int)(end - start - 1);
                if (namelen >= MAX_NAME_LEN) namelen = MAX_NAME_LEN - 1;
                strncpy(comm, start + 1, namelen);
                comm[namelen] = '\0';

                long dummy;
                sscanf(end + 2,
                    "%c %d %d %d %d %d %lu %lu %lu %lu %lu %ld %ld"
                    " %ld %ld %ld %ld %ld %ld %lu %lu %ld",
                    &state,
                    &(int){0}, &(int){0}, &(int){0}, &(int){0}, &(int){0},
                    &(unsigned long){0}, &(unsigned long){0},
                    &(unsigned long){0}, &(unsigned long){0},
                    &(unsigned long){0},
                    &utime, &stime,
                    &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
                    &(unsigned long){0}, &vsize, &rss);

                procs[count].pid = pid;
                strncpy(procs[count].name, comm, MAX_NAME_LEN - 1);
                procs[count].name[MAX_NAME_LEN - 1] = '\0';
                /* rough CPU estimate using utime+stime ticks */
                long ticks = sysconf(_SC_CLK_TCK);
                procs[count].cpu_percent = (float)(utime + stime) / ticks;
                /* mem percent */
                if (mem.total > 0)
                    procs[count].mem_percent = (float)(rss * page_kb) / mem.total * 100.0f;
                else
                    procs[count].mem_percent = 0.0f;
                count++;
            }
        }
        fclose(f);
    }
    closedir(dir);
    qsort(procs, count, sizeof(ProcessInfo), cmp_cpu);
    return count;
}
