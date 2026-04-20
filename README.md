# ⚡ SYSMON — Terminal System Monitor

> A real-time Linux system monitor built from scratch in **C**, inspired by `htop`.  
> Displays live CPU, memory, disk, uptime, and top processes — all inside your terminal.

---

## 📸 Preview

```
╔══════════════════════════════════════════════════════════╗
║           ⚡  SYSMON - System Monitor v1.0  ⚡            ║
║                  Built in C by Yosuf                    ║
╚══════════════════════════════════════════════════════════╝

  ── System Stats ──────────────────────────────────────────

  ▶ CPU Usage    [████████████░░░░░░░░░░░░░░░░░░]  42.3%
    Cores: 8  |  Model: Intel(R) Core(TM) i7-10750H

  ▶ Memory       [██████████████████░░░░░░░░░░░░]  61.0%
    Used: 9830 MB  |  Total: 16384 MB  |  Free: 6553 MB

  ▶ Disk (/)     [████████░░░░░░░░░░░░░░░░░░░░░░]  27.4%
    Used: 123.5 GB  |  Total: 450.0 GB  |  Free: 326.5 GB

  ▶ Uptime       3 days, 07:42:15

  ▶ Top Processes:
    PID      NAME                 CPU%       MEM%
    ───────  ───────────────────  ────────   ────────
    1423     chrome               38.2       12.4
    892      python3              5.1        3.2
    101      sshd                 0.3        0.1
```

---

## 🚀 Features

- 📊 **Live CPU usage** with color-coded progress bar (green → yellow → red)
- 🧠 **Memory stats** — used, free, total in MB
- 💾 **Disk usage** for root filesystem
- ⏱️ **System uptime** — days, hours, minutes, seconds
- ⚙️ **Top 5 processes** sorted by CPU usage
- 🔄 **Auto-refresh** every second (configurable)
- 🎨 **Color terminal UI** using ANSI escape codes — no external libraries

---

## 🛠️ Tech Stack

| Layer        | Technology              |
|--------------|-------------------------|
| Language     | C (C99)                 |
| OS           | Linux (Unix)            |
| Data Sources | `/proc/stat`, `/proc/meminfo`, `/proc/cpuinfo`, `/proc/uptime`, `/proc/[pid]/stat` |
| Build Tool   | GNU Make                |
| UI           | ANSI escape codes       |

---

## 📦 Build & Run

### Requirements
- Linux (Ubuntu, Debian, Arch, etc.)
- GCC compiler
- GNU Make

### Steps

```bash
# 1. Clone the repo
git clone https://github.com/YOUR_USERNAME/sysmon.git
cd sysmon

# 2. Build
make

# 3. Run (refreshes every 1 second)
./sysmon

# 4. Optional: custom refresh interval (e.g. every 2 seconds)
./sysmon 2
```

---

## 📁 Project Structure

```
sysmon/
├── src/
│   ├── sysmon.c      # Main loop, terminal UI, rendering
│   └── sysinfo.c     # Linux /proc reader — CPU, memory, disk, processes
├── include/
│   └── sysmon.h      # Structs and function declarations
├── Makefile          # Build configuration
└── README.md
```

---

## 🧠 How It Works

| Feature       | Source File         | Linux API Used          |
|---------------|---------------------|--------------------------|
| CPU usage     | `sysinfo.c`         | `/proc/stat`             |
| Memory        | `sysinfo.c`         | `/proc/meminfo`          |
| Disk usage    | `sysinfo.c`         | `statvfs()` syscall      |
| Uptime        | `sysinfo.c`         | `/proc/uptime`           |
| Processes     | `sysinfo.c`         | `/proc/[pid]/stat`       |
| Terminal UI   | `sysmon.c`          | ANSI escape codes        |

CPU usage is calculated by sampling `/proc/stat` between refresh intervals — the same method used by tools like `htop` and `top`.

---

## 🎯 What I Learned

- Reading and parsing Linux `/proc` virtual filesystem
- Low-level C systems programming (file I/O, structs, pointers)
- Terminal control with ANSI escape codes
- Process scheduling concepts (CPU ticks, clock ticks via `sysconf`)
- Memory management without external libraries
- Unix build tooling with `make`

---

## 🔮 Planned Features

- [ ] Network I/O monitoring (bytes sent/received)
- [ ] Per-core CPU breakdown
- [ ] Kill process by PID from the UI
- [ ] Log output to file
- [ ] Config file support

---

## 👤 Author

**Yosuf** — Computer Programming Student @ Seneca Polytechnic  
📍 Toronto, Canada  
🔗 [LinkedIn](https://linkedin.com/in/YOUR_PROFILE) | [GitHub](https://github.com/YOUR_USERNAME)

---

## 📄 License

MIT License — free to use, modify, and distribute.
