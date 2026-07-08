/*
Lesson B1
File I/O in C

write(sock, &frame, sizeof(frame));   // send CAN frame
read(sock, &frame, sizeof(frame));    // receive CAN frame

        Standard C (stdio)          Linux System Calls
Functions   fopen, fread, fwrite, fclose    open, read, write, close
Returns     FILE * pointer       integer file descriptor
Buffered?   Yes — faster for text   No — direct kernel access
Use for     Text files, logs, config   Devices, sockets, precise control

FILE *logfile = fopen("can_log.txt", "w");
fprintf(logfile, "data here\n");
fclose(logfile);

FILE *f = fopen("filename.txt", "mode");

Mode                    Meaning
"r"                     Read only — file must exist
"w"                     Write — creates file, overwrites if exists
"a"                     Append — adds to end of existing file
"r+                     "Read and write
"b"                     Binary mode — add to any: "rb", "wb"

Always check if fopen succeeded:
FILE *f = fopen("data.txt", "r");
if (f == NULL) {
    perror("Failed to open file");
    return 1;
}

Reading Files — Three Ways
// 1. Read one character at a time
int c = fgetc(f);

// 2. Read one line at a time
char line[256];
fgets(line, sizeof(line), f);

// 3. Read formatted data (like scanf but from file)
int value;
fscanf(f, "%d", &value);

Writing Files — Three Ways
c// 1. Write one character
fputc('A', f);

// 2. Write a string
fputs("hello\n", f);

// 3. Write formatted (like printf but to file)
fprintf(f, "RPM: %d  Temp: %d\n", rpm, temp);

open — Open a File
#include <fcntl.h>

int fd = open("filename.txt", flags);
int fd = open("filename.txt", flags, mode);


Flag                Meaning
O_RDONLY            Read only
O_WRONLY            Write only
O_RDWR              Read and write
O_CREAT             Create if doesn't exist
O_TRUNC             Clear file on open
O_APPEND            Write to end


Combine flags with |:
int fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
The 0644 is the permission for the new file — same as chmod 644.

read and write
// Read
char buf[128];
int nbytes = read(fd, buf, sizeof(buf));

// Write
char *msg = "hello\n";
write(fd, msg, strlen(msg));

lseek — Move Position in a File
#include <unistd.h>

lseek(fd, 0, SEEK_SET);    // go to beginning
lseek(fd, 0, SEEK_END);    // go to end
lseek(fd, 10, SEEK_CUR);   // move 10 bytes forward from current position

Whence          Meaning
SEEK_SET        From beginning of file
SEEK_END        From end of file
SEEK_CUR        From current position

close
cclose(fd);

Practical Program — Sensor Data File

nano file_io.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define FILENAME "sensor_data.txt"

// ── Write sensor data using system calls ─────────────
void write_sensor_data() {
    int fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Failed to open file for writing");
        return;
    }

    printf("Writing sensor data...\n");

    // Simulate writing 5 sensor readings
    char buf[128];
    int readings[][2] = {
        {800,  90},
        {950,  91},
        {1100, 92},
        {1300, 93},
        {1500, 94},
    };

    for (int i = 0; i < 5; i++) {
        int len = snprintf(buf, sizeof(buf),
                  "RPM=%d TEMP=%d\n",
                  readings[i][0], readings[i][1]);
        write(fd, buf, len);
        printf("  Wrote: RPM=%d TEMP=%d\n",
               readings[i][0], readings[i][1]);
    }

    close(fd);
    printf("Write complete.\n\n");
}

// ── Read sensor data using stdio ─────────────────────
void read_sensor_data() {
    FILE *f = fopen(FILENAME, "r");
    if (f == NULL) {
        perror("Failed to open file for reading");
        return;
    }

    printf("Reading sensor data...\n");

    char line[128];
    int rpm, temp, count = 0;

    while (fgets(line, sizeof(line), f) != NULL) {
        // Parse each line
        if (sscanf(line, "RPM=%d TEMP=%d", &rpm, &temp) == 2) {
            count++;
            printf("  Reading %d: RPM=%d  TEMP=%d°C\n",
                   count, rpm, temp);
        }
    }

    fclose(f);
    printf("Read complete. Total readings: %d\n\n", count);
}

// ── Append new data using stdio ───────────────────────
void append_sensor_data(int rpm, int temp) {
    FILE *f = fopen(FILENAME, "a");
    if (f == NULL) {
        perror("Failed to open file for appending");
        return;
    }

    fprintf(f, "RPM=%d TEMP=%d\n", rpm, temp);
    fclose(f);
    printf("Appended: RPM=%d TEMP=%d\n", rpm, temp);
}

// ── Get file size using lseek ─────────────────────────
void show_file_size() {
    int fd = open(FILENAME, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open file");
        return;
    }

    // lseek to end returns current position = file size
    long size = lseek(fd, 0, SEEK_END);
    close(fd);
    printf("File size: %ld bytes\n\n", size);
}

// ── Main ─────────────────────────────────────────────
int main() {
    printf("=== File I/O Demo ===\n\n");

    // 1. Write initial data
    write_sensor_data();

    // 2. Read it back
    read_sensor_data();

    // 3. Show file size
    show_file_size();

    // 4. Append more readings
    printf("Appending new readings...\n");
    append_sensor_data(1700, 95);
    append_sensor_data(1900, 96);
    printf("\n");

    // 5. Read again to see all data
    read_sensor_data();

    // 6. Final file size
    show_file_size();

    return 0;
}

Compile:
gcc file_io.c -o file_io
Run:
./file_io

=== File I/O Demo ===

Writing sensor data...
  Wrote: RPM=800  TEMP=90
  Wrote: RPM=950  TEMP=91
  Wrote: RPM=1100 TEMP=92
  Wrote: RPM=1300 TEMP=93
  Wrote: RPM=1500 TEMP=94
Write complete.

Reading sensor data...
  Reading 1: RPM=800  TEMP=90°C
  Reading 2: RPM=950  TEMP=91°C
  Reading 3: RPM=1100 TEMP=92°C
  Reading 4: RPM=1300 TEMP=93°C
  Reading 5: RPM=1500 TEMP=94°C
Read complete. Total readings: 5

File size: 85 bytes

Appending new readings...
Appended: RPM=1700 TEMP=95
Appended: RPM=1900 TEMP=96

Reading sensor data...
  Reading 1: RPM=800  TEMP=90°C
  Reading 2: RPM=950  TEMP=91°C
  Reading 3: RPM=1100 TEMP=92°C
  Reading 4: RPM=1300 TEMP=93°C
  Reading 5: RPM=1500 TEMP=94°C
  Reading 6: RPM=1700 TEMP=95°C
  Reading 7: RPM=1900 TEMP=96°C
Read complete. Total readings: 7

File size: 119 bytes


stdio (fopen/fclose)          system calls (open/close)
────────────────────          ─────────────────────────
FILE *f = fopen(...)          int fd = open(...)
fgets / fscanf                read(fd, buf, size)
fprintf / fputs               write(fd, buf, size)
fclose(f)                     lseek(fd, pos, whence)
                              close(fd)

Use for: text files,          Use for: devices, sockets,
logs, configs                 hardware, precise control
*/

