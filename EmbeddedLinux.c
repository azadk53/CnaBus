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
#include <sys/sysmacros.h>
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

/*
Lesson C2
GPIO from Linux

3.3V  ●  ● 5V
GPIO2 ●  ● 5V
GPIO3 ●  ● GND
GPIO4 ●  ● GPIO14
GND  ●  ● GPIO15



Interface   Path    Style
Sysfs(older)    /sys/class/gpio/    read/write text files
Character device (modern)   /dev/gpiochip0  ioctl calls

/sys/class/gpio/
├── export          ← write pin number here to activate it
├── unexport        ← write pin number here to deactivate it
└── gpio4/          ← appears after exporting pin 4
    ├── direction   ← write "in" or "out"
    ├── value       ← write "1"/"0" or read "1"/"0"
    └── edge        ← for interrupts: "rising", "falling", "both"

Three steps to use a GPIO pin:
# Step 1 — export the pin (activate it)
echo 4 > /sys/class/gpio/export

# Step 2 — set direction
echo out > /sys/class/gpio/gpio4/direction

# Step 3 — set value
echo 1 > /sys/class/gpio/gpio4/value   # HIGH
echo 0 > /sys/class/gpio/gpio4/value   # LOW
To read an input pin:
echo in > /sys/class/gpio/gpio4/direction
cat /sys/class/gpio/gpio4/value   # prints 0 or 1
When done — unexport:
echo 4 > /sys/class/gpio/gpio unexport

// Export pin
int fd = open("/sys/class/gpio/export", O_WRONLY);
write(fd, "4", 1);
close(fd);

// Set direction
fd = open("/sys/class/gpio/gpio4/direction", O_WRONLY);
write(fd, "out", 3);
close(fd);

// Set value HIGH
fd = open("/sys/class/gpio/gpio4/value", O_WRONLY);
write(fd, "1", 1);
close(fd);


Practical Program — GPIO Controller

nano gpio.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

// ── GPIO base path ────────────────────────────────────
#define GPIO_BASE    "/sys/class/gpio"
#define SIM_BASE     "/tmp/gpio_sim"    // simulation path

// ── Use simulation if real GPIO not available ─────────
static int use_simulation = 0;

// ── Helper: write string to file ─────────────────────
int write_file(const char *path, const char *value) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        return -1;
    }
    int n = write(fd, value, strlen(value));
    close(fd);
    return n;
}

// ── Helper: read string from file ────────────────────
int read_file(const char *path, char *buf, int size) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        return -1;
    }
    int n = read(fd, buf, size - 1);
    if (n > 0) buf[n] = '\0';
    close(fd);
    return n;
}

// ── Setup simulation environment ──────────────────────
void setup_simulation(int pin) {
    char path[128];

    // Create pin directory
    snprintf(path, sizeof(path), "%s/gpio%d", SIM_BASE, pin);
    mkdir(SIM_BASE, 0755);
    mkdir(path, 0755);

    // Create direction file
    char dir_path[128];
    snprintf(dir_path, sizeof(dir_path),
             "%s/gpio%d/direction", SIM_BASE, pin);
    int fd = open(dir_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) { write(fd, "in", 2); close(fd); }

    // Create value file
    char val_path[128];
    snprintf(val_path, sizeof(val_path),
             "%s/gpio%d/value", SIM_BASE, pin);
    fd = open(val_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) { write(fd, "0", 1); close(fd); }

    printf("[SIM] Created simulated GPIO pin %d\n", pin);
}

// ── Export a GPIO pin ─────────────────────────────────
int gpio_export(int pin) {
    char path[128];
    char pin_str[8];

    snprintf(pin_str, sizeof(pin_str), "%d", pin);

    if (use_simulation) {
        setup_simulation(pin);
        return 0;
    }

    // Check if already exported
    snprintf(path, sizeof(path),
             "%s/gpio%d", GPIO_BASE, pin);
    if (access(path, F_OK) == 0) {
        printf("[GPIO] Pin %d already exported\n", pin);
        return 0;
    }

    snprintf(path, sizeof(path), "%s/export", GPIO_BASE);
    if (write_file(path, pin_str) < 0) {
        perror("gpio_export");
        return -1;
    }

    usleep(100000);    // wait 100ms for sysfs to create files
    printf("[GPIO] Exported pin %d\n", pin);
    return 0;
}

// ── Unexport a GPIO pin ───────────────────────────────
int gpio_unexport(int pin) {
    char path[128];
    char pin_str[8];

    snprintf(pin_str, sizeof(pin_str), "%d", pin);

    if (use_simulation) {
        printf("[SIM] Unexported pin %d\n", pin);
        return 0;
    }

    snprintf(path, sizeof(path), "%s/unexport", GPIO_BASE);
    if (write_file(path, pin_str) < 0) {
        perror("gpio_unexport");
        return -1;
    }

    printf("[GPIO] Unexported pin %d\n", pin);
    return 0;
}

// ── Set pin direction ─────────────────────────────────
int gpio_set_direction(int pin, const char *direction) {
    char path[128];
    const char *base = use_simulation ? SIM_BASE : GPIO_BASE;

    snprintf(path, sizeof(path),
             "%s/gpio%d/direction", base, pin);

    if (write_file(path, direction) < 0) {
        perror("gpio_set_direction");
        return -1;
    }

    printf("[GPIO] Pin %d direction set to '%s'\n",
           pin, direction);
    return 0;
}

// ── Set pin value (0 or 1) ────────────────────────────
int gpio_write(int pin, int value) {
    char path[128];
    const char *base = use_simulation ? SIM_BASE : GPIO_BASE;

    snprintf(path, sizeof(path),
             "%s/gpio%d/value", base, pin);

    const char *val_str = value ? "1" : "0";

    if (write_file(path, val_str) < 0) {
        perror("gpio_write");
        return -1;
    }

    printf("[GPIO] Pin %d set to %s (%s)\n",
           pin, val_str, value ? "HIGH" : "LOW");
    return 0;
}

// ── Read pin value ────────────────────────────────────
int gpio_read(int pin) {
    char path[128];
    char buf[4];
    const char *base = use_simulation ? SIM_BASE : GPIO_BASE;

    snprintf(path, sizeof(path),
             "%s/gpio%d/value", base, pin);

    if (read_file(path, buf, sizeof(buf)) < 0) {
        perror("gpio_read");
        return -1;
    }

    int value = atoi(buf);
    printf("[GPIO] Pin %d read: %d (%s)\n",
           pin, value, value ? "HIGH" : "LOW");
    return value;
}

// ── Blink an LED (toggle pin repeatedly) ─────────────
void gpio_blink(int pin, int times, int delay_ms) {
    printf("[GPIO] Blinking pin %d  %dx  delay=%dms\n",
           pin, times, delay_ms);

    for (int i = 0; i < times; i++) {
        gpio_write(pin, 1);
        usleep(delay_ms * 1000);
        gpio_write(pin, 0);
        usleep(delay_ms * 1000);
    }
}

// ── Check if real GPIO available ──────────────────────
int check_gpio_available() {
    struct stat st;
    if (stat("/sys/class/gpio/export", &st) == 0) {
        return 1;    // real GPIO available
    }
    return 0;
}

// ── Main ─────────────────────────────────────────────
int main() {
    printf("=== GPIO Controller ===\n\n");

    // Check if real GPIO available
    if (!check_gpio_available()) {
        printf("Real GPIO not available (WSL2)\n");
        printf("Running in simulation mode\n\n");
        use_simulation = 1;
    }

    int led_pin    = 4;    // GPIO4 — LED
    int button_pin = 17;   // GPIO17 — Button

    // ── Setup LED pin as output ───────────────────────
    printf("--- Setting up LED pin (GPIO%d) ---\n",
           led_pin);
    gpio_export(led_pin);
    gpio_set_direction(led_pin, "out");
    printf("\n");

    // ── Setup button pin as input ─────────────────────
    printf("--- Setting up button pin (GPIO%d) ---\n",
           button_pin);
    gpio_export(button_pin);
    gpio_set_direction(button_pin, "in");
    printf("\n");

    // ── Control LED ───────────────────────────────────
    printf("--- Controlling LED ---\n");
    gpio_write(led_pin, 1);    // turn on
    sleep(1);
    gpio_write(led_pin, 0);    // turn off
    sleep(1);
    printf("\n");

    // ── Read button ───────────────────────────────────
    printf("--- Reading button ---\n");
    int state = gpio_read(button_pin);
    printf("Button state: %s\n\n",
           state ? "PRESSED" : "NOT PRESSED");

    // ── Blink LED 5 times ─────────────────────────────
    printf("--- Blinking LED 5 times ---\n");
    gpio_blink(led_pin, 5, 200);
    printf("\n");

    // ── Cleanup ───────────────────────────────────────
    printf("--- Cleanup ---\n");
    gpio_write(led_pin, 0);    // ensure LED is off
    gpio_unexport(led_pin);
    gpio_unexport(button_pin);

    printf("\n=== Done ===\n");
    return 0;
}

gcc gpio.c -o gpio
./gpio


=== GPIO Controller ===

Real GPIO not available (WSL2)
Running in simulation mode

--- Setting up LED pin (GPIO4) ---
[SIM] Created simulated GPIO pin 4
[GPIO] Pin 4 direction set to 'out'

--- Setting up button pin (GPIO17) ---
[SIM] Created simulated GPIO pin 17
[GPIO] Pin 17 direction set to 'in'

--- Controlling LED ---
[GPIO] Pin 4 set to 1 (HIGH)
[GPIO] Pin 4 set to 0 (LOW)

--- Reading button ---
[GPIO] Pin 17 read: 0 (LOW)
Button state: NOT PRESSED

--- Blinking LED 5 times ---
[GPIO] Blinking pin 4  5x  delay=200ms
[GPIO] Pin 4 set to 1 (HIGH)
[GPIO] Pin 4 set to 0 (LOW)
[GPIO] Pin 4 set to 1 (HIGH)
[GPIO] Pin 4 set to 0 (LOW)
[GPIO] Pin 4 set to 1 (HIGH)
[GPIO] Pin 4 set to 0 (LOW)
[GPIO] Pin 4 set to 1 (HIGH)
[GPIO] Pin 4 set to 0 (LOW)
[GPIO] Pin 4 set to 1 (HIGH)
[GPIO] Pin 4 set to 0 (LOW)

--- Cleanup ---
[GPIO] Pin 4 set to 0 (LOW)
[SIM] Unexported pin 4
[SIM] Unexported pin 17

=== Done ===

Check the simulated files were created:
ls /tmp/gpio_sim/
cat /tmp/gpio_sim/gpio4/direction
cat /tmp/gpio_sim/gpio4/value

access() — Check if File Exists
if (access(path, F_OK) == 0) {
    // file exists
}

F_OK checks existence. Other flags:

R_OK — readable
W_OK — writable
X_OK — executable

snprintf() — Safe String Formatting
char path[128];
snprintf(path, sizeof(path),
         "%s/gpio%d/value", base, pin);

usleep(100000);    // sleep 100ms (100,000 microseconds)
usleep(200000);    // sleep 200ms


*/