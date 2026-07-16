/*
Lesson C1
Device Files and /dev

What Are Device Files?
You've heard "everything is a file" in Linux many times now. This lesson is where that concept becomes real for embedded work.
When Linux wants to expose hardware to user programs, it creates a device file in /dev/. Your program opens the file, reads from it, writes to it — and it's actually talking to the hardware driver in the kernel.

Your C program
     │
     │  open("/dev/ttyS0", ...)
     │  read(fd, buf, size)
     ▼
Linux Kernel
     │
     │  serial port driver
     ▼
Physical UART hardware

ls -l /dev/ | head -30

crw-rw-rw-  1 root dialout 4,  64 Jun 10 /dev/ttyS0
brw-rw----  1 root disk    8,   0 Jun 10 /dev/sda
crw-rw-rw-  1 root root    1,   3 Jun 10 /dev/null
crw-rw-rw-  1 root root    1,   5 Jun 10 /dev/zero
crw-rw-rw-  1 root root    1,   8 Jun 10 /dev/random

c  →  character device  →  data flows byte by byte
b  →  block device      →  data flows in fixed blocks

Major number  →  which driver handles this device
Minor number  →  which specific device instance

crw-rw-rw- 1 root dialout 4, 64 /dev/ttyS0
                            │   │
                            │   └── minor: which serial port (port 0)
                            └────── major: 4 = serial driver

Major   Driver  Devices
1       Memory  /dev/null, /dev/zero, /dev/random
4       Serial  /dev/ttyS0, /dev/ttyS1
8       SCSI disk   /dev/sda, /dev/sdb
89      I2C     /dev/i2c-0, /dev/i2c-1
153     SPI     /dev/spidev0.0

/dev/null — The Black Hole
Anything written to it disappears. Reading from it returns nothing.

# Throw away output you don't want
./can_logger > /dev/null

# Silence both stdout and stderr
./can_logger > /dev/null 2>&1

/dev/zero — Infinite Zeros
Reading from it gives an endless stream of zero bytes.

# Create a 1MB file filled with zeros
dd if=/dev/zero of=empty_file bs=1024 count=1024

/dev/random — Random Data
Reading gives cryptographically random bytes.

# Generate a random hex string
head -c 16 /dev/random | xxd

/proc and /sys — Virtual Filesystems
Two special folders that aren't real files on disk — the kernel generates their contents on the fly:
/proc — Process and kernel info

cat /proc/cpuinfo      # CPU details
cat /proc/meminfo      # Memory usage
cat /proc/version      # Kernel version
cat /proc/uptime       # How long system has been running
ls /proc/              # Each number is a running process PID
cat /proc/$$/status    # Info about current shell process

/sys — Hardware info and control

ls /sys/class/          # hardware classes
ls /sys/class/net/      # network interfaces
ls /sys/class/gpio/     # GPIO pins (on real hardware)
ls /sys/class/thermal/  # temperature sensors
cat /sys/class/thermal/thermal_zone0/temp  # CPU temperature

On a Raspberry Pi or embedded board, /sys/class/gpio/ lets you control GPIO pins just by reading and writing files — no special library needed.

Reading from a Device File in C
The pattern is identical to reading a regular file — open, read, close:

int fd = open("/dev/urandom", O_RDONLY);
if (fd < 0) {
    perror("open /dev/urandom");
    return 1;
}

unsigned char random_bytes[8];
int n = read(fd, random_bytes, sizeof(random_bytes));

printf("Random bytes: ");
for (int i = 0; i < n; i++) {
    printf("%02X ", random_bytes[i]);
}
printf("\n");

close(fd);


nano device_files.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

// ── Read bytes from a device file ────────────────────
void read_device(const char *path, int bytes) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("  Cannot open %s\n", path);
        return;
    }

    unsigned char buf[256];
    if (bytes > (int)sizeof(buf)) bytes = sizeof(buf);

    int n = read(fd, buf, bytes);
    if (n < 0) {
        perror("  read failed");
        close(fd);
        return;
    }

    printf("  Read %d bytes: ", n);
    for (int i = 0; i < n && i < 16; i++) {
        printf("%02X ", buf[i]);
    }
    if (n > 16) printf("...");
    printf("\n");

    close(fd);
}

// ── Read a text file and print content ───────────────
void read_text(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        printf("  Cannot open %s\n", path);
        return;
    }

    char line[256];
    int lines = 0;
    while (fgets(line, sizeof(line), f) && lines < 3) {
        // Remove newline
        line[strcspn(line, "\n")] = '\0';
        printf("  %s\n", line);
        lines++;
    }

    fclose(f);
}

// ── Get file type from stat ───────────────────────────
const char* file_type(const char *path) {
    struct stat st;
    if (stat(path, &st) < 0) return "unknown";

    switch (st.st_mode & S_IFMT) {
        case S_IFCHR:  return "character device";
        case S_IFBLK:  return "block device";
        case S_IFREG:  return "regular file";
        case S_IFDIR:  return "directory";
        case S_IFLNK:  return "symbolic link";
        case S_IFIFO:  return "named pipe";
        case S_IFSOCK: return "socket";
        default:       return "unknown";
    }
}

// ── Get major and minor numbers ───────────────────────
void print_device_info(const char *path) {
    struct stat st;
    if (stat(path, &st) < 0) {
        printf("  Cannot stat %s\n", path);
        return;
    }

    printf("  Type:  %s\n", file_type(path));
    printf("  Major: %d\n", (int)major(st.st_rdev));
    printf("  Minor: %d\n", (int)minor(st.st_rdev));
}

// ── Write to /dev/null ────────────────────────────────
void demo_dev_null() {
    printf("\n=== /dev/null ===\n");
    print_device_info("/dev/null");

    int fd = open("/dev/null", O_WRONLY);
    if (fd >= 0) {
        char *msg = "This data disappears forever";
        int n = write(fd, msg, strlen(msg));
        printf("  Wrote %d bytes to /dev/null "
               "(they're gone)\n", n);
        close(fd);
    }
}

// ── Read from /dev/zero ───────────────────────────────
void demo_dev_zero() {
    printf("\n=== /dev/zero ===\n");
    print_device_info("/dev/zero");
    printf("  Reading 8 bytes:\n");
    read_device("/dev/zero", 8);
}

// ── Read from /dev/urandom ────────────────────────────
void demo_dev_urandom() {
    printf("\n=== /dev/urandom ===\n");
    print_device_info("/dev/urandom");
    printf("  Reading 8 random bytes:\n");
    read_device("/dev/urandom", 8);
    printf("  Reading 8 more (different each time):\n");
    read_device("/dev/urandom", 8);
}

// ── Read from /proc ───────────────────────────────────
void demo_proc() {
    printf("\n=== /proc virtual filesystem ===\n");

    printf("\n/proc/version:\n");
    read_text("/proc/version");

    printf("\n/proc/uptime:\n");
    read_text("/proc/uptime");

    printf("\n/proc/meminfo (first 3 lines):\n");
    read_text("/proc/meminfo");
}

// ── Read from /sys ────────────────────────────────────
void demo_sys() {
    printf("\n=== /sys virtual filesystem ===\n");

    printf("\nNetwork interfaces in /sys/class/net/:\n");

    FILE *f = popen("ls /sys/class/net/", "r");
    if (f) {
        char line[64];
        while (fgets(line, sizeof(line), f)) {
            line[strcspn(line, "\n")] = '\0';
            printf("  %s\n", line);
        }
        pclose(f);
    }

    printf("\nCPU temperature (/sys/class/thermal/):\n");
    read_text("/sys/class/thermal/thermal_zone0/temp");
}

// ── Main ─────────────────────────────────────────────
int main() {
    printf("=== Device Files and /dev Explorer ===\n");

    demo_dev_null();
    demo_dev_zero();
    demo_dev_urandom();
    demo_proc();
    demo_sys();

    printf("\n=== Done ===\n");
    return 0;
}


gcc device_files.c -o device_files
./device_files


=== Device Files and /dev Explorer ===

=== /dev/null ===
  Type:  character device
  Major: 1
  Minor: 3
  Wrote 28 bytes to /dev/null (they're gone)

=== /dev/zero ===
  Type:  character device
  Major: 1
  Minor: 5
  Reading 8 bytes:
  Read 8 bytes: 00 00 00 00 00 00 00 00

=== /dev/urandom ===
  Type:  character device
  Major: 1
  Minor: 9
  Reading 8 random bytes:
  Read 8 bytes: A3 F2 11 8C 4D 22 B1 9E ...
  Reading 8 more (different each time):
  Read 8 bytes: 77 3C D4 05 F9 AA 62 11 ...

=== /proc virtual filesystem ===

/proc/version:
  Linux version 6.18.33.2-microsoft-standard-WSL2

/proc/uptime:
  1234.56 789.01

/proc/meminfo (first 3 lines):
  MemTotal:      8192000 kB
  MemFree:       4096000 kB
  MemAvailable:  5120000 kB

=== /sys virtual filesystem ===

Network interfaces in /sys/class/net/:
  eth0
  lo
  vcan0

CPU temperature (/sys/class/thermal/):
  (may not be available in WSL2)

=== Done ===


stat() — Get File Information

struct stat st;
stat("/dev/null", &st);

Fills a struct stat with everything about a file:

st.st_mode    →  file type + permissions
st.st_size    →  file size in bytes
st.st_rdev    →  device major/minor numbers
st.st_uid     →  owner user ID
st.st_mtime   →  last modified time

S_IFMT mask — Extract File Type

switch (st.st_mode & S_IFMT) {
    case S_IFCHR:  // character device
    case S_IFBLK:  // block device
    case S_IFREG:  // regular file
    case S_IFDIR:  // directory
}

st.st_mode contains both the file type and permissions packed together. S_IFMT is a bitmask that extracts just the file type bits.

major() and minor()

major(st.st_rdev)   →  extract major number
minor(st.st_rdev)   →  extract minor number

st.st_rdev contains both major and minor numbers packed into one integer. These macros unpack them.

popen() — Run a Shell Command and Read Output

FILE *f = popen("ls /sys/class/net/", "r");
char line[64];
while (fgets(line, sizeof(line), f)) {
    printf("%s", line);
}
pclose(f);

popen runs a shell command and gives you its output as a FILE* — you read it like a normal file. pclose instead of fclose when done.

Hardware          Device file        What you do
────────          ───────────        ───────────
UART serial   →   /dev/ttyS0    →   open + read/write
SPI sensor    →   /dev/spidev0.0→   open + ioctl + read/write
I2C sensor    →   /dev/i2c-1    →   open + ioctl + read/write
GPIO pin      →   /sys/class/gpio → open + write "1" or "0"
CAN interface →   socket(AF_CAN)→   socket + bind (already done!)

Path        Type        Purpose
/dev/null   char device discard output
/dev/zero   char device source of zeros
/dev/urandom char device random bytes
/dev/ttyS0  char device   serial port
/dev/sda    block device    hard disk
/dev/i2c-1  char device     I2C bus
/dev/spidev0.0  char device SPI bus
/proc/      virtual fs      kernel/process info
/sys/       virtual fs      hardware info/control





*/