/*
Lesson B2
Error Handling in C

Why This Lesson Matters
In embedded systems things go wrong constantly:

A device file doesn't exist yet
A sensor stops responding
A file permission is wrong
A network interface goes down

When a system call fails it does two things:

Returns -1 (or NULL for pointer functions)
Sets a global variable called errno with the error code

int fd = open("/dev/nonexistent", O_RDONLY);
// fd = -1  ← failure
// errno = 2  ← ENOENT (No such file or directory)

errno value     Constant      Meaning
1               EPERM       Operation not permitted
2               ENOENT      No such file or directory
13              EACCES      Permission denied
16              EBUSY       Device busy
22              EINVAL      Invalid argument
32              EPIPE       Broken pipe
110             ETIMEDOUT   Connection timed out


perror() — Quickest Way
#include <stdio.h>

int fd = open("/dev/fake", O_RDONLY);
if (fd < 0) {
    perror("open failed");
}

open failed: No such file or directory

strerror() — Get Error as String
#include <string.h>
#include <errno.h>

int fd = open("/dev/fake", O_RDONLY);
if (fd < 0) {
    printf("Error code: %d\n", errno);
    printf("Error message: %s\n", strerror(errno));
}

Error code: 2
Error message: No such file or directory

errno directly — For specific error handling
#include <errno.h>

int fd = open("/dev/i2c-1", O_RDWR);
if (fd < 0) {
    if (errno == ENOENT) {
        printf("I2C device not found — is the module loaded?\n");
    } else if (errno == EACCES) {
        printf("Permission denied — try: sudo chmod 666 /dev/i2c-1\n");
    } else {
        perror("Unexpected error opening I2C device");
    }
}

errno gets overwritten by the next system call. Always read it right after a failure:
int fd = open("/dev/fake", O_RDONLY);
if (fd < 0) {
    int saved_errno = errno;       // save it immediately
    printf("Doing something else...\n");  // this might change errno
    printf("Error was: %s\n", strerror(saved_errno));  // use saved
}


void die(const char *msg) {
    perror(msg);
    exit(1);
}

// Usage — much cleaner
int fd = open("/dev/spidev0.0", O_RDWR);
if (fd < 0) die("Failed to open SPI device");

int n = read(fd, buf, size);
if (n < 0) die("Failed to read from SPI device");

nano error_handling.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

// ── Helper: print error and exit ─────────────────────
void die(const char *msg) {
    fprintf(stderr, "FATAL: %s: %s\n", msg, strerror(errno));
    exit(1);
}

// ── Helper: print warning but continue ───────────────
void warn(const char *msg) {
    fprintf(stderr, "WARNING: %s: %s\n", msg, strerror(errno));
}

// ── Try to open a file safely ────────────────────────
int safe_open(const char *path, int flags) {
    int fd = open(path, flags);
    if (fd < 0) {
        int saved = errno;
        fprintf(stderr, "ERROR opening '%s': ", path);

        switch(saved) {
            case ENOENT:
                fprintf(stderr, "File not found\n");
                break;
            case EACCES:
                fprintf(stderr, "Permission denied — check chmod\n");
                break;
            case EBUSY:
                fprintf(stderr, "Device busy — already in use\n");
                break;
            default:
                fprintf(stderr, "%s\n", strerror(saved));
        }
        return -1;
    }
    return fd;
}

// ── Write with error checking ─────────────────────────
int safe_write(int fd, const char *data, int len) {
    int written = write(fd, data, len);
    if (written < 0) {
        warn("write failed");
        return -1;
    }
    if (written < len) {
        fprintf(stderr, "WARNING: partial write (%d of %d bytes)\n",
                written, len);
        return -1;
    }
    return written;
}

// ── Read with error checking ──────────────────────────
int safe_read(int fd, char *buf, int len) {
    int nbytes = read(fd, buf, len);
    if (nbytes < 0) {
        warn("read failed");
        return -1;
    }
    if (nbytes == 0) {
        printf("End of file reached\n");
        return 0;
    }
    return nbytes;
}

// ── Main ─────────────────────────────────────────────
int main() {
    printf("=== Error Handling Demo ===\n\n");

    // 1. Try opening a file that doesn't exist
    printf("Test 1: Open non-existent file\n");
    int fd = safe_open("/tmp/does_not_exist.txt", O_RDONLY);
    if (fd < 0) printf("  Handled gracefully.\n\n");

    // 2. Try opening a real file
    printf("Test 2: Open real file\n");
    fd = safe_open("/proc/version", O_RDONLY);
    if (fd >= 0) {
        char buf[256];
        int n = safe_read(fd, buf, sizeof(buf)-1);
        if (n > 0) {
            buf[n] = '\0';
            printf("  Read: %s\n", buf);
        }
        close(fd);
    }

    // 3. Create and write to a file
    printf("Test 3: Write to file\n");
    fd = open("/tmp/test_output.txt",
              O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) die("Cannot create test file");

    char *data = "sensor=RPM value=1500\n";
    int result = safe_write(fd, data, strlen(data));
    if (result > 0) {
        printf("  Wrote %d bytes successfully\n", result);
    }
    close(fd);

    // 4. Try to write to a read-only file descriptor
    printf("\nTest 4: Write to read-only fd\n");
    fd = open("/tmp/test_output.txt", O_RDONLY);
    if (fd >= 0) {
        result = safe_write(fd, "test", 4);
        if (result < 0) printf("  Handled gracefully.\n");
        close(fd);
    }

    // 5. Show errno values directly
    printf("\nTest 5: errno values\n");
    open("/nonexistent", O_RDONLY);
    printf("  errno after failed open: %d (%s)\n",
           errno, strerror(errno));

    printf("\nDone.\n");
    return 0;
}

gcc error_handling.c -o error_handling
./error_handling
=== Error Handling Demo ===

Test 1: Open non-existent file
  ERROR opening '/tmp/does_not_exist.txt': File not found
  Handled gracefully.

Test 2: Open real file
  Read: Linux version 6.1.0-microsoft-standard-WSL2 ...

Test 3: Write to file
  Wrote 22 bytes successfully

Test 4: Write to read-only fd
  WARNING: write failed: Bad file descriptor
  Handled gracefully.

Test 5: errno values
  errno after failed open: 2 (No such file or directory)

Done.

stderr vs stdout
You might have noticed fprintf(stderr, ...) in the code. There are actually three standard streams in Linux:
Stream         fd      Purpose     Command
stdin          0       Input       keyboard
stdout         1       Normal output    printf()
stderr         2       Error output  fprintf(stderr,...)


// Standard embedded error checking pattern:

int fd = open(device_path, O_RDWR);
if (fd < 0) {
    perror("open");
    return -1;         // or exit(1) if fatal
}

int n = read(fd, buf, size);
if (n < 0) {
    perror("read");
    close(fd);
    return -1;
}

// success — use the data


Tool                 Use case
perror("msg")       Quick error print — message + errno description
strerror(errno)     Get error as string — use in fprintf or log files
errno               Check specific error type with if/switch
fprintf(stderr,...) Print errors to stderr not stdout
Save errno immediately  Before any other call overwrites it
die() helper            Fatal errors — print and exit
warn() helper           Non-fatal — log and continue
*/