#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "../include/sysmon.h"

#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"
#define BG_BLACK    "\033[40m"
#define CLEAR       "\033[H\033[J"

void draw_bar(float percent, int width) {
    int filled = (int)((percent / 100.0f) * width);
    const char *color = (percent > 80) ? RED : (percent > 50) ? YELLOW : GREEN;

    printf("%s[", BOLD);
    printf("%s", color);
    for (int i = 0; i < width; i++) {
        if (i < filled)
            printf("█");
        else
            printf("░");
    }
    printf("%s]%s", RESET BOLD, RESET);
}

void print_header() {
    printf(CYAN BOLD);
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║           ⚡  SYSMON - System Monitor v1.0  ⚡            ║\n");
    printf("║                  Built in C by Yosuf                    ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    printf(RESET);
}

void print_cpu(CpuInfo *cpu) {
    printf(BOLD CYAN "  ▶ CPU Usage    " RESET);
    draw_bar(cpu->usage_percent, 30);
    printf(YELLOW "  %.1f%%\n" RESET, cpu->usage_percent);
    printf(WHITE "    Cores: %d  |  Model: %s\n" RESET, cpu->cores, cpu->model);
}

void print_memory(MemInfo *mem) {
    float percent = ((float)(mem->total - mem->available) / mem->total) * 100.0f;
    printf(BOLD CYAN "  ▶ Memory       " RESET);
    draw_bar(percent, 30);
    printf(YELLOW "  %.1f%%\n" RESET, percent);
    printf(WHITE "    Used: %lu MB  |  Total: %lu MB  |  Free: %lu MB\n" RESET,
           (mem->total - mem->available) / 1024,
           mem->total / 1024,
           mem->available / 1024);
}

void print_disk(DiskInfo *disk) {
    float percent = ((float)(disk->total - disk->free) / disk->total) * 100.0f;
    printf(BOLD CYAN "  ▶ Disk (/)     " RESET);
    draw_bar(percent, 30);
    printf(YELLOW "  %.1f%%\n" RESET, percent);
    printf(WHITE "    Used: %.1f GB  |  Total: %.1f GB  |  Free: %.1f GB\n" RESET,
           (float)(disk->total - disk->free) / (1024 * 1024),
           (float)disk->total / (1024 * 1024),
           (float)disk->free / (1024 * 1024));
}

void print_uptime(long uptime_seconds) {
    long days    = uptime_seconds / 86400;
    long hours   = (uptime_seconds % 86400) / 3600;
    long minutes = (uptime_seconds % 3600) / 60;
    long seconds = uptime_seconds % 60;
    printf(BOLD CYAN "  ▶ Uptime       " RESET);
    printf(GREEN "%ld days, %02ld:%02ld:%02ld\n" RESET, days, hours, minutes, seconds);
}

void print_top_processes(ProcessInfo *procs, int count) {
    printf(BOLD CYAN "\n  ▶ Top Processes:\n" RESET);
    printf(WHITE "    %-8s %-20s %-10s %-10s\n" RESET, "PID", "NAME", "CPU%", "MEM%");
    printf(WHITE "    %-8s %-20s %-10s %-10s\n" RESET, "───────", "───────────────────", "────────", "────────");
    for (int i = 0; i < count && i < 5; i++) {
        const char *color = (procs[i].cpu_percent > 50) ? RED :
                            (procs[i].cpu_percent > 20) ? YELLOW : WHITE;
        printf("    %s%-8d %-20s %-10.1f %-10.1f%s\n",
               color,
               procs[i].pid,
               procs[i].name,
               procs[i].cpu_percent,
               procs[i].mem_percent,
               RESET);
    }
}

void print_footer() {
    time_t now = time(NULL);
    char *t = ctime(&now);
    t[strlen(t) - 1] = '\0';
    printf(BOLD CYAN "\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║  🕐 %s  |  Press Ctrl+C to exit  ║\n", t);
    printf("╚══════════════════════════════════════════════════════════╝\n" RESET);
}

int main(int argc, char *argv[]) {
    int interval = 1;
    if (argc > 1) interval = atoi(argv[1]);
    if (interval < 1) interval = 1;

    CpuInfo    cpu;
    MemInfo    mem;
    DiskInfo   disk;
    ProcessInfo procs[MAX_PROCESSES];
    int proc_count = 0;
    long uptime_sec = 0;

    while (1) {
        get_cpu_info(&cpu);
        get_mem_info(&mem);
        get_disk_info(&disk);
        get_uptime(&uptime_sec);
        proc_count = get_top_processes(procs, MAX_PROCESSES);

        printf(CLEAR);
        print_header();
        printf(CYAN BOLD "\n  ── System Stats ──────────────────────────────────────────\n\n" RESET);
        print_cpu(&cpu);
        printf("\n");
        print_memory(&mem);
        printf("\n");
        print_disk(&disk);
        printf("\n");
        print_uptime(uptime_sec);
        print_top_processes(procs, proc_count);
        print_footer();

        sleep(interval);
    }
    return 0;
}